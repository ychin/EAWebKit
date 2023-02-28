/*H********************************************************************************/
/*!
    \File netconnnx.cpp

    \Description
        Provides network setup and teardown support. Does not actually create any
        kind of network connections.

    \Copyright
        Copyright (c) 2010 Electronic Arts Inc.

    \Version 04/05/2010 (jbrookes) First version; a vanilla port from Unix to NX
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nn/socket.h>
#include <nn/account.h>
#include <nn/nifm.h>
#include <nn/oe.h>
#include <nn/settings.h>
#include <nn/time.h>

#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtycert.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protossl.h"
#include "DirtySDK/proto/protoupnp.h"
#include "netconncommon.h"
#include "netconnlocaluser.h"

#include "IEAUser/IEAUser.h"
#include "EAUser/NXUser.h"

/*** Defines **********************************************************************/

//! UPNP port
#define NETCONN_DEFAULT_UPNP_PORT       (3659)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

typedef struct NetConnUserT
{
    nn::account::Uid                        Uid;                                              //!< associated nn::account::Uid
    nn::account::UserHandle                 UserHandle;                                       //!< associated nn::account::UserHandle
    char                                    strNickname[nn::account::NicknameBytesMax + 1];   //!< associated nickname (gamertag)
} NetConnUserT;

//! private module state
typedef struct NetConnRefT
{
    NetConnCommonRefT       Common;                             //!< cross-platform netconn data (must come first!)
    NetConnLocalUserRefT    *pLocalUser;                        //!< local user container for IEAUser
    NetConnUserT            aUsers[NETCONN_MAXLOCALUSERS];      //!< NetConnUsers
    const EA::User::NXUser  *aEAUsers[NETCONN_MAXLOCALUSERS];   //!< EAUser known by NetConn - populated when pIEAUserEventList is processed

    enum
    {
        ST_INIT,                        //!< initialization
        ST_CONN,                        //!< bringing up network interface
        ST_IDLE,                        //!< active
        ST_EXT_CLEANUP                  //!< cleaning up external instances from previous NetConn connection, can't proceed before all cleaned up
    } eState;                           //!< internal connection state

    ProtoUpnpRefT   *pProtoUpnp;        //!< protoupnp module state
    uint32_t        uConnStatus;        //!< connection status (surfaced to user)
    int32_t         iPeerPort;          //!< peer port to be opened by upnp; if zero, still find upnp router but don't open a port
    int32_t         iThreadCpuAffinity; //!< cpu affinity used for our internal threads
    uint8_t         bConnectDelayed;    //!< if TRUE, NetConnConnect is being delayed until external cleanup phase completes

} NetConnRefT;

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

//! mapping table to map NX country definitions to DirtySock encodings
static uint16_t _NetConn_NXCountryMap[][2] =
{
    { 'ja', LOBBYAPI_COUNTRY_JAPAN },
    { 'nl', LOBBYAPI_COUNTRY_NETHERLANDS },
    { 'fr', LOBBYAPI_COUNTRY_FRANCE },
    { 'de', LOBBYAPI_COUNTRY_GERMANY },
    { 'it', LOBBYAPI_COUNTRY_ITALY },
    { 'pt', LOBBYAPI_COUNTRY_PORTUGAL },
    { 'es', LOBBYAPI_COUNTRY_SPAIN },
    { 'ru', LOBBYAPI_COUNTRY_RUSSIAN_FEDERATION },
    { 'ko', LOBBYAPI_COUNTRY_KOREA_REPUBLIC_OF },
    { 'zh', LOBBYAPI_COUNTRY_CHINA }
};

//! global module ref
static NetConnRefT *_NetConn_pRef = NULL;

/*** Private Functions ************************************************************/

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

    NetPrintf(("netconnnx: netconn status changed to %s\n", strConnStatus));
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetInterfaceType

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
    
    return(uIfType);
}

/*F********************************************************************************/
/*!
    \Function _NetConnGetPlatformEnvironment

    \Description
        Uses ticket issuer to determine platform environment.

    \Input *pRef    - module state

    \Output
        uint32_t    - platform environment (NETCONN_PLATENV_*)

    \Version 10/07/2009 (jbrookes)
*/
/********************************************************************************F*/
static uint32_t _NetConnGetPlatformEnvironment(NetConnRefT *pRef)
{
    uint32_t uPlatEnv = NETCONN_PLATENV_PROD;   // default to production environment
    return(uPlatEnv);
}

/*F********************************************************************************/
/*!
    \Function _NetConnConnect

    \Description
        Start connection process

    \Input *pRef    - netconn module state

    \Output
        int32_t     - 0 for success, negative for failure

    \Version 9/20/2013 (tcho)
*/
/********************************************************************************F*/
static int32_t _NetConnConnect(NetConnRefT *pRef)
{
    int32_t iResult = 0;

    // start up network interface
    if ((iResult = SocketControl(NULL, 'conn', 0, NULL, NULL)) == 0)
    {
        pRef->eState = NetConnRefT::ST_CONN;
    }
    else
    {
        NetPrintf(("netconnnx: failed to initialize network stack\n"));
        _NetConnUpdateConnStatus(pRef, '-skt');
        return(-2);
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _NetConnUpdate

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
    int32_t iExternalCleanupListCount;

    // update local users list
    NetConnLocalUserUpdate(pRef->pLocalUser);

    // process the cleanup list
    iExternalCleanupListCount = NetConnCommonProcessExternalCleanupList(&pRef->Common);

    // waiting for external cleanup list to be empty
    if (pRef->eState == NetConnRefT::ST_EXT_CLEANUP)
    {
        if (iExternalCleanupListCount == 0)
        {
            pRef->eState = NetConnRefT::ST_INIT;

            if (pRef->bConnectDelayed)
            {
                pRef->bConnectDelayed = FALSE;

                if (_NetConnConnect(pRef) < 0)
                {
                    // assumption: _NetConnConnect() has updated pRef->uConnStatus with appropriate error state
                    NetPrintf(("netconnnx: critical error - can't complete connection setup\n"));
                }
            }
        }
    }

    // wait for network active status
    if (pRef->eState == NetConnRefT::ST_CONN)
    {
        uint32_t uSocketConnStatus = SocketInfo(NULL, 'conn', 0, NULL, 0);
        
        if (pRef->uConnStatus != uSocketConnStatus)
        {
            _NetConnUpdateConnStatus(pRef, uSocketConnStatus);
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

    // update connection status while idle
    if (pRef->eState == NetConnRefT::ST_IDLE)
    {
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
}

/*F********************************************************************************/
/*!
    \Function   _NetConnAddLocalUser

    \Description
        Add a local user

    \Input *pLocalUserRef   - local user module reference
    \Input iLocalUserIndex  - local user index
    \Input *pIEAUser        - pointer to IEAUser object

    \Version 09/23/2018 (tcho)
*/
/********************************************************************************F*/
static void _NetConnAddLocalUser(NetConnLocalUserRefT *pLocalUserRef, int32_t iLocalUserIndex, const EA::User::IEAUser *pIEAUser)
{
    NetConnRefT *pRef = (NetConnRefT *)pLocalUserRef->pNetConn;
    const EA::User::NXUser *pNXUser = static_cast<const EA::User::NXUser *>(pIEAUser);
    nn::account::Nickname nickName;

    nn::account::GetNickname(&nickName, pNXUser->GetNNUid());
   
    if (pRef->aEAUsers[iLocalUserIndex] == NULL)
    {
        pRef->aEAUsers[iLocalUserIndex] = pNXUser;
        pRef->aEAUsers[iLocalUserIndex]->AddRef();

        if (pRef->aUsers[iLocalUserIndex].Uid == nn::account::InvalidUid)
        {
            pRef->aUsers[iLocalUserIndex].Uid = pNXUser->GetNNUid();
            pRef->aUsers[iLocalUserIndex].UserHandle = pNXUser->GetNNUserHandle();
            ds_memcpy(&pRef->aUsers[iLocalUserIndex].strNickname, nickName.name, nn::account::NicknameBytesMax + 1);

            NetPrintf(("netconnnx: netconn user added at local user index %d GamerTag %s\n", iLocalUserIndex, nickName.name));
        }
        else
        {
            NetPrintf(("netconnnx: failed to add EAUser at index %d with local user because Uid is out of sync GamerTag %s.\n", iLocalUserIndex, nickName.name));
        }
    }
    else
    {
        NetPrintf(("netconnnx: failed to add EAUser at index %d with local because entry already used GamerTag %s\n", iLocalUserIndex, nickName.name));
    }
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
        if (pRef->aEAUsers[iLocalUserIndex]->GetUserID() == pIEAUser->GetUserID())
        {
            pRef->aUsers[iLocalUserIndex].Uid = nn::account::InvalidUid;
            ds_memclr(&(pRef->aUsers[iLocalUserIndex].UserHandle), sizeof(pRef->aUsers[iLocalUserIndex].UserHandle));
            NetPrintf(("netconnnx: netconn user removed at local user index %d \n", iLocalUserIndex));
        }
        else
        {
            NetPrintf(("netconnnx: failed to remove local user at index %d due to uid mismatch\n", iLocalUserIndex));
        }
    }
    else
    {
        NetPrintf(("netconnnx: failed to remove IEAUSER at index %d - no local user at that spot\n", iLocalUserIndex));
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

    \Version 09/26/2018 (tcho)
*/
/********************************************************************************F*/
static int32_t _NetConnQueryIEAUser(NetConnRefT *pRef, nn::account::Uid id)
{
    int32_t iResult = -1;
    int32_t i = 0;

    for (; i < NETCONN_MAXLOCALUSERS; i++)
    {
        if (pRef->aEAUsers[i] != NULL && ((EA::User::NXUser *)pRef->aEAUsers[i])->GetNNUid() == id)
        {
            iResult = i;
            break;
        }
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnConvertLanguageCode

    \Description
       Strips off the 2 bytes from the IETF language code

    \Input lang  - IEAUser UserId that we will use to match the index

    \Output
        uint16_t - 2 bytes from the IETF language code

    \Version 10/18/2018 (tcho)
*/
/********************************************************************************F*/
static uint16_t _NetConnConvertLanguageCode(const char *lang)
{
    uint16_t uConvertedLanguageCode = lang[0] << 8;
    uConvertedLanguageCode |= lang[1];
    return(uConvertedLanguageCode);
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
    int32_t iResult;
    int32_t iExternalCleanupListCnt;
    int32_t iLocalUserIndex;

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

    // make sure the external cleanup list is empty before proceeding
    if ((iExternalCleanupListCnt = NetConnCommonProcessExternalCleanupList(&pRef->Common)) != 0)
    {
        #if DIRTYCODE_LOGGING
        static uint32_t uLastTimeExtCleanup = 0;
        if (!uLastTimeExtCleanup || NetTickDiff(NetTick(), uLastTimeExtCleanup) > 1000)
        {
            NetPrintf(("netconnnx: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
            uLastTimeExtCleanup = NetTick();
        }
        #endif

        // signal "try again"
        NetPrintf(("netconnx: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
        return(NETCONN_ERROR_ISACTIVE);
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

    // free the local users
    if (pRef->pLocalUser != NULL)
    {
        NetConnLocalUserDestroy(pRef->pLocalUser);
    }

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        // release EAUser if we still have a reference on it.
        if (pRef->aEAUsers[iLocalUserIndex])
        {
            pRef->aEAUsers[iLocalUserIndex]->Release();
            pRef->aEAUsers[iLocalUserIndex] = NULL;
        }
    }

    // shutdown the network code
    SocketDestroy(0);

    // shutdown netconn common
    NetConnCommonShutdown(&pRef->Common);
    _NetConn_pRef = NULL;

    nn::time::Finalize();

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
        pParams can contain the following terms:

        \verbatim
            -asyncsend              - socket send operation will be offloaded to a dedicated thread (for all sockets)
            -noupnp                 - disable upnp support
            -nosocketinit           - dirtynet layer will not handle the life time of the nintendo socket library
            -servicename            - set servicename <game-year-platform> required for SSL us
            -silent=<true/false>    - silent fail, if true first party dialog will not be displayed when no network is available
            -affinity=<mask as hex> - the cpu affinity for our internal threads
        \endverbatim

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStartup(const char *pParams)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iThreadPrio = 10;
    int32_t iResult = 0;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    char strThreadCpuAffinity[16];

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allow NULL params
    if (pParams == NULL)
    {
        pParams = "";
    }

    // debug display of input params
    NetPrintf(("netconnnx: startup params='%s'\n", pParams));

    // common startup
    // pRef shall hold the address of the NetConnRefT after completion if no error occured
    iResult = NetConnCommonStartup(sizeof(NetConnRefT), pParams, (NetConnCommonRefT**)(&pRef));

    // treat the result of the common startup, if already started simply early out
    if (iResult == NETCONN_ERROR_ALREADY_STARTED)
    {
        return(0);
    }
    else if (iResult < 0)
    {
        return(iResult);
    }

    // local user startup
    if ((pRef->pLocalUser = NetConnLocalUserInit(pRef, _NetConnAddLocalUser, _NetConnRemoveLocalUser)) == NULL)
    {
        return(NETCONN_ERROR_NO_MEMORY);
    }

    pRef->eState = NetConnRefT::ST_INIT;
    pRef->iPeerPort = NETCONN_DEFAULT_UPNP_PORT;

    // skip nintendo socket library initialization
    SocketControl(NULL, 'sint', strstr(pParams, "-nosocketinit") != NULL, NULL, NULL);

    // set silent fail for socket library (do not display first party dialog)
    SocketControl(NULL, 'sfal', strstr(pParams, "-silent=true") != NULL, NULL, NULL);

    // get the thread cpu affinity setting from our startup params, defaulting to 0x0
    ds_memclr(strThreadCpuAffinity, sizeof(strThreadCpuAffinity));
    NetConnCopyParam(strThreadCpuAffinity, sizeof(strThreadCpuAffinity), "-affinity=", pParams, "0x0");
    pRef->iThreadCpuAffinity =(int32_t) strtol(strThreadCpuAffinity, NULL, 16);

    // create network instance
    if ((iResult = SocketCreate(iThreadPrio, 0, pRef->iThreadCpuAffinity) < 0))
    {
        _NetConnShutdownInternal(pRef, 0);

        if (iResult == DIRTYAPI_SOCKET_ERR_PLATFORM_SPECIFIC)
        {
            NetPrintf(("netconnnx: unable to create network interface please try again\n"));
            return(NETCONN_ERROR_RETRY);
        }
        else
        {
            NetPrintf(("netconnnx: unable to create network interface\n"));
            return(NETCONN_ERROR_SOCKET_CREATE);
        }
    }

    // enable async send for all sockets if "-asyncsend" netconn param is specified
    SocketControl(NULL, 'asnd', strstr(pParams, "-asyncsend") != NULL, NULL, NULL);

    // create and configure dirtycert
    if (NetConnDirtyCertCreate(pParams))
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnnx: unable to create dirtycert\n"));
        return(NETCONN_ERROR_DIRTYCERT_CREATE);
    }

    // start up protossl
    if (ProtoSSLStartup() < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnnx: unable to startup protossl\n"));
        return(NETCONN_ERROR_PROTOSSL_CREATE);
    }

    // create the upnp module
    if (!strstr(pParams, "-noupnp"))
    {
        pRef->pProtoUpnp = ProtoUpnpCreate();
        if (pRef->pProtoUpnp == NULL)
        {
            _NetConnShutdownInternal(pRef, 0);
            NetPrintf(("netconnnx: unable to start up protoupnp\n"));
            return(NETCONN_ERROR_PROTOUPNP_CREATE);
        }
    }

    // add netconn task handle
    if (NetConnIdleAdd(_NetConnUpdate, pRef) < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnnx: unable to add netconn task handler\n"));
        return(NETCONN_ERROR_INTERNAL);
    }

    // clear user array
    for (int iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        pRef->aUsers[iLocalUserIndex].Uid = nn::account::InvalidUid;
        ds_memclr(&(pRef->aUsers[iLocalUserIndex].UserHandle), sizeof(pRef->aUsers[iLocalUserIndex].UserHandle));        
    }

    nn::time::Initialize();
    nn::account::Initialize();
    
    // save ref
    _NetConn_pRef = pRef;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function    NetConnQuery

    \Description
        Returns the index of the given nickname. If the nickname is not
        present, treat pList as a pointer to an UID, and find the user by
        UID. Otherwise return the index of the first logged in user.

    \Input *pNickname   - nickname of the user
    \Input *pList       - UID to get index of
    \Input iSize        - unused

    \Output
        int32_t         - negative=user not logged in, otherwise index of user

    \Version 12/17/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t NetConnQuery(const char *pNickname, NetConfigRecT *pList, int32_t iSize)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iLocalUserIndex;

    // if nickname is specified then query using that
    if (pNickname != NULL)
    {
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            if (ds_strnicmp(pRef->aUsers[iLocalUserIndex].strNickname, pNickname, sizeof(pRef->aUsers[iLocalUserIndex].strNickname)) == 0)
            {
                return(iLocalUserIndex);
            }
        }
    }
    // otherwise if uid is specified query using that
    else if (pList != NULL)
    {
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            if (memcmp(&pRef->aUsers[iLocalUserIndex].Uid, pList, sizeof(pRef->aUsers[iLocalUserIndex].Uid)) == 0)
            {
                return(iLocalUserIndex);
            }
        }
    }
    // otherwise find the first signed in user
    else
    {
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            const EA::User::NXUser *pUser = pRef->aEAUsers[iLocalUserIndex];
            if ((pUser != NULL) && pUser->IsSignedIn())
            {
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
                      "peerport=<port>" to specify peer port to be opened by upnp.
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
                pRef->iPeerPort = (int32_t)strtol(pOpt+9, NULL, 10);
            }
        }
            NetPrintf(("netconnnx: upnp peerport=%d %s\n", pRef->iPeerPort, (pRef->iPeerPort == NETCONN_DEFAULT_UPNP_PORT ? "(default)" : "(selected via netconnconnect param)")));

        if (pRef->eState == NetConnRefT::ST_EXT_CLEANUP)
        {
            pRef->bConnectDelayed = TRUE;
            NetPrintf(("netconnnx: delaying completion of NetConnConnect() until external cleanup phase is completed\n"));
        }
        else
        {
            _NetConnConnect(pRef);
        }
    }
    else
    {
        NetPrintf(("netconnnx: NetConnConnect() ignored because already connected!\n"));
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

    // shut down networking
    if (pRef->eState != NetConnRefT::ST_INIT)
    {
        // bring down network interface
        SocketControl(NULL, 'disc', 0, NULL, NULL);

        // abort upnp operations
        if (pRef->pProtoUpnp != NULL)
        {
            ProtoUpnpControl(pRef->pProtoUpnp, 'abrt', 0, 0, NULL);
        }

        // transition to external cleanup state
        pRef->eState = NetConnRefT::ST_EXT_CLEANUP;
        pRef->uConnStatus = 0;
    }
    // done
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
            snam: set DirtyCert service name
        \endverbatim

        Unhandled selectors are passed through to SocketControl()

    \Version 1.0 04/27/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnControl(int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue, void *pValue2)
{
    NetConnRefT *pRef = _NetConn_pRef;;

    // make sure module is started before allowing any other control calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnnx: warning - calling NetConnControl() while module is not initialized\n"));
        return(-1);
    }
    // set dirtycert service name
    if (iControl == 'snam')
    {
        return(DirtyCertControl('snam', 0, 0, pValue));
    }
    // pass through unhandled selectors to SocketControl()
    return(SocketControl(NULL, iControl, iValue, pValue, pValue2));
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
            affn: thread cpu affinity setting
            bbnd: TRUE if broadband, else FALSE
            conn: connection status: +onl=online, ~<code>=in progress, -<err>=NETCONN_ERROR_*
            envi: EA back-end environment type in use (-1=not available, 0=pending)
            eusr: returns the IEAUser pointer in pBuf given the user index
            gtag: return the nintendo account nickname given the user index
            iusr: reutrn the user index give the nintendo account uid in pBuf (deprecated)
            nsai: return the NSA id for the given user index
            nuid: return nintendo account uid for the given user index
            nurh: return nintendo user handle for the given user index
            onln: true/false indication of whether network is operational
            open: true/false indication of whether network code running
            type: connection type: NETCONN_IFTYPE_* bitfield
            upnp: return protoupnp external port info, if available
            usri: return the user index for the given IEAUser
            vers: return DirtySDK version
        \endverbatim

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStatus(int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // initialize output buffer
    if (pBuf != NULL)
    {
        memset(pBuf, 0, iBufSize);
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
        NetPrintf(("netconnnx: warning - calling NetConnStatus() while module is not initialized\n"));
        return(-1);
    }

    // return the thread cpu affinity setting
    if (iKind == 'affn')
    {
        return(pRef->iThreadCpuAffinity);
    }

    // return broadband (TRUE/FALSE)
    if (iKind == 'bbnd')
    {
        return(TRUE);
    }
    // connection status
    if (iKind == 'conn')
    {
        return(pRef->uConnStatus);
    }
    // EA back-end environment type in use
    if (iKind == 'envi')
    {
        return(_NetConnGetPlatformEnvironment(pRef));
    }
    // returns the corresponding IEAUser pointer given the user index
    if (iKind == 'eusr')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS))
        {
            if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(void *)))
            {
                EA::User::IEAUser **pIEAUser = (EA::User::IEAUser **)pBuf;
                *pIEAUser = (EA::User::IEAUser *)pRef->aEAUsers[iData];
                return(0);
            }
        }
        return(-1);
    }
    // return logged in gamertag (nickname)
    if (iKind == 'gtag')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnnx: NetConnStatus('gtag') user index out of range\n"));
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < (int32_t)sizeof(nn::account::Nickname)))
        {
            NetPrintf(("netconnnx: NetConnStatus('gtag') invalid buffer or buffer size\n"));
            return(-2);
        }

        if (pRef->aUsers[iData].Uid == nn::account::InvalidUid)
        {
            NetPrintf(("netconnnx: NetConnStatus('gtag') invalid user account\n"));
            return(-3);
        }

        ds_memcpy(pBuf, pRef->aUsers[iData].strNickname, nn::account::NicknameBytesMax + 1);
        return(0);
    }

    // not supported on nx (iusr is deprecated)
    if (iKind == 'iusr')
    {
        NetPrintf(("netconnnx: NetConnStatus('iusr') not support on the nx\n"));
        return(0);
    }

    // return locality code
    if (iKind == 'locl')
    {
        // get the IETF language code
        nn::settings::LanguageCode lang = nn::oe::GetDesiredLanguage();
        uint16_t uLanguage = _NetConnConvertLanguageCode(lang.string);
        uint16_t uCountry = LOBBYAPI_COUNTRY_UNKNOWN;

        // if language code only have the language try to look up the country
        if (lang.string[2] == '\0')
        {
            for (size_t i = 0; i < sizeof(_NetConn_NXCountryMap) / sizeof(_NetConn_NXCountryMap[0]); i++)
            {
                if (_NetConn_NXCountryMap[i][0] == uLanguage)
                {
                    uCountry = _NetConn_NXCountryMap[i][1];
                    break;
                }
            }
        }
        // if the language code has a country retrieve it
        else
        {
            uCountry = _NetConnConvertLanguageCode(lang.string + 3);
        }

        return(LOBBYAPI_LocalizerTokenCreate(uLanguage, uCountry));
    }

    if (iKind == 'nsai')
    {
        nn::Result result;

        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnnx: NetConnStatus('nsai') user index out of range\n"));
            return (-1);
        }

        if ((pBuf == NULL) || (iBufSize < (int32_t)sizeof(nn::account::NetworkServiceAccountId)))
        {
            NetPrintf(("netconnnx: NetConnStatus('nsai') invalid buffer or buffer size\n"));
            return(-2);
        }

        if (pRef->aUsers[iData].Uid == nn::account::InvalidUid)
        {
            NetPrintf(("netconnnx: NetConnStatus('nsai') invalid user account\n"));
            return(-3);
        }

        result = nn::account::EnsureNetworkServiceAccountAvailable(pRef->aUsers[iData].UserHandle);
        if (result.IsFailure())
        {
            NetPrintf(("netconnnx: NetConnStatus('nsai') failed to obtain a netwrok service account\n"));
            return(-4);
        }
        else
        {
            result = nn::account::GetNetworkServiceAccountId((nn::account::NetworkServiceAccountId *)pBuf, pRef->aUsers[iData].UserHandle);
            if (result.IsFailure())
            {
                NetPrintf(("netconnnx: NetConnStatus('nsai') failed to retreive a netwrok service account id\n"));
                return (-5);
            }
        }
        
        return (0);
    }

    // return nintendo account uid for a give user index
    if (iKind == 'nuid')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnnx: NetConnStatus('nuid') user index out of range\n"));
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < (int32_t)sizeof(nn::account::Uid)))
        {
            NetPrintf(("netconnnx: NetConnStatus('nuid') invalid buffer or buffer size\n"));
            return(-2);
        }

        if (pRef->aUsers[iData].Uid == nn::account::InvalidUid)
        {
            NetPrintf(("netconnnx: NetConnStatus('nuid') invalid user account\n"));
            return(-3);
        }

        ds_memcpy(pBuf, &pRef->aUsers[iData].Uid, sizeof(nn::account::Uid));
        return(0);
    }

    // return nintendo user handle for the given user index
    if (iKind == 'nurh')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnnx: NetConnStatus('nurh') user index out of range\n"));
            return(-1);
        }

        if ((pBuf == NULL) || (iBufSize < (int32_t)sizeof(nn::account::UserHandle)))
        {
            NetPrintf(("netconnnx: NetConnStatus('nurh') invalid buffer or buffer size\n"));
            return(-2);
        }

        if (pRef->aUsers[iData].Uid == nn::account::InvalidUid)
        {
            NetPrintf(("netconnnx: NetConnStatus('nurh') invalid user account\n"));
            return(-3);
        }

        ds_memcpy(pBuf, &pRef->aUsers[iData].UserHandle, sizeof(nn::account::UserHandle));
        return(0);
    }

    // see if connected to ISP/LAN
    if (iKind == 'onln')
    {
        return(pRef->uConnStatus == '+onl');
    }

    // return status of plug
    if (iKind == 'plug')
    {
        return nn::nifm::IsNetworkAvailable();
    }

    // return interface type (more verbose)
    if (iKind == 'type')
    {
        return(_NetConnGetInterfaceType(pRef));
    }

    // return the user index given the IEAUser
    if (iKind == 'usri')
    {
        if ((pBuf == NULL) || (iBufSize < (int32_t)sizeof(EA::User::IEAUser)))
        {
            NetPrintf(("netconnnx: NetConnStatus('usri') invalid buffer or buffer sizer\n"));
            return (-1);
        }

        EA::User::NXUser *pNXUser = (EA::User::NXUser *)pBuf;
        return (_NetConnQueryIEAUser(pRef, pNXUser->GetNNUid()));
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
    
    // pass unrecognized options to SocketInfo
    return(SocketInfo(NULL, iKind, iData, pBuf, iBufSize));
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
    \Function    NetConnAddLocalUser

    \Description
        Use this function to tell netconn about a newly detected local user on the local console.

    \Input iLocalUserIndex  - index at which DirtySDK needs to insert this user it its internal user array
    \Input *pLocalUser      - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 09/23/2018 (tcho)
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

    \Version 09/23/2018 (tcho)
*/
/*************************************************************************************************F*/
int32_t NetConnRemoveLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserRemove(iLocalUserIndex, pLocalUser));
}
