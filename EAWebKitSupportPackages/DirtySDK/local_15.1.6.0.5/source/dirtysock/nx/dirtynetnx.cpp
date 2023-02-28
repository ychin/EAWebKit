/*H********************************************************************************/
/*!
    \File dirtynetnx.cpp

    \Description
        Provides a wrapper that translates the Unix network interface to the
        DirtySock portable networking interface.

        Currently we do not support LDN as a local area connection will not
        allow us to support our tech stack.

    \Copyright
        Copyright (c) 2010 Electronic Arts Inc.

    \Version 08/09/2018 (tcho) First version; a vanilla port to Linux from PS3
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

#include <nn/os.h>
#include <nn/socket.h>
#include <nn/nifm.h>
#include <nn/ldn.h>
#include <nn/nifm/nifm_ApiIpAddress.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtythread.h"
#include "DirtySDK/dirtysock/netconn.h"

#include "dirtynetpriv.h"       // private include for dirtynet common functions

/*** Defines **********************************************************************/

#define INVALID_SOCKET          (-1)

#define SOCKET_MAX_NIFM_RETRY   (3)

#define SOCKET_MAX_IS_HOLD_TIME (10 *1000) // max NIFM Is hold time is 10 seconds (nintendo do not allow game to stall for moe than 10 seconds)

#define SOCKET_MAXPOLL          (32)

#define SOCKET_VERBOSE          (DIRTYCODE_DEBUG && FALSE)

/*** Type Definitions *************************************************************/
//! NIFM states
typedef enum SocketNIFMStateE
{
    SOCKET_NIFM_UNINITIALIZED,
    SOCKET_NIFM_INITIALIZING,
    SOCKET_NIFM_SUBMIT_NETWORK_REQ,
    SOCKET_NIFM_WAIT_NETWORK_REQ,
    SOCKET_NIFM_INITIALIZE_SOCKET,
    SOCKET_NIFM_IP_OPTAINED
} SocketNIFMStateE;

//! private socketlookup structure containing extra data
typedef struct SocketLookupPrivT
{
    HostentT    Host;      //!< must come first!
} SocketLookupPrivT;

//! dirtysock connection socket structure
struct SocketT
{
    SocketT *pNext;             //!< link to next active
    SocketT *pKill;             //!< link to next killed socket

    int32_t iFamily;            //!< protocol family
    int32_t iType;              //!< protocol type
    int32_t iProto;             //!< protocol ident

    int8_t  iOpened;            //!< negative=error, zero=not open (connecting), positive=open
    uint8_t bImported;          //!< whether socket was imported or not
    uint8_t bVirtual;           //!< if true, socket is virtual
    uint8_t bHasData;           //!< zero if no data, else has data ready to be read
    uint8_t bInCallback;        //!< in a socket callback
    uint8_t uBrokenFlag;        //!< 0 or 1=might not be broken, >1=broken socket
    uint8_t bAsyncRecv;         //!< if true, async recv is enabled
    uint8_t bAsyncSend;         //!< if true, async send is enabled
    uint8_t bSendCbs;           //!< TRUE if send cbs are enabled, false otherwise
    int8_t  iVerbose;           //!< debug level
    uint8_t __pad[2];

    int32_t uSocket;            //!< unix socket ref
    int32_t iLastError;         //!< last socket error

    struct sockaddr LocalAddr;  //!< local address
    struct sockaddr RemoteAddr; //!< remote address

    uint16_t uVirtualPort;      //!< virtual port, if set
    uint16_t uPollIdx;          //!< index in blocking poll() operation

    SocketRateT SendRate;       //!< send rate estimation data
    SocketRateT RecvRate;       //!< recv rate estimation data

    int32_t iCallMask;          //!< valid callback events
    uint32_t uCallLast;         //!< last callback tick
    uint32_t uCallIdle;         //!< ticks between idle calls
    void *pCallRef;             //!< reference calback value
    int32_t(*pCallback)(SocketT *pSocket, int32_t iFlags, void *pRef);

    NetCritT RecvCrit;          //!< receive critical section
    NetCritT SendCrit;          //!< send critical section
    int32_t iRecvErr;           //!< last error that occurred
    uint32_t uRecvFlag;         //!< flags from recv operation
    int32_t iRbufSize;          //!< read buffer size (bytes)
    int32_t iSbufSize;          //!< send buffer size (bytes)

    struct sockaddr RecvAddr;   //!< receive address

    SocketPacketQueueT *pSendQueue;
    SocketPacketQueueT *pRecvQueue;
    SocketPacketQueueEntryT *pRecvPacket;
};

//! standard ipv4 packet header (see RFC791)
typedef struct HeaderIpv4
{
    uint8_t verslen;      //!< version and length fields (4 bits each)
    uint8_t service;      //!< type of service field
    uint8_t length[2];    //!< total packet length (header+data)
    uint8_t ident[2];     //!< packet sequence number
    uint8_t frag[2];      //!< fragmentation information
    uint8_t time;         //!< time to live (remaining hop count)
    uint8_t proto;        //!< transport protocol number
    uint8_t check[2];     //!< header checksum
    uint8_t srcaddr[4];   //!< source ip address
    uint8_t dstaddr[4];   //!< dest ip address
} HeaderIpv4;

//! local state
typedef struct SocketStateT
{
    SocketT  *pSockList;                //!< master socket list
    SocketT  *pSockKill;                //!< list of killed sockets
    HostentT *pHostList;                //!< list of ongoing name resolution operations

    uint16_t aVirtualPorts[SOCKET_MAXVIRTUALPORTS]; //!< virtual port list

    // module memory group
    int32_t  iMemGroup;                 //!< module mem group id
    void     *pMemGroupUserData;        //!< user data associated with mem group

    uint32_t uConnStatus;               //!< current connection status
    uint32_t uLocalAddr;                //!< local internet address for active interface
    int32_t  iMaxPacket;                //!< maximum packet size

    uint8_t  aMacAddr[6];               //!< MAC address for active interface
    int8_t   iVerbose;                  //!< debug output verbosity
    uint8_t  bAsyncSend;                //!< enable/disable socket creation defaulting to async send used

    volatile int32_t iRecvLife;         //!< variable used to control life of recv thread
    volatile int32_t iSendLife;         //!< variable used to control life of send thread

    DirtyConditionRefT *pSendThreadCond;//!< synchronization resource used to wake up the send thread
    NetCritT SendThreadCrit;            //!< critical section associated with the above condition (pSendThreadCond)

    uint8_t          bSocketInit;       //!< is socket library initialized
    uint8_t          uNifmRetry;        //!< NIFM network request retries left
    uint32_t         uNetworkHoldTime;  //!< NIFM network is hold start time
    NetCritT         nifmCrit;          //!< NIFM state crit
    SocketNIFMStateE eSocketNIFMState;  //!< NIFM socket state

    SocketHostnameCacheT *pHostnameCache; //!< hostname cache

    SocketSendCallbackEntryT aSendCbEntries[SOCKET_MAXSENDCALLBACKS]; //!< collection of send callbacks
} SocketStateT;

/*** Variables ********************************************************************/

//! module state ref
static SocketStateT *_Socket_pState = NULL;

//! skip socket library initialization
static uint8_t _Socket_bSocketInitialize = FALSE;

// silent failure (do not display first party dialog)
static uint8_t _Socket_bSilentFail = FALSE;

// socket configuration and memory pool
static nn::socket::ConfigDefaultWithMemory _Socket_ConfigWithMemory;

#if DIRTYSOCK_ERRORNAMES
//! getaddrinfo() error result table
static const DirtyErrT _GAI_ErrList[] =
{
    // These defines from <nn/socket/netdb.h> have values that exactly match
    // the nn::socket::AiErrno enum values from <nn/socket/socket_Errno.h>.
 
    DIRTYSOCK_ErrorName(EAI_ADDRFAMILY),    //  1  Address family for hostname not supported.
    DIRTYSOCK_ErrorName(EAI_AGAIN),         //  2  Temporary failure in name resolution.
    DIRTYSOCK_ErrorName(EAI_BADFLAGS),      //  3  Invalid value for 'ai_flags' field.
    DIRTYSOCK_ErrorName(EAI_FAIL),          //  4  Non-recoverable failure in name resolution.
    DIRTYSOCK_ErrorName(EAI_FAMILY),        //  5  'ai_family' not supported.
    DIRTYSOCK_ErrorName(EAI_MEMORY),        //  6  Memory allocation failure.
    DIRTYSOCK_ErrorName(EAI_NODATA),        //  7  No address associated with hostname.
    DIRTYSOCK_ErrorName(EAI_NONAME),        //  8  Hostname or servname not provided, or not known.
    DIRTYSOCK_ErrorName(EAI_SERVICE),       //  9  Servname not supported for 'ai_socktype'.
    DIRTYSOCK_ErrorName(EAI_SOCKTYPE),      // 10  'ai_socktype' not supported.
    DIRTYSOCK_ErrorName(EAI_SYSTEM),        // 11  System error returned in errno.
    DIRTYSOCK_ErrorName(EAI_BADHINTS),      // 12  Invalid value for hints.
    DIRTYSOCK_ErrorName(EAI_PROTOCOL),      // 13  Resolved protocol is unknown.
    DIRTYSOCK_ErrorName(EAI_OVERFLOW),      // 14  Argument buffer overflow.
    DIRTYSOCK_ListEnd()
};
#endif

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function    _SocketGetErrno

    \Description
        returns the Nintendo socket error

    \Output
        int32_t - nintendo error code 

    \Version 09/23/2019 (tcho)
*/
/********************************************************************************F*/
static int32_t _SocketGetErrno() 
{ 
    return((int32_t)nn::socket::GetLastError()); 
}

/*F********************************************************************************/
/*!
    \Function    _SocketTranslateError2

    \Description
        Translate a BSD error to dirtysock

    \Input iErr     - BSD error code
    \Input iErrno   - errno or override of the value

    \Output
        int32_t     - dirtysock error (SOCKERR_*)

    \Version 06/21/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _SocketTranslateError2(int32_t iErr, int32_t iErrno)
{
    if (iErr < 0)
    {
        iErr = iErrno;

        if ((iErr == (int32_t)nn::socket::Errno::EWouldBlock) || (iErr == (int32_t)nn::socket::Errno::EInProgress))
            iErr = SOCKERR_NONE;
        else if (iErr == (int32_t)nn::socket::Errno::EHostUnreach)
            iErr = SOCKERR_UNREACH;
        else if (iErr == (int32_t)nn::socket::Errno::ENotConn)
            iErr = SOCKERR_NOTCONN;
        else if (iErr == (int32_t)nn::socket::Errno::EConnRefused)
            iErr = SOCKERR_REFUSED;
        else if (iErr == (int32_t)nn::socket::Errno::EConnReset)
            iErr = SOCKERR_CONNRESET;
        else if ((iErr == (int32_t)nn::socket::Errno::EBadf) || (iErr == (int32_t)nn::socket::Errno::EPipe))
            iErr = SOCKERR_BADPIPE;
        else
            iErr = SOCKERR_OTHER;
    }
    return(iErr);
}

/*F********************************************************************************/
/*!
    \Function    _SocketTranslateError

    \Description
        Translate a BSD error to dirtysock error

    \Input iErr     - BSD error code

    \Output
        int32_t     - dirtysock error (SOCKERR_*)

    \Version 06/19/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _SocketTranslateError(int32_t iErr)
{
    return(_SocketTranslateError2(iErr, _SocketGetErrno()));
}

/*F********************************************************************************/
/*!
    \Function    _SocketInit

    \Description
        Socket library initalize

    \Output
        int32_t     - zero=success, negative=error

    \Version 08/01/2018 (tcho)
*/
/********************************************************************************F*/
static int32_t _SocketInit()
{
    SocketStateT *pState = _Socket_pState;
    nn::Result result;

    if (pState->bSocketInit == FALSE)
    {
        result = nn::socket::Initialize(_Socket_ConfigWithMemory);

        if (result.IsFailure())
        {
            NetPrintf(("dirtynetnx: unable to %s socket library initialize\n"));
            return(-1);
        }

        pState->bSocketInit = TRUE;
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    _SocketFinalize

    \Description
        Socket library finalize

    \Output
        int32_t     - zero=success, negative=error

    \Version 08/01/2018 (tcho)
*/
/********************************************************************************F*/
static int32_t _SocketFinalize()
{
    SocketStateT *pState = _Socket_pState;
    nn::Result result;

    if (pState->bSocketInit == TRUE)
    {
        // if we didn't initialize the socket library do not finalize it
        if (_Socket_bSocketInitialize == FALSE)
        {
            result = nn::socket::Finalize();

            if (result.IsFailure())
            {
                NetPrintf(("dirtynetnx: unable to %s socket library finalize\n"));
                return(-1);
            }
        }

        // reset the bSocketInit back to _Socket_bSocketInitialize
        pState->bSocketInit = _Socket_bSocketInitialize;
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    _SocketNIFMUpdate

    \Description
        Update NIFM state

    \Input pState        - SocketStateT

    \Version 08/01/2018 (tcho)
*/
/********************************************************************************F*/
static void _SocketNIFMUpdate(SocketStateT *pState)
{
    // currently do not support ldn
    if (nn::ldn::GetState() != nn::ldn::State_None)
    {
        _SocketFinalize();
        if (pState->eSocketNIFMState != SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED)
        {
            pState->uConnStatus = '-dsc';
            pState->uLocalAddr = 0;
            pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED;
        }
        return;
    }

    if (pState->eSocketNIFMState == SocketNIFMStateE::SOCKET_NIFM_INITIALIZING)
    {
        nn::Result result = nn::nifm::Initialize();
        if (result.IsFailure())
        {
            NetPrintf(("dirtynetnx: unable to initialize network interface\n"));
            return;
        }
        pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_SUBMIT_NETWORK_REQ;
    }

    if (pState->eSocketNIFMState == SocketNIFMStateE::SOCKET_NIFM_SUBMIT_NETWORK_REQ)
    {
        nn::nifm::SubmitNetworkRequest();
        pState->uNetworkHoldTime = NetTick();
        pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_WAIT_NETWORK_REQ;
    }

    if (pState->eSocketNIFMState == SocketNIFMStateE::SOCKET_NIFM_WAIT_NETWORK_REQ)
    {
        uint8_t isError = TRUE;

        if ((NetTickDiff(NetTick(), pState->uNetworkHoldTime) < SOCKET_MAX_IS_HOLD_TIME))
        {
            if (nn::nifm::IsNetworkRequestOnHold())
            {
                NetPrintfVerbose((pState->iVerbose, 1, "dirtynetnx: waiting for network interface availability\n"));
                return;
            }
            else if (nn::nifm::IsNetworkAvailable())
            {
                isError = FALSE;
                pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_INITIALIZE_SOCKET;
                NetPrintf(("dirtynetnx: network is available\n"));
            }
            // if silent fail is true do not handle the error
            else if (_Socket_bSilentFail == TRUE)
            {
                pState->uConnStatus = '-dsc';
                pState->uNifmRetry = 0;
                pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED;
                NetPrintf(("dirtynetnx: network not available\n"));
                return;
            }
            // if we cannot acquire the network retry
            else if (nn::nifm::HandleNetworkRequestErrorResult())
            {
                // this error is properly handled
                isError = FALSE;

                if (pState->uNifmRetry > 0)
                {
                    pState->uNifmRetry--;
                    pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_SUBMIT_NETWORK_REQ;
                    NetPrintf(("dirtynetnx: network not available, trying to resubmit netwrok request. %d retries left.\n", pState->uNifmRetry));
                    return;
                }
                else
                {
                    NetPrintf(("dirtynetnx: no more submit network request retries left!\n"));
                }

                if (nn::nifm::IsNetworkAvailable() == 0)
                {
                    pState->uConnStatus = '-dsc';
                    pState->uNifmRetry = 0;
                    pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED;
                    NetPrintf(("dirtynetnx: network not available\n"));
                    return;
                }
            }
        }
        else
        {
            NetPrintf(("dirtynetnx: network request on hold timeout reached!\n"));
        }

        // error not handled
        if (isError == TRUE)
        {
            pState->uConnStatus = '-dsc';
            pState->uNifmRetry = 0;
            pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED;
            NetPrintf(("dirtynetnx: network not available\n"));
            return;
        }
    }

    if (pState->eSocketNIFMState == SocketNIFMStateE::SOCKET_NIFM_INITIALIZE_SOCKET)
    {
        if (_SocketInit() == 0)
        {
            in_addr netOutInAddr;
            nn::Result result = nn::nifm::GetCurrentPrimaryIpAddress(&netOutInAddr);
            if (result.IsFailure())
            {
                NetPrintf(("dirtynetnx: unable to get IP address, error = %d\n", result.GetDescription()));
                return;
            }
            pState->uConnStatus = '+onl';
            pState->uLocalAddr = nn::socket::InetNtohl(netOutInAddr.s_addr);
            pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_IP_OPTAINED;
            NetPrintf(("dirtynetnx: IP address = %s\n", nn::socket::InetNtoa(netOutInAddr)));
        }
    }

    if (pState->eSocketNIFMState == SocketNIFMStateE::SOCKET_NIFM_IP_OPTAINED)
    {
        if (nn::nifm::IsNetworkAvailable() == 0)
        {
            pState->uConnStatus = '-dsc';
            pState->eSocketNIFMState = SocketNIFMStateE::SOCKET_NIFM_UNINITIALIZED;
            NetPrintf(("dirtynetnx: disconnected from the network\n"));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketCreateSocket

    \Description
        Create a system level socket.

    \Input iAddrFamily  - address family (AF_INET)
    \Input iType        - socket type (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...)
    \Input iProto       - protocol type for SOCK_RAW (unused by others)

    \Output
        int32_t         - socket handle

    \Version 16/02/2012 (szhu)
*/
/********************************************************************************F*/
static int32_t _SocketCreateSocket(int32_t iAddrFamily, int32_t iType, int32_t iProto)
{
    int32_t iSocket;
    // create socket
    if ((iSocket = nn::socket::Socket(iAddrFamily, iType, iProto)) >= 0)
    {
        const uint32_t uTrue = 1;
        // if dgram, allow broadcast
        if (iType == SOCK_DGRAM)
        {
            nn::socket::SetSockOpt(iSocket, SOL_SOCKET, SO_BROADCAST, &uTrue, sizeof(uTrue));
        }
        // if a raw socket, set header include
        if (iType == SOCK_RAW)
        {
            nn::socket::SetSockOpt(iSocket, IPPROTO_IP, IP_HDRINCL, &uTrue, sizeof(uTrue));
        }
        // set nonblocking operation
        if (nn::socket::Fcntl(iSocket, F_SETFL, O_NONBLOCK) < 0)
        {
            NetPrintf(("dirtynetnx: error trying to make socket non-blocking (err=%d)\n", _SocketGetErrno()));
        }
    }
    else
    {
        NetPrintf(("dirtynetnx: socket() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
    }
    return(iSocket);
}

/*F********************************************************************************/
/*!
    \Function    _SocketOpen

    \Description
        Create a new transfer endpoint. A socket endpoint is required for any
        data transfer operation.  If iSocket != -1 then used existing socket.

    \Input iSocket      - Socket descriptor to use or -1 to create new
    \Input iAddrFamily  - address family (AF_INET)
    \Input iType        - socket type (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...)
    \Input iProto       - protocol type for SOCK_RAW (unused by others)
    \Input iOpened      - 0=not open (connecting), 1=open

    \Output
        SocketT *       - socket reference

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static SocketT *_SocketOpen(int32_t iSocket, int32_t iAddrFamily, int32_t iType, int32_t iProto, int32_t iOpened)
{
    SocketStateT *pState = _Socket_pState;
    const int32_t iQueueSize = (iType != SOCK_STREAM) ? 1 : 8;
    SocketT *pSocket;

    // only proceed if this is not a ldn socket and we have obtained ip
    if (nn::ldn::GetState() != nn::ldn::State_None)
    {
        NetPrintf(("dirtynetnx: does not support ldn sockets!\n"));
        return(NULL);
    }

    NetCritEnter(&pState->nifmCrit);
    if (pState->eSocketNIFMState != SOCKET_NIFM_IP_OPTAINED)
    {
        NetPrintf(("dirtynetnx: invalid connection state(%d)\n", pState->eSocketNIFMState));
        NetCritLeave(&pState->nifmCrit);
        return(NULL);
    }
    NetCritLeave(&pState->nifmCrit);

    // allocate memory
    if ((pSocket = (SocketT *)DirtyMemAlloc(sizeof(*pSocket), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetnx: unable to allocate memory for socket\n"));
        return(NULL);
    }
    ds_memclr(pSocket, sizeof(*pSocket));

    // open a socket (force to AF_INET if we are opening it)
    if ((iSocket == -1) && ((iSocket = _SocketCreateSocket(iAddrFamily = AF_INET, iType, iProto)) < 0))
    {
        NetPrintf(("dirtynetnx: error %s creating socket\n", DirtyErrGetName(iSocket)));
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        return(NULL);
    }

    // create inbound packet queue
    if ((pSocket->pRecvQueue = SocketPacketQueueCreate(iQueueSize, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetnx: failed to create inbound packet queue for socket\n"));
        nn::socket::Close(iSocket);
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        return(NULL);
    }

    // create outbound packet queue
    if ((pSocket->pSendQueue = SocketPacketQueueCreate(iQueueSize, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetnx: failed to create outbound packet queue for socket\n"));
        SocketPacketQueueDestroy(pSocket->pRecvQueue);
        nn::socket::Close(iSocket);
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        return(NULL);
    }

    // set family/proto info
    pSocket->iFamily = iAddrFamily;
    pSocket->iType = iType;
    pSocket->iProto = iProto;
    pSocket->uSocket = iSocket;
    pSocket->iOpened = iOpened;
    pSocket->iLastError = SOCKERR_NONE;
    pSocket->bAsyncRecv = ((iType == SOCK_DGRAM) || (iType == SOCK_RAW)) ? TRUE : FALSE;
    pSocket->bAsyncSend = pState->bAsyncSend;
    pSocket->bSendCbs = TRUE;
    pSocket->iVerbose = 1;

    // inititalize critical sections
    NetCritInit(&pSocket->RecvCrit, "inet-recv");
    NetCritInit(&pSocket->SendCrit, "inet-send");

    // install into list
    NetCritEnter(NULL);
    pSocket->pNext = pState->pSockList;
    pState->pSockList = pSocket;
    NetCritLeave(NULL);

    // return the socket
    return(pSocket);
}

/*F********************************************************************************/
/*!
    \Function    _SocketReopen

    \Description
        Recreate a socket endpoint.

    \Input *pSocket     - socket ref

    \Output
        SocketT *       - socket ref on success, NULL for error

    \Notes
        This function should not be called from the async/idle thread,
        so socket recreation cannot happen in SocketRecvfrom.

    \Version 16/02/2012 (szhu)
*/
/********************************************************************************F*/
static SocketT *_SocketReopen(SocketT *pSocket)
{
    // we need recvcrit to prevent the socket from being used by the async/idle thread
    // for non-virtual non-tcp sockets only
    if (!pSocket->bVirtual && ((pSocket->iType == SOCK_DGRAM) || (pSocket->iType == SOCK_RAW)))
    {
        SocketT *pResult = NULL;
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);
        NetPrintf(("dirtynetnx: trying to recreate the socket handle for %x->%d\n", pSocket, pSocket->uSocket));

        // close existing socket handle
        if (pSocket->uSocket != INVALID_SOCKET)
        {
            nn::socket::Close(pSocket->uSocket);
            pSocket->uSocket = INVALID_SOCKET;
        }

        // create socket
        if ((pSocket->uSocket = _SocketCreateSocket(pSocket->iFamily, pSocket->iType, pSocket->iProto)) >= 0)
        {
            // set socket buffer size
            if (pSocket->iRbufSize > 0)
            {
                SocketControl(pSocket, 'rbuf', pSocket->iRbufSize, NULL, 0);
            }
            if (pSocket->iSbufSize > 0)
            {
                SocketControl(pSocket, 'sbuf', pSocket->iSbufSize, NULL, 0);
            }
            // bind if previous socket was bound to a specific port
            if (SockaddrInGetPort(&pSocket->LocalAddr) != 0)
            {
                int32_t iResult;
                // always set reuseaddr flag for socket recreation
                SocketControl(pSocket, 'radr', 1, NULL, 0);
                // we don't call SocketBind() here (to avoid virtual socket translation)
                if ((iResult = nn::socket::Bind(pSocket->uSocket, &pSocket->LocalAddr, sizeof(pSocket->LocalAddr))) < 0)
                {
                    pSocket->iLastError = _SocketTranslateError(iResult);
                    NetPrintf(("dirtynetnx: bind() to %a:%d failed (err=%s)\n",
                        SockaddrInGetAddr(&pSocket->LocalAddr),
                        SockaddrInGetPort(&pSocket->LocalAddr),
                        DirtyErrGetName(_SocketGetErrno())));
                }
            }
            // connect if previous socket was connected to a specific remote
            if (SockaddrInGetPort(&pSocket->RemoteAddr) != 0)
            {
                struct sockaddr SockAddr;
                // make a copy of remote
                ds_memcpy_s(&SockAddr, sizeof(SockAddr), &pSocket->RemoteAddr, sizeof(pSocket->RemoteAddr));
                SocketConnect(pSocket, &SockAddr, sizeof(SockAddr));
            }
            // success
            pSocket->uBrokenFlag = 0;
            pResult = pSocket;
            NetPrintf(("dirtynetnx: socket recreation (%x->%d) succeeded.\n", pSocket, pSocket->uSocket));
        }
        else
        {
            pSocket->iLastError = _SocketTranslateError(pSocket->uSocket);
            NetPrintf(("dirtynetnx: socket recreation (%x) failed.\n", pSocket));
        }

        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
        return(pResult);
    }
    return(NULL);
}

/*F********************************************************************************/
/*!
    \Function _SocketClose

    \Description
        Disposes of a SocketT, including disposal of the SocketT allocated memory.  Does
        NOT dispose of the unix socket ref.

    \Input *pSocket - socket to close

    \Output
        int32_t     - negative=error, else zero

    \Version 06/21/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _SocketClose(SocketT *pSocket)
{
    SocketStateT *pState = _Socket_pState;
    uint32_t bSockInList;
    SocketT **ppSocket;

    // remove sock from linked list
    NetCritEnter(NULL);
    for (ppSocket = &pState->pSockList, bSockInList = FALSE; *ppSocket != NULL; ppSocket = &(*ppSocket)->pNext)
    {
        if (*ppSocket == pSocket)
        {
            *ppSocket = pSocket->pNext;
            bSockInList = TRUE;
            break;
        }
    }
    NetCritLeave(NULL);

    // make sure the socket is in the socket list (and therefore valid)
    if (bSockInList == FALSE)
    {
        NetPrintf(("dirtynetnx: warning, trying to close socket 0x%08x that is not in the socket list\n", (intptr_t)pSocket));
        return(-1);
    }

    // finish any idle call
    NetIdleDone();

    // mark as closed
    pSocket->uSocket = INVALID_SOCKET;
    pSocket->iOpened = FALSE;

    // kill critical sections
    NetCritKill(&pSocket->RecvCrit);
    NetCritKill(&pSocket->SendCrit);

    // destroy packet queues
    if (pSocket->pRecvQueue != NULL)
    {
        SocketPacketQueueDestroy(pSocket->pRecvQueue);
    }
    if (pSocket->pSendQueue != NULL)
    {
        SocketPacketQueueDestroy(pSocket->pSendQueue);
    }

    // put into killed list
    NetCritEnter(NULL);
    pSocket->pKill = pState->pSockKill;
    pState->pSockKill = pSocket;
    NetCritLeave(NULL);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    _SocketIdle

    \Description
        Call idle processing code to give connections time.

    \Input *pData   - pointer to socket state

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketIdle(void *pData)
{
    SocketStateT *pState = (SocketStateT *)pData;
    SocketT *pSocket;
    uint32_t uTick;
    uint32_t bSendThreadSignaled = FALSE;

    // for access to g_socklist and g_sockkill
    NetCritEnter(NULL);

    // get current tick
    uTick = NetTick();

    // process nifm
    NetCritEnter(&pState->nifmCrit);
    _SocketNIFMUpdate(pState);
    NetCritLeave(&pState->nifmCrit);

    // walk socket list and perform any callbacks
    for (pSocket = pState->pSockList; pSocket != NULL; pSocket = pSocket->pNext)
    {
        // see if we should do callback
        if ((pSocket->uCallIdle != 0) &&
            (pSocket->pCallback != NULL) &&
            (!pSocket->bInCallback) &&
            (NetTickDiff(uTick, pSocket->uCallLast) > (signed)pSocket->uCallIdle))
        {
            pSocket->bInCallback = TRUE;
            (pSocket->pCallback)(pSocket, 0, pSocket->pCallRef);
            pSocket->bInCallback = FALSE;
            pSocket->uCallLast = uTick = NetTick();
        }

        /* If any socket has pending data in its send queue, wake up the send thread.
           We do this to mitigate cases where the socket send thread missed previous wakeup attempts.
           This phenomenon can happen when the wakeup attempt is being done before the socket send thread
           is blocked on DirtyConditionWait(). Not having this code here could potentially result in
           data being stuck in the send queue until the next send. Combined with the wakeup attempt
           directly in SocketSendto(), this solution ensures outbound data is not unreasonably delayed. */
        if ((bSendThreadSignaled == FALSE) && (pSocket->bVirtual == FALSE) && (pSocket->uSocket != INVALID_SOCKET) && (pSocket->bAsyncSend == TRUE))
        {
            if (SocketPacketQueueStatus(pSocket->pSendQueue, 'pnum') != 0)
            {
                NetCritEnter(&pState->SendThreadCrit);
                if (!DirtyConditionSignal(pState->pSendThreadCond))
                {
                    NetPrintf(("dirtynetnx: failed to wake up send thread\n"));
                }
                NetCritLeave(&pState->SendThreadCrit);
                bSendThreadSignaled = TRUE;  // flag that we no longer need to do this until the next idle
            }
        }
    }

    // delete any killed sockets
    while ((pSocket = pState->pSockKill) != NULL)
    {
        pState->pSockKill = pSocket->pKill;
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }

    // process hostname list, delete completed lookup requests
    SocketHostnameListProcess(&pState->pHostList, pState->iMemGroup, pState->pMemGroupUserData);

    // for access to g_socklist and g_sockkill
    NetCritLeave(NULL);
}

/*F*************************************************************************************/
/*!
    \Function    _SocketRecvfromPacketQueue

    \Description
        Check if there is a pending inbound packet in the socket packet queue.

    \Input *pSocket     - pointer to socket
    \Input *pBuf        - [out] buffer to receive data
    \Input iLen         - length of recv buffer
    \Input *pFrom       - [out] address data was received from (NULL=ignore)
    \Input *pFromLen    - [out] length of address

    \Output
        int32_t         - size of packet extracted from queue, 0 if no packet

    \Version 04/20/2016 (mclouatre)
*/
/************************************************************************************F*/
static int32_t _SocketRecvfromPacketQueue(SocketT *pSocket, const char *pBuf, int32_t iLen, struct sockaddr *pFrom, int32_t *pFromLen)
{
    int32_t iResult = 0;

    // make sure destination buffer is valid
    if ((iLen > 0) && (pBuf != NULL))
    {
        // get a packet
        if (pSocket->iType != SOCK_STREAM)
        {
            iResult = SocketPacketQueueRem(pSocket->pRecvQueue, (uint8_t *)pBuf, iLen, &pSocket->RecvAddr);
        }
        else
        {
            iResult = SocketPacketQueueRemStream(pSocket->pRecvQueue, (uint8_t *)pBuf, iLen);
        }

        if (iResult > 0)
        {
            if (pFrom != NULL)
            {
                ds_memcpy_s(pFrom, sizeof(*pFrom), &pSocket->RecvAddr, sizeof(pSocket->RecvAddr));
                *pFromLen = sizeof(*pFrom);
            }
        }
    }

    return(iResult);
}

/*F*************************************************************************************/
/*!
    \Function    _SocketSendFromPacketQueue

    \Description
        Check if there is a pending outbound packet in the socket packet queue, and 
        attempt to send it over the system socket.

    \Input *pSocket     - pointer to socket

    \Output
        int32_t         - positive: success; negative: failure; 0: done

    \Version 04/20/2016 (mclouatre)
*/
/************************************************************************************F*/
static int32_t _SocketSendFromPacketQueue(SocketT *pSocket)
{
    int32_t iResult;
    uint8_t *pPacketData = NULL;
    int32_t iPacketSize;
    struct sockaddr *pPacketAddr = NULL;

    NetCritEnter(&pSocket->SendCrit);
    if ((iResult = SocketPacketQueueGetHead(pSocket->pSendQueue, &pPacketData, &iPacketSize, &pPacketAddr)) > 0)
    {
        // it is important to leave the crit before performing the system send
        NetCritLeave(&pSocket->SendCrit);

        // check if send addr is valid
        if ((pPacketAddr)->sa_family == AF_UNSPEC)
        {
            if ((iResult = (int32_t)nn::socket::Send(pSocket->uSocket, pPacketData, iPacketSize, 0)) < 0)
            {
                if (_SocketGetErrno() != (int32_t)nn::socket::Errno::EWouldBlock)
                {
                    NetPrintf(("dirtynetnx: send() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
                }
            }
        }
        else
        {
            #if SOCKET_VERBOSE
            NetPrintf(("dirtynetnx: sending %d bytes to %a:%d\n", iPacketSize, SockaddrInGetAddr(pPacketAddr), SockaddrInGetPort(pPacketAddr)));
            #endif
            if ((iResult = (int32_t)nn::socket::SendTo(pSocket->uSocket, pPacketData, iPacketSize, 0, pPacketAddr, sizeof(struct sockaddr))) < 0)
            {
                NetPrintf(("dirtynetnx: sendto(%a:%d) failed (err=%s)\n", SockaddrInGetAddr(pPacketAddr), SockaddrInGetPort(pPacketAddr), DirtyErrGetName(_SocketGetErrno())));
            }
        }
    }
    else
    {
        NetCritLeave(&pSocket->SendCrit);
    }

    if (iResult >= 0)
    {
        NetCritEnter(&pSocket->SendCrit);
        // was the packet entirely sent?
        if (iResult == iPacketSize)
        {
            // remove entry from queue head
            SocketPacketQueueRem(pSocket->pSendQueue, NULL, 0, NULL);
        }
        else if (iResult != 0)
        {
            // update queue entry such that it no longer includes the succesfully transmitted portion of the data
            SocketPacketQueueTouchHead(pSocket->pSendQueue, iResult);
        }
        NetCritLeave(&pSocket->SendCrit);
    }
    else
    {
        if (iResult == SOCKERR_BADPIPE)
        {
            // recreate socket (iLastError will be set in _SocketReopen)
            _SocketReopen(pSocket);
        }
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _SocketLookupDone

    \Description
        Callback to determine if gethostbyname is complete.

    \Input *pHost   - pointer to host lookup record

    \Output
        int32_t     - zero=in progess, neg=done w/error, pos=done w/success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _SocketLookupDone(HostentT *pHost)
{
    // return current status
    return(pHost->done);
}

/*F********************************************************************************/
/*!
    \Function _SocketLookupFree

    \Description
        Release resources used by SocketLookup()

    \Input *pHost   - pointer to host lookup record

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketLookupFree(HostentT *pHost)
{
    // release resource
    pHost->refcount -= 1;
}

/*F********************************************************************************/
/*!
    \Function    _SocketLookupThread

    \Description
        Socket lookup thread

    \Input *_pRef       - thread argument (hostent record)

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketLookupThread(void *_pRef)
{
    SocketStateT *pState = _Socket_pState;
    HostentT *pHost = (HostentT *)_pRef;
    int32_t iResult;
    struct addrinfo Hints, *pList = NULL;
    char strThreadId[32];

    // if state is null the module has been shut down
    if (pState == NULL)
    {
        return;
    }

    // get the thread id
    DirtyThreadGetThreadId(strThreadId, sizeof(strThreadId));

    // setup lookup
    ds_memclr(&Hints, sizeof(Hints));
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM; // set specific socktype to limit to one result per type
    Hints.ai_protocol = IPPROTO_TCP; // set specific proto to limit to one result per type

    // start lookup
    NetPrintf(("dirtynetnx: lookup thread start; name=%s (thid=%s)\n", pHost->name, strThreadId));
    if ((iResult = nn::socket::GetAddrInfo(pHost->name, NULL, &Hints, &pList)) == 0)
    {
        // extract ip address
        pHost->addr = SocketNtohl(((struct sockaddr_in *)pList->ai_addr)->sin_addr.s_addr);

        // mark success
        NetPrintf(("dirtynetnx: %s=%a\n", pHost->name, pHost->addr));
        pHost->done = 1;

        // add hostname to cache
        SocketHostnameCacheAdd(pState->pHostnameCache, pHost->name, pHost->addr, 1 /*pState->iVerbose*/);

        // release memory
        nn::socket::FreeAddrInfo(pList);
    }
    else
    {
        // unsuccessful
        NetPrintf(("dirtynetnx: getaddrinfo('%s', ...) failed err=%s\n", pHost->name, DirtyErrGetNameList(iResult, _GAI_ErrList)));
        pHost->done = -1;
    }

    // note thread completion
    pHost->thread = 1;

    NetPrintf(("dirtynetnx: lookup thread exit; name=%s (thid=%s)\n", pHost->name, strThreadId));

    // release thread-allocated refcount on hostname resource
    pHost->refcount -= 1;
}

/*F********************************************************************************/
/*!
    \Function    _SocketRecvfrom

    \Description
        Receive data from a remote host on a datagram socket.

    \Input *pSocket     - socket reference
    \Input *pBuf        - buffer to receive data
    \Input iLen         - length of recv buffer
    \Input *pFrom       - address data was received from (NULL=ignore)
    \Input *pFromLen    - length of address

    \Output
        int32_t         - positive=data bytes received, else error

    \Version 09/10/2004 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _SocketRecvfrom(SocketT *pSocket, char *pBuf, int32_t iLen, struct sockaddr *pFrom, int32_t *pFromLen)
{
    int32_t iResult;

    // make sure socket ref is valid
    if (pSocket->uSocket == INVALID_SOCKET)
    {
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    if (pFrom != NULL)
    {
        // do the receive
        if ((iResult = (int32_t)nn::socket::RecvFrom(pSocket->uSocket, pBuf, iLen, 0, pFrom, (socklen_t *)pFromLen)) > 0)
        {
            // save recv timestamp
            SockaddrInSetMisc(pFrom, NetTick());
        }
    }
    else
    {
        iResult = (int32_t)nn::socket::Recv(pSocket->uSocket, pBuf, iLen, 0);
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    _SocketRecvToPacketQueue

    \Description
        Attempt to receive data from the given socket and to push it directly
        in the packet queue.

    \Input *pState  - pointer to module state
    \Input *pSocket - pointer to socket to read from

    \Version 10/21/2004 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketRecvToPacketQueue(SocketStateT *pState, SocketT *pSocket)
{
    int32_t iRecvResult;

    // early exit if packet queue is full or this is a virtual socket
    if (SocketPacketQueueStatus(pSocket->pRecvQueue, 'pful') || (pSocket->bVirtual == TRUE))
    {
        return;
    }

    // get a packet queue entry to receive into
    pSocket->pRecvPacket = SocketPacketQueueAlloc(pSocket->pRecvQueue);

    // try and receive some data
    if ((pSocket->iType == SOCK_DGRAM) || (pSocket->iType == SOCK_RAW))
    {
        int32_t iFromLen = sizeof(pSocket->RecvAddr);
        iRecvResult = _SocketRecvfrom(pSocket, (char *)pSocket->pRecvPacket->aPacketData, sizeof(pSocket->pRecvPacket->aPacketData), &pSocket->RecvAddr, &iFromLen);
    }
    else
    {
        iRecvResult = _SocketRecvfrom(pSocket, (char *)pSocket->pRecvPacket->aPacketData, sizeof(pSocket->pRecvPacket->aPacketData), NULL, 0);
    }

    // if the read completed successfully, save the originator address, packet size and reception time; forward data to socket callback if needed
    if (iRecvResult > 0)
    {
        pSocket->pRecvPacket->iPacketSize = iRecvResult;
        pSocket->pRecvPacket->uPacketTick = NetTick();
        ds_memcpy_s(&pSocket->pRecvPacket->PacketAddr, sizeof(pSocket->pRecvPacket->PacketAddr), &pSocket->RecvAddr, sizeof(pSocket->RecvAddr));

        // see if we should issue callback
        if ((pSocket->uCallLast != (unsigned)-1) && (pSocket->pCallback != NULL) && (pSocket->iCallMask & CALLB_RECV))
        {
            pSocket->uCallLast = (unsigned)-1;
            (pSocket->pCallback)(pSocket, 0, pSocket->pCallRef);
            pSocket->uCallLast = NetTick();
        }
    }
    else
    {
        if (_SocketGetErrno() != (int32_t)nn::socket::Errno::EAgain)
        {
            // if we are using a TCP socket and we didn't receive positive bytes, we are closed
            if ((pSocket->iType == SOCK_STREAM) && (iRecvResult <= 0))
            {
                NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: [%p] connection %s\n", pSocket, (iRecvResult == 0) ? "closed" : "failed"));
                pSocket->iOpened = -1;
            }
            else
            {
                NetPrintf(("dirtynetnx: [%p] _SocketRecvfrom() to packet queue returned %d (err=%s)\n", pSocket, iRecvResult, DirtyErrGetName(_SocketGetErrno())));
            }
        }

        // clean up resources that were reserved for the receive operation
        SocketPacketQueueAllocUndo(pSocket->pRecvQueue);
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketSendToPacketQueue

    \Description
        Enqueue caller's data in outbound packet queue.

    \Input *pState  - pointer to module state
    \Input *pSocket - pointer to socket to send to
    \Input *pBuf    - the data to be sent
    \Input iLen     - size of data
    \Input *pTo     - the address to send to (NULL=use connection address)
    \Input iToLen   - length of address

    \Output
        int32_t     - >0: success (nb of bytes sent); 0: packet queue full; -1: packet too large for

    \Version 07/08/2020 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _SocketSendToPacketQueue(SocketStateT *pState, SocketT *pSocket, const char *pBuf, int32_t iLen, const struct sockaddr *pTo, int32_t iToLen)
{
    int32_t iResult;

    NetCritEnter(&pSocket->SendCrit);

    // early exit if packet queue is full
    if (SocketPacketQueueStatus(pSocket->pSendQueue, 'pful'))
    {
        NetCritLeave(&pSocket->SendCrit);
        return(0);
    }

    iResult = SocketPacketQueueAdd2(pSocket->pSendQueue, (const uint8_t *)pBuf, iLen, const_cast<struct sockaddr *>(pTo), (pSocket->iType == SOCK_STREAM ? TRUE : FALSE));

    NetCritLeave(&pSocket->SendCrit);

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    _SocketRecvThread

    \Description
        Wait for incoming data and deliver it immediately to the socket callback,
        if registered.

    \Input  *pArg    - pointer to Socket module state

    \Version 10/21/2004 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketRecvThread(void *pArg)
{
    typedef struct PollListT
    {
        SocketT *aSockets[SOCKET_MAXPOLL];
        struct pollfd aPollFds[SOCKET_MAXPOLL];
        int32_t iCount;
    } PollListT;

    PollListT pollList, previousPollList;
    SocketT *pSocket;
    int32_t iListIndex, iResult;
    SocketStateT *pState = (SocketStateT *)pArg;
    char strThreadId[32];

    // get the thread id
    DirtyThreadGetThreadId(strThreadId, sizeof(strThreadId));

    // show we are alive
    pState->iRecvLife = 1;
    NetPrintf(("dirtynetnx: recv thread running (thid=%s)\n", strThreadId));

    // reset contents of pollList
    ds_memclr(&pollList, sizeof(pollList));

    // loop until done
    while (pState->iRecvLife == 1)
    {
        // reset contents of previousPollList
        ds_memclr(&previousPollList, sizeof(previousPollList));

        // make a copy of the poll list used for the last poll() call
        for (iListIndex = 0; iListIndex < pollList.iCount; iListIndex++)
        {
            // copy entry from pollList to previousPollList
            previousPollList.aSockets[iListIndex] = pollList.aSockets[iListIndex];
            previousPollList.aPollFds[iListIndex] = pollList.aPollFds[iListIndex];
        }
        previousPollList.iCount = pollList.iCount;

        // reset contents of pollList in preparation for the next poll() call
        ds_memclr(&pollList, sizeof(pollList));

        // acquire global critical section for access to socket list
        NetCritEnter(NULL);

        // walk the socket list and do two things:
        //    1- if the socket is ready for reading, perform the read operation
        //    2- if the buffer in which inbound data is saved is empty, initiate a new low-level read operation for that socket
        for (pSocket = pState->pSockList; (pSocket != NULL) && (pollList.iCount < SOCKET_MAXPOLL); pSocket = pSocket->pNext)
        {
            // only handle non-virtual sockets with asyncrecv enabled
            if ((pSocket->bVirtual == FALSE) && (pSocket->uSocket != INVALID_SOCKET) && (pSocket->bAsyncRecv == TRUE))
            {
                // acquire socket critical section
                NetCritEnter(&pSocket->RecvCrit);

                // was this socket in the poll list of the previous poll() call
                for (iListIndex = 0; iListIndex < previousPollList.iCount; iListIndex++)
                {
                    if (previousPollList.aSockets[iListIndex] == pSocket)
                    {
                        // socket was in previous poll list!
                        // now check if poll() notified that this socket is ready for reading
                        if (previousPollList.aPollFds[iListIndex].revents & POLLIN)
                        {
                            /*
                            Note:
                            The poll() doc states that some error codes returned by the function
                            may only apply to one of the sockets in the poll list. For this reason,
                            we check the polling result for all entries in the list regardless
                            of the return value of poll().
                            */

                            // ready for reading, so go ahead and read
                            _SocketRecvToPacketQueue(pState, previousPollList.aSockets[iListIndex]);
                        }
                        /*
                        POLLNVAL: The file descriptor is not open. we need to exclude this socket
                        handle from being added to the poll list, otherwise the poll() will keep
                        returning 1 at once.

                        Due to the race-condition (the main thread might have recreated the socket
                        before we reach here), 'brokenflag' is accumulated rather simply set to TRUE, and
                        the socket will be excluded from the poll list only if it's marked as broken
                        twice or more ('brokenflag' is reset to 0 when recreating the socket, so actually
                        the max possible value of 'brokenflag' is 2).
                        */
                        else if (previousPollList.aPollFds[iListIndex].revents & POLLNVAL)
                        {
                            NetPrintf(("dirtynetnx: marking socket (%x->%d) as broken upon POLLNVAL\n", pSocket, pSocket->uSocket));
                            pSocket->uBrokenFlag++;
                        }
                        break;
                    }
                }

                /* if the socket is not virtual, the socket is open (TCP) and if there is room in the recv queue,
                   then add this socket to the poll list to be used by the next poll() call */
                if (!SocketPacketQueueStatus(pSocket->pRecvQueue, 'pful') && (pSocket->uSocket != INVALID_SOCKET) && (pSocket->uBrokenFlag <= 1) && ((pSocket->iType != SOCK_STREAM) || (pSocket->iOpened > 0)))
                {
                    // add socket to poll list
                    pollList.aSockets[pollList.iCount] = pSocket;
                    pollList.aPollFds[pollList.iCount].fd = pSocket->uSocket;
                    pollList.aPollFds[pollList.iCount].events = POLLIN;
                    pollList.iCount += 1;
                }

                // release socket critical section
                NetCritLeave(&pSocket->RecvCrit);
            }
        }

        // release global critical section
        NetCritLeave(NULL);

        // any sockets?
        if (pollList.iCount > 0)
        {
            // poll for data (wait up to 50ms)
            iResult = nn::socket::Poll(pollList.aPollFds, pollList.iCount, 50);

            if (iResult < 0)
            {
                NetPrintf(("dirtynetnx: poll() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));

                // stall for 50ms because experiment shows that next call to poll() may not block
                // internally if a socket is alreay in error.
                nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(50));
            }
        }
        else
        {
            // no sockets, so stall for 50ms
            nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(50));
        }
    }

    // indicate we are done
    NetPrintf(("dirtynetnx: receive thread exit\n"));
    pState->iRecvLife = 0;
}


/*F********************************************************************************/
/*!
    \Function    _SocketSendThread

    \Description
        Thread used to offload calls to nn::socket::Send() and nn::socket::SendTo()
        from main thread. These calls have proven to block for an unreasonable period
        (even if socket is configured for unblocking mode) resulting in performance
        impact for main thread.

    \Input  *pArg    - pointer to Socket module state

    \Notes
        In this function, the usage of the condition mutex and the associated crit
        takes two things into account:
            1- DirtyConditionWait() shall be called before the main thread attempts
               a call to DirtyConditionSignal() to wake this thread up. If the main
               thread calls DirtyConditionSignal() too early, then this thread
               will miss the signal and block when calling DirtyConditionWait()
               until DirtyConditionSignal() is called again.
            2- DirtyConditionWait() internally leaves the critical section before
               sleeping, and it re-enters the criticaa section when it wakes up.

    \Version 07/08/2020 (mclouatre)
*/
/********************************************************************************F*/
static void _SocketSendThread(void *pArg)
{
    SocketT *pSocket;
    SocketStateT *pState = (SocketStateT *)pArg;
    char strThreadId[32];

    // get the thread id
    DirtyThreadGetThreadId(strThreadId, sizeof(strThreadId));
    NetPrintf(("dirtynetnx: send thread running (thid=%s)\n", strThreadId));

    /* To avoid a possible race condition where the main thread would call DirtyConditionSignal()
       before we have a chance to call DirtyConditionWait(), we enter the crit before signaling the thread
       is alive. Because DirtyConditionWait() internally releases the crit before sleeping, we now have
       (1) life=alive and (2) initiate wait on signal both performed atomically. */
    NetCritEnter(&pState->SendThreadCrit);

    // show we are alive
    pState->iSendLife = 1;

    // loop until done
    while (pState->iSendLife == 1)
    {
        DirtyConditionWait(pState->pSendThreadCond, &pState->SendThreadCrit);

        /* DirtyConditionWait() internally re-enters the crit when it unblocks.
           Leave that crit explicitly here since the following activity is already
           thread-safe. */
        NetCritLeave(&pState->SendThreadCrit);

        NetCritEnter(NULL);
        // walk the socket list and flush outbound packet queues
        for (pSocket = pState->pSockList; (pSocket != NULL); pSocket = pSocket->pNext)
        {
            // only handle non-virtual sockets with async send enabled
            if ((pSocket->bVirtual == FALSE) && (pSocket->uSocket != INVALID_SOCKET) && (pSocket->bAsyncSend == TRUE))
            {
                while (_SocketSendFromPacketQueue(pSocket) > 0);
            }
        }
        NetCritLeave(NULL);

        /* Re-enter the crit before calling DirtyConditionWait again*/
        NetCritEnter(&pState->SendThreadCrit);
    }

    NetCritLeave(&pState->SendThreadCrit);

    // indicate we are done
    NetPrintf(("dirtynetnx: send thread exit\n"));
    pState->iSendLife = 0;
}

/*F********************************************************************************/
/*!
    \Function    _SocketGetMacAddress

    \Description
        Attempt to retreive MAC address of the system.

    \Input *pState  - pointer to module state

    \Output
        uint8_t     - TRUE if MAC address found, FALSE otherwise

    \Notes
        Usage of getifaddrs() is preferred over usage of ioctl() with a socket to save
        the socket creation step. However, not all platforms support the AF_LINK address
        family. In those cases, usage of ioctl() can't be avoided.

    \Version 05/12/2004 (mclouatre)
*/
/********************************************************************************F*/
static uint8_t _SocketGetMacAddress(SocketStateT *pState)
{
    NetPrintf(("dirtynetnx: can't do MAC address query with ioctl(SIOCGIFHWADDR)\n"));
    return(FALSE);
}

/*F********************************************************************************/
/*!
    \Function    _SocketInfoGlobal

    \Description
        Return information about global state

    \Input iInfo    - selector for desired information
    \Input iData    - selector specific
    \Input *pBuf    - return buffer
    \Input iLen     - buffer length

    \Output
        int32_t     - selector-specific

    \Notes
        These selectors need to be documented in SocketInfo() to allow our
        documentation generation to pick them up.

    \Version 03/31/2017 (eesponda)
*/
/********************************************************************************F*/
static int32_t _SocketInfoGlobal(int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen)
{
    SocketStateT *pState = _Socket_pState;

    if (iInfo == 'addr')
    {
        if (pState->uLocalAddr == 0)
        {
            in_addr netOutInAddr;
            nn::Result result = nn::nifm::GetCurrentPrimaryIpAddress(&netOutInAddr);
            if (result.IsFailure())
            {
                NetPrintf(("dirtynetnx: unable to get IP address, error = %d\n", result.GetDescription()));
            }
            else
            {
                pState->uLocalAddr = nn::socket::InetNtohl(netOutInAddr.s_addr);
            }
        }
        return(pState->uLocalAddr);
    }
    // get socket bound to given port
    if ((iInfo == 'bind') || (iInfo == 'bndu'))
    {
        SocketT *pSocket;
        struct sockaddr BindAddr;
        int32_t iFound = -1;

        // for access to socket list
        NetCritEnter(NULL);

        // walk socket list and find matching socket
        for (pSocket = pState->pSockList; pSocket != NULL; pSocket = pSocket->pNext)
        {
            // if iInfo is 'bndu', only consider sockets of type SOCK_DGRAM
            // note: 'bndu' stands for "bind udp"
            if ((iInfo == 'bind') || ((iInfo == 'bndu') && (pSocket->iType == SOCK_DGRAM)))
            {
                // get socket info
                SocketInfo(pSocket, 'bind', 0, &BindAddr, sizeof(BindAddr));
                if (SockaddrInGetPort(&BindAddr) == iData)
                {
                    *(SocketT **)pBuf = pSocket;
                    iFound = 0;
                    break;
                }
            }
        }

        // for access to g_socklist and g_sockkill
        NetCritLeave(NULL);
        return(iFound);
    }

    if (iInfo == 'conn')
    {
        return(pState->uConnStatus);
    }

    // get MAC address
    if ((iInfo == 'ethr') || (iInfo == 'macx'))
    {
        uint8_t aZeros[6] = { 0, 0, 0, 0, 0, 0 };
        uint8_t bFound = TRUE;

        // early exit if user-provided buffer not correct
        if ((pBuf == NULL) && (iLen < (signed)sizeof(pState->aMacAddr)))
        {
            return(-1);
        }

        // try to get mac address if we don't already have it
        if (!memcmp(pState->aMacAddr, aZeros, sizeof(pState->aMacAddr)))
        {
            bFound = _SocketGetMacAddress(pState);
        }

        if (bFound)
        {
            // copy MAC address in user-provided buffer and signal success
            ds_memcpy(pBuf, &pState->aMacAddr, sizeof(pState->aMacAddr));
            return(0);
        }

        // signal failure - no MAC address found
        return(-1);
    }

    // return max packet size
    if (iInfo == 'maxp')
    {
        return(pState->iMaxPacket);
    }

    // get send callback function pointer (iData specifies index in array)
    if (iInfo == 'sdcf')
    {
        if ((pBuf != NULL) && (iLen == sizeof(pState->aSendCbEntries[iData].pSendCallback)))
        {
            ds_memcpy(pBuf, &pState->aSendCbEntries[iData].pSendCallback, sizeof(pState->aSendCbEntries[iData].pSendCallback));
            return(0);
        }

        NetPrintf(("dirtynetnx: 'sdcf' selector used with invalid paramaters\n"));
        return(-1);
    }
    // get send callback user data pointer (iData specifies index in array)
    if (iInfo == 'sdcu')
    {
        if ((pBuf != NULL) && (iLen == sizeof(pState->aSendCbEntries[iData].pSendCallref)))
        {
            ds_memcpy(pBuf, &pState->aSendCbEntries[iData].pSendCallref, sizeof(pState->aSendCbEntries[iData].pSendCallref));
            return(0);
        }

        NetPrintf(("dirtynetnx: 'sdcu' selector used with invalid paramaters\n"));
        return(-1);
    }
    // return global debug output level
    if (iInfo == 'spam')
    {
        return(pState->iVerbose);
    }

    NetPrintf(("dirtynetnx: unhandled global SocketInfo() selector '%C'\n", iInfo));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    _SocketControlGlobal

    \Description
        Process a global control message (type specific operation)

    \Input iOption  - the option to pass
    \Input iData1   - message specific parm
    \Input *pData2  - message specific parm
    \Input *pData3  - message specific parm

    \Output
        int32_t     - message specific result (-1=unsupported message)

    \Notes
        These selectors need to be documented in SocketControl() to allow our
        documentation generation to pick them up.

    \Version 03/31/2017 (eesponda)
*/
/********************************************************************************F*/
static int32_t _SocketControlGlobal(int32_t iOption, int32_t iData1, void *pData2, void *pData3)
{
    SocketStateT *pState = _Socket_pState;

    // set async send for all sockets
    if (iOption == 'asnd')
    {
        pState->bAsyncSend = iData1 ? TRUE : FALSE;
        NetPrintf(("dirtynetnx: async send %s for all non-virtual sockets\n", pState->bAsyncSend ? "enabled" : "disabled"));
        return(0);
    }

    // init network stack and bring up interface
    if (iOption == 'conn')
    {
        NetCritEnter(&pState->nifmCrit);

        NetPrintf(("dirtynetnx: setting network ~con state.\n"));
        if (pState->eSocketNIFMState == SOCKET_NIFM_UNINITIALIZED)
        {
            pState->uConnStatus = '~con';
            pState->uNifmRetry = SOCKET_MAX_NIFM_RETRY;
            pState->eSocketNIFMState = SOCKET_NIFM_INITIALIZING;
        }
        else if (pState->eSocketNIFMState == SOCKET_NIFM_IP_OPTAINED)
        {
            // in this case we are already logged in
            // acquire address/mac address
            SocketInfo(NULL, 'addr', 0, NULL, 0);
            SocketInfo(NULL, 'macx', 0, NULL, 0);//

            // mark us as online
            NetPrintf(("dirtynetnx: addr=%a mac=%02x:%02x:%02x:%02x:%02x:%02x\n", pState->uLocalAddr, pState->aMacAddr[0], pState->aMacAddr[1], pState->aMacAddr[2], pState->aMacAddr[3], pState->aMacAddr[4], pState->aMacAddr[5]));
            pState->uConnStatus = '+onl';
        }

        NetCritLeave(&pState->nifmCrit);
        return(0);
    }
    // bring down interface
    if (iOption == 'disc')
    {
        NetCritEnter(&pState->nifmCrit);

        NetPrintf(("dirtynetnx: disconnecting from network\n"));
        pState->eSocketNIFMState = SOCKET_NIFM_UNINITIALIZED;
        pState->uConnStatus = '-off';

        NetCritLeave(&pState->nifmCrit);
        return(0);
    }
    // set max udp packet size
    if (iOption == 'maxp')
    {
        NetPrintf(("dirtynetnx: setting max udp packet size to %d\n", iData1));
        pState->iMaxPacket = iData1;
        return(0);
    }
    // set/unset send callback (iData1=TRUE for set - FALSE for unset, pData2=callback, pData3=callref)
    if (iOption == 'sdcb')
    {
        SocketSendCallbackEntryT sendCbEntry;
        sendCbEntry.pSendCallback = (SocketSendCallbackT *)pData2;
        sendCbEntry.pSendCallref = pData3;

        if (iData1)
        {
            return(SocketSendCallbackAdd(&pState->aSendCbEntries[0], &sendCbEntry));
        }
        else
        {
            return(SocketSendCallbackRem(&pState->aSendCbEntries[0], &sendCbEntry));
        }
    }
    // silent failure (to display first party dialogs or not)
    if (iOption == 'sfal')
    {
        _Socket_bSilentFail = iData1;
        return(0);
    }
    // skip socket library initialization or not
    if (iOption == 'sint')
    {
        _Socket_bSocketInitialize = iData1;
        return(0);
    }
    // set debug spam level
    if (iOption == 'spam')
    {
        // module level debug level
        pState->iVerbose = iData1;
        return(0);
    }
    // mark a port as virtual
    if (iOption == 'vadd')
    {
        int32_t iPort;

        // find a slot to add virtual port
        for (iPort = 0; pState->aVirtualPorts[iPort] != 0; iPort++)
            ;
        if (iPort < SOCKET_MAXVIRTUALPORTS)
        {
            NetPrintfVerbose((pState->iVerbose, 1, "dirtynetnx: added port %d to virtual port list\n", iData1));
            pState->aVirtualPorts[iPort] = (uint16_t)iData1;
            return(0);
        }
    }
    // remove port from virtual port list
    if (iOption == 'vdel')
    {
        int32_t iPort;

        // find virtual port in list
        for (iPort = 0; (iPort < SOCKET_MAXVIRTUALPORTS) && (pState->aVirtualPorts[iPort] != (uint16_t)iData1); iPort++)
            ;
        if (iPort < SOCKET_MAXVIRTUALPORTS)
        {
            NetPrintfVerbose((pState->iVerbose, 1, "dirtynetnx: removed port %d from virtual port list\n", iData1));
            pState->aVirtualPorts[iPort] = 0;
            return(0);
        }
    }
    // unhandled
    NetPrintf(("dirtynetnx: unhandled global SocketControl() option '%C'\n", iOption));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    _SocketCreateThread()

    \Description
        Create transmission or reception thread.

    \Input *pState              - pointer to module state
    \Input *pThreadName         - thread name
    \Input *pThreadFct          - thread function
    \Input iThreadPrio          - priority to start threads with
    \Input iThreadCpuAffinity   - cpu affinity to start threads with

    \Output
        int32_t                 - 0: success; negative: failure

    \Version 07/09/2020 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _SocketCreateThread(SocketStateT *pState, const char *pThreadName, void pThreadFct(void *), int32_t iThreadPrio, int32_t iThreadCpuAffinity)
{
    // create high-priority receive thread
    DirtyThreadConfigT ThreadConfig;

    // configure threading for receive thread
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = pThreadName;
    ThreadConfig.iAffinity = iThreadCpuAffinity;
    ThreadConfig.iPriority = iThreadPrio;
    ThreadConfig.iVerbosity = pState->iVerbose;

    return(DirtyThreadCreate(pThreadFct, pState, &ThreadConfig));
}



/*** Public Functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function    SocketCreate

    \Description
        Create new instance of socket interface module.  Initializes all global
        resources and makes module ready for use.

    \Input iThreadPrio        - priority to start threads with
    \Input iThreadStackSize   - stack size to start threads with (in bytes)
    \Input iThreadCpuAffinity - cpu affinity to start threads with

    \Output
        int32_t               - negative=error, zero=success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketCreate(int32_t iThreadPrio, int32_t iThreadStackSize, int32_t iThreadCpuAffinity)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    int32_t iResult;

    // Query mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // error if already started
    if (pState != NULL)
    {
        NetPrintf(("dirtynetnx: SocketCreate() called while module is already active\n"));
        return(DIRTYAPI_SOCKET_ERR_ALREADY_ACTIVE);
    }

    // print version info
    NetPrintf(("dirtynetnx: DirtySDK v%d.%d.%d.%d.%d\n", DIRTYSDK_VERSION_YEAR, DIRTYSDK_VERSION_SEASON, DIRTYSDK_VERSION_MAJOR, DIRTYSDK_VERSION_MINOR, DIRTYSDK_VERSION_PATCH));

    // alloc and init state ref
    if ((pState = (SocketStateT *)DirtyMemAlloc(sizeof(*pState), SOCKET_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetnx: unable to allocate module state\n"));
        return(DIRTYAPI_SOCKET_ERR_NO_MEMORY);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->iMaxPacket = SOCKET_MAXUDPRECV;
    pState->iVerbose = 1;
    pState->bSocketInit = _Socket_bSocketInitialize;

    // init the critical section
    NetCritInit(&pState->nifmCrit, "dirtynetnx-nifmcrit");

    // startup network libs
    NetLibCreate(iThreadPrio, iThreadStackSize, iThreadCpuAffinity);

    // add our idle handler
    NetIdleAdd(&_SocketIdle, pState);

    // create hostname cache
    if ((pState->pHostnameCache = SocketHostnameCacheCreate(iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetnx: unable to create hostname cache\n"));
        SocketDestroy((uint32_t)(-1));
        return(DIRTYAPI_SOCKET_ERR_HOST_NAME_CACHE);
    }

    if ((iResult = _SocketCreateThread(pState, "SocketRecv", _SocketRecvThread, iThreadStackSize, iThreadCpuAffinity)) == 0)
    {
        // wait for receive thread startup
        while (pState->iRecvLife == 0)
        {
            nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(1));
        }
    }
    else
    {
        NetPrintf(("dirtynetnx: unable to create recv thread (err=%d)\n", iResult));
        pState->iRecvLife = 0;
    }

    // allocate resources required to synchronize send thread
    if ((pState->pSendThreadCond = DirtyConditionCreate("SendThreadCond")) == NULL)
    {
        NetPrintf(("dirtynetnx: unable to initialize SendThread condition\n"));
        SocketDestroy(0);
        return(DIRTYAPI_SOCKET_ERR_PLATFORM_SPECIFIC);
    }
    NetCritInit(&pState->SendThreadCrit, "SendThreadCrit");

    if ((iResult = _SocketCreateThread(pState, "SocketSend", _SocketSendThread, iThreadStackSize, iThreadCpuAffinity)) == 0)
    {
        // wait for send thread startup
        while (pState->iSendLife == 0)
        {
            nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(1));
        }
    }
    else
    {
        NetPrintf(("dirtynetnx: unable to create send thread (err=%d)\n", iResult));
        pState->iSendLife = 0;
    }

    // save state
    _Socket_pState = pState;

    // initialize NIFM
    SocketControl(NULL, 'conn', 0, NULL, NULL);
    while ((pState->uNifmRetry != 0) && (pState->eSocketNIFMState != SocketNIFMStateE::SOCKET_NIFM_IP_OPTAINED))
    {
        NetCritEnter(&pState->nifmCrit);
        _SocketNIFMUpdate(pState);
        NetCritLeave(&pState->nifmCrit);
    }

    if (pState->eSocketNIFMState != SocketNIFMStateE::SOCKET_NIFM_IP_OPTAINED)
    {
        NetPrintf(("dirtynetnx: unable to initialize NIFM, NIFM state = %i\n", pState->eSocketNIFMState));
        SocketDestroy(0);
        return(DIRTYAPI_SOCKET_ERR_PLATFORM_SPECIFIC);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function    SocketDestroy

    \Description
        Release resources and destroy module.

    \Input uShutdownFlags   - shutdown flags

    \Output
        int32_t             - negative=error, zero=success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketDestroy(uint32_t uShutdownFlags)
{
    SocketStateT *pState = _Socket_pState;

    // error if not active
    if (pState == NULL)
    {
        NetPrintf(("dirtynetnx: SocketDestroy() called while module is not active\n"));
        return(-1);
    }

    NetPrintf(("dirtynetnx: shutting down\n"));

    // wait until all lookup threads are done
    while (pState->pHostList != NULL)
    {
        volatile HostentT **ppHost;
        int32_t iSocketLookups;

        // check for lookup threads that are still active
        for (ppHost = (volatile HostentT **)&pState->pHostList, iSocketLookups = 0; *ppHost != NULL; ppHost = (volatile HostentT **)&(*ppHost)->pNext)
        {
            iSocketLookups += (*ppHost)->thread ? 0 : 1;
        }
        // if no ongoing socket lookups, we're done
        if (iSocketLookups == 0)
        {
            break;
        }
        NetConnSleep(1);
    }

    // kill idle callbacks
    NetIdleDel(&_SocketIdle, pState);

    // let any idle event finish
    NetIdleDone();

    if (pState->iRecvLife == 1)
    {
        // tell receive thread to quit
        pState->iRecvLife = 2;
        // wait for thread to terminate
        while (pState->iRecvLife > 0)
        {
            nn::os::SleepThread(nn::TimeSpan::FromMicroSeconds(1000));
        }
    }

    if (pState->iSendLife == 1)
    {
        // tell send thread to quit
        pState->iSendLife = 2;

        // wake send thread up
        NetCritEnter(&pState->SendThreadCrit);
        if (!DirtyConditionSignal(pState->pSendThreadCond))
        {
            NetPrintf(("dirtynetnx: failed to wake up send thread\n"));
        }
        NetCritLeave(&pState->SendThreadCrit);

        // wait for thread to terminate
        while (pState->iSendLife > 0)
        {
            nn::os::SleepThread(nn::TimeSpan::FromMicroSeconds(1000));
        }
    }

    // release resources associated with send thread
    if (pState->pSendThreadCond != NULL)
    {
        DirtyConditionDestroy(pState->pSendThreadCond);
        pState->pSendThreadCond = NULL;
    }
    NetCritKill(&pState->SendThreadCrit);


    // close any remaining sockets
    NetCritEnter(NULL);
    while (pState->pSockList != NULL)
    {
        SocketClose(pState->pSockList);
    }
    _SocketFinalize();

    NetCritLeave(NULL);

    // clear the kill list
    _SocketIdle(pState);

    // destroy hostname cache
    if (pState->pHostnameCache != NULL)
    {
        SocketHostnameCacheDestroy(pState->pHostnameCache);
    }

    // destroy the crit
    NetCritKill(&pState->nifmCrit);

    // shut down network libs
    NetLibDestroy(0);

    // dispose of state
    DirtyMemFree(pState, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    _Socket_pState = NULL;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    SocketOpen

    \Description
        Create a new transfer endpoint. A socket endpoint is required for any
        data transfer operation.

    \Input iAddrFamily  - address family (AF_INET)
    \Input iType        - socket type (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...)
    \Input iProto       - protocol type for SOCK_RAW (unused by others)

    \Output
        SocketT *       - socket reference

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
SocketT *SocketOpen(int32_t iAddrFamily, int32_t iType, int32_t iProto)
{
    return(_SocketOpen(-1, iAddrFamily, iType, iProto, 0));
}

/*F********************************************************************************/
/*!
    \Function    SocketClose

    \Description
        Close a socket. Performs a graceful shutdown of connection oriented protocols.

    \Input *pSocket     - socket reference

    \Output
        int32_t         - negative=error, else zero

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketClose(SocketT *pSocket)
{
    int32_t iSocket = pSocket->uSocket;

    // stop sending
    SocketShutdown(pSocket, SOCK_NOSEND);

    // dispose of SocketT
    if (_SocketClose(pSocket) < 0)
    {
        return(-1);
    }

    // close unix socket if allocated
    if (iSocket >= 0)
    {
        // close socket
        if (nn::socket::Close(iSocket) < 0)
        {
            NetPrintf(("dirtynetnx: close() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
        }
    }

    // success
    return(0);
}

/*F********************************************************************************/
/*!
    \Function SocketImport

    \Description
        Import a socket.  The given socket ref may be a SocketT, in which case a
        SocketT pointer to the ref is returned, or it can be an actual unix socket ref,
        in which case a SocketT is created for the unix socket ref.

    \Input uSockRef - socket reference

    \Output
        SocketT *   - pointer to imported socket, or NULL

    \Version 01/14/2005 (jbrookes)
*/
/********************************************************************************F*/
SocketT *SocketImport(intptr_t uSockRef)
{
    SocketStateT *pState = _Socket_pState;
    socklen_t iProtoSize;
    int32_t iProto;
    SocketT *pSock;

    // see if this socket is already in our socket list
    NetCritEnter(NULL);
    for (pSock = pState->pSockList; pSock != NULL; pSock = pSock->pNext)
    {
        if (pSock == (SocketT *)uSockRef)
        {
            break;
        }
    }
    NetCritLeave(NULL);

    // if socket is in socket list, just return it
    if (pSock != NULL)
    {
        return(pSock);
    }

    // get info from socket ref
    iProtoSize = sizeof(iProto);
    if (nn::socket::GetSockOpt((int32_t)uSockRef, SOL_SOCKET, SO_TYPE, &iProto, &iProtoSize) == 0)
    {
        // create the socket
        pSock = _SocketOpen((int32_t)uSockRef, AF_INET, iProto, 0, 0);

        // update local and remote addresses
        SocketInfo(pSock, 'bind', 0, &pSock->LocalAddr, sizeof(pSock->LocalAddr));
        SocketInfo(pSock, 'peer', 0, &pSock->RemoteAddr, sizeof(pSock->RemoteAddr));

        // mark it as imported
        pSock->bImported = TRUE;
    }
    else
    {
        NetPrintf(("dirtynetnx: getsockopt(SO_TYPE) failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
    }

    return(pSock);
}

/*F********************************************************************************/
/*!
    \Function SocketRelease

    \Description
        Release an imported socket.

    \Input *pSocket - pointer to socket

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
void SocketRelease(SocketT *pSocket)
{
    // if it wasn't imported, nothing to do
    if (pSocket->bImported == FALSE)
    {
        return;
    }

    // dispose of SocketT, but leave the sockref alone
    _SocketClose(pSocket);
}

/*F********************************************************************************/
/*!
    \Function    SocketShutdown

    \Description
        Perform partial/complete shutdown of socket indicating that either sending
        and/or receiving is complete.

    \Input *pSocket - socket reference
    \Input iHow     - SOCK_NOSEND and/or SOCK_NORECV

    \Output
        int32_t     - negative=error, else zero

    \Version 09/10/2004 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketShutdown(SocketT *pSocket, int32_t iHow)
{
    int32_t iResult = 0;

    // only shutdown a connected socket
    if (pSocket->iType != SOCK_STREAM)
    {
        pSocket->iLastError = SOCKERR_NONE;
        return(pSocket->iLastError);
    }

    // make sure socket ref is valid
    if (pSocket->uSocket == INVALID_SOCKET)
    {
        pSocket->iLastError = SOCKERR_NONE;
        return(pSocket->iLastError);
    }

    // translate how
    if (iHow == SOCK_NOSEND)
    {
        iHow = SHUT_WR;
    }
    else if (iHow == SOCK_NORECV)
    {
        iHow = SHUT_RD;
    }
    else if (iHow == (SOCK_NOSEND | SOCK_NORECV))
    {
        iHow = SHUT_RDWR;
    }

    // do the shutdown
    if (nn::socket::Shutdown(pSocket->uSocket, iHow) < 0)
    {
        iResult = _SocketGetErrno();

        // log only useful messages
        if (iResult != (int32_t)nn::socket::Errno::ENotConn)
        {
            NetPrintf(("dirtynetnx: shutdown() failed (err=%s)\n", DirtyErrGetName(iResult)));
        }
    }

    pSocket->iLastError = _SocketTranslateError(iResult);
    return(pSocket->iLastError);
}

/*F********************************************************************************/
/*!
    \Function    SocketBind

    \Description
        Bind a local address/port to a socket.

    \Input *pSocket - socket reference
    \Input *pName   - local address/port
    \Input iNameLen - length of name

    \Output
        int32_t     - standard network error code (SOCKERR_xxx)

    \Notes
        If either address or port is zero, then they are filled in automatically.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketBind(SocketT *pSocket, const struct sockaddr *pName, int32_t iNameLen)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iResult;

    // make sure socket is valid
    if (pSocket->uSocket < 0)
    {
        NetPrintf(("dirtynetnx: attempt to bind invalid socket\n"));
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // save local address
    ds_memcpy_s(&pSocket->LocalAddr, sizeof(pSocket->LocalAddr), pName, sizeof(*pName));

    // is the bind port a virtual port?
    if (pSocket->iType == SOCK_DGRAM)
    {
        int32_t iPort;
        uint16_t uPort;

        if ((uPort = SockaddrInGetPort(pName)) != 0)
        {
            // find virtual port in list
            for (iPort = 0; (iPort < SOCKET_MAXVIRTUALPORTS) && (pState->aVirtualPorts[iPort] != uPort); iPort++)
                ;
            if (iPort < SOCKET_MAXVIRTUALPORTS)
            {
                // acquire socket critical section
                NetCritEnter(&pSocket->RecvCrit);

                // check to see if the socket is bound
                if (pSocket->bVirtual && (pSocket->uVirtualPort != 0))
                {
                    NetPrintf(("dirtynetnx: [%p] failed to bind socket to %u which was already bound to port %u virtual\n", pSocket, uPort, pSocket->uVirtualPort));
                    NetCritLeave(&pSocket->RecvCrit);
                    return(pSocket->iLastError = SOCKERR_INVALID);
                }

                // close winsock socket
                NetPrintf(("dirtynetnx: [%p] making socket bound to port %d virtual\n", pSocket, uPort));
                if (pSocket->uSocket != INVALID_SOCKET)
                {
                    nn::socket::Shutdown(pSocket->uSocket, SOCK_NOSEND);
                    nn::socket::Close(pSocket->uSocket);
                    pSocket->uSocket = INVALID_SOCKET;
                }
                /* increase socket queue size; this protects virtual sockets from having data pushed into
                them and overwriting previous data that hasn't been read yet */
                pSocket->pRecvQueue = SocketPacketQueueResize(pSocket->pRecvQueue, 4, pState->iMemGroup, pState->pMemGroupUserData);
                // mark socket as virtual
                pSocket->uVirtualPort = uPort;
                pSocket->bVirtual = TRUE;

                // release socket critical section
                NetCritLeave(&pSocket->RecvCrit);
                return(0);
            }
        }
    }

    // do the bind
    if ((iResult = nn::socket::Bind(pSocket->uSocket, pName, iNameLen)) < 0)
    {
        NetPrintf(("dirtynetnx: bind() to port %d failed (err=%s)\n", SockaddrInGetPort(pName), DirtyErrGetName(_SocketGetErrno())));
    }
    else if (SockaddrInGetPort(&pSocket->LocalAddr) == 0)
    {
        iNameLen = sizeof(pSocket->LocalAddr);
        iResult = nn::socket::GetSockName(pSocket->uSocket, &pSocket->LocalAddr, (socklen_t *)&iNameLen);
        NetPrintf(("dirtynetnx: bind(port=0) succeeded, local address=%a:%d.\n",
            SockaddrInGetAddr(&pSocket->LocalAddr),
            SockaddrInGetPort(&pSocket->LocalAddr)));
    }

    pSocket->iLastError = _SocketTranslateError(iResult);
    return(pSocket->iLastError);
}

/*F********************************************************************************/
/*!
    \Function    SocketConnect

    \Description
        Initiate a connection attempt to a remote host.

    \Input *pSocket - socket reference
    \Input *pName   - pointer to name of socket to connect to
    \Input iNameLen - length of name

    \Output
        int32_t     - standard network error code (SOCKERR_xxx)

    \Notes
        Only has real meaning for stream protocols. For a datagram protocol, this
        just sets the default remote host.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketConnect(SocketT *pSocket, struct sockaddr *pName, int32_t iNameLen)
{
    int32_t iResult;

    NetPrintf(("dirtynetnx: connecting to %a:%d\n", SockaddrInGetAddr(pName), SockaddrInGetPort(pName)));

    // do the connect
    pSocket->iOpened = 0;
    if ((iResult = nn::socket::Connect(pSocket->uSocket, pName, iNameLen)) == 0)
    {
        // if connect succeeded (usually for udp sockets) or attempting to establish a non-blocking connection save correct address
        ds_memcpy_s(&pSocket->RemoteAddr, sizeof(pSocket->RemoteAddr), pName, sizeof(*pName));
    }
    else if (_SocketGetErrno() == (int32_t)nn::socket::Errno::EHostUnreach)
    {
        /* if host is unreachable, purge from hostname cache (if present).  this is helpful in
        situations where the addressing scheme has changed; for example if a device switches
        from an IPv4 hosted connection to an IPv6 connection.  in such a case the old (now
        invalid) address is purged from the cache more quickly than if we waited for it to
        expire from the normal cache timeout */
        SocketHostnameCacheDel(_Socket_pState->pHostnameCache, NULL, SockaddrInGetAddr(pName), _Socket_pState->iVerbose);
    }
    else if (_SocketGetErrno() != (int32_t)nn::socket::Errno::EInProgress)
    {
        NetPrintf(("dirtynetnx: connect() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
    }

    pSocket->iLastError = _SocketTranslateError(iResult);
    return(pSocket->iLastError);
}

/*F********************************************************************************/
/*!
    \Function    SocketListen

    \Description
        Start listening for an incoming connection on the socket.  The socket must already
        be bound and a stream oriented connection must be in use.

    \Input *pSocket - socket reference to bound socket (see SocketBind())
    \Input iBacklog - number of pending connections allowed

    \Output
        int32_t     - standard network error code (SOCKERR_xxx)

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketListen(SocketT *pSocket, int32_t iBacklog)
{
    int32_t iResult;

    // do the listen
    if ((iResult = nn::socket::Listen(pSocket->uSocket, iBacklog)) < 0)
    {
        NetPrintf(("dirtynetnx: listen() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
    }

    pSocket->iLastError = _SocketTranslateError(iResult);
    return(pSocket->iLastError);
}

/*F********************************************************************************/
/*!
    \Function    SocketAccept

    \Description
        Accept an incoming connection attempt on a socket.

    \Input *pSocket     - socket reference to socket in listening state (see SocketListen())
    \Input *pAddr       - pointer to storage for address of the connecting entity, or NULL
    \Input *pAddrLen    - pointer to storage for length of address, or NULL

    \Output
        SocketT *       - the accepted socket, or NULL if not available

    \Notes
        The integer pointed to by addrlen should on input contain the number of characters
        in the buffer addr.  On exit it will contain the number of characters in the
        output address.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
SocketT *SocketAccept(SocketT *pSocket, struct sockaddr *pAddr, int32_t *pAddrLen)
{
    SocketT *pOpen = NULL;
    int32_t iResult;

    pSocket->iLastError = SOCKERR_INVALID;

    // make sure we have an INET socket
    if ((pSocket->uSocket == INVALID_SOCKET) || (pSocket->iFamily != AF_INET))
    {
        NetPrintf(("dirtynetnx: accept() called on invalid socket\n"));
        return(NULL);
    }

    // make sure turn parm is valid
    if ((pAddr != NULL) && (*pAddrLen < (signed)sizeof(struct sockaddr)))
    {
        NetPrintf(("dirtynetnx: accept() called with invalid address\n"));
        return(NULL);
    }

    // perform inet accept
    if (pAddrLen)
    {
        *pAddrLen = sizeof(*pAddr);
    }

    iResult = nn::socket::Accept(pSocket->uSocket, pAddr, (socklen_t *)pAddrLen);

    if (iResult > 0)
    {
        // Allocate socket structure and install in list
        pOpen = _SocketOpen(iResult, pSocket->iFamily, pSocket->iType, pSocket->iProto, 1);
        pSocket->iLastError = SOCKERR_NONE;
    }
    else
    {
        pSocket->iLastError = _SocketTranslateError(iResult);

        if (_SocketGetErrno() != (int32_t)nn::socket::Errno::EWouldBlock)
        {
            NetPrintf(("dirtynetnx: accept() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
        }
    }

    // return the socket
    return(pOpen);
}

/*F********************************************************************************/
/*!
    \Function    SocketSendto

    \Description
        Send data to a remote host. The destination address is supplied along with
        the data. Should only be used with datagram sockets as stream sockets always
        send to the connected peer.

    \Input *pSocket - socket reference
    \Input *pBuf    - the data to be sent
    \Input iLen     - size of data
    \Input iFlags   - unused
    \Input *pTo     - the address to send to (NULL=use connection address)
    \Input iToLen   - length of address

    \Output
        int32_t     - >0: number of bytes sent, 0: outbound queue full, <0: SOCKERR_XXX

    \Notes
        UDP datagrams larger than SOCKET_MAXUDPRECV bytes are not supported with async
        send mode.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketSendto(SocketT *pSocket, const char *pBuf, int32_t iLen, int32_t iFlags, const struct sockaddr *pTo, int32_t iToLen)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iResult = -1;

    if (pSocket->bSendCbs)
    {
        // if installed, give socket callback right of first refusal
        if ((iResult = SocketSendCallbackInvoke(&pState->aSendCbEntries[0], pSocket, pSocket->iType, pBuf, iLen, pTo)) > 0)
        {
            return(iResult);
        }
    }

    // make sure socket ref is valid
    if (pSocket->uSocket < 0)
    {
#if DIRTYCODE_LOGGING
        uint32_t uAddr = 0, uPort = 0;
        if (pTo)
        {
            uAddr = SockaddrInGetAddr(pTo);
            uPort = SockaddrInGetPort(pTo);
        }
        NetPrintf(("dirtynetnx: attempting to send to %a:%d on invalid socket\n", uAddr, uPort));
#endif
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // handle optional data rate throttling
    if ((iLen = SocketRateThrottle(&pSocket->SendRate, pSocket->iType, iLen, "send")) == 0)
    {
        return(0);
    }

    // synchronous send or asynchronous send (offloaded to dedicated send thread)
    if ((pSocket->bVirtual == TRUE) || (pSocket->uSocket == INVALID_SOCKET) || (pSocket->bAsyncSend == FALSE))
    {
        /* synchronous send */

        // use appropriate version
        if (pTo == NULL)
        {
            if ((iResult = (int32_t)nn::socket::Send(pSocket->uSocket, pBuf, iLen, 0)) < 0)
            {
                if (_SocketGetErrno() != (int32_t)nn::socket::Errno::EWouldBlock)
                    NetPrintf(("dirtynetnx: send() failed (err=%s)\n", DirtyErrGetName(_SocketGetErrno())));
            }
        }
        else
        {
            // do the send
            #if SOCKET_VERBOSE
            NetPrintf(("dirtynetnx: sending %d bytes to %a:%d\n", iLen, SockaddrInGetAddr(pTo), SockaddrInGetPort(pTo)));
            #endif

            if ((iResult = (int32_t)nn::socket::SendTo(pSocket->uSocket, pBuf, iLen, 0, pTo, iToLen)) < 0)
            {
                NetPrintf(("dirtynetnx: sendto(%a:%d) failed (err=%s)\n", SockaddrInGetAddr(pTo), SockaddrInGetPort(pTo), DirtyErrGetName(_SocketGetErrno())));
            }
        }
        // translate error
        pSocket->iLastError = iResult = _SocketTranslateError(iResult);
        if (iResult == SOCKERR_BADPIPE)
        {
            // recreate socket (iLastError will be set in _SocketReopen)
            if (_SocketReopen(pSocket))
            {
                // success, re-send (should either work or error out)
                return(SocketSendto(pSocket, pBuf, iLen, iFlags, pTo, iToLen));
            }
            // failed to recreate the socket, error
        }
    }
    else
    {
        /* asynchronous send */

        iResult = _SocketSendToPacketQueue(pState, pSocket, pBuf, iLen, pTo, iToLen);

        NetCritEnter(&pState->SendThreadCrit);
        if (iResult >= 0)
        {
            /* wake up the socket send thread
               Note: When iResult is 0, it means the send queue is full, which indicates that
               the socket send thread missed previous wakeup attempts. This phenomenon can happen
               when the wakeup attempt is being done before the socket send thread is blocked on
               DirtyConditionWait(). To mitigate it, we re-attempt a socket send thread wake up here
               upon full queue detection and from _SocketIdle(), upon non-empty queue detection. */
            if (!DirtyConditionSignal(pState->pSendThreadCond))
            {
                NetPrintf(("dirtynetnx: failed to wake up send thread\n"));
            }
        }
        else if (iResult == -1)
        {
            // packet too large
            pSocket->iLastError = SOCKERR_UNSUPPORT;
            NetPrintf(("dirtynetnx: UDP datagrams larger than SOCKET_MAXUDPRECV bytes are not supported with async send mode\n"));
        }
        else if (iResult != 0)
        {
            pSocket->iLastError = SOCKERR_OTHER;
            NetPrintf(("dirtynetnx: unhandled error in asynchronous send path.\n"));
        }
        NetCritLeave(&pState->SendThreadCrit);
    }

    // update data rate estimation
    SocketRateUpdate(&pSocket->SendRate, iResult, "send");
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    SocketRecvfrom

    \Description
        Receive data from a remote host. If socket is a connected stream, then data can
        only come from that source. A datagram socket can receive from any remote host.

    \Input *pSocket     - socket reference
    \Input *pBuf        - buffer to receive data
    \Input iLen         - length of recv buffer
    \Input iFlags       - unused
    \Input *pFrom       - address data was received from (NULL=ignore)
    \Input *pFromLen    - length of address

    \Output
        int32_t         - positive=data bytes received, else standard error code

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketRecvfrom(SocketT *pSocket, char *pBuf, int32_t iLen, int32_t iFlags, struct sockaddr *pFrom, int32_t *pFromLen)
{
    int32_t iRecv = -1, iErrno = 0;

    // clear "hasdata" hint
    pSocket->bHasData = 0;

    // handle rate throttling, if enabled
    if ((iLen = SocketRateThrottle(&pSocket->RecvRate, pSocket->iType, iLen, "recv")) == 0)
    {
        return(0);
    }
    // handle if the socket was killed
    if (pSocket->pKill != NULL)
    {
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    /* sockets marked for async recv had actual receive operation take place in the thread. sockets marked as virtual have the
    packet pushed into them (specific to unix as we disable async receive for singlethreaded mode). */
    if ((pSocket->bAsyncRecv == TRUE) || (pSocket->bVirtual == TRUE))
    {
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);

        /* given the socket could be either a TCP or UDP socket we handle the no data condition the same.
           this is due to the below, when we do error conversion we override the system error with EWOULDBLOCK because
           with TCP zero would be mean closed. if we are doing direct recv calls then the translation will
           convert based on the errno returned after the call */
        if ((iRecv = _SocketRecvfromPacketQueue(pSocket, pBuf, iLen, pFrom, pFromLen)) == 0)
        {
            iRecv = -1;
            iErrno = (int32_t)nn::socket::Errno::EWouldBlock;
        }

        // when data is obtained from the packet queue, we lose visibility on system socket errors
        pSocket->iLastError = SOCKERR_NONE;

        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
    }
    else // non-async recvthread socket
    {
        // do direct recv call
        if (((iRecv = _SocketRecvfrom(pSocket, pBuf, iLen, pFrom, pFromLen)) < 0) && ((iErrno = _SocketGetErrno()) != (int32_t)nn::socket::Errno::EAgain))
        {
            NetPrintf(("dirtynetnx: _SocketRecvfrom() failed on a SOCK_STREAM socket (err=%s)\n", DirtyErrGetName(iErrno)));
        }
    }

    // do error conversion
    iRecv = (iRecv == 0) ? SOCKERR_CLOSED : _SocketTranslateError2(iRecv, iErrno);

    // update data rate estimation
    SocketRateUpdate(&pSocket->RecvRate, iRecv, "recv");

    // return the error code
    pSocket->iLastError = iRecv;
    return(iRecv);
}

/*F********************************************************************************/
/*!
    \Function    SocketInfo

    \Description
        Return information about an existing socket.

    \Input *pSocket - socket reference
    \Input iInfo    - selector for desired information
    \Input iData    - selector specific
    \Input *pBuf    - return buffer
    \Input iLen     - buffer length

    \Output
        int32_t     - selector-specific

    \Notes
        iInfo can be one of the following:

    \verbatim
        'addr' - returns interface address; iData=destination address for routing
        'bind' - return bind data (if pSocket == NULL, get socket bound to given port)
        'bndu' - return bind data (only with pSocket=NULL, get SOCK_DGRAM socket bound to given port)
        'conn' - connection status
        'eth0' - returns 0 if we have a valid ip for eth0 device. negative if error or no address found (Android Only)
        'ethr'/'macx' - local ethernet address (returned in pBuf), 0=success, negative=error
        '?ip6' - return TRUE if ipv4 address specified in iData is virtual, and fill in pBuf with ipv6 address if not NULL
        'maxp' - return configured max packet size
        'maxr' - return configured max recv rate (bytes/sec; zero=uncapped)
        'maxs' - return configured max send rate (bytes/sec; zero=uncapped)
        'pdrp' - return socket packet queue number of packets dropped
        'peer' - peer info (only valid if connected)
        'pmax' - return socket packet queue max depth
        'pnum' - return socket packet queue current depth
        'ratr' - return current recv rate estimation (bytes/sec)
        'rats' - return current send rate estimation (bytes/sec)
        'read' - return if socket has data available for reading
        'sdcf' - get installed send callback function pointer (iData specifies index in array)
        'sdcu' - get installed send callback userdata pointer (iData specifies index in array)
        'serr' - last socket error
        'psiz' - return socket packet queue max size
        'sock' - return socket associated with the specified DirtySock socket
        'spam' - return debug level for debug output
        'stat' - socket status
        'virt' - TRUE if socket is virtual, else FALSE
        'wan0' - returns true if we have a valid ip for wlan0 device. Negative if error or no address found (Android Only)
    \endverbatim

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen)
{
    // always zero results by default
    if (pBuf != NULL)
    {
        ds_memclr(pBuf, iLen);
    }

    if (_Socket_pState->bSocketInit == FALSE)
    {
        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: socket lib not initialized\n"));
        return(-1);
    }

    // handle global socket options
    if (pSocket == NULL)
    {
        return(_SocketInfoGlobal(iInfo, iData, pBuf, iLen));
    }

    // return local bind data
    if (iInfo == 'bind')
    {
        if (iLen >= (signed)sizeof(pSocket->LocalAddr))
        {
            if (pSocket->bVirtual == TRUE)
            {
                SockaddrInit((struct sockaddr *)pBuf, AF_INET);
                SockaddrInSetPort((struct sockaddr *)pBuf, pSocket->uVirtualPort);
            }
            else
            {
                nn::socket::GetSockName(pSocket->uSocket, (sockaddr *)pBuf, (socklen_t *)&iLen);
            }
            return(0);
        }
    }

    // return configured max recv rate
    if (iInfo == 'maxr')
    {
        return(pSocket->RecvRate.uMaxRate);
    }

    // return configured max send rate
    if (iInfo == 'maxs')
    {
        return(pSocket->SendRate.uMaxRate);
    }

    // return whether the socket is virtual or not
    if (iInfo == 'virt')
    {
        return(pSocket->bVirtual);
    }

    /*
    make sure the socket is alive
    ** AFTER THIS POINT WE ENSURE THE SOCKET DESCRIPTOR AND PACKET QUEUE ARE VALID **
    */
    if (pSocket->pKill != NULL)
    {
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // return local peer data
    if ((iInfo == 'conn') || (iInfo == 'peer'))
    {
        if (iLen >= (signed)sizeof(pSocket->LocalAddr))
        {
            nn::socket::GetPeerName(pSocket->uSocket, (sockaddr *)pBuf, (socklen_t *)&iLen);
        }
        return(0);
    }

    // get packet queue info
    if ((iInfo == 'pdrp') || (iInfo == 'pmax') || (iInfo == 'pnum') || (iInfo == 'psiz'))
    {
        int32_t iResult;
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);
        // get packet queue status
        iResult = SocketPacketQueueStatus(pSocket->pRecvQueue, iInfo);
        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
        // return success
        return(iResult);
    }

    // return current recv rate estimation
    if (iInfo == 'ratr')
    {
        return(pSocket->RecvRate.uCurRate);
    }

    // return current send rate estimation
    if (iInfo == 'rats')
    {
        return(pSocket->SendRate.uCurRate);
    }

    // return if socket has data
    if (iInfo == 'read')
    {
        return(pSocket->bHasData);
    }

    // return last socket error
    if (iInfo == 'serr')
    {
        return(pSocket->iLastError);
    }

    // return unix socket ref
    if (iInfo == 'sock')
    {
        return(pSocket->uSocket);
    }

    // return socket status
    if (iInfo == 'stat')
    {
        struct pollfd PollFd;

        // if not a connected socket, return TRUE
        if (pSocket->iType != SOCK_STREAM)
        {
            return(1);
        }

        // if not connected, use poll to determine connect
        if (pSocket->iOpened == 0)
        {
            ds_memclr(&PollFd, sizeof(PollFd));
            PollFd.fd = pSocket->uSocket;
            PollFd.events = POLLOUT;
            if (nn::socket::Poll(&PollFd, 1, 0) != 0)
            {
                /*
                Experimentation shows that on connect failed:
                Android: (only) POLLERR is returned.
                iOS5: (only) POLLHUP is returned.
                Linux: both POLLERR and POLLHUP are returned (POLLERR|POLLHUP).

                To make the code work on all platforms, we test if any of POLLERR and POLLHUP was set.
                */
                if ((PollFd.revents & POLLERR) || (PollFd.revents & POLLHUP))
                {
                    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: read exception on connect\n"));
                    pSocket->iOpened = -1;
                }
                // if socket is writable, that means connect succeeded
                else if (PollFd.revents & POLLOUT)
                {
                    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: connection open\n"));
                    pSocket->iOpened = 1;
                }
            }
        }

        /* if previously connected, make sure connect still valid. we only do this when not doing async receive for two
           reasons
           1. there is a race condition between the poll waking up and querying the bytes available. if the bytes are
              read on the receive thread between the poll and ioctl then it would think the socket is closed because the
              socket has already been drained
           2. our reasoning behind using the async receive thread could be the cost of recv, plus other calls may be
              expensive as well. the async receive thread will already set the correct state on the socket thus we can
              skip the query and return iOpened back to the user */
        if (!pSocket->bAsyncRecv && (pSocket->iOpened > 0))
        {
            ds_memclr(&PollFd, sizeof(PollFd));
            PollFd.fd = pSocket->uSocket;
            PollFd.events = POLLIN;
            if (nn::socket::Poll(&PollFd, 1, 0) != 0)
            {
                // if we got an exception, that means connect failed (usually closed by remote peer)
                if ((PollFd.revents & POLLERR) || (PollFd.revents & POLLHUP))
                {
                    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: connection failure\n"));
                    pSocket->iOpened = -1;
                }
                else if (PollFd.revents & POLLIN)
                {
                    int32_t iAvailBytes = 1;
                    // get number of bytes for read (might be less than actual bytes, so it can only be used for zero-test)
                    if (nn::socket::Ioctl(pSocket->uSocket, FIONREAD, &iAvailBytes, sizeof(iAvailBytes)) != 0)
                    {
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: ioctl(FIONREAD) failed (err=%s).\n", DirtyErrGetName(_SocketGetErrno())));
                    }
                    // if socket is readable but there's no data available for read, connect was closed
                    else if (iAvailBytes == 0)
                    {
                        pSocket->iLastError = SOCKERR_CLOSED;
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetnx: connection closed\n"));
                        pSocket->iOpened = -1;
                    }
                }
            }
        }
        /* if we still have packets in the queue, tell the caller that we are still open. this makes sure they read the
           complete stream of data */
        else if (pSocket->bAsyncRecv && (pSocket->iOpened < 0) && (SocketInfo(pSocket, 'pnum', 0, NULL, 0) != 0))
        {
            return(1);
        }

        // return connect status
        return(pSocket->iOpened);
    }

    // unhandled option?
    NetPrintf(("dirtynetnx: unhandled SocketInfo() option '%C'\n", iInfo));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    SocketCallback

    \Description
        Register a callback routine for notification of socket events.  Also includes
        timeout support.

    \Input *pSocket - socket reference
    \Input iMask    - valid callback events (CALLB_NONE, CALLB_SEND, CALLB_RECV)
    \Input iIdle    - if nonzero, specifies the number of ticks between idle calls
    \Input *pRef    - user data to be passed to proc
    \Input *pProc   - user callback

    \Output
        int32_t     - zero

    \Notes
        A callback will reset the idle timer, so when specifying a callback and an
        idle processing time, the idle processing time represents the maximum elapsed
        time between calls.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketCallback(SocketT *pSocket, int32_t iMask, int32_t iIdle, void *pRef, int32_t(*pProc)(SocketT *pSock, int32_t iFlags, void *pRef))
{
    pSocket->uCallIdle = iIdle;
    pSocket->iCallMask = iMask;
    pSocket->pCallRef = pRef;
    pSocket->pCallback = pProc;
    pSocket->uCallLast = NetTick() - iIdle;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    SocketControl

    \Description
        Process a control message (type specific operation)

    \Input *pSocket - socket to control, or NULL for module-level option
    \Input iOption  - the option to pass
    \Input iData1   - message specific parm
    \Input *pData2  - message specific parm
    \Input *pData3  - message specific parm

    \Output
        int32_t     - message specific result (-1=unsupported message)

    \Notes
        iOption can be one of the following:

    \verbatim
        'arcv' - set async receive enable/disable (default enabled for DGRAM/RAW, disabled for TCP)
        'asnd' - when pSocket is NULL: enable/disable socket creation defaulting to async send used
                 when pSocket is not NULL: set async send enable/disable
        'conn' - init network stack
        'disc' - bring down network stack
        'maxp' - set max udp packet size
        'maxr' - set max recv rate (bytes/sec; zero=uncapped)
        'maxs' - set max send rate (bytes/sec; zero=uncapped)
        'nbio' - set nonblocking/blocking mode (TCP only, iData1=TRUE (nonblocking) or FALSE (blocking))
        'ndly' - set TCP_NODELAY state for given stream socket (iData1=zero or one)
        'pdev' - set simulated packet deviation
        'plat' - set simulated packet latency
        'plos' - set simulated packet loss
        'pque' - set socket packet queue depth
        'push' - push data into given socket receive buffer (iData1=size, pData2=data ptr, pData3=sockaddr ptr)
        'radr' - set SO_REUSEADDR On the specified socket
        'rbuf' - set socket recv buffer size
        'sbuf' - set socket send buffer size
        'scbk' - enable/disable "send callbacks usage" on specified socket (defaults to enable)
        'sdcb' - set/unset send callback (iData1=TRUE for set - FALSE for unset, pData2=callback, pData3=callref)
        'sfal' - set/unset silent fail if set to TRUE NIFM errors will not be handled (no first party dialog will be shown) (iData1=TRUE for set - FALSE for unset)
        'sint' - is nintendo socket library already intialized (iData1=TRUE or FALSE)
        'soli' - set SO_LINGER On the specified socket, iData1 is timeout in seconds
        'spam' - set debug level for debug output
        'vadd' - add a port to virtual port list
        'vdel' - del a port from virtual port list
    \endverbatim

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketControl(SocketT *pSocket, int32_t iOption, int32_t iData1, void *pData2, void *pData3)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iResult;

    // handle global controls
    if (pSocket == NULL)
    {
        return(_SocketControlGlobal(iOption, iData1, pData2, pData3));
    }

    // set async recv enable
    if (iOption == 'arcv')
    {
        // set socket async recv flag
        pSocket->bAsyncRecv = iData1 ? TRUE : FALSE;
        NetPrintf(("dirtynetnx: [%p] async recv mode %s\n", pSocket, pSocket->bAsyncRecv ? "enabled" : "disabled"));
        return(0);
    }

    // set async send enable
    if (iOption == 'asnd')
    {
        // set socket async send flag (will be ignored if socket is virtual)
        pSocket->bAsyncSend = iData1 ? TRUE : FALSE;
        NetPrintf(("dirtynetnx: [%p] async send mode %s\n", pSocket, pSocket->bAsyncSend ? "enabled" : "disabled"));
        return(0);
    }

    // set max recv rate
    if (iOption == 'maxr')
    {
        NetPrintf(("dirtynetnx: setting max recv rate to %d bytes/sec\n", iData1));
        pSocket->RecvRate.uMaxRate = iData1;
        return(0);
    }
    // set max send rate
    if (iOption == 'maxs')
    {
        NetPrintf(("dirtynetnx: setting max send rate to %d bytes/sec\n", iData1));
        pSocket->SendRate.uMaxRate = iData1;
        return(0);
    }
    // enable/disable "send callbacks usage" on specified socket (defaults to enable)
    if (iOption == 'scbk')
    {
        if (pSocket->bSendCbs != (iData1 ? TRUE : FALSE))
        {
            NetPrintf(("dirtynetnx: send callbacks usage changed from %s to %s for socket ref %p\n", (pSocket->bSendCbs ? "ON" : "OFF"), (iData1 ? "ON" : "OFF"), pSocket));
            pSocket->bSendCbs = (iData1 ? TRUE : FALSE);
        }
        return(0);
    }
    // set debug spam level
    if (iOption == 'spam')
    {
        // per-socket debug level
        pSocket->iVerbose = iData1;
        return(0);
    }

    /*
    make sure the socket is alive
    ** AFTER THIS POINT WE ENSURE THE SOCKET DESCRIPTOR AND PACKET QUEUE ARE VALID **
    */
    if (pSocket->pKill != NULL)
    {
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // if a stream socket, set nonblocking/blocking mode
    if ((iOption == 'nbio') && (pSocket->iType == SOCK_STREAM))
    {
        int32_t iVal = nn::socket::Fcntl(pSocket->uSocket, F_GETFL, O_NONBLOCK);
        iVal = iData1 ? (iVal | O_NONBLOCK) : (iVal & ~O_NONBLOCK);
        iResult = nn::socket::Fcntl(pSocket->uSocket, F_SETFL, iVal);
        pSocket->iLastError = _SocketTranslateError(iResult);
        NetPrintf(("dirtynetnx: setting socket:0x%x to %s mode %s (LastError=%d).\n", pSocket, iData1 ? "nonblocking" : "blocking", iResult ? "failed" : "succeeded", pSocket->iLastError));
        return(pSocket->iLastError);
    }
    // if a stream socket, set TCP_NODELAY state
    if ((iOption == 'ndly') && (pSocket->iType == SOCK_STREAM))
    {
        iResult = nn::socket::SetSockOpt(pSocket->uSocket, IPPROTO_TCP, TCP_NODELAY, &iData1, sizeof(iData1));
        pSocket->iLastError = _SocketTranslateError(iResult);
        return(pSocket->iLastError);
    }
    // set simulated packet loss or packet latency
    if ((iOption == 'pdev') || (iOption == 'plat') || (iOption == 'plos'))
    {
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);
        // forward selector to packet queue
        iResult = SocketPacketQueueControl(pSocket->pRecvQueue, iOption, iData1);
        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
        return(iResult);
    }

    // change packet queue size
    if (iOption == 'pque')
    {
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);
        // resize the queue
        pSocket->pRecvQueue = SocketPacketQueueResize(pSocket->pRecvQueue, iData1, pState->iMemGroup, pState->pMemGroupUserData);
        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
        // return success
        return(0);
    }

    // push data into receive buffer
    if (iOption == 'push')
    {
        // acquire socket critical section
        NetCritEnter(&pSocket->RecvCrit);

        // don't allow data that is too large (for the buffer) to be pushed
        if (iData1 > SOCKET_MAXUDPRECV)
        {
            NetPrintf(("dirtynetnx: request to push %d bytes of data discarded (max=%d)\n", iData1, SOCKET_MAXUDPRECV));
            NetCritLeave(&pSocket->RecvCrit);
            return(-1);
        }

        // add packet to queue
        SocketPacketQueueAdd2(pSocket->pRecvQueue, (uint8_t *)pData2, iData1, (struct sockaddr *)pData3, FALSE);
        // remember we have data
        pSocket->bHasData = 1;

        // release socket critical section
        NetCritLeave(&pSocket->RecvCrit);

        // see if we should issue callback
        if ((pSocket->pCallback != NULL) && (pSocket->iCallMask & CALLB_RECV))
        {
            pSocket->pCallback(pSocket, 0, pSocket->pCallRef);
        }
        return(0);
    }
    // set SO_REUSEADDR
    if (iOption == 'radr')
    {
        iResult = nn::socket::SetSockOpt(pSocket->uSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&iData1, sizeof(iData1));
        pSocket->iLastError = _SocketTranslateError(iResult);
        return(pSocket->iLastError);
    }
    // set socket receive buffer size
    if ((iOption == 'rbuf') || (iOption == 'sbuf'))
    {
        int32_t iOldSize, iNewSize;
        int32_t iSockOpt = (iOption == 'rbuf') ? SO_RCVBUF : SO_SNDBUF;
        socklen_t uOptLen = 4;

        // get current buffer size
        nn::socket::GetSockOpt(pSocket->uSocket, SOL_SOCKET, iSockOpt, (char *)&iOldSize, &uOptLen);

        // set new size
        iResult = nn::socket::SetSockOpt(pSocket->uSocket, SOL_SOCKET, iSockOpt, (const char *)&iData1, sizeof(iData1));
        if ((pSocket->iLastError = _SocketTranslateError(iResult)) == SOCKERR_NONE)
        {
            // save new buffer size
            if (iOption == 'rbuf')
            {
                pSocket->iRbufSize = iData1;
            }
            else
            {
                pSocket->iSbufSize = iData1;
            }
        }

        // get new size
        nn::socket::GetSockOpt(pSocket->uSocket, SOL_SOCKET, iSockOpt, (char *)&iNewSize, &uOptLen);

        NetPrintf(("dirtynetnx: setsockopt(%s) changed buffer size from %d to %d\n", (iOption == 'rbuf') ? "SO_RCVBUF" : "SO_SNDBUF", iOldSize, iNewSize));
        return(pSocket->iLastError);
    }
    // set SO_LINGER
    if (iOption == 'soli')
    {
        struct linger lingerOptions;
        lingerOptions.l_onoff = TRUE;
        lingerOptions.l_linger = iData1;
        iResult = nn::socket::SetSockOpt(pSocket->uSocket, SOL_SOCKET, SO_LINGER, &lingerOptions, sizeof(lingerOptions));
        pSocket->iLastError = _SocketTranslateError(iResult);
        return(pSocket->iLastError);
    }
    // unhandled
    NetPrintf(("dirtynetnx: unhandled control option '%C'\n", iOption));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    SocketGetLocalAddr

    \Description
        Returns the "external" local address (ie, the address as a machine "out on
        the Internet" would see as the local machine's address).

    \Output
        uint32_t        - local address

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
uint32_t SocketGetLocalAddr(void)
{
    SocketStateT *pState = _Socket_pState;
    if (pState->uLocalAddr == 0)
    {
        pState->uLocalAddr = SocketInfo(NULL, 'addr', 0, NULL, 0);
    }
    return(pState->uLocalAddr);
}

/*F********************************************************************************/
/*!
    \Function    SocketLookup

    \Description
        Lookup a host by name and return the corresponding Internet address. Uses
        a callback/polling system since the socket library does not allow blocking.

    \Input *pText   - pointer to null terminated address string
    \Input iTimeout - number of milliseconds to wait for completion

    \Output
        HostentT *  - hostent struct that includes callback vectors

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
HostentT *SocketLookup(const char *pText, int32_t iTimeout)
{
    SocketStateT *pState = _Socket_pState;
    SocketLookupPrivT *pPriv;
    int32_t iAddr, iResult;
    HostentT *pHost, *pHostRef;
    DirtyThreadConfigT ThreadConfig;

    NetPrintf(("dirtynetnx: looking up address for host '%s'\n", pText));

    // dont allow negative timeouts
    if (iTimeout < 0)
    {
        return(NULL);
    }

    // create new structure
    pPriv = (SocketLookupPrivT *)DirtyMemAlloc(sizeof(*pPriv), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    ds_memclr(pPriv, sizeof(*pPriv));
    pHost = &pPriv->Host;

    // setup callbacks
    pHost->Done = &_SocketLookupDone;
    pHost->Free = &_SocketLookupFree;
    // copy over the target address
    ds_strnzcpy(pHost->name, pText, sizeof(pHost->name));

    // look for refcounted lookup
    if ((pHostRef = SocketHostnameAddRef(&pState->pHostList, &pPriv->Host, TRUE)) != NULL)
    {
        DirtyMemFree(pPriv, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        return(pHostRef);
    }

    // check for dot notation, then check hostname cache
    if (((iAddr = SocketInTextGetAddr(pText)) != 0) || ((iAddr = SocketHostnameCacheGet(pState->pHostnameCache, pText, pState->iVerbose)) != 0))
    {
        // we've got a dot-notation address
        pHost->addr = iAddr;
        pHost->done = 1;
        // return completed record
        return(pHost);
    }

    /* add an extra refcount for the thread; this ensures the host structure survives until the thread
    is done with it.  this must be done before thread creation. */
    pHost->refcount += 1;

    // configure threading
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "SocketLookup";
    ThreadConfig.iAffinity = NetConnStatus('affn', 0, NULL, 0);
    ThreadConfig.iVerbosity = pState->iVerbose - 1;

    // create dns lookup thread
    if ((iResult = DirtyThreadCreate(_SocketLookupThread, pPriv, &ThreadConfig)) < 0)
    {
        NetPrintf(("dirtynetnx: failed to create lookup thread (err=%d)\n", iResult));
        pPriv->Host.done = -1;
        // remove refcount we just added
        pHost->refcount -= 1;
    }

    // return the host reference
    return(pHost);
}

/*F********************************************************************************/
/*!
    \Function    SocketHost

    \Description
        Return the host address that would be used in order to communicate with
        the given destination address.

    \Input *pHost   - [out] local sockaddr struct
    \Input iHostlen - length of structure (sizeof(host))
    \Input *pDest   - remote sockaddr struct
    \Input iDestlen - length of structure (sizeof(dest))

    \Output
        int32_t     - zero=success, negative=error

    \Version 12/12/2003 (sbevan)
*/
/********************************************************************************F*/
int32_t SocketHost(struct sockaddr *pHost, int32_t iHostlen, const struct sockaddr *pDest, int32_t iDestlen)
{
    SocketStateT *pState = _Socket_pState;

    // must be same kind of addresses
    if (iHostlen != iDestlen)
    {
        return(-1);
    }

    // do family specific lookup
    if (pDest->sa_family == AF_INET)
    {
        // special case destination of zero or loopback to return self
        if ((SockaddrInGetAddr(pDest) == 0) || (SockaddrInGetAddr(pDest) == 0x7f000000))
        {
            ds_memcpy(pHost, pDest, iHostlen);
            return(0);
        }
        else
        {
            ds_memclr(pHost, iHostlen);
            pHost->sa_family = AF_INET;

            // uLocalAddr can only be accessed in the context of the callbackCrit
            SockaddrInSetAddr(pHost, pState->uLocalAddr);
            return(0);
        }
    }

    // unsupported family
    ds_memclr(pHost, iHostlen);
    return(-3);
}
