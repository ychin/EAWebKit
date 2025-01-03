/*H*************************************************************************************************

    netconnxboxone.cpp

    Description:
        Provides network setup and teardown support. Does not actually create any
        kind of network connections.

        Please refer to the NetConnXboxOne Refactor TDD for more info.
        https://docs.developer.ea.com/display/GOSInternal/%5BTDD%5DNetConnXboxOne+Refactor

    Copyright (c) 2001-2015 Electronic Arts Inc.

    \Version 12/11/2012 (jbrookes)
    \Version 09/25/2015 (tcho) Complete refactor to reduce complexity.  

*************************************************************************************************H*/

/*** Include files ********************************************************************************/

#define WIN32_LEAN_AND_MEAN 1               // avoid extra stuff
#include <windows.h>
#include <winsock2.h>
#include <wrl.h>                            // for ComPtr
#include <robuffer.h>                       // for IBufferByteAccess

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/dirtysock/xboxone/dirtyauthxboxone.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtycert.h"
#include "DirtySDK/dirtysock/dirtythread.h"
#include "DirtySDK/dirtylang.h"             // for locality macros and definitions
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonparse.h"
#include "netconncommon.h"
#include "netconnlocaluser.h"

#include "IEAUser/IEAUser.h"
#include "EAUser/CapilanoUser.h"

using namespace Windows::Xbox::Networking;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Xbox::ApplicationModel::Core;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Xbox::System;
using namespace Windows::Xbox::UI;
using namespace Microsoft::Xbox::Services;


/*** Defines **************************************************************************************/

// LOCALE_SNAME: defined in WinNls.h (WINVER >= 0x0600). This block is just there for the case where the define is absent.
#ifndef LOCALE_SNAME
#define LOCALE_SNAME                    0x0000005c   // locale name (ie: en-us)
#endif

// Maximum number of times to retry polling the network status
#define NETCONN_MAX_NETWORK_STATUS_RETRIES (3)

// Time between network status polls
#define NETCONN_NETWORK_STATUS_RETRY_TIME_MS (5000)

// Time in ms to wait for the network connectivity level to become xbox live
#define NETCONN_MAX_WAIT_TIME_FOR_XBLWAIT (5000)

// environment determination timeout 
#define NETCONNXBOXONE_ENV_TIMEOUT (10 * 1000) //10 seconds

// Global EA TMS SCID setup for us by MS, and the propped ea_environment.json file contain environment info
#define NETCONNXBOXONE_GLOBAL_EA_TMS_SCID   "c1e40100-ffb3-44f2-916b-be7650f08966"
#define NETCONNXBOXONE_GLOBAL_EA_ENV_FILE   "ea_environment.json"
#define NETCONNXBOXONE_RETAIL_SANDBOX       "RETAIL"
#define NETCONNXBOXONE_DEFAULT_SANDBOX      NETCONNXBOXONE_RETAIL_SANDBOX
#define NETCONNXBOXONE_CERT_SANDBOX         "CERT"
#define NETCONNXBOXONE_CERTDEBUG_SANDBOX    "CERT.DEBUG"
#define NETCONNXBOXONE_INTERNAL_CERT_SANDBOX "EARW.3"
#define NETCONNXBOXONE_TEST_SANDBOX         "EARW.1"
#define NETCONNXBOXONE_EAACCESS_SANDBOX     "XDP1.0"
#define NETCONNXBOXONE_STR_SANDBOX_LENGTH   32

// country Code String Length
#define NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH 3

/*** Macros ***************************************************************************************/

/*** Type Definitions *****************************************************************************/

//! netconn main module states
typedef enum NetConnStateE
{
    ST_IDLE = 0,
    ST_RUNNING,
    ST_SUSPEND,
    ST_RESUME,
    ST_FAILED,
    ST_CLEANUP,
    ST_XBLWAIT
} NetConnStateE;

//! netconn running sub states
typedef enum NetConnRunStateE
{
    ST_RUN_INIT = 0,
    ST_RUN_LOGIN,
    ST_RUN_TOKEN,
    ST_RUN_ENV,
    ST_RUN_ONLINE,
} NetConnRunStateE;

//! account picker states
typedef enum NetConnAccountPickerStateE
{
    ST_ACTPICK_IDLE = 0,    //!< initial account picker state
    ST_ACTPICK_OPEN,        //!< account picker is open
    ST_ACTPICK_CLOSE        //!< account picker is closed
} NetConnAccountPickerStateE;

//! token acquisition state
typedef enum NetConnTokenStateE
{
    ST_TOKEN_INVALID = 0,    //!< token has not yet been requested
    ST_TOKEN_FAILED,         //!< token acquisition failed
    ST_TOKEN_INPROG,         //!< token acquisition in progress
    ST_TOKEN_VALID           //!< token valid
} NetConnTokenStateE;

//! network status thread state
typedef enum NetConnNetworkStatusStateE
{
    ST_STATUS_INVALID = 0,  //!< network status thread is not started
    ST_STATUS_VALID,        //!< network status thread gone through the first time
    ST_STATUS_INPROGRESS,   //!< network status thread in progress
    ST_STATUS_RETRY,        //!< network status thread need to retry
    ST_STATUS_EXIT          //!< network status thread has exited 
} NetConnStatusStateE;

//! environment determination async op state
typedef enum NetConnEnvStateE
{
    ST_ENV_INVALID = 0,     //!< environment determination has not started yet
    ST_ENV_INPROGRESS,      //!< environment determination in progress
    ST_ENV_COMPLETED,       //!< environment determination done and valid
} NetConnEnvStateE;

typedef struct NetConnTokenT
{
    NetConnTokenStateE  eTokenStatus;       //!< token acquisition status
    int32_t             iTokenAcquTick;     //!< token acquisition time
    uint8_t             bFailureReported;   //!< whether token acquisition failure was at least reported once to the user
    uint8_t             _pad[3];   
} NetConnTokenT;

typedef struct NetConnUserT
{
    uint64_t                Xuid;
    NetConnTokenT           token;
    NetCritT                asyncOpCrit;    //!< used to protect against concurrent calls to 'tick' from different threads
    char                    strGamertag[16];
    uint8_t                 bCritInitialized;
    uint8_t                 _pad[3];
} NetConnUserT;

//! private module state
typedef struct NetConnRefT
{
    NetConnCommonRefT           Common;                             //!< cross-platform netconn data (must come first!)
    NetConnLocalUserRefT       *pLocalUser;                         //!< local user container for IEAUser
    NetConnUserT                aUsers[NETCONN_MAXLOCALUSERS];
    const EA::User::CapilanoUser *aEAUsers[NETCONN_MAXLOCALUSERS];  //!< EAUsers known by NetConn - populated when pIEAUserEventList is processed

    // event change tokens
    Windows::Foundation::EventRegistrationToken CurrentUserChangedEventToken;
    Windows::Foundation::EventRegistrationToken NetworkStatusChangedEventToken;
    Windows::Foundation::EventRegistrationToken ResumingEventToken;
    Windows::Foundation::EventRegistrationToken SuspendingEventToken;
 
    char                        strAuthUrn[128];             //!< URN used with DirtyAuth
    char                        strSandboxId[NETCONNXBOXONE_STR_SANDBOX_LENGTH]; //!< sandbox id defined in the title storage
    char                        strSandboxNameTest[NETCONNXBOXONE_STR_SANDBOX_LENGTH];  //!< test sandbox name (may be overridden)
    DirtyAuthRefT               *pDirtyAuth;                 //!< dirtyauth ref

    NetConnStateE               eNetConnState;               //!< netconn state
    NetConnRunStateE            eNetConnRunState;            //!< netconn run state
    NetConnStateE               eNetConnStateBeforeSuspd;    //!< netconn state before suspend. This is used to resume to the correct state

    uint32_t                    uConnStatus;                 //!< connection status (surfaced to user)
    uint32_t                    uNetworkStatusRetryCount;    //!< network status retry count
    uint32_t                    uNetworkStatusRetryTimer;    //!< network status retry timer
    HANDLE                      hNetworkStatusEvent;         //!< event used to wake the network status update thread
    volatile HANDLE             hNetworkStatusThread;        //!< handle for the newtwork status thread
    NetCritT                    networkStatusCrit;           //!< to protect the network status state
    NetConnNetworkStatusStateE  eNetConnNetworkStatusState;  //!< netconn status thread state
    Windows::Networking::Connectivity::NetworkConnectivityLevel eThreadConnectivityLevel; //!< network status thread network connectivity output variable
    Windows::Networking::Connectivity::NetworkConnectivityLevel eConnectivityLevel;       //!< current network connectivity

    uint32_t                    uXblWaitTimer;               //!< xblwait timer
    NetConnStateE               eNetConnStateAfterXblWait;   //!< conn state to go to after xbl wait

    NetConnAccountPickerStateE  eActPickerAsyncOpState;      //!< account picker async op state
    IAsyncOperation<AccountPickerResult^> ^ActPickerAsyncOp;
    
    uint32_t                    uEnvTimer;                   //!< environment determination retry timer
    uint32_t                    uPlatEnv;                    //!< one of the NETCONN_PLATENV_* values
    NetConnEnvStateE            eNetConnEnvState;            //!< environment determination state
    IAsyncOperation<TitleStorage::TitleStorageBlobResult^> ^PlatEnvAsyncOp;

    int32_t                     iThreadCpuAffinity;          //!< affinity mask we set for our internal threads

    int32_t                     iIsTrial;                    //!< flag to cache IsTrial value, == 0 :false, >0 :true, <0 : unknown

    uint8_t                     bDelayConn;                  //!< delay connect due to external cleanup
    uint8_t                     bUpdateNetworkStatus;        //!< starts a network status update
    uint8_t                     bShowAccountPicker;          //!< show the account picker if there is no signed-in user
    uint8_t                     bNoLogin;                    //!< skip login and token acquisition process
} NetConnRefT;

/*** Variables ************************************************************************************/
#if defined DIRTYCODE_LOGGING
 static const char *_NetConn_strNetConnState[] =
{
    "ST_IDLE",          //!< NetConnStateE::ST_IDLE = 0
    "ST_RUNNING",       //!< NetConnStateE::ST_RUNNING = 1
    "ST_SUSPEND",       //!< NetConnStateE::ST_SUSPEND = 2
    "ST_RESUME",        //!< NetConnStateE::ST_RESUME = 3
    "ST_FAILED",        //!< NetConnStateE::ST_FAILED = 4
    "ST_CLEANUP",       //!< NetConnStateE::ST_CLEANUP = 5
    "ST_XBLWAIT"        //!< NetConnStateE::ST_XBLWAIT = 6
};

static const char *_NetConn_strNetRunConnState[] =
{
    "ST_RUN_INIT",      //!< NetConnRunStateE::ST_RUN_INIT = 0
    "ST_RUN_LOGIN",     //!< NetConnRunStateE::ST_RUN_LOGIN = 1
    "ST_RUN_TOKEN",     //!< NetConnRunStateE::ST_RUN_TOKEN = 2
    "ST_RUN_ENV",       //!< NetConnRunStateE::ST_RUN_ENV = 3
    "ST_RUN_ONLINE"     //!< NetConnRunStateE::ST_RUN_ONLINE = 4
};
#endif

//! global module ref
static NetConnRefT *_NetConn_pRef = NULL;

/*** Function Prototypes **************************************************************************/
/*** Private Functions ****************************************************************************/
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
#if DIRTYCODE_LOGGING
    NetPrintf(("netconnxboxone: netconn status changed from %C to %C\n", pRef->uConnStatus, uNewConnStatus));
#endif
    pRef->uConnStatus = uNewConnStatus;
}

/*F********************************************************************************/
/*!
    \Function   _NetConnMoveToNextRunState

    \Description
        Transition NetConn in to a new running state

    \Input *pRef             - pointer to net NetConn module ref
    \Input *eNewRunState     - new net conn run state

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnMoveToNextRunState(NetConnRefT *pRef, NetConnRunStateE eNewRunState)
{
#if DIRTYCODE_LOGGING
    NetPrintf(("netconnxboxone: netconn transitioning running state from %s to %s\n", _NetConn_strNetRunConnState[pRef->eNetConnRunState], _NetConn_strNetRunConnState[eNewRunState]));
#endif
    pRef->eNetConnRunState = eNewRunState;
}

/*F********************************************************************************/
/*!
    \Function   _NetConnMoveToNextState

    \Description
        Transition NetConn in to a new state

    \Input *pRef             - pointer to net NetConn module ref
    \Input eNewState         - new net conn state

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnMoveToNextState(NetConnRefT *pRef, NetConnStateE eNewState)
{
#if DIRTYCODE_LOGGING
    NetPrintf(("netconnxboxone: netconn transitioning state from %s to %s\n", _NetConn_strNetConnState[pRef->eNetConnState], _NetConn_strNetConnState[eNewState]));
#endif
    pRef->eNetConnState = eNewState;
}

/*F********************************************************************************/
/*!
    \Function _NetConnShowAccountPicker

    \Description
        Shows MS Account Picker Dialog

    \Input *pRef    - state ref

    \Version 27/02/2014 (tcho)
*/
/********************************************************************************F*/
static void _NetConnShowAccountPicker(NetConnRefT *pRef)
{
    try
    {
        // opening the account picker
        NetPrintf(("netconnxboxone: account picker opened\n"));
        pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_OPEN;
        pRef->ActPickerAsyncOp = SystemUI::ShowAccountPickerAsync(nullptr, Windows::Xbox::UI::AccountPickerOptions::AllowGuests);
        pRef->ActPickerAsyncOp->Completed = ref new AsyncOperationCompletedHandler<AccountPickerResult^>
        ([pRef](IAsyncOperation<AccountPickerResult^>^ refAsyncOp, Windows::Foundation::AsyncStatus status)
        {
            switch(status)
            {
                case Windows::Foundation::AsyncStatus::Error:
                {
                    NetPrintf(("netconnxboxone: [%p] _NetConnShowAccountPicker() An error occured in call to ShowAccountPickerAsync(), ErrorCode=0x%08x\n", pRef, refAsyncOp->ErrorCode));
                    break;
                }
                default:
                    break;
            }

            pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_CLOSE;
            NetPrintf(("netconnnxboxconn: account picker closed\n"));
        });
    }
    catch(Exception ^ e)
    {
        NetPrintf(("netconnxboxone: exception caught while trying open the account picker dialog (%S/0x%08x)\n", e->Message->Data(), e->HResult));
        pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_CLOSE;
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnProcessNetworkStatus

    \Description
        Process the NetConn netowrk status updates

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnProcessNetworkStatus(NetConnRefT * pRef)
{
    NetConnNetworkStatusStateE eTempNetworkStatusState;

    NetCritEnter(&pRef->networkStatusCrit);
    eTempNetworkStatusState = pRef->eNetConnNetworkStatusState;
    NetCritLeave(&pRef->networkStatusCrit);

    // save the result of the network status thread
    if (eTempNetworkStatusState == NetConnNetworkStatusStateE::ST_STATUS_VALID)
    {
        pRef->eConnectivityLevel = pRef->eThreadConnectivityLevel;
    }

    if (pRef->bUpdateNetworkStatus)
    {
        // if the network status thread is not in progress proceed
        if (eTempNetworkStatusState == NetConnNetworkStatusStateE::ST_STATUS_VALID || eTempNetworkStatusState == NetConnNetworkStatusStateE::ST_STATUS_INVALID)
        {
            // wake the network status thread
            SetEvent(pRef->hNetworkStatusEvent);
            pRef->bUpdateNetworkStatus = FALSE;
            pRef->uNetworkStatusRetryCount = 0;
            pRef->uNetworkStatusRetryTimer = 0;
        }
    }
    // MS requirement
    // In the case of network connectivity level check throwing exceptions we have been advised by 
    // MS to retry the check for a couple times before saying connectivity is lost.
    else if (pRef->eNetConnNetworkStatusState ==  NetConnNetworkStatusStateE::ST_STATUS_RETRY)
    {
        uint32_t uCurTime = NetTick();

        if (pRef->uNetworkStatusRetryCount >= NETCONN_MAX_NETWORK_STATUS_RETRIES)
        {
            // reach the maximum number of retries 
            // we will consider network conectivity lost
            pRef->uNetworkStatusRetryCount = 0;
            pRef->uNetworkStatusRetryTimer = 0;
            pRef->uNetworkStatusRetryTimer = 0;
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-net');

        }
        else if ((NetTickDiff(uCurTime, pRef->uNetworkStatusRetryTimer) > NETCONN_NETWORK_STATUS_RETRY_TIME_MS))
        {
            if (eTempNetworkStatusState != NetConnNetworkStatusStateE::ST_STATUS_INPROGRESS)
            {
                NetPrintf(("netconnxboxone: attempting to update network connectivity level\n"));

                // wake the network status thread
                SetEvent(pRef->hNetworkStatusEvent);
                pRef->uNetworkStatusRetryTimer = uCurTime;
                ++(pRef->uNetworkStatusRetryCount);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetworkStatusThread

    \Description
        Transition NetConn in to a new state

    \Input *pUserData             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetworkStatusThread(void *pUserData)
{
    NetConnRefT *pRef = (NetConnRefT *)pUserData;

    #if DIRTYCODE_LOGGING
    const char *_strConnectivityLevel[] =
    {
        "None",                         // Windows::Networking::Connectivity::NetworkConnectivityLevel::None                        == 0
        "LocalAccess",                  // Windows::Networking::Connectivity::NetworkConnectivityLevel::LocalAccess                 == 1
        "ConstrainedInternetAccess",    // Windows::Networking::Connectivity::NetworkConnectivityLevel::ConstrainedInternetAccess   == 2
        "InternetAccess",               // Windows::Networking::Connectivity::NetworkConnectivityLevel::InternetAccess              == 3
        "XboxLiveAccess"                // Windows::Networking::Connectivity::NetworkConnectivityLevel::XboxLiveAccess              == 4
    };
    #endif

    // network status thread started
    NetPrintf(("netconnxboxone: network status thread started (thid=%d)\n", GetCurrentThreadId()));

    // assign the handle to the thread identifier
    pRef->hNetworkStatusThread = GetCurrentThread();

    while (pRef->hNetworkStatusThread != 0)
    {
        // wait until we wake up or perform a check for the first time
        if (WaitForSingleObject(pRef->hNetworkStatusEvent, INFINITE) == WAIT_OBJECT_0)
        {
            uint8_t bRetry = FALSE;
            NetConnNetworkStatusStateE eTempNetworkStatusState;

            ResetEvent(pRef->hNetworkStatusEvent);

            NetCritEnter(&pRef->networkStatusCrit);
            eTempNetworkStatusState = pRef->eNetConnNetworkStatusState;
            NetCritLeave(&pRef->networkStatusCrit);

            eTempNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_INPROGRESS; 
            NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnxboxone: network status thread woke up, attempting to update network connectivity level (thid=%d)\n", GetCurrentThreadId()));

            // if we are shutting down we can ignore the connectivity check
            if (pRef->hNetworkStatusThread == 0)
            {
                NetPrintf(("netconnxboxone: network status thread is shutting down(thid=%d)\n", GetCurrentThreadId()));
                continue;
            }

            ConnectionProfile^ refConnectionProfile;
            Windows::Networking::Connectivity::NetworkConnectivityLevel eCurrentConnectivityLevel = Windows::Networking::Connectivity::NetworkConnectivityLevel::None;
            try
            {
                refConnectionProfile = Windows::Networking::Connectivity::NetworkInformation::GetInternetConnectionProfile();
                
                // getInternetConnectionProfile() can actually return a nullptr. if it does, assume no internet connection (as per sample code)
                if (refConnectionProfile != nullptr)
                {
                   eCurrentConnectivityLevel = refConnectionProfile->GetNetworkConnectivityLevel();
                }
                else
                {
                   NetPrintf(("netconnxboxone: null internet connection profile (thid=%d)\n", GetCurrentThreadId()));
                   eCurrentConnectivityLevel = Windows::Networking::Connectivity::NetworkConnectivityLevel::None;
                }
            }
            catch (Exception ^ e)
            {
                NetPrintf(("netconnxboxone: exception caught while trying to update the connectivity level (%S/0x%08x) (thid=%d)\n", e->Message->Data(), e->HResult, GetCurrentThreadId()));
                eTempNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_RETRY;
                bRetry = TRUE;
            }
            
            if (bRetry == FALSE)
            {
                if (eCurrentConnectivityLevel != pRef->eConnectivityLevel)
                {
                    NetPrintf(("netconnxboxone: change in connectivity level detected\n"));
                    pRef->eThreadConnectivityLevel = eCurrentConnectivityLevel;

                    // refresh address
                    NetConnStatus('addr', 1, NULL, 0);
                    #if DIRTYCODE_LOGGING
                    // display address
                    NetPrintf(("netconnxboxone: connectivity level %s (%d) addr=%a (thid=%d)\n", _strConnectivityLevel[(int)pRef->eThreadConnectivityLevel], pRef->eThreadConnectivityLevel, NetConnStatus('addr', 0, NULL, 0), GetCurrentThreadId()));
                    #endif
                }

                eTempNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_VALID;
            }
            
            NetCritEnter(&pRef->networkStatusCrit);
            pRef->eNetConnNetworkStatusState = eTempNetworkStatusState;
            NetCritLeave(&pRef->networkStatusCrit); 
        }
    }

    // we are exiting the thread
    NetPrintf(("netconnxboxone: network status thread exit\n"));
    NetCritEnter(&pRef->networkStatusCrit);
    pRef->eNetConnNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_EXIT;
    NetCritLeave(&pRef->networkStatusCrit);
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetEnvironment

    \Description
        Determine environment using sandbox Id and it will TMS as a backup

    \Input *pRef        - netconn module state

    \Version 06/24/2013 (mcorcoran)
*/
/********************************************************************************F*/
static void _NetConnGetEnvironment(NetConnRefT *pRef)
{
    XboxLiveContext^ context = nullptr;
    int32_t iIndex;

    // set to invalid environment and pending state
    pRef->uPlatEnv = (unsigned)-1;
    pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_INPROGRESS;
    

    // Query for the SandboxId using the XboxLiveConfiguration
    // If the environment is an official Microsoft sandbox that maps directly to an environment then return it
    // Otherwise this is a custom EA sandbox so you need to query to see which environment we are on using for this title
    wcstombs(pRef->strSandboxId, Windows::Xbox::Services::XboxLiveConfiguration::SandboxId->Data(), sizeof(pRef->strSandboxId));
    if ((ds_stricmp(pRef->strSandboxId, NETCONNXBOXONE_RETAIL_SANDBOX) == 0) || (ds_stricmp(pRef->strSandboxId, NETCONNXBOXONE_EAACCESS_SANDBOX) == 0))
    {
        pRef->uPlatEnv = NETCONN_PLATENV_PROD;
        NetPrintf(("netconnxboxone: [%p] Direct Sandbox to Environment mapping found : SandboxId='%s' Environment='PROD'\n", pRef, pRef->strSandboxId));
    }
    else if ((ds_stricmp(pRef->strSandboxId, NETCONNXBOXONE_CERT_SANDBOX) == 0) || (ds_stricmp(pRef->strSandboxId, NETCONNXBOXONE_CERTDEBUG_SANDBOX) == 0) || (ds_stricmp(pRef->strSandboxId, NETCONNXBOXONE_INTERNAL_CERT_SANDBOX) == 0))
    {
        pRef->uPlatEnv = NETCONN_PLATENV_CERT;
        NetPrintf(("netconnxboxone: [%p] Direct Sandbox to Environment mapping found : SandboxId='%s' Environment='CERT'\n", pRef, pRef->strSandboxId));
    }
    else if (ds_stricmp(pRef->strSandboxId, pRef->strSandboxNameTest) == 0)
    {
        pRef->uPlatEnv = NETCONN_PLATENV_TEST;
        NetPrintf(("netconnxboxone: [%p] Direct Sandbox to Environment mapping found : SandboxId='%s' Environment='TEST'\n", pRef, pRef->strSandboxId));
    }

    // Have our environment mapping don't bother doing the TMS lookup
    if (pRef->uPlatEnv != (unsigned)-1)
    {
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
        return;
    }

    // get any signed in user to make the TMS call below
    iIndex = NetConnQuery(NULL, NULL, 0);
    if (iIndex < 0)
    {
        // This should really never happen.  The user would have to call
        // NetConnConnect() with a user signed in (otherwise we wouldn't have
        // gotten here), and then within one frame/idle(), signed out.
        NetPrintf(("netconnxboxone: no user is signed in\n"));
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
        _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
        _NetConnUpdateConnStatus(pRef, '-act');
        return;
    }

    try
    {
        context = ref new XboxLiveContext(pRef->aEAUsers[iIndex]->GetFirstPartyObject());
    }
    catch (Exception ^ e)
    {
        NetPrintf(("netconnxboxone: (Error) unable to instantiate XboxLiveContext (%S/0x%08x)\n", e->Message->Data(), e->HResult));
        NetPrintf((
            "netconnxboxone: WARNING!!! Microsoft.Xbox.Services.dll is likely not loaded. Please add "
            "a reference to 'XBox Services API' in your top-level project file to make this feature useable.\n"));
        pRef->uPlatEnv = NETCONN_PLATENV_PROD; // default to prod
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
        return;
    }

    TimeSpan ts;
    ts.Duration = 60000000; // 6 seconds
    context->Settings->HttpTimeoutWindow = ts; // The total window for the request(s) to execute
    ts.Duration = 30000000; // 3 seconds
    context->Settings->HttpTimeout = ts; // The timeout for a single request
    ts.Duration = 0; // 0 seconds
    context->Settings->HttpRetryDelay = ts; // the delay, after a request fails before the next request.

#if DIRTYCODE_LOGGING
    context->Settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Verbose;
    context->Settings->EnableServiceCallRoutedEvents = true;

    context->Settings->ServiceCallRouted += ref new EventHandler<XboxServiceCallRoutedEventArgs^>(
        [=](Platform::Object^, XboxServiceCallRoutedEventArgs^ args)
        {
            // Display HTTP errors to screen for easy debugging
            NetPrintf(("netconnxboxone: [XboxLiveServices] %S %S\n", args->HttpMethod->Data(), args->Url->AbsoluteUri->Data()));
            NetPrintf(("netconnxboxone: [XboxLiveServices] %S\n", args->RequestHeaders->Data()));
            NetPrintf(("netconnxboxone: [XboxLiveServices] \n"));
            NetPrintf(("netconnxboxone: [XboxLiveServices] %S\n", args->RequestBody->RequestMessageString->Data()));
            NetPrintf(("netconnxboxone: [XboxLiveServices] Response Status: %S\n", args->HttpStatus.ToString()->Data()));
            NetPrintf(("netconnxboxone: [XboxLiveServices] %S\n", args->ResponseHeaders->Data()));
            NetPrintf(("netconnxboxone: [XboxLiveServices] \n"));
            NetPrintf(("netconnxboxone: [XboxLiveServices] %S\n", args->ResponseBody->Data()));
        });
#endif

    TitleStorage::TitleStorageBlobMetadata^ metadata = nullptr;

    try
    {
        metadata = ref new TitleStorage::TitleStorageBlobMetadata(NETCONNXBOXONE_GLOBAL_EA_TMS_SCID, TitleStorage::TitleStorageType::GlobalStorage, NETCONNXBOXONE_GLOBAL_EA_ENV_FILE, TitleStorage::TitleStorageBlobType::Json, context->User->XboxUserId);
    }
    catch (Exception ^refException)
    {
        NetPrintf(("netconnxboxone: exception thrown when creating TitleStorageBlobMetadata (%S/0x%08x)\n", refException->Message->Data(), refException->HResult));
        pRef->uPlatEnv = NETCONN_PLATENV_PROD;
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
        return;
    }

    Windows::Storage::Streams::Buffer^ buffer = ref new Windows::Storage::Streams::Buffer(1024);

    try
    {
        pRef->PlatEnvAsyncOp = context->TitleStorageService->DownloadBlobAsync(metadata, buffer, TitleStorage::TitleStorageETagMatchCondition::NotUsed, "");
    }
    catch (Exception ^ e)
    {
        NetPrintf(("netconnxboxone: (Error) exception thrown when calling DownloadBlobAsync() (%S/0x%08x)\n", e->Message->Data(), e->HResult));
        pRef->uPlatEnv = NETCONN_PLATENV_PROD; // default to prod
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
        return;
    }

    pRef->PlatEnvAsyncOp->Completed = ref new AsyncOperationCompletedHandler<TitleStorage::TitleStorageBlobResult^>
        ([pRef](IAsyncOperation<TitleStorage::TitleStorageBlobResult^>^ refAsyncOp, Windows::Foundation::AsyncStatus status)
        {
            pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;

            switch (status)
            {
                case Windows::Foundation::AsyncStatus::Completed:
                {
                    uint32_t uPlatEnv = NETCONN_PLATENV_PROD; // default to prod

                    TitleStorage::TitleStorageBlobResult^ result = nullptr;
                    try
                    {
                        result = refAsyncOp->GetResults();
                    }
                    catch (Platform::Exception ^e)
                    {
                        NetPrintf(("netconnxboxone: _NetConnGetEnvironment() Exception thrown (%S/0x%08x)\n", e->Message->Data(), e->HResult));
                    }

                    if (result != nullptr)
                    {
                        Microsoft::WRL::ComPtr<IUnknown> pBuffer((IUnknown*)result->BlobBuffer);
                        Microsoft::WRL::ComPtr<IBufferByteAccess> pBufferByteAccess;
                        pBuffer.As(&pBufferByteAccess);

                        // get pointer to data
                        byte* pData = nullptr;
                        if (SUCCEEDED(pBufferByteAccess->Buffer(&pData)))
                        {
                            uint16_t aJsonList[2048];
                            if (JsonParse(aJsonList, sizeof(aJsonList), (char*)pData, result->BlobBuffer->Length) >= 0)
                            {
                                char strEnvironment[32] = "";
                                const char *pEnvironment = JsonFind(aJsonList, "environmentInfo.environment");
                                if (pEnvironment)
                                {
                                    JsonGetString(pEnvironment, strEnvironment, sizeof(strEnvironment), "");

                                    if (ds_stricmp(strEnvironment, "SDEV") == 0)
                                    {
                                        uPlatEnv = NETCONN_PLATENV_DEV;
                                    }
                                    else if (ds_stricmp(strEnvironment, "STEST") == 0)
                                    {
                                        uPlatEnv = NETCONN_PLATENV_TEST;
                                    }
                                    else if (ds_stricmp(strEnvironment, "SCERT") == 0)
                                    {
                                        uPlatEnv = NETCONN_PLATENV_CERT;
                                    }
                                    else if (ds_stricmp(strEnvironment, "PROD") == 0)
                                    {
                                        uPlatEnv = NETCONN_PLATENV_PROD;
                                    }
                                }

                                NetPrintf(("netconnxboxone: [%p] " NETCONNXBOXONE_GLOBAL_EA_ENV_FILE " retrieved from TMS reports: SandboxId='%s', Environment='%s'\n", pRef, pRef->strSandboxId, strEnvironment));
                            }
                            else
                            {
                                NetPrintf(("netconnxboxone: [%p] _NetConnGetEnvironment() failed to parse json response\n", pRef));
                            }
                        }
                        else
                        {
                            // this should pretty much never happen
                            NetPrintf(("netconnxboxone: [%p] _NetConnGetEnvironment() failed to obtain pointer to internal data for BlobBuffer\n", pRef));
                        }
                    }

                    // assign platform environment
                    pRef->uPlatEnv = uPlatEnv;
                    break;
                }
                case Windows::Foundation::AsyncStatus::Error:
                {
                    NetPrintf(("netconnxboxone: _NetConnGetEnvironment() An error occured in call to DownloadBlobAsync(), ErrorCode=0x%08x.\n", refAsyncOp->ErrorCode));

                    if ((pRef->uEnvTimer == 0) || (NetTickDiff(NetTick(), pRef->uEnvTimer) < NETCONNXBOXONE_ENV_TIMEOUT))
                    {
                        if ((pRef->uEnvTimer == 0))
                        {
                            pRef->uEnvTimer = NetTick();
                        }
                        
                        if ((pRef->eNetConnState == NetConnStateE::ST_RUNNING) && (pRef->uConnStatus >> 24 == '~') && (pRef->eNetConnRunState == NetConnRunStateE::ST_RUN_ENV))
                        {
                            NetConnSleep(1000);                      //Retry every 1 second if still in a connecting state
                            pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_INVALID;
                            NetPrintf(("netconnxboxone: retrying environment determination.\n"));
                        }
                        break;
                    }
                    else
                    {            
                        pRef->uEnvTimer = 0; //Reseting the env timeout 
                        if (pRef->eConnectivityLevel < Windows::Networking::Connectivity::NetworkConnectivityLevel::XboxLiveAccess)
                        {
                            NetPrintf(("netconnxboxone: lost XboxLive connection while determining environment.\n"));
                            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
                            _NetConnUpdateConnStatus(pRef, '-net');
                        }
                        else
                        {
                            pRef->uPlatEnv = NETCONN_PLATENV_PROD; // if an error occurs, just default to prod
                            NetPrintf(("netconnxboxone: environment determination failed. Retry timeout exceeded, defaulting to PROD.\n"));
                        }
                        break;
                    }
                }
                case Windows::Foundation::AsyncStatus::Canceled:
                {
                    // The user may have called NetConnDisconnect() before +onl, in which case, don't modify the value of pRef->uPlatEnv or pRef->strSandboxId
                    NetPrintf(("netconnxboxone: _NetConnGetEnvironment() Canceled.\n"));
                    break;
                }

                case Windows::Foundation::AsyncStatus::Started:
                {
                    NetPrintf(("netconnxboxone: _NetConnGetEnvironment() Started\n"));
                    break;
                }

                default:
                {
                    NetPrintf(("netconnxboxone: _NetConnGetEnvironment() Unhandled AsyncStatus %d\n", status));
                    break;
                }
            }

            // in the case of started, we expect to get another callback so don't kill it yet
            if (status != Windows::Foundation::AsyncStatus::Started)
            {
                pRef->PlatEnvAsyncOp->Close();
                delete pRef->PlatEnvAsyncOp;
                pRef->PlatEnvAsyncOp = nullptr;
            }
        });
}

/*F********************************************************************************/
/*!
    \Function _NetConnIsGetEnvInProgress

    \Description
        Returns TRUE if TitleStorageService->DownloadBlobAsync() is in progress.

    \Input *pRef        - netconn module state
    \Output
            int32_t     - zero=success; -1=try again; other negative=error

    \Version 06/08/2016 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _NetConnIsGetEnvInProgress(NetConnRefT *pRef)
{
    int32_t iResult = 0;   // default to success

    if (pRef->PlatEnvAsyncOp != nullptr)
    {
        iResult = -1;
    }

    return(iResult);
}


/*F********************************************************************************/
/*!
    \Function _NetConnCheckAuthToken

    \Description
        Check if any pending async op for acquisition of Security Token from MS STS
        is completed.

    \Input *pRef            - state ref

    \Version 02/18/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnCheckAuthToken(NetConnRefT *pRef)
{
    int32_t iUserIndex, iResult;
    NetConnTokenT *pTokenSpace;

    for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
    {
        pTokenSpace = &pRef->aUsers[iUserIndex].token;

        if (pTokenSpace->eTokenStatus == ST_TOKEN_INPROG)
        {
            int32_t iTokenLen;

            // make token request
            if ((iResult = DirtyAuthCheckToken(iUserIndex, pRef->strAuthUrn, &iTokenLen, NULL)) == DIRTYAUTH_SUCCESS)
            {
                NetPrintf(("netconnxboxone: token acquisition for user %d completed - token length = %d\n", iUserIndex, iTokenLen));
                pTokenSpace->eTokenStatus = ST_TOKEN_VALID;
            }
            else if (iResult != DIRTYAUTH_PENDING)
            {
                NetPrintf(("netconnxboxone: token acquisition failed for user %d\n", iUserIndex));

                pTokenSpace->eTokenStatus = ST_TOKEN_FAILED;
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnRequestAuthToken

    \Description
        Initiate async op for acquisition of Security Token from MS STS.

    \Input *pRef            - state ref
    \Input iUserIndex       - user index

    \Version 12/01/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _NetConnRequestAuthToken(NetConnRefT *pRef, int32_t iUserIndex)
{
    int32_t iResult;
    NetConnTokenT *pTokenSpace = &pRef->aUsers[iUserIndex].token;

    NetPrintf(("netconnxboxone: acquiring XSTS token for user %d\n", iUserIndex));

    // make token request
    if ((iResult = DirtyAuthGetToken2(iUserIndex, pRef->strAuthUrn, TRUE, DIRTYAUTH_DEFAULT_TOKEN_TIMEOUT)) == DIRTYAUTH_PENDING)
    {
        pTokenSpace->eTokenStatus = ST_TOKEN_INPROG;
    }
    else if (iResult == DIRTYAUTH_SUCCESS)
    {
        NetPrintf(("netconnxboxone: token acquisition for user %d completed (was cached)\n", iUserIndex));

        pTokenSpace->eTokenStatus = ST_TOKEN_VALID;
    }
    else
    {
        NetPrintf(("netconnxboxone: failed to initiate token acquisition for user %d\n", iUserIndex));

        pTokenSpace->eTokenStatus = ST_TOKEN_FAILED;
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnXblUserMask

    \Description
        return the bitmask of logged-in users.

    \Input *pRef    - state ref

    \Output
        uint32_t    - mask of users bits 0-15

    \Version 12/09/2013 (cvienneau)
*/
/********************************************************************************F*/
static uint32_t _NetConnXblUserMask(NetConnRefT *pRef)
{
    uint32_t iIndex;
    uint32_t uUserMask = 0;

    for (iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
    {
        if (pRef->aEAUsers[iIndex] != NULL)
        {
            if (pRef->aEAUsers[iIndex]->IsSignedIn())
            {
                uUserMask |= (1 << iIndex);
            }
        }
    }
    return(uUserMask);
}

/*F********************************************************************************/
/*!
    \Function _NetConnClearUser

    \Description
        Clear the user found at iUserIndex from NetConnUserT array

    \Input *pRef        - NetConnRefT reference
    \Input *iUserIndex  - iUserIndex in NetConnUserT array

    \Version 11/07/2013 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnClearUser(NetConnRefT *pRef, int32_t iUserIndex)
{
    //clear all the other fields
    //we don't memset because we want to be explicit about eTokenStatus value.
    pRef->aUsers[iUserIndex].token.bFailureReported = FALSE;
    pRef->aUsers[iUserIndex].token.eTokenStatus = ST_TOKEN_INVALID;

    //we don't memset because we're going to keep the critical section
    pRef->aUsers[iUserIndex].strGamertag[0] = '\0';

    pRef->aUsers[iUserIndex].Xuid = 0;
}

/*F********************************************************************************/
/*!
    \Function _NetConnInitUser

    \Description
        Init the user found at iUserIndex from NetConnUserT array

    \Input *pRef            - NetConnRefT reference
    \Input *iLocalUserIndex - iUserIndex in NetConnUserT array
    \Input *pEAUser         - the concrete EAUser we use on this platform

    \Output
        int32_t             - 0 for success; negative for failure

    \Version 05/23/2014 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _NetConnInitUser(NetConnRefT *pRef, int32_t iLocalUserIndex, const EA::User::CapilanoUser *pEAUser)
{
    User ^refXboxUser = pEAUser->GetFirstPartyObject();
    int32_t iResult = 0; // default to success

    if (refXboxUser != nullptr)
    {
        const uint64_t uXuid = _wtoi64(refXboxUser->XboxUserId->Data());
        if (pRef->aUsers[iLocalUserIndex].Xuid == 0)
        {
            // fill our internal table at the specified location

            wcstombs(pRef->aUsers[iLocalUserIndex].strGamertag, refXboxUser->DisplayInfo->Gamertag->Data(), sizeof(pRef->aUsers[iLocalUserIndex].strGamertag));
            pRef->aUsers[iLocalUserIndex].Xuid = uXuid;
        }
        // this user is supposed to be unchanged
        else if (pRef->aUsers[iLocalUserIndex].Xuid != uXuid)
        {
            NetPrintf(("netconnxboxone: _NetConnUpdateUserList() detected a critical misalignment when adding new local user 0x%08x\n", pEAUser->GetUserID()));
        }
    }
    else
    {
        iResult = -1;
    }

    return(iResult);
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
    const EA::User::CapilanoUser *pCapilanoUser = static_cast<const EA::User::CapilanoUser *>(pIEAUser);

    if (pRef->aEAUsers[iLocalUserIndex] == NULL)
    {
        if (_NetConnInitUser(pRef, iLocalUserIndex, pCapilanoUser) == 0)
        {
            pRef->aEAUsers[iLocalUserIndex] = pCapilanoUser;
            pRef->aEAUsers[iLocalUserIndex]->AddRef();

            NetPrintf(("netconnxboxone: netconn user added at local user index %d - xuid=0x%llx, gtag=%s, localUserId=0x%08x\n",
                iLocalUserIndex, pRef->aUsers[iLocalUserIndex].Xuid, pRef->aUsers[iLocalUserIndex].strGamertag, pCapilanoUser->GetUserID()));
        }
        else
        {
            NetPrintf(("netconnxboxone: failed to add local user at index %d with local user id 0x%08x\n", iLocalUserIndex, (uint32_t)pCapilanoUser->GetUserID()));
        }
    }
    else
    {
        NetPrintf(("netconnxboxone: failed to add EAUser at index %d with local user id 0x%08x because entry already used with local user id 0x%08x\n",
            iLocalUserIndex, (uint32_t)pCapilanoUser->GetUserID(), (uint32_t)pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryIEAUser

    \Description
        Finds the local user index for the passed in IEAUser UserId

    \Input *pRef    - module reference
    \Input id       - IEAUser UserId that we will use to match the index

    \Output
        int32_t - index of the user, negative=not found
*/
/********************************************************************************F*/
static int32_t _NetConnQueryIEAUser(NetConnRefT *pRef, uint32_t id)
{
    int32_t iResult = -1;
    int32_t i = 0;

    for (; i < NETCONN_MAXLOCALUSERS; i++)
    {
        if (pRef->aEAUsers[i] != NULL &&
            (uint32_t)pRef->aEAUsers[i]->GetUserID() == id)
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
                NetPrintf(("netconnxboxone: netconn user removed at local user index %d - xuid=0x%llx, gtag=%s, localUserId=0x%08x\n",
                iLocalUserIndex, pRef->aUsers[iLocalUserIndex].Xuid, pRef->aUsers[iLocalUserIndex].strGamertag, pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
                _NetConnClearUser(pRef, iLocalUserIndex);

                pRef->aEAUsers[iLocalUserIndex]->Release();
                pRef->aEAUsers[iLocalUserIndex] = NULL;
            }
        else
        {
            NetPrintf(("netconnxboxone: failed to remove local user at index %d - local user ids do not match (0x%08x vs 0x%08x)\n",
                iLocalUserIndex, (uint32_t)pIEAUser->GetUserID(), (uint32_t)pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
        }
    }
    else
    {
        NetPrintf(("netconnxboxone: failed to remove IEAUSER at index %d - no local user at that spot\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunInitState

    \Description
        Process the NetConn Run Init state.
        This state resets variables to prep netconn for the connect process

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunInitState(NetConnRefT * pRef)
{
    // reset variables needed for the connect process
    pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_IDLE;
    pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_INVALID;
    pRef->uConnStatus = 0;
    pRef->uEnvTimer = 0;

    // ready to move into the next state
    _NetConnMoveToNextRunState(pRef, NetConnRunStateE::ST_RUN_LOGIN);
    _NetConnUpdateConnStatus(pRef, '~act');
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunLoginState

    \Description
        Process the NetConn Run Login state
        Determine if we need to sign in with first party or not

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunLoginState(NetConnRefT * pRef)
{
    int32_t iUserIndex = NetConnQuery(NULL, NULL, 0);
 
    if (pRef->eActPickerAsyncOpState == NetConnAccountPickerStateE::ST_ACTPICK_IDLE && !pRef->bShowAccountPicker)
    {
        pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_CLOSE;
    }
    else if (pRef->eActPickerAsyncOpState == NetConnAccountPickerStateE::ST_ACTPICK_IDLE && pRef->bShowAccountPicker)
    {
        if (iUserIndex < 0)
        {
                _NetConnShowAccountPicker(pRef);            
        }
        else
        {
            // There is a signed in user
            pRef->eActPickerAsyncOpState = NetConnAccountPickerStateE::ST_ACTPICK_CLOSE;
        }
    }  

    if (pRef->eActPickerAsyncOpState == NetConnAccountPickerStateE::ST_ACTPICK_CLOSE)
    {
        if (iUserIndex < 0)
        {
            // if we still don't have a user logged in we fail
            NetPrintf(("netconnxboxone: no user signed in\n"));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-act'); 
        }
        else
        {
            // signed in user found, transition to token state
            _NetConnMoveToNextRunState(pRef, NetConnRunStateE::ST_RUN_TOKEN);
            _NetConnUpdateConnStatus(pRef, '~tok');
        }
    }

}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunTokenState

    \Description
        Process the NetConn Token state
        Creates the DirtyAuth module for Token Aquisition

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunTokenState(NetConnRefT * pRef)
{
    // is any user signed in
    if (NetConnQuery(NULL, NULL, 0) >= 0)
    {
        if ((pRef->pDirtyAuth == NULL) && ((pRef->pDirtyAuth = DirtyAuthCreate(TRUE)) == NULL))
        {
            NetPrintf(("netconnxboxone: error creating DirtyAuth\n"));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-xht');
        }
        else
        {
            // transition to running env state
            _NetConnMoveToNextRunState(pRef, NetConnRunStateE::ST_RUN_ENV);
            _NetConnUpdateConnStatus(pRef, '~env');
        }
    }
    else
    {
        NetPrintf(("netconnxboxone: no user signed in\n"));
        _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
        _NetConnUpdateConnStatus(pRef, '-act');
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunEnvState

    \Description
        Process the NetConn Env state
        Starts the Environment Determination process

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunEnvState(NetConnRefT * pRef)
{
    if (pRef->eNetConnEnvState == NetConnEnvStateE::ST_ENV_INVALID)
    {
        // start the environment determination 
        try
        {
            _NetConnGetEnvironment(pRef);
        }
        catch (Exception ^ e)
        {
            // DICE reported that other exceptions are possible above the ones that are caught inside of _NetConnGetEnvironment.
            // This is to catch all exceptions and cleanup approiately
            NetPrintf(("netconnxboxone: (Error) unexpected exception thrown when fetching environment (%S/0x%08x).\n", e->Message->Data(), e->HResult));
            pRef->uPlatEnv = NETCONN_PLATENV_PROD; // default to prod
            ds_strnzcpy(pRef->strSandboxId, NETCONNXBOXONE_DEFAULT_SANDBOX, sizeof(pRef->strSandboxId));
            if (pRef->PlatEnvAsyncOp)
            {
                pRef->PlatEnvAsyncOp->Cancel();
                pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_COMPLETED;
            }
        }
    }
    // poll the environment variable to check for completion
    else if (pRef->eNetConnEnvState == NetConnEnvStateE::ST_ENV_COMPLETED)
    {
        // if completed transition to online state
        if (pRef->uPlatEnv != (unsigned)-1)
        {
            _NetConnMoveToNextRunState(pRef, NetConnRunStateE::ST_RUN_ONLINE);
            _NetConnUpdateConnStatus(pRef, '+onl');
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunOnlineState

    \Description
        Process the NetConn Online state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunOnlineState(NetConnRefT * pRef)
{
    // check auth token
    _NetConnCheckAuthToken(pRef);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnCleanupState

    \Description
        Process the NetConn cleanup state

    \Input *pRef             - pointer to net NetConn module ref
    \Input bCleanupComplete  - TRUE if external cleanup list is empty, FALSE otherwise

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnCleanupState(NetConnRefT * pRef, uint32_t bCleanupComplete)
{
    // check if external cleanup is done
    if (bCleanupComplete)
    {
        if (pRef->bDelayConn)
        {
            pRef->bDelayConn = FALSE;
            pRef->uXblWaitTimer = 0;
            pRef->eNetConnStateAfterXblWait = NetConnStateE::ST_RUNNING;
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_XBLWAIT);
        }
        else
        {
            // cleanup is done we can go back to idle
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_IDLE);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnIdleState

    \Description
        Process the NetConn Idle state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnIdleState(NetConnRefT * pRef)
{
}

/*F********************************************************************************/
/*!
    \Function   _NetConnSuspendState

    \Description
        Process the NetConn Suspended state. Note this is called when we get a
        suspending event. 


    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnSuspendState(NetConnRefT * pRef)
{
}

/*F********************************************************************************/
/*!
    \Function   _NetConnResumeState

    \Description
        Process the NetConn Resume state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnResumeState(NetConnRefT * pRef)
{
    // when we reach xbox live access we will resume back to the state before we suspended
    NetPrintf(("netconnxboxone: Xbox live access reached! Exiting out of the resuming state. Resuming back to the %s state\n", _NetConn_strNetConnState[pRef->eNetConnStateBeforeSuspd]));

    _NetConnMoveToNextState(pRef, pRef->eNetConnStateBeforeSuspd);
    
    // if we resume back into the running env state we should restart the env determination flow
    // if we dont do this there a potential to get stuck waiting the TMS callback
    if (pRef->eNetConnRunState == NetConnRunStateE::ST_RUN_ENV)
    {
        pRef->eNetConnEnvState = NetConnEnvStateE::ST_ENV_INVALID;
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnXblWaitState

    \Description
        Process the NetConn XblWait state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 03/31/2016 (tcho)
*/
/********************************************************************************F*/
static void _NetConnXblWaitState(NetConnRefT * pRef)
{
    if ((pRef->eNetConnStateAfterXblWait != NetConnStateE::ST_RESUME) && (pRef->eNetConnStateAfterXblWait != NetConnStateE::ST_RUNNING))
    {
        // invalid state jump after a xbl wait
        NetPrintf(("netconnxboxone: invalid state transition from %s to %s.\n", _NetConn_strNetConnState[ST_XBLWAIT], _NetConn_strNetConnState[pRef->eNetConnStateAfterXblWait]));
        _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
        _NetConnUpdateConnStatus(pRef, '-sta');
        return;
    }

    /* 
    MS confirmed a platform bug:  after a game is started or resumed, there is a small window during which the connectivity level can't be queried reliably.
    To work around the issue, MS suggested that we should not query the connectivity level before the NetworkStatusChanged event is notified by the system.
    If this notification does not come within a 5 sec window, the connectivity level ConnectivityLevel::None should be assumed.
    */

    // intialize the timer for first time
    if (pRef->uXblWaitTimer == 0)
    {
        pRef->uXblWaitTimer = NetTick();
        return;
    }

    // check network connectivity level
    if (NetConnStatus('xcon', 0, NULL, 0))
    {
        _NetConnMoveToNextState(pRef, pRef->eNetConnStateAfterXblWait);
        if (pRef->eNetConnStateAfterXblWait == NetConnStateE::ST_RUNNING)
        {
            _NetConnMoveToNextRunState(pRef, NetConnRunStateE::ST_RUN_INIT);
        }
    }
    else if (NetTickDiff(NetTick(), pRef->uXblWaitTimer) >= NETCONN_MAX_WAIT_TIME_FOR_XBLWAIT)
    {
        // we did not reach xbox live access within the timeout period so we assume we lost network connectivity
        NetPrintf(("netconnxboxone: network connectivity is lost %s.\n", (pRef->eNetConnStateAfterXblWait == NetConnStateE::ST_RESUME)? "after resuming" : ""));
        _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
        _NetConnUpdateConnStatus(pRef, '-net');
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnFailedState

    \Description
        Process the NetConn Failed state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnFailedState(NetConnRefT * pRef)
{
}

/*F********************************************************************************/
/*!
    \Function _NetConnRegisterNetworkStatusChange

    \Description
        Register network status change event

    \Input *pRef   - pointer to NetConn module ref

    \Version 1/21/2020 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRegisterNetworkStatusChange(NetConnRefT *pRef)
{
    if (!pRef->NetworkStatusChangedEventToken.Value)
    {
        try
        {
            NetPrintf(("netconnxboxone: adding network status changed event handler \n"));
            pRef->NetworkStatusChangedEventToken = (NetworkInformation::NetworkStatusChanged += ref new NetworkStatusChangedEventHandler(
                [=](Platform::Object ^refObj)
            {
                // during suspend/resume, it is expected that you will see multiple events being fired where the connection profile will switch from None to XboxLiveAccess. 
                // however, the NetworkStatusChanged event can also fire if any properties on the NetworkInformation object change underneath. 
                // so having the event fire multiple times without actually changing the connectivity level is expected.
                // from the forums people have experience this event firing 45 times a second with no actual changes in connectivity.
                NetPrintfVerbose((pRef->Common.iDebugLevel, 1, "netconnxboxone: NetworkStatusChangedEvent triggered. tick=%d\n", NetTick()));
                pRef->bUpdateNetworkStatus = TRUE;
            }));
        }
        catch (Exception ^ e)
        {
            NetPrintf(("netconnxboxone: failed to register NetworkStatusChanged event (err=%s)\n", DirtyErrGetName(e->HResult)));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRunningState

    \Description
        Process the NetConn Running state

    \Input *pRef             - pointer to net NetConn module ref

    \Version 09/21/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnRunningState(NetConnRefT * pRef)
{
    NetConnNetworkStatusStateE eTempNetworkStatusState;

    NetCritEnter(&pRef->networkStatusCrit);
    eTempNetworkStatusState = pRef->eNetConnNetworkStatusState;
    NetCritLeave(&pRef->networkStatusCrit);

    // defer the check if the network connectivity level is not valid (this is for the case of a cable pull)
    if (eTempNetworkStatusState == NetConnNetworkStatusStateE::ST_STATUS_VALID)
    {
        if (!NetConnStatus('xcon', 0, NULL, 0))
        {
            // connectivity level not at xbox live access
            NetPrintf(("netconnxboxone: network connectivity is lost.\n"));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-net');
            return;
        }
    }
    else
    {
        // defer processing the running state unitl we have a valid connectivity level
        return;
    }

    switch(pRef->eNetConnRunState)
    {
        case NetConnRunStateE::ST_RUN_INIT:
            _NetConnRunInitState(pRef);
            break;
        
        case NetConnRunStateE::ST_RUN_LOGIN:
            _NetConnRunLoginState(pRef);
            break;
        
        case NetConnRunStateE::ST_RUN_TOKEN:
            _NetConnRunTokenState(pRef);
            break;
        case NetConnRunStateE::ST_RUN_ENV:
            _NetConnRunEnvState(pRef);
            break;
    
        case NetConnRunStateE::ST_RUN_ONLINE:
            _NetConnRunOnlineState(pRef);
            break;
    
        default:
            // in theory we should never get here
            NetPrintf(("netconnxboxone: encountered an unknown running state"));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-sta');
            break;   
    }
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdate

    \Description
        Update status of NetConn module.  This function is called by NetConnIdle.

    \Input *pData   - pointer to NetConn module ref
    \Input uTick    - current tick count

    \Version 12/13/2012 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnUpdate(void *pData, uint32_t uTick)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iExternalCleanupListCount;

    // register network status change handler (this is incase the registration threw an exception)
    _NetConnRegisterNetworkStatusChange(pRef);

    // process the network status updates
    _NetConnProcessNetworkStatus(pRef);

    // update local users list
    NetConnLocalUserUpdate(pRef->pLocalUser);

    // process the cleanup list
    iExternalCleanupListCount = NetConnCommonProcessExternalCleanupList(&pRef->Common);

    // check to see if we have any users login to first party (only need to do this when we resume or we are in a online state because of the possibility of signing in with the account picker)
    if ((pRef->eNetConnState == NetConnStateE::ST_RESUME) || ((pRef->eNetConnState == NetConnStateE::ST_RUNNING) && (pRef->eNetConnRunState == NetConnRunStateE::ST_RUN_ONLINE)))
    {
        if (NetConnQuery(NULL, NULL, 0) < 0)
        {
            NetPrintf(("netconnxboxone: no user signed in\n"));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-act');
        }
    }

    // process netconn states
    switch(pRef->eNetConnState)
    {
        case NetConnStateE::ST_IDLE:
            _NetConnIdleState(pRef);
            break;
    
        case NetConnStateE::ST_RUNNING:
            _NetConnRunningState(pRef);
            break;
        
        case NetConnStateE::ST_RESUME:
            _NetConnResumeState(pRef);
            break;
    
        case NetConnStateE::ST_FAILED:
            _NetConnFailedState(pRef);
            break;
    
         case NetConnStateE::ST_SUSPEND:
            // do not need to do any processing as program is suspended
            // actual processing is done at the suspending event
            break;
         
         case NetConnStateE::ST_CLEANUP:
             _NetConnCleanupState(pRef, ((iExternalCleanupListCount == 0)?TRUE:FALSE));
             break;

         case NetConnStateE::ST_XBLWAIT:
             _NetConnXblWaitState(pRef);
             break;

         default:
            // in theory we should never get here
            NetPrintf(("netconnxboxone: encountered an unknown state (%d)\n", pRef->eNetConnState));
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_FAILED);
            _NetConnUpdateConnStatus(pRef, '-sta');
            break;
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetConnShutdownInternal

    \Description
        Shutdown the network code and return to idle state for internal use

    \Input  pRef       - netconn ref
    \Input  bStopEE    - ignored in xbox one implementation

    \Output
        int32_t        - negative=error, else zero

    \Version 1/13/2020 (tcho)
*/
/********************************************************************************F*/
int32_t _NetConnShutdownInternal(NetConnRefT *pRef, uint32_t bStopEE)
{
    int32_t iExternalCleanupListCnt, iResult = 0;

    // decrement and check the refcount
    if ((iResult = NetConnCommonCheckRef((NetConnCommonRefT*)pRef)) < 0)
    {
        return(iResult);
    }

    if (_NetConn_pRef != NULL)
    {
        // disconnect network interfaces
        NetConnDisconnect();
    }

    // make sure the external cleanup list is empty before proceeeding
    if ((iExternalCleanupListCnt = NetConnCommonProcessExternalCleanupList(&pRef->Common)) != 0)
    {
        #if DIRTYCODE_LOGGING
        static uint32_t uLastTimeExtCleanup = 0;
        if (!uLastTimeExtCleanup || NetTickDiff(NetTick(), uLastTimeExtCleanup) > 1000)
        {
            NetPrintf(("netconnxboxone: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
            uLastTimeExtCleanup = NetTick();
        }
        #endif

        // signal "try again"
        NetPrintf(("netconnxboxone: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
        return(NETCONN_ERROR_ISACTIVE);
    }

    // make sure platenv async op is completed before proceeding
    if (pRef->PlatEnvAsyncOp != nullptr)
    {
        #if DIRTYCODE_LOGGING
        static uint32_t uLastTimePlatEnv = 0;
        if (!uLastTimePlatEnv || NetTickDiff(NetTick(), uLastTimePlatEnv) > 1000)
        {
            NetPrintf(("netconnxboxone: deferring shutdown because platenv async still in progress\n"));
            uLastTimePlatEnv = NetTick();
        }
        #endif

        // signal "try again"
        NetPrintf(("netconnxboxone: deferring shutdown because platenv async still in progress\n"));
        return(NETCONN_ERROR_ISACTIVE);
    }

    if (pRef->NetworkStatusChangedEventToken.Value)
    {
        NetworkInformation::NetworkStatusChanged -= pRef->NetworkStatusChangedEventToken;
        pRef->NetworkStatusChangedEventToken.Value = 0;
    }

    if (pRef->SuspendingEventToken.Value)
    {
        CoreApplication::Suspending -= pRef->SuspendingEventToken;
        pRef->SuspendingEventToken.Value = 0;
    }

    if (pRef->ResumingEventToken.Value)
    {
        CoreApplication::Resuming -= pRef->ResumingEventToken;
        pRef->ResumingEventToken.Value = 0;
    }

    // kill the network status thread if it was started
    if (pRef->hNetworkStatusThread != 0)
    {
        NetPrintf(("netconnxboxone: waiting for network status thread to exit.\n"));

        while (pRef->eNetConnNetworkStatusState == NetConnNetworkStatusStateE::ST_STATUS_INPROGRESS)
        {
            // wait till the thread is not in progress
            NetConnSleep(50);
        }

        pRef->hNetworkStatusThread = 0;
        SetEvent(pRef->hNetworkStatusEvent);

        while (pRef->eNetConnNetworkStatusState != NetConnNetworkStatusStateE::ST_STATUS_EXIT)
        {
            // wait till the thread exits
            NetConnSleep(50);
        }

        CloseHandle(pRef->hNetworkStatusEvent);
    }

    // shut down protossl
    ProtoSSLShutdown();

    // shut down dirtycert
    DirtyCertDestroy();

    // remove netconn idle task
    NetConnIdleDel(_NetConnUpdate, pRef);

    // shut down Idle handler
    NetConnIdleShutdown();

    // free the local users
    if (pRef->pLocalUser != NULL)
    {
        NetConnLocalUserDestroy(pRef->pLocalUser);
    }

    // shut down dirtysock
    SocketDestroy(0);

    for (int32_t iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
    {
        _NetConnClearUser(pRef, iUserIndex);

        // release critical section
        if (pRef->aUsers[iUserIndex].bCritInitialized)
        {
            NetCritKill(&pRef->aUsers[iUserIndex].asyncOpCrit);
            pRef->aUsers[iUserIndex].bCritInitialized = FALSE;
        }

        // release EAUser if we still have a reference on it.
        if (pRef->aEAUsers[iUserIndex])
        {
            pRef->aEAUsers[iUserIndex]->Release();
            pRef->aEAUsers[iUserIndex] = NULL;
        }
    }

    // destroy the network status crit
    NetCritKill(&pRef->networkStatusCrit);

    // common shutdown (must come last as this frees the memory)
    NetConnCommonShutdown(&pRef->Common);
    _NetConn_pRef = NULL;

    return(0);
}

/*** Public functions *****************************************************************************/

/*F********************************************************************************/
/*!
    \Function NetConnStartup

    \Description
        Bring the network connection module to life. Does not actually start any
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
            -noupnp  : disables UPNP
            -servicename=<game-year-platform> : set servicename required for SSL use
            -affinity=<mask as hex> : the cpu affinity we use for our internal threads
        \endverbatim

    \Version 12/01/2012 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStartup(const char *pParams)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iUserIndex, iResult;
    uintptr_t uTempHandle = 1;
    char strThreadCpuAffinity[16];
    DirtyThreadConfigT ThreadConfig;

    // allow NULL params
    if (pParams == NULL)
    {
        pParams = "";
    }

    // debug display of input params
    NetPrintf(("netconnxboxone: startup params='%s'\n", pParams));

    // common startup
    // pRef shall hold the address of the NetConnRefT after completion if no error occured
    iResult = NetConnCommonStartup(sizeof(*pRef), pParams, (NetConnCommonRefT**)(&pRef));

    // treat the result of the common startup, if already started simply early out
    if (iResult == NETCONN_ERROR_ALREADY_STARTED)
    {
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

    // default to PROD environment, will eventually be overwritten with value obtained from ea_environment.json file in TMS
    pRef->uPlatEnv = NETCONN_PLATENV_PROD;
    ds_strnzcpy(pRef->strSandboxId, NETCONNXBOXONE_DEFAULT_SANDBOX, sizeof(pRef->strSandboxId));
    // default test sandbox name (may be overridden)
    ds_strnzcpy(pRef->strSandboxNameTest, NETCONNXBOXONE_TEST_SANDBOX, sizeof(pRef->strSandboxNameTest));

    // initialize user-specific data
    for(iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
    {
        NetCritInit(&pRef->aUsers[iUserIndex].asyncOpCrit, "netconn-userasyncopcrit");
        _NetConnClearUser(pRef, iUserIndex);
        pRef->aUsers[iUserIndex].bCritInitialized = TRUE;
    }

    // get affinity string from our startup params, defaulting to 0x0
    ds_memclr(strThreadCpuAffinity, sizeof(strThreadCpuAffinity));
    NetConnCopyParam(strThreadCpuAffinity, sizeof(strThreadCpuAffinity), "-affinity=", pParams, "0x0");
    pRef->iThreadCpuAffinity = strtol(strThreadCpuAffinity, NULL, 16);

    // start up dirtysock
    if (SocketCreate(THREAD_PRIORITY_HIGHEST, 0, pRef->iThreadCpuAffinity) != 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnxboxone: unable to start up dirtysock\n"));
        return(NETCONN_ERROR_SOCKET_CREATE);
    }

    // create and configure dirtycert
    if (NetConnDirtyCertCreate(pParams))
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnxboxone: unable to create dirtycert\n"));
        return(NETCONN_ERROR_DIRTYCERT_CREATE);
    }

    // start up protossl
    if (ProtoSSLStartup() < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnxboxone: unable to start up protossl\n"));
        return(NETCONN_ERROR_PROTOSSL_CREATE);
    }

    // add netconn task handle
    if (NetConnIdleAdd(_NetConnUpdate, pRef) < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnxboxone: unable to add netconn task handler\n"));
        return(NETCONN_ERROR_INTERNAL);
    }

    // startup microsoft secure sockets, so socket calls don't silently fail
    // this will load all the port configurations (SecureDeviceAssociationTemplate) from networkmanifest.xml
    try
    {
        Windows::Xbox::Networking::SecureDeviceAssociationTemplate::Templates;
    }
    catch (Exception ^ e)
    {
        NetPrintf(("netconnxboxone: failed loading SecureDeviceAssociationTemplate (err=%s)\n", DirtyErrGetName(e->HResult)));
    }

    _NetConnRegisterNetworkStatusChange(pRef);

    if (!pRef->SuspendingEventToken.Value)
    {
        NetPrintf(("netconnxboxone: adding suspending event handler \n"));
        
        pRef->SuspendingEventToken = (CoreApplication::Suspending += ref new EventHandler<Windows::ApplicationModel::SuspendingEventArgs ^>(
            [=](Platform::Object ^refObj, Platform::Object ^refObj2)
        {
            NetPrintf(("netconnxboxone: SuspendingEvent triggered   tick=%d\n", NetTick()));
            
            // assuming we can't resume while suspended, we only want to resume back to a state before we are suspended
            if (pRef->eNetConnState != NetConnStateE::ST_RESUME)
            {
                pRef->eNetConnStateBeforeSuspd = pRef->eNetConnState;
            }

            // we are doing the state transition and processing the suspend state here because we will 
            // not get any cpu time once we fully suspend
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_SUSPEND);
            _NetConnSuspendState(pRef);
        }));
    }

    // when you resume users are logged in fresh
    if (!pRef->ResumingEventToken.Value)
    {
        NetPrintf(("netconnxboxone: adding resuming event handler \n"));

        pRef->ResumingEventToken = (CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(
            [=](Platform::Object ^refObj, Platform::Object ^refObj2)
        {
            NetPrintf(("netconnxboxone: ResumingEvent triggered. tick=%d\n", NetTick()));
            
            // move into the xbl wait state
            pRef->uXblWaitTimer = 0;
            pRef->eConnectivityLevel = Windows::Networking::Connectivity::NetworkConnectivityLevel::None;
            pRef->eThreadConnectivityLevel = Windows::Networking::Connectivity::NetworkConnectivityLevel::None;
            pRef->eNetConnStateAfterXblWait = NetConnStateE::ST_RESUME;
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_XBLWAIT);         
        }));
    }

    // intialize the network status crit
    NetCritInit(&pRef->networkStatusCrit, "network status crit");

    // setup the network update thread
    pRef->eNetConnNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_INVALID;
    pRef->hNetworkStatusEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // this is to prevent the thread from exiting before we get a valid handle back
    pRef->hNetworkStatusThread = (HANDLE)uTempHandle;

    // configure thread parameters
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "NetworkStatus";
    ThreadConfig.iAffinity = pRef->iThreadCpuAffinity;
    ThreadConfig.iVerbosity = 1;

    // start the network status thread
    if ((iResult = DirtyThreadCreate(_NetworkStatusThread, pRef, &ThreadConfig)) == 0)
    {
        // force a network status update for the first time
        pRef->bUpdateNetworkStatus = TRUE;
    }
    else
    {
        pRef->eNetConnNetworkStatusState = NetConnNetworkStatusStateE::ST_STATUS_EXIT;
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnxboxone: error %d creating network status thread\n", iResult));
        return(NETCONN_ERROR_PLATFORM_SPECIFIC);
    }

    try
    {
        if (Windows::ApplicationModel::Store::CurrentApp::LicenseInformation->IsTrial)
        {
            pRef->iIsTrial = 1;
        }
        else
        {
            pRef->iIsTrial = 0;
        }
    }
    catch (...)
    {
        NetPrintf(("Windows::ApplicationModel::Store::CurrentApp::LicenseInformation->IsTrial failed\n"));
        pRef->iIsTrial = -1;
    }

    // save ref
    _NetConn_pRef = pRef;

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnQuery

    \Description
        Returns the index of the given gamertag. If the gamertag is not
        present, treat pList as a pointer to an XUID, and find the user by
        XUID. Otherwise return the index of the first logged in user.

    \Input *pGamertag   - gamer tag
    \Input *pList       - XUID to get index of
    \Input iListSize    - unused

    \Output
        int32_t         - negative=gamertag not logged in, else index of user

    \Version  07/20/2012 (mcorcoran)
*/
/*************************************************************************************************F*/
int32_t NetConnQuery(const char *pGamertag, NetConfigRecT *pList, int32_t iListSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    if (pGamertag != NULL)
    {
        // Find the matching gamertag
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
        {
            if (ds_strnicmp(pRef->aUsers[iIndex].strGamertag, pGamertag, sizeof(pRef->aUsers[iIndex].strGamertag)) == 0)
            {
                // We've found the right user, get their index
                return(iIndex);
            }
        }
    }
    else if (pList != NULL)
    {
        // Find the matching Xuid
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
        {
            if (memcmp(&pRef->aUsers[iIndex].Xuid, pList, sizeof(pRef->aUsers[iIndex].Xuid)) == 0)
            {
                // We've found the right user, get their index
                return(iIndex);
            }
        }
    }
    else
    {
        // Return the index of the first online user
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
        {
            if (pRef->aEAUsers[iIndex] != NULL && pRef->aEAUsers[iIndex]->IsSignedIn())
            {
                // We've found an online user, get their index
                return(iIndex);
            }
        }
    }

    return(-1);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnConnect

    \Description
        Used to bring the networking online with a specific configuration. Uses the
        configuration returned by NetConnQuery.

    \Input *pConfig - the configuration entry from NetConnQuery
    \Input *pOption - asciiz list of config parameters
                      "silent=<true/false>"   to specifiy whether to use silent login.
    \Input iData    - platform-specific

    \Output
        int32_t     - negative=error, zero=success

    \Version 12/01/2012 (jbrookes)
*/
/*************************************************************************************************F*/
int32_t NetConnConnect(const NetConfigRecT *pConfig, const char *pOption, int32_t iData)
{
    NetConnRefT *pRef = _NetConn_pRef;

    NetPrintf(("netconnxboxone: connecting...\n"));

    // we can only perform a connect when we are in NetConnSate Idle
    // we will ignore the operation if was are not in the right state 

    if ((pRef->eNetConnState == NetConnStateE::ST_IDLE) || (pRef->eNetConnState == NetConnStateE::ST_CLEANUP))
    {
        pRef->bShowAccountPicker = FALSE;

        // process connection options
        if (pOption != NULL)
        {
            // find out if we are doing a silent login or not
            if (strstr(pOption, "silent=false") != NULL)
            {
                pRef->bShowAccountPicker = TRUE;
            }
        }
        
        if (pRef->eNetConnState == NetConnStateE::ST_CLEANUP)
        {
            // defer connect till external clean up is done
            pRef->bDelayConn = TRUE;
        }
        else
        {
            pRef->uXblWaitTimer = 0;
            pRef->eNetConnStateAfterXblWait = NetConnStateE::ST_RUNNING;
            _NetConnMoveToNextState(pRef, NetConnStateE::ST_XBLWAIT);
        }
    }
    else if (pRef->eNetConnState == NetConnStateE::ST_RUNNING)
    {
        NetPrintf(("netconnxboxone: NetconnConnect is already inprogress or already connected.\n"));
        return(-1);
    }
    else
    {
        NetPrintf(("netconnxbone: NetConnConnect cannot be performed in %s state.\n", _NetConn_strNetConnState[pRef->eNetConnState]));
        return(-2);
    }

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnDisconnect

    \Description
        Used to bring down the network connection. After calling this, it would
        be necessary to call NetConnConnect to bring the connection back up or
        NetConnShutdown to completely shutdown all network support.

    \Output
        int32_t     - negative=error, zero=success

    \Version 12/01/2012 (jbrookes)
*/
/*************************************************************************************************F*/
int32_t NetConnDisconnect(void)
{
    NetConnRefT *pRef = _NetConn_pRef;
    NetConnTokenT *pTokenSpace = NULL;
    int32_t iUserIndex;

    NetPrintf(("netconnxboxone: disconnecting...\n"));

    pRef->uEnvTimer = 0;
    pRef->uConnStatus = 0;
    pRef->uNetworkStatusRetryCount = 0;    
    pRef->uNetworkStatusRetryTimer = 0;    

    // shut down xhttp/xauth if started
    if (pRef->pDirtyAuth != NULL)
    {
        DirtyAuthDestroy(pRef->pDirtyAuth);
        pRef->pDirtyAuth = NULL;
    }

    if (pRef->PlatEnvAsyncOp != nullptr)
    {
        pRef->PlatEnvAsyncOp->Cancel();
        NetPrintf(("netconnxboxone: releasing env determination resources deferred to external cleanup because async op still in progress\n"));
        NetConnControl('recu', 0, 0, (void *)_NetConnIsGetEnvInProgress, pRef);
    }

    // go into the cleanup state
    _NetConnMoveToNextState(pRef, NetConnStateE::ST_CLEANUP);

    // reset status of token requests that are failed or in progress
    for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
    {
        pTokenSpace = &pRef->aUsers[iUserIndex].token;
        if (pTokenSpace->eTokenStatus != ST_TOKEN_VALID)
        {
            pTokenSpace->eTokenStatus = ST_TOKEN_INVALID;
            pTokenSpace->bFailureReported = FALSE;
        }
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function NetConnControl

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
            'aurn' - stands for "Authenticaiton URN"; sets the URN to be used when acquiring an XSTS token
            'recu' - stands for "register for external cleanup"; add a new entry to the external cleanup list
            'snam' - set DirtyCert service name
            'spam' - set debug output verbosity (0...n)
            'tick' - initiate a new XSTS token acquisition for the user index specified in iValue; <0=error, 0=retry (a request is already pending), >0=request issued
            'tsbx' - set test sandbox name (pValue; default=EARW.1 - call before NetConnConnect())
            'xblu' - stands for "xbl update"; force an immediate update of the different users' signin state
            'xtag' - retrieve gamertag from xuid. pValue: XUID, pValue2: user-provided pData
            'xtcb' - set pointer(pValue) to callback for retrieving gamertag from XUID
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonControl()

    \Version 12/01/2012 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnControl(int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue, void *pValue2)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // make sure module is started before allowing any other control calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnxboxone: warning - calling NetConnControl() while module is not initialized\n"));
        return(-1);
     }

    // sets the URN
    if (iControl == 'aurn')
    {
        ds_strnzcpy(pRef->strAuthUrn, (const char *)pValue, sizeof(pRef->strAuthUrn));
        NetPrintf(("netconnxboxone: authentication URN set to %s\n", pRef->strAuthUrn));
        return(0);
    }

    // register for external cleanup
    if (iControl == 'recu')
    {
        return(NetConnCommonAddToExternalCleanupList(&pRef->Common, (NetConnExternalCleanupCallbackT)pValue, pValue2));
    }

    // request refresh of XSTS token
    if (iControl == 'tick')
    {
        int32_t iUserIndex = iValue;
        int32_t iRetCode = 0;  // default to "try again"

        if (iUserIndex < NETCONN_MAXLOCALUSERS)
        {
            NetPrintf(("netconnxboxone: NetConnControl 'tick' for user index %d  (URN = %s)\n", iUserIndex, pRef->strAuthUrn));

            switch (pRef->aUsers[iUserIndex].token.eTokenStatus)
            {
                // token acquisition is currently in progress
                case ST_TOKEN_INPROG:
                    break;

                default:
                    _NetConnRequestAuthToken(pRef, iUserIndex);
                    iRetCode = 1;  // signal success
                    break;
            }
        }
        else
        {
            NetPrintf(("netconnxboxone: NetConnControl 'tick' for invalid user index %d\n", iUserIndex));
            iRetCode = -1;  // signal failure
        }

        return(iRetCode);
    }

    // set test sandbox name
    if ((iControl == 'tsbx') && (pValue != NULL))
    {
        ds_strnzcpy(pRef->strSandboxNameTest, (const char *)pValue, sizeof(pRef->strSandboxNameTest));
        NetPrintf(("netconnxboxone: setting test sandbox name to '%s'\n", pRef->strSandboxNameTest));
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
        NetPrintf(("netconnxboxone: changing debug level from %d to %d\n", pRef->Common.iDebugLevel, iValue));
        pRef->Common.iDebugLevel = iValue;
        // allow to fall through to socketcontrol
    }
    #endif

    // pass through unhandled selectors to NetConnCommon
    return(NetConnCommonControl(&pRef->Common, iControl, iValue, iValue2, pValue, pValue2));
}

/*F*************************************************************************************************/
/*!
    \Function NetConnStatus

    \Description
        Check general network connection status. Different selectors return
        different status attributes.

    \Input iKind    - status selector
    \Input iData    - (optional) selector specific
    \Input *pBuf    - (optional) pointer to output buffer
    \Input iBufSize - (optional) size of output buffer

    \Output
        int32_t     - selector specific

    \Notes
        iKind can be one of the following:

        \verbatim
            addr: ip address of client
            affn: thread cpu affinity setting
            bbnd: broadband true or false
            chat: TRUE if local user cannot chat, else FALSE
            conn: true/false indication of whether connection in progress
            envi: EA back-end environment type in use (-1=not available, NETCONN_PLATENV_DEV, NETCONN_PLATENV_TEST, NETCONN_PLATENV_CERT, NETCONN_PLATENV_PROD), and sandbox name via pBuf if provided (32 bytes)
            eusr: returns the IEAUser pointer in pBuf given the user index
            ethr: mac address of primary adapter (returned in pBuf), 0=success, negative=error
            gtag: gamertag returned in pBuf, 0=success, negative=error
            gust: returns 1 if the given user is a guest, returns 0 for a normal player, returns a negative value is the operation failed.
            locl: return locality (Windows locale) for local system, ex. 'enUS'
                  For unrecognized locale, 'zzZZ' will be returned.
            locn: return location On Xbox One this is the same as locl
            macx: mac address of primary adapter (returned in pBuf), 0=success, negative=error
            mask: returns the mask of user that are logged in.
            mgrp: return memory group and user data pointer
            ncon: true/false indication of whether network connectivity is at least Internet.
            nste: enum indicating Windows::Networking::Connectivity::NetworkConnectivityLevel.
            natt: returns the NAT Type as an int32_t (returned in pBuf). If pBuf is not specified the NAT type will be returned in the return value
            onln: true/false indication of whether network is operational
            open: true/false indication of whether network code running
            susp: return TRUE if we are suspended and FALSE if we are not
            tick: fills pBuf with XSTS Token if available. return value: 0:"try again"; <0:error; >0:number of bytes copied into pBuf
            tria: returns value indicating if the running title has a trial license. return value: -1:"failed to fetch trial info"; 0:not trial; 1:trial
            type: NETCONN_IFTYPE_*
            vers: return DirtySDK version
            xadd: >0:number of bytes (local SecureDeviceAddress blob) copied in pBuf, negative=error
            xcon: true/false indicating if we have xbox live access
            xlan: retrieve the language code of the active user as a char * (returned in pBuf). pBuf must be at least be NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH in length.
            xnfo: (deprecated) return status of user account info (gamertag and xuid) for a given user index (iData)
            xtry: retrieve the country code of the active user as a char* (returned in pBuf). pBuf must be at least be NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH in length.
            xuid: xuid of client returned in pBuf, 0=success, negative=error
            xusr: retrieves a reference to the User object at user index iData, pBuf is a pointer to the (User^). 0=success, negative=error
            ixid: return the index of the user matching the xuid in pBuf 
            iusr: return the index of the IEAUser::GetUserId() passed via iData. Should be called from the thread that calls NetConnIdle(). negative=error (deprecated)
            usri: return the user index for the given IEAUser
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonStatus()

    \Version 12/01/2012 (jbrookes)
*/
/*************************************************************************************************F*/
int32_t NetConnStatus(int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // see if network code is initialized
    if (iKind == 'open')
    {
        return(pRef != NULL);
    }
    // return DirtySDK version
    if (iKind == 'vers')
    {
        return(DIRTYVERS);
    }

    // make sure module is started before allowing any other status calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnxboxone: warning - calling NetConnStatus() while module is not initialized\n"));
        return(-1);
    }

    // return the thread cpu affinity
    if (iKind == 'affn')
    {
        return(pRef->iThreadCpuAffinity);
    }

    // return memory group and user data pointer
    if (iKind == 'mgrp')
    {
        if ((pBuf != NULL) && (iBufSize == sizeof(void *)))
        {
            *((void **)pBuf) = pRef->Common.pMemGroupUserData;
        }
        return(pRef->Common.iMemGroup);
    }

    // return whether we are broadband or not
    if (iKind == 'bbnd')
    {
        // assume broadband
        return(TRUE);
    }

    // return ability of local user to chat
    if (iKind == 'chat')
    {
        return(FALSE);  // chat is not disabled
    }

    // return connection status
    if (iKind == 'conn')
    {
        return(pRef->uConnStatus);
    }

    // EA back-end environment type in use
    if (iKind == 'envi')
    {
        if (pRef->uConnStatus == '+onl')
        {
            if (pBuf != NULL)
            {
                if (iBufSize >= sizeof(pRef->strSandboxId))
                {
                    ds_strnzcpy((char*)pBuf, pRef->strSandboxId, iBufSize);
                }
                else
                {
                    NetPrintf(("netconnxboxone: insufficient buffer space provided to 'envi' %d required\n", sizeof(pRef->strSandboxId)));
                }
            }

            return(pRef->uPlatEnv);
        }
        return(-1);
    }

    // returns the corresponding IEAUser pointer given the user index
    if (iKind == 'eusr')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS))
        {
           if ((pBuf != NULL) && (iBufSize >= sizeof(void *)))
           {
               void **pIEAUser = reinterpret_cast<void **>(pBuf);
               *pIEAUser = (void *)pRef->aEAUsers[iData];   
               return (0);
           }
        }
        return (-1);
    }
    
    // return host mac address
    if ((iKind == 'ethr') || (iKind == 'macx'))
    {
        NetPrintf(("netconnxboxone: mac address query is currently unsupported on xboxone\n"));
        return(-1);
    }

    // return email address
    if (iKind == 'mail')
    {
        NetPrintf(("netconnxboxone: NetConnStatus('mail') is unsupported on xboxone\n"));
        return(-1);
    }

    //mask: returns the mask of user that are logged in.
    if (iKind == 'mask')
    {
        return(_NetConnXblUserMask(pRef));
    }

    // return logged in gamertag
    if (iKind == 'gtag')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < sizeof(pRef->aUsers[iData].strGamertag)))
        {
            // invalid buffer or buffer too small
            return(-2);
        }

        if (pRef->aUsers[iData].strGamertag[0] == '\0')
        {
            // no valid gamertag at the specified user index
            return(-3);
        }

        // fill user-provided buffer with gamertag
        ds_memcpy(pBuf, pRef->aUsers[iData].strGamertag, sizeof(pRef->aUsers[iData].strGamertag));

        return(0);
    }

    // reutrn 1 if the user is a guest user , 0 if user is a normal user
    if (iKind == 'gust')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            // index out of range
            NetPrintf(("netconnxboxone: netconnstatus('gust') user index out of range\n"));
            return(-1);
        }

        if (pRef->aEAUsers[iData] != NULL)
        {
            if (pRef->aEAUsers[iData]->IsGuest())
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }
        
        NetPrintf(("netconnxboxone: netconnstatus('gust') failed for user at index %i\n", iData));
        return(-2);
    }

    // return Windows locale information
    if (iKind == 'locl')
    {
        char country[NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH];
        char language[NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH];
        int32_t iIndex = 0;
        int32_t iLocale = 0;

        if ((NetConnStatus('xtry', 0 , country, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH) >= 0) && (NetConnStatus('xlan', 0, language, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH) >= 0))
        {
            for(iIndex = 0; iIndex < NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH - 1; ++iIndex)
            {
                iLocale += language[iIndex] << (24 - (iIndex * 8));
            }

            for(iIndex = 0; iIndex < NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH - 1; ++iIndex)
            {
                iLocale += country[iIndex] << (8 - (iIndex * 8));
            }

            return(iLocale);
        }
        else
        {
            NetPrintf(("netconnxboxone: NetConnStatus('locl') Unable to retrieve language/country code.\n"));
            return(LOBBYAPI_LOCALITY_UNKNOWN);
        }
    }

    // return Windows location information
    if (iKind == 'locn')
    {
        //On the Xbox One locn is the same as locl
        return(NetConnStatus('locl', 0, NULL, 0));
    }

    // return NAT Type info
    if (iKind == 'natt')
    {
        int32_t iResult = 0;

        try
        {
            SecureDeviceAddress ^refSecureDeviceAddr = SecureDeviceAddress::GetLocal();
            iResult = (int32_t) refSecureDeviceAddr->NetworkAccessType;

        }
        catch (Exception ^ e)
        {
            NetPrintf(("netconnxboxone:  NetConnStatus('natt') exception caught (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            iResult = -2;
        }

        if (pBuf != NULL  && iResult >= 0)
        {
            if(iBufSize == sizeof(int32_t))
            {
                ds_memcpy(pBuf, &iResult, iBufSize);
                iResult = 0;
            }
            else
            {
                NetPrintf(("netconnxboxone:  NetConnStatus('natt') Invalid arguments.\n"));
                iResult = -1;
            }
        }

        return(iResult);
    }

    // check network connectivity
    if (iKind == 'ncon')
    {
        return (int32_t)(pRef->eConnectivityLevel >= Windows::Networking::Connectivity::NetworkConnectivityLevel::InternetAccess);
    }

    //return network status enum provided by the OS. This allows the user to get finer details if needed (say make a distinction between internet vs xbox live connection).
    if(iKind == 'nste')
    {
        return (int32_t)(pRef->eConnectivityLevel);
    }

    // see if connected to ISP/LAN
    if (iKind == 'onln')
    {
        return(pRef->uConnStatus == '+onl');
    }

    // return TRUE if we are suspended
    if (iKind == 'susp')
    {
        return(pRef->eNetConnState == NetConnStateE::ST_SUSPEND);
    }
    
    // return ticket info (if available)
    if (iKind == 'tick')
    {
        uint8_t bNeedRefresh = FALSE;   // default to "no need to refresh ticket"
        uint8_t bCacheValid = FALSE;    // default to "ticket cache content is not valid"
        int32_t iResult = 0;            // default to "try again"
        int32_t iUserIndex = iData;
        int32_t iTokenLen = 0;
        int32_t iTokenStatus;

        if (iUserIndex >= NETCONN_MAXLOCALUSERS || iUserIndex < 0)
        {
            NetPrintf(("netconnxboxone: NetConnStatus 'tick' for invalid user index %d\n", iUserIndex));
            return(-1);
        }

        NetPrintf(("netconnxboxone: NetConnStatus 'tick' for user index %d  (URN = %s)\n", iUserIndex, pRef->strAuthUrn));

        NetCritEnter(&pRef->aUsers[iUserIndex].asyncOpCrit);

        switch (pRef->aUsers[iUserIndex].token.eTokenStatus)
        {
            // token has not been requested yet
            case ST_TOKEN_INVALID:
                bNeedRefresh = TRUE;
                break;

            // token aquisition failed
            case ST_TOKEN_FAILED:
                if (pRef->aUsers[iUserIndex].token.bFailureReported == TRUE)
                {
                    // attempt a new token request
                    pRef->aUsers[iUserIndex].token.bFailureReported = FALSE;
                    pRef->aUsers[iUserIndex].token.eTokenStatus = ST_TOKEN_INVALID;
                    bNeedRefresh = TRUE;
                }
                else
                {
                    // report the error to the xsts token server
                    pRef->aUsers[iUserIndex].token.bFailureReported = TRUE;
                    iResult = -1;
                }
                break;

            // token acquisition is currently in progress
            case ST_TOKEN_INPROG:
                break;

            // token is valid
            case ST_TOKEN_VALID:
                iTokenStatus = DirtyAuthCheckToken(iUserIndex, pRef->strAuthUrn, &iTokenLen, NULL);
                
                if (iTokenStatus != DIRTYAUTH_SUCCESS)
                {
                    if (iTokenStatus == DIRTYAUTH_EXPIRED)
                    {
                        NetPrintf(("netconnxboxone: ticket cache for user %d expired, refreshing now...\n", iUserIndex));
                    }
                    bNeedRefresh = TRUE;
                }
                else
                {
                    bCacheValid = TRUE;
                }
                break;

            default:
                NetPrintf(("netconnxboxone: critical error - invalid token status \n"));
                iResult = -1;
                break;
        }

        // return cached ticket to caller if possible
        if (bCacheValid)
        {
            // add one to the token to guarantee null termination
            iResult = iTokenLen + 1;

            if (pBuf)
            {
                char *pTokenBuf = (char *)pBuf;

                if (iBufSize >= (iTokenLen + 1))
                {
                    // retrieve cached token
                    DirtyAuthCheckToken(iUserIndex, pRef->strAuthUrn, NULL, (char *)pTokenBuf);

                    // null terminate
                    pTokenBuf[iTokenLen] = '\0';

                    // log token
                    NetPrintWrap((const char *)pTokenBuf, 132);
                }
                else
                {
                    NetPrintf(("netconnxboxone: user-provided buffer is too small (%d/%d)\n", iBufSize, (iTokenLen + 1)));
                    iResult = -1;
                }
            }
        }

        NetCritLeave(&pRef->aUsers[iUserIndex].asyncOpCrit);

        // initiate a ticket cache refresh if needed
        if (bNeedRefresh)
        {
            _NetConnRequestAuthToken(pRef, iUserIndex);
        }

        return(iResult);
    }

    if (iKind == 'tria')
    {
        int32_t iResult = -1;
        if (pRef->iIsTrial >= 0)
        {
            iResult = pRef->iIsTrial;
        }
        else
        {
            try
            {
                if (Windows::ApplicationModel::Store::CurrentApp::LicenseInformation->IsTrial)
                {
                    iResult = 1;
                }
                else
                {
                    iResult = 0;
                }
            }
            catch (...)
            {
                NetPrintf(("Windows::ApplicationModel::Store::CurrentApp::LicenseInformation->IsTrial failed\n"));
                iResult = -1;
            }
            pRef->iIsTrial = iResult;
        }

        return(iResult);
    }

    // return what type of interface we are connected with
    if (iKind == 'type')
    {
        // assume broadband
        return(NETCONN_IFTYPE_ETHER);
    }

    // return local SecureDeviceAddress blob (use pBuf=NULL to only query blob size)
    if (iKind == 'xadd')
    {
        int32_t iResult = 0;

        try
        {
            SecureDeviceAddress ^refSecureDeviceAddr = SecureDeviceAddress::GetLocal();
            IBuffer ^refSecureDeviceAddressBuffer = refSecureDeviceAddr->GetBuffer();

            if (pBuf != NULL)
            {
                if ((unsigned)iBufSize >= refSecureDeviceAddressBuffer->Length)
                {
                    DataReader ^refDataReader = DataReader::FromBuffer(refSecureDeviceAddressBuffer);
                    refDataReader->ReadBytes(ArrayReference<uint8_t>((uint8_t *)pBuf, refSecureDeviceAddressBuffer->Length));
                    iResult = refSecureDeviceAddressBuffer->Length;

                    NetPrintMem(pBuf, refSecureDeviceAddressBuffer->Length, "local-secure-addr-blob");
                }
                else
                {
                    NetPrintf(("netconnxboxone: local SecureDeviceAddress blob (%d bytes) does not fit in user's buffer (%d bytes)\n",
                        refSecureDeviceAddressBuffer->Length, iBufSize));
                    iResult = -1;
                }
            }
        }
        catch (Exception ^ e)
        {
            NetPrintf(("netconnxboxone: exception caught while trying to get local SecureDeviceAddress blob (%S/0x%08x)\n",
                e->Message->Data(), e->HResult));
            iResult = -2;
        }

        return(iResult);
    }

    //Check for Xbox Live Connectivity
    if (iKind == 'xcon')
    {
        return(int32_t)(pRef->eConnectivityLevel >= Windows::Networking::Connectivity::NetworkConnectivityLevel::XboxLiveAccess);
    }

    //return xbox one language code
    if (iKind == 'xlan')
    {
        wchar_t langCode[NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH];

        if (pBuf == NULL)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xlan') null buffer passed in.\n"));
            return(-1);
        }
        else if (iBufSize < NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xlan') buffer size is too small.\n"));
            return(-2);
        }
        
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, langCode, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH) <= 0)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xlan') cannot retrieve the language code.\n"));
            return(-3);
        }

        wcstombs((char *)pBuf, langCode, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH);
        
        return(0);
    }

    // (deprecated) return status of user account info (gamertag and xuid)
    if (iKind == 'xnfo')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if (pRef->aUsers[iData].strGamertag[0] == '\0')
        {
            // no valid gamertag at the specified user index
            return(-2);
        }

        return(0);
    }

    //return xbox one country code
    if (iKind == 'xtry')
    {
        wchar_t countryCode[NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH];

        if (pBuf == NULL)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xtry') null buffer passed in.\n"));
            return(-1);
        }
        else if (iBufSize < NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xtry') buffer size is too small.\n"));
            return(-2);
        }

        if (GetGeoInfoW(GetUserGeoID(GEOCLASS_NATION), GEO_ISO2, countryCode, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH, 0) <= 0)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('xtry') cannot retrieve the country code.\n"));
            return(-3);
        }

        wcstombs((char *)pBuf, countryCode, NETCONNXBOXONE_STR_COUNTRY_LANG_CODE_LENGTH);

        return(0);
    }

    // return xuid
    if (iKind == 'xuid')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < sizeof(pRef->aUsers[iData].Xuid)))
        {
            // invalid buffer or buffer too small
            return(-2);
        }

        if (pRef->aUsers[iData].Xuid == 0)
        {
            // no valid XUID at the specified user index
            return(-3);
        }

        ds_memcpy(pBuf, &pRef->aUsers[iData].Xuid, sizeof(pRef->aUsers[iData].Xuid));
        return(0);
    }

    if (iKind == 'xusr')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize != sizeof(User^)))
        {
            // invalid buffer or buffer too small
            return(-2);
        }

        if (pRef->aEAUsers[iData] == NULL)
        {
            // no user at specified index
            return(-3);
        }

        *((User^*)pBuf) = pRef->aEAUsers[iData]->GetFirstPartyObject();
        return(0);
    }

    if (iKind == 'ixid')
    {
        int32_t iRet = -2;
        int32_t iLocalUserIndex = 0;

        if (pBuf == NULL)
        {
            return(-1);
        }

        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if ((pRef->aEAUsers[iLocalUserIndex] != NULL) && (wcscmp(pRef->aEAUsers[iLocalUserIndex]->GetUserXboxID()->Data(), (const wchar_t *)pBuf) == 0))
            {
                iRet = iLocalUserIndex;
                break;
            }
        }

        return(iRet);
    }

    // 'iusr' is deprecated please use 'usri' 
    if (iKind == 'iusr')
    {
        return(_NetConnQueryIEAUser(pRef, (uint32_t)iData));
    }

    // return the user index of the IEAUser
    if (iKind == 'usri')
    {
        if (pBuf == NULL)
        {
            NetPrintf(("netconnxboxone: NetConnStatus('usri') invalid buffer\n"));
            return (-1);
        }

        EA::User::IEAUser *pIEAUser = (EA::User::IEAUser *)pBuf;
        return (_NetConnQueryIEAUser(pRef, (uint32_t)pIEAUser->GetUserID()));
    }

    // pass unrecognized options to NetConnCommon
    return(NetConnCommonStatus(&pRef->Common, iKind, iData, pBuf, iBufSize));
}

/*F*************************************************************************************************/
/*!
    \Function     NetConnShutdown

    \Description
        Shutdown the network code and return to idle state.

    \Input bStopEE  - ignored in xbox one implementation

    \Output
        int32_t     - negative=error, zero=success

    \Version 12/01/2012 (jbrookes)
*/
/*************************************************************************************************F*/
int32_t NetConnShutdown(uint32_t bStopEE)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // error see if already stopped
    if (pRef == NULL)
    {
        return(NETCONN_ERROR_NOTACTIVE);
    }

    return(_NetConnShutdownInternal(pRef, bStopEE));
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnSleep

    \Description
        Sleep the application (yield thread) for some number of milliseconds.

    \Input iMilliSecs    - number of milliseconds to block for

    \Version 12/01/2012 (jbrookes)
*/
/*************************************************************************************************F*/
void NetConnSleep(int32_t iMilliSecs)
{
    Sleep((DWORD)iMilliSecs);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnAddLocalUser

    \Description
        Use this function to tell netconn about a newly detected local user on the local console.

    \Input iLocalUserIndex  - index at which DirtySDK needs to insert this user it its internal user array
    \Input pLocalUser       - pointer to associated IEAUser

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
    \Input pLocalUser       - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 01/16/2014 (mclouatre)
*/
/*************************************************************************************************F*/
int32_t NetConnRemoveLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserRemove(iLocalUserIndex, pLocalUser));
}
