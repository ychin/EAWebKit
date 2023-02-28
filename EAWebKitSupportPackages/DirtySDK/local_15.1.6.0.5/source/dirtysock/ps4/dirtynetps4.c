/*H********************************************************************************/
/*!
    \File dirtynetps4.c

    \Description
        Provides a wrapper that translates the PS4 network interface to the
        DirtySock portable networking interface.

    \Copyright
        Copyright (c) 2012 Electronic Arts Inc.

    \Version 04/05/2012 (jbrookes) First version; a vanilla port to PS4 from unix
    \Version 01/23/2013 (amakoukji) Second version; replaced FreeBSD API with ps4 versions
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <libnetctl.h>
#include <net.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtythread.h"

#include "dirtynetpriv.h"       // private include for dirtynet common functions

/*** Defines **********************************************************************/

#define INVALID_SOCKET      (-1)

#define SOCKET_MAXPOLL      (32)

#define SOCKET_VERBOSE      (DIRTYCODE_DEBUG && FALSE)

/*** Type Definitions *************************************************************/

//! private socketlookup structure containing extra data
typedef struct SocketLookupPrivT
{
    HostentT    Host;      //!< must come first!
    int32_t     iSceMemPoolId;
    SceNetId    iSceResolverId;
    SceNetId    iSceEpollId;
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
    uint8_t bHasdata;           //!< zero if no data, else has data ready to be read
    uint8_t uBrokenFlag;        //!< 0 or 1=might not be broken, >1=broken socket
    uint8_t bAsyncRecv;         //!< if true, async recv is enabled
    uint8_t bSendCbs;           //!< TRUE if send cbs are enabled, false otherwise
    int8_t  iVerbose;           //!< debug level
    

    SceNetId uSocket;           //!< ps4 socket ref
    int32_t iLastError;         //!< last socket error

    struct sockaddr LocalAddr;  //!< local address
    struct sockaddr RemoteAddr; //!< remote address

    uint16_t uVirtualport;      //!< virtual port, if set
    uint16_t uPollidx;          //!< index in blocking poll() operation

    SocketRateT SendRate;       //!< send rate estimation data
    SocketRateT RecvRate;       //!< recv rate estimation data

    int32_t iCallMask;          //!< valid callback events
    uint32_t uCallLast;         //!< last callback tick
    uint32_t uCallIdle;         //!< ticks between idle calls
    void *pCallRef;             //!< reference calback value
    int32_t (*pCallback)(SocketT *pSocket, int32_t iFlags, void *pRef);
    uint8_t bInCallback;        //!< in a socket callback
    uint8_t __pad[3];

    NetCritT RecvCrit;          //!< receive critical section
    int32_t iRecvErr;           //!< last error that occurred
    uint32_t uRecvflag;         //!< flags from recv operation

    int32_t iRbufsize;          //!< read buffer size (bytes)
    int32_t iSbufsize;          //!< send buffer size (bytes)

    struct sockaddr RecvAddr;   //!< receive address

    SocketPacketQueueT *pRecvQueue;
    SocketPacketQueueEntryT *pRecvPacket;
};

//! standard ipv4 packet header (see RFC791)
typedef struct HeaderIpv4
{
    uint8_t uVerslen;      //!< version and length fields (4 bits each)
    uint8_t uService;      //!< type of service field
    uint8_t aLength[2];    //!< total packet length (header+data)
    uint8_t aIdent[2];     //!< packet sequence number
    uint8_t aFrag[2];      //!< fragmentation information
    uint8_t uTime;         //!< time to live (remaining hop count)
    uint8_t uProto;        //!< transport protocol number
    uint8_t aCheck[2];     //!< header checksum
    uint8_t aSrcaddr[4];   //!< source ip address
    uint8_t aDstaddr[4];   //!< dest ip address
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
    uint8_t  bNetAlreadyInitialized;
    uint8_t  uNetCtlConnectionState;    //!< current netctl connection state
    uint8_t  _pad1;

    volatile int32_t iRecvLife;

    SocketHostnameCacheT *pHostnameCache; //!< hostname cache

    SocketSendCallbackEntryT aSendCbEntries[SOCKET_MAXSENDCALLBACKS]; //!< collection of send callbacks

    int32_t iVerbose;                   //!< debug log verbosity level

    int32_t  iNetEventCid;              //!< callback id for net events
    uint8_t  bNetEventOccured;          //!< boolean indicated that a net event occured and needs to be processed
    uint8_t  _pad2[3];
} SocketStateT;

/*** Variables ********************************************************************/

//! module state ref
static SocketStateT *_Socket_pState = NULL;

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function    _SocketNetCtlCallback

    \Description
        Handler for Sony online events

    \Input  eventType  - type
    \Input *arg        - user info, in this case _Socket_pState

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static void _SocketNetCtlCallback(int32_t eventType, void *arg)
{
    int32_t iResult = 0, iLastErrorCode;
    SocketStateT *pState = (SocketStateT*)arg;
    pState->bNetEventOccured = TRUE;
    NetPrintf(("dirtynetps4: sceNetCtl event of type %d\n", eventType));
    if(eventType == SCE_NET_CTL_EVENT_TYPE_DISCONNECTED)
    {
        if((iResult = sceNetCtlGetResult(eventType, &iLastErrorCode)) == 0)
        {
            NetPrintf(("dirtynetps4: sceNetCtlRegisterCallback() failed (err=%s)\n", DirtyErrGetName(iLastErrorCode)));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _SockAddrToSceAddr

    \Description
        Translate DirtySock socket address to Sony socket address.

    \Input *pSceAddr    - [out] pointer to Sce address to set up
    \Input *pSockAddr   - pointer to source DirtySock address

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static void _SockAddrToSceAddr(struct SceNetSockaddr *pSceAddr, const struct sockaddr *pSockAddr)
{
    ds_memcpy_s(pSceAddr, sizeof(*pSceAddr), pSockAddr, sizeof(*pSockAddr));
    pSceAddr->sa_len = sizeof(*pSceAddr);
    pSceAddr->sa_family = (unsigned char)pSockAddr->sa_family;
}

/*F********************************************************************************/
/*!
    \Function    _SceAddrToSockAddr

    \Description
        Translate Sony socket address to DirtySock socket address.

    \Input *pSockAddr   - [out] pointer to source DirtySock address to set up
    \Input *pSceAddr    - pointer to Sce address

    \Version 02/04/2013 (cvienneau)
*/
/********************************************************************************F*/
static void _SceAddrToSockAddr(struct sockaddr *pSockAddr, const struct SceNetSockaddr *pSceAddr)
{
    ds_memcpy_s(pSockAddr, sizeof(*pSockAddr), pSceAddr, sizeof(*pSceAddr));
    pSockAddr->sa_len = sizeof(*pSockAddr);
    pSockAddr->sa_family = (unsigned char)pSceAddr->sa_family;
}

/*F********************************************************************************/
/*!
    \Function    _AddrFamilyToSceAddrFamily

    \Description
        Translate DirtySock socket address to Sony socket address.

    \Input iAddrFamily    - address family to translate

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _AddrFamilyToSceAddrFamily(int32_t iAddrFamily)
{
    int32_t iRetVal = 0;
    // Add to this struct if new families are ever supported
    switch(iAddrFamily)
    {
        case AF_INET:
            iRetVal = SCE_NET_AF_INET;
            break;
        default:
            NetPrintf(("dirtynetps4: CRITICAL ERROR Unsupported address family %d, using SCE_NET_AF_INET\n", iAddrFamily));
            iRetVal = SCE_NET_AF_INET;
            break;
    }

    return(iRetVal);
}

/*F********************************************************************************/
/*!
    \Function    _SocketTypeToSceSocketType

    \Description
        Translate DirtySock socket type to Sony socket type.

    \Input iType    - type to translate

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _SocketTypeToSceSocketType(int32_t iType)
{
    int32_t iRetVal = 0;
    // Add to this struct if new types are ever supported
    switch(iType)
    {
        case SOCK_STREAM:
            iRetVal = SCE_NET_SOCK_STREAM;
            break;
        case SOCK_DGRAM:
            iRetVal = SCE_NET_SOCK_DGRAM;
            break;
        case SOCK_RAW:
            iRetVal = SCE_NET_SOCK_RAW;
            break;
        default:
            NetPrintf(("dirtynetps4: CRITICAL ERROR Unsupported socket type %d\n", iType));
            iRetVal = iType;
            break;
    }

    return(iRetVal);
}

/*F********************************************************************************/
/*!
    \Function    _ProtocolToSceProtocol

    \Description
        Translate DirtySock socket protocol to Sony socket protocol.

    \Input iProto    - protocol to translate

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _ProtocolToSceProtocol(int32_t iProto)
{
    int32_t iRetVal = 0;
    // Add to this struct if new protocols are ever supported
    switch(iProto)
    {
        case IPPROTO_IP:
        case IPPROTO_IPV4:
            iRetVal = SCE_NET_IPPROTO_IP;
            break;
        case IPPROTO_ICMP:
            iRetVal = SCE_NET_IPPROTO_ICMP;
            break;
        case IPPROTO_TCP:
            iRetVal = SCE_NET_IPPROTO_TCP;
            break;
        case IPPROTO_UDP:
            iRetVal = SCE_NET_IPPROTO_UDP;
            break;
        default:
            NetPrintf(("dirtynetps4: CRITICAL ERROR Unsupported socket protocol %d\n", iProto));
            iRetVal = iProto;
            break;
    }
    return(iRetVal);
}

/*F********************************************************************************/
/*!
    \Function    _SocketTranslateError2

    \Description
        Translate a Net lib error to dirtysock

    \Input iErr     - SCE error code
    \Input iErrno   - sce_net_errno or override of the value

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
        if ((iErr == SCE_NET_EWOULDBLOCK ) || (iErr == SCE_NET_EINPROGRESS))
            iErr = SOCKERR_NONE;
        else if (iErr == SCE_NET_EHOSTUNREACH)
            iErr = SOCKERR_UNREACH;
        else if (iErr == SCE_NET_ENOTCONN)
            iErr = SOCKERR_NOTCONN;
        else if (iErr == SCE_NET_ECONNREFUSED)
            iErr = SOCKERR_REFUSED;
        else if (iErr == SCE_NET_ECONNRESET)
            iErr = SOCKERR_CONNRESET;
        else if ((iErr == SCE_NET_EBADF) || (iErr == SCE_NET_EPIPE))
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
        Translate a Net lib error to dirtysock

    \Input iErr     - SCE error code

    \Output
        int32_t     - dirtysock error (SOCKERR_*)

    \Version 06/19/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _SocketTranslateError(int32_t iErr)
{
    return(_SocketTranslateError2(iErr, sce_net_errno));
}

/*F********************************************************************************/
/*!
    \Function _SocketNetCtlUpdate

    \Description
        Finds the active interface, and gets its IP address and MAC address for
        later use.

    \Input pState - state of module

    \Version 06/22/2005 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketNetCtlUpdate(SocketStateT *pState)
{
    #if DIRTYCODE_LOGGING
    static const char *_StateNames[] = { "Disconnected", "Connecting", "IPObtaining", "IPObtained" };
    #endif

    int32_t iResult, iState = -1;

    sceNetCtlCheckCallback();
    if (!pState->bNetEventOccured)
    {
        return;
    }
    else
    {
        if ((iResult = sceNetCtlGetState(&iState)) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetCtlGetState() failed (err=%s)\n", DirtyErrGetName(iResult)));
            return;
        }
        pState->bNetEventOccured = FALSE;
    }

    // compare to previous state
    if (pState->uNetCtlConnectionState != (uint8_t)iState)
    {
        NetPrintf(("dirtynetps4: %s->%s\n", _StateNames[pState->uNetCtlConnectionState], _StateNames[iState]));
    }

    // did we get IP address?
    if ((pState->uNetCtlConnectionState != SCE_NET_CTL_STATE_IPOBTAINED) && (iState == SCE_NET_CTL_STATE_IPOBTAINED))
    {
        // acquire address/mac address
        SocketInfo(NULL, 'addr', 0, NULL, 0);
        SocketInfo(NULL, 'macx', 0, NULL, 0);
        // mark us as online
        NetPrintf(("dirtynetps4: addr=%a mac=%02x:%02x:%02x:%02x:%02x:%02x\n", pState->uLocalAddr,
            pState->aMacAddr[0], pState->aMacAddr[1], pState->aMacAddr[2],
            pState->aMacAddr[3], pState->aMacAddr[4], pState->aMacAddr[5]));
        pState->uConnStatus = '+onl';
    }
    else if (iState != SCE_NET_CTL_STATE_IPOBTAINED)
    {
        NetPrintf(("dirtynetps4: got disconnect event at %d\n", NetTick()));
        pState->uConnStatus = '-dsc';
        pState->uLocalAddr = 0;
    }

    // update connection state
    pState->uNetCtlConnectionState = (uint8_t)iState;
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
    int32_t iResult;

    // create socket
    if ((iSocket = sceNetSocket("DS socket", _AddrFamilyToSceAddrFamily(iAddrFamily), _SocketTypeToSceSocketType(iType), _ProtocolToSceProtocol(iProto))) >= 0)
    {
        uint32_t uTrue = 1;
        // if dgram, allow broadcast
        if (iType == SOCK_DGRAM)
        {
            if ((iResult = sceNetSetsockopt(iSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST, &uTrue, sizeof(uTrue))) != 0)
            {
                NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            }
        }
        // if a raw socket, set header include
        if (iType == SOCK_RAW)
        {
            if ((iResult = sceNetSetsockopt(iSocket, SCE_NET_IPPROTO_IP, SCE_NET_IP_HDRINCL, &uTrue, sizeof(uTrue))) != 0)
            {
                NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_IPPROTO_IP, SCE_NET_IP_HDRINCL) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            }
        }
        // set nonblocking operation
        if ((iResult = sceNetSetsockopt(iSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &uTrue, sizeof(uTrue))) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
    }
    else
    {
        NetPrintf(("dirtynetps4: sceNetSocket() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
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

    // allocate memory
    if ((pSocket = DirtyMemAlloc(sizeof(*pSocket), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetps4: unable to allocate memory for socket\n"));
        return(NULL);
    }
    ds_memclr(pSocket, sizeof(*pSocket));

    // open a sony socket (force to AF_INET if we are opening it)
    if ((iSocket == INVALID_SOCKET) && ((iSocket = _SocketCreateSocket(iAddrFamily = AF_INET, iType, iProto)) < 0))
    {
        NetPrintf(("dirtynetps4: error %s creating socket\n", DirtyErrGetName(iSocket)));
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        return(NULL);
    }

    // create packet queue
    if ((pSocket->pRecvQueue = SocketPacketQueueCreate(iQueueSize, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetps4: failed to create socket queue for socket\n"));
        sceNetSocketClose(iSocket);
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
    pSocket->bSendCbs = TRUE;
    pSocket->iVerbose = 1;

    NetCritInit(&pSocket->RecvCrit, "inet-recv");

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
    // for non-virtual non-tcp sockets only
    if (!pSocket->bVirtual && ((pSocket->iType == SOCK_DGRAM) || (pSocket->iType == SOCK_RAW)))
    {
        SocketT *pResult = NULL;
        // acquire socket receive critical section
        NetCritEnter(&pSocket->RecvCrit);
        NetPrintf(("dirtynetps4: trying to recreate the socket handle for %x->%d\n", pSocket, pSocket->uSocket));

        // close existing socket handle
        if (pSocket->uSocket != INVALID_SOCKET)
        {
            if (sceNetSocketClose(pSocket->uSocket) < 0)
            {
                NetPrintf(("dirtynetps4: sceNetSocketClose(%d) failed (err=%s)\n", pSocket->uSocket, DirtyErrGetName(sce_net_errno)));
            }
            pSocket->uSocket = INVALID_SOCKET;
        }

        // create socket
        if ((pSocket->uSocket = _SocketCreateSocket(pSocket->iFamily, pSocket->iType, pSocket->iProto)) >= 0)
        {
            // set socket buffer size
            if (pSocket->iRbufsize > 0)
            {
                SocketControl(pSocket, 'rbuf', pSocket->iRbufsize, NULL, 0);
            }
            if (pSocket->iSbufsize > 0)
            {
                SocketControl(pSocket, 'sbuf', pSocket->iSbufsize, NULL, 0);
            }
            // bind if previous socket was bound to a specific port
            if (SockaddrInGetPort(&pSocket->LocalAddr) != 0)
            {
                int32_t iResult;
                struct SceNetSockaddr sceSocketAddr;

                // always set reuseaddr flag for socket recreation
                SocketControl(pSocket, 'radr', 1, NULL, 0);
                // we don't call SocketBind() here (to avoid virtual socket translation)

                _SockAddrToSceAddr(&sceSocketAddr, &pSocket->LocalAddr);
                if ((iResult = sceNetBind(pSocket->uSocket, &sceSocketAddr, sizeof(pSocket->LocalAddr))) < 0)
                {
                    pSocket->iLastError = _SocketTranslateError(iResult);
                    NetPrintf(("dirtynetps4: sceNetBind() to %a:%d failed (err=%s)\n",
                               SockaddrInGetAddr(&pSocket->LocalAddr),
                               SockaddrInGetPort(&pSocket->LocalAddr),
                               DirtyErrGetName(sce_net_errno)));
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
            NetPrintf(("dirtynetps4: socket recreation (%x->%d) succeeded.\n", pSocket, pSocket->uSocket));
        }
        else
        {
            pSocket->iLastError = _SocketTranslateError(pSocket->uSocket);
            NetPrintf(("dirtynetps4: socket recreation (%x) failed.\n", pSocket));
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
        NOT dispose of the ps4 socket ref.

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
        NetPrintf(("dirtynetps4: warning, trying to close socket 0x%08x that is not in the socket list\n", (intptr_t)pSocket));
        return(-1);
    }

    // finish any idle call
    NetIdleDone();

    // mark as closed
    pSocket->uSocket = INVALID_SOCKET;
    pSocket->iOpened = FALSE;

    // kill critical section
    NetCritKill(&pSocket->RecvCrit);

    // destroy packet queue
    if (pSocket->pRecvQueue != NULL)
    {
        SocketPacketQueueDestroy(pSocket->pRecvQueue);
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

    // for access to g_socklist and g_sockkill
    NetCritEnter(NULL);

    // get current tick
    uTick = NetTick();

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
    }

    // delete any killed sockets
    while ((pSocket = pState->pSockKill) != NULL)
    {
        pState->pSockKill = pSocket->pKill;
        DirtyMemFree(pSocket, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }

    // process dns cache list, delete expired entries
    SocketHostnameCacheProcess(pState->pHostnameCache, pState->iVerbose);

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

/*F********************************************************************************/
/*!
    \Function _SocketLookupDone

    \Description
        Callback to determine if gethostbyname is complete.

    \Input *pHost   - pointer to host lookup record

    \Output
        int32_t     - zero=in progess, neg=done w/error, pos=done w/success

    \Version 12/19/2012 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _SocketLookupDone(HostentT *pHost)
{
    SocketStateT *pState = _Socket_pState;
    SceNetEpollEvent resultsList[1];
    int32_t addrNtohlTemp;
    int32_t iPollResult, iErrorResult = 0;
    SocketLookupPrivT *pPriv = (SocketLookupPrivT *)pHost;  //convert to platform dependant version for id's

    if (pHost->done == 0)
    {
        //check for event
        iPollResult = sceNetEpollWait(pPriv->iSceEpollId, resultsList, 1, 0);

        if (iPollResult > 0)
        {
            //check for error
            if (sceNetResolverGetError(pPriv->iSceResolverId, &iErrorResult) < 0)
            {
                NetPrintf(("dirtynetps4: sceNetResolverGetError() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            }
            if (iErrorResult != 0)
            {
                NetPrintf(("dirtynetps4: sceNetResolverStartNtoa() failed %s, (err=%s)\n", pHost->name, DirtyErrGetName(iErrorResult)));
                pHost->done = -1;
            }

            //check to see if we did get our addr
            if (pHost->addr != 0)
            {
                //need to reorder the results
                addrNtohlTemp = pHost->addr;
                pHost->addr = SocketNtohl(*(uint32_t *)&addrNtohlTemp);
                pHost->done = 1;
                NetPrintf(("dirtynetps4: lookup success; %s=%a\n", pHost->name, pHost->addr));
                // add hostname to cache
                SocketHostnameCacheAdd(pState->pHostnameCache, pHost->name, pHost->addr, pState->iVerbose);
            }
        }
        else if (iPollResult < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollWait() dns failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            pHost->done = -1;
        }
    }

    return(pHost->done);
}

/*F********************************************************************************/
/*!
    \Function _SocketLookupFree

    \Description
        Release resources used by SocketLookup()

    \Input *pHost   - pointer to host lookup record

    \Version 12/19/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _SocketLookupFree(HostentT *pHost)
{
    SocketLookupPrivT *pPriv = (SocketLookupPrivT *)pHost; //convert to platform dependant version for id's
    int32_t iResult;

    // destroy sce epoll, pool, and resolver, in reverse order of allocation
    if (pPriv->iSceResolverId >= 0)
    {
        if ((iResult = sceNetResolverAbort(pPriv->iSceResolverId, 0)) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetResolverAbort(%d) failed (err=%s)\n", pPriv->iSceResolverId, DirtyErrGetName(sce_net_errno)));
        }

        if ((iResult = sceNetResolverDestroy(pPriv->iSceResolverId)) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetResolverDestroy(%d) failed (err=%s)\n", pPriv->iSceResolverId, DirtyErrGetName(sce_net_errno)));
        }
        pPriv->iSceResolverId = -1;
    }
    if (pPriv->iSceMemPoolId >= 0)
    {
        if ((iResult = sceNetPoolDestroy(pPriv->iSceMemPoolId)) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetPoolDestroy(%d) failed (err=%s)\n", pPriv->iSceMemPoolId, DirtyErrGetName(sce_net_errno)));
        }
        pPriv->iSceMemPoolId = -1;
    }
    if (pPriv->iSceEpollId >= 0)
    {
        if ((iResult = sceNetEpollDestroy(pPriv->iSceEpollId)) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollDestroy(%d) failed (err=%s)\n", pPriv->iSceEpollId, DirtyErrGetName(sce_net_errno)));
        }
        pPriv->iSceEpollId = -1;
    }

    // release resource
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
        struct SceNetSockaddr sceSocketAddr;
        _SockAddrToSceAddr(&sceSocketAddr, pFrom);
        iResult = sceNetRecvfrom(pSocket->uSocket, pBuf, iLen, 0, &sceSocketAddr, (socklen_t *)pFromLen);
        _SceAddrToSockAddr(pFrom, &sceSocketAddr);
        if (iResult > 0)
        {
            // save recv timestamp
            SockaddrInSetMisc(pFrom, NetTick());
        }
    }
    else
    {
        iResult = sceNetRecv(pSocket->uSocket, pBuf, iLen, 0);
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
        if ((!pSocket->bInCallback) && (pSocket->pCallback != NULL) && (pSocket->iCallMask & CALLB_RECV))
        {
            pSocket->bInCallback = TRUE;
            (pSocket->pCallback)(pSocket, 0, pSocket->pCallRef);
            pSocket->bInCallback = FALSE;
            pSocket->uCallLast = NetTick();
        }
    }
    else
    {
        if (sce_net_errno != SCE_NET_EAGAIN)
        {
            // if we are using a TCP socket and we didn't receive positive bytes, we are closed
            if ((pSocket->iType == SOCK_STREAM) && (iRecvResult <= 0))
            {
                NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: [%p] connection %s\n", pSocket, (iRecvResult == 0) ? "closed" : "failed"));
                pSocket->iOpened = -1;
            }
            else
            {
                NetPrintf(("dirtynetps4: [%p] _SocketRecvfrom() to packet queue returned %d (err=%s)\n", pSocket, iRecvResult, DirtyErrGetName(sce_net_errno)));
            }
        }

        // clean up resources that were reserved for the receive operation
        SocketPacketQueueAllocUndo(pSocket->pRecvQueue);
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketCreateEpoll

    \Description
        Create a new sce epoll state.

    \Output
        SceNetId    - the sceEpollId of the new epoll state

    \Version 11/29/2012 (cvienneau)
*/
/********************************************************************************F*/
static SceNetId _SocketCreateEpoll()
{
    SceNetId newId = sceNetEpollCreate("DirtySDK_SocketRecvThread", 0);
    if (newId < 0)
    {
        NetPrintf(("dirtynetps4: error, failed sceNetEpollCreate DirtySDK_SocketRecvThread (err=%s)\n", DirtyErrGetName(sce_net_errno)));
    }
    return(newId);
}

/*F********************************************************************************/
/*!
    \Function    _SocketDestroyEpoll

    \Description
        Destroy existing epoll state

    \Input sceEpollId - id of existing state to destroy

    \Version 11/29/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _SocketDestroyEpoll(SceNetId sceEpollId)
{
    if (sceEpollId >= 0)
    {
        if (sceNetEpollDestroy(sceEpollId) < 0)
        {
            NetPrintf(("dirtynetps4: error, failed sceNetEpollDestroy (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketAddToEpoll

    \Description
        Add the provided socket to sce epoll system.

    \Input sceEpollId  - id of epoll state to add to
    \Input *pSocket - pointer to socket

    \Version 11/29/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _SocketAddToEpoll(SceNetId sceEpollId, SocketT *pSocket)
{
    SceNetEpollEvent eventData;
    eventData.data.ptr = pSocket;
    eventData.events = SCE_NET_EPOLLIN;
    eventData.ident = pSocket->uSocket;
    if (sceNetEpollControl(sceEpollId, SCE_NET_EPOLL_CTL_ADD, eventData.ident, &eventData) < 0)
    {
        NetPrintf(("dirtynetps4: error, failed sceNetEpollControl SCE_NET_EPOLL_CTL_ADD (err=%s)\n", DirtyErrGetName(sce_net_errno)));
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketAddDnsToEpoll

    \Description
        Add the provided host name lookup to sce epoll system.

    \Input *pPriv   - pointer to host lookup record; private, platform dependant

    \Version 12/19/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _SocketAddDnsToEpoll(SocketLookupPrivT *pPriv)
{
    int32_t iResult;
    SceNetEpollEvent eventData;

    //create epoll system
    if ((pPriv->iSceEpollId = sceNetEpollCreate("DirtySDK_DNS", 0)) >= 0)
    {
        // allocate mempool for resolver
        if ((pPriv->iSceMemPoolId = sceNetPoolCreate("SocketLookupMemPool", 4 * 1024, 0)) >= 0)
        {
            // create resolver
            if ((pPriv->iSceResolverId = sceNetResolverCreate("SocketLookup", pPriv->iSceMemPoolId, 0)) >= 0)
            {
                //begin lookup
                if ((iResult = sceNetResolverStartNtoa(pPriv->iSceResolverId, pPriv->Host.name, (SceNetInAddr *)(&pPriv->Host.addr), pPriv->Host.timeout * 1000, 5, SCE_NET_RESOLVER_ASYNC)) >= 0)
                {
                    eventData.data.ptr = pPriv;
                    eventData.events = SCE_NET_EPOLLIN;  //it should really be SCE_NET_EPOLLDESCID, but documentations says to use SCE_NET_EPOLLIN for now
                    eventData.ident = pPriv->iSceResolverId;
                    if (sceNetEpollControl(pPriv->iSceEpollId, SCE_NET_EPOLL_CTL_ADD, eventData.ident, &eventData) < 0)
                    {
                        NetPrintf(("dirtynetps4: error, failed sceNetEpollControl dns, SCE_NET_EPOLL_CTL_ADD(err=%s)\n", DirtyErrGetName(sce_net_errno)));
                        pPriv->Host.done = -1;
                    }
                }
                else
                {
                    NetPrintf(("dirtynetps4: sceNetResolverStartNtoA() failed to start (err=%s)\n", DirtyErrGetName(iResult)));
                    pPriv->Host.done = -1;
                }
            }
            else
            {
                NetPrintf(("dirtynetps4: sceNetResolverCreate() failed (err=%s)\n", DirtyErrGetName(pPriv->iSceResolverId)));
                pPriv->Host.done = -1;
            }
        }
        else
        {
            NetPrintf(("dirtynetps4: sceNetPoolCreate() failed (err=%s)\n", DirtyErrGetName(pPriv->iSceMemPoolId)));
            pPriv->Host.done = -1;
        }
    }
    else
    {
        NetPrintf(("dirtynetps4: error, failed sceNetEpollCreate DirtySDK_DNS (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        pPriv->Host.done = -1;
    }
}

/*F********************************************************************************/
/*!
    \Function    _SocketRecvThread

    \Description
        Wait for incoming data and deliver it immediately to the socket callback,
        if registered.

    \Input  *pArg    - pointer to Socket module state

    \Version 11/28/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _SocketRecvThread(void *pArg)
{
    SceNetEpollEvent resultsList[SOCKET_MAXPOLL];
    SocketT *pSocket;
    int32_t iListIndex, iPollCount, iPollResult = 0;
    SceNetId sceEpollId = -1;
    SocketStateT *pState = (SocketStateT *)pArg;
    char strThreadId[32];

    // get the thread id
    DirtyThreadGetThreadId(strThreadId, sizeof(strThreadId));

    // show we are alive
    NetPrintf(("dirtynetps4: recv thread running (thid=%s)\n", strThreadId));
    pState->iRecvLife = 1;

    // loop until done
    while(pState->iRecvLife == 1)
    {
    //clear up epoll's internal structures by re-creating it
        _SocketDestroyEpoll(sceEpollId);
    sceEpollId = _SocketCreateEpoll();
    iPollCount = 0;

        // acquire global critical section for access to socket list
        NetCritEnter(NULL);

        // walk the socket list and do two things:
        //    1- if the socket is ready for reading, perform the read operation
        //    2- if the buffer in which inbound data is saved is empty, initiate a new low-level read operation for that socket
        for (pSocket = pState->pSockList; (pSocket != NULL) && (iPollCount < SOCKET_MAXPOLL); pSocket = pSocket->pNext)
        {
            // only handle non-virtual sockets with asyncrecv enabled
            if ((pSocket->bVirtual == FALSE) && (pSocket->uSocket != INVALID_SOCKET) && (pSocket->bAsyncRecv == TRUE))
            {
                // acquire socket critical section
                NetCritEnter(&pSocket->RecvCrit);

                // was this socket in the results list of the previous poll() call
                for (iListIndex = 0; iListIndex < iPollResult; iListIndex++)
                {
                    if (resultsList[iListIndex].data.ptr == pSocket)
                    {
                        // socket was in previous results list!
                        // now check if poll() notified that this socket is ready for reading
                        if (resultsList[iListIndex].events & SCE_NET_EPOLLIN)
                        {
                            // ready for reading, so go ahead and read
                                _SocketRecvToPacketQueue(pState, (SocketT *)resultsList[iListIndex].data.ptr);
                        }
                        break;
                    }
                }

                /* if the socket is not virtual, the socket is open (TCP) and if there is room in the recv queue,
                   then add this socket to the poll list to be used by the next poll() call */
                if (!SocketPacketQueueStatus(pSocket->pRecvQueue, 'pful') && (pSocket->uBrokenFlag <= 1) && ((pSocket->iType != SOCK_STREAM) || (pSocket->iOpened > 0)))
                {
                    // add socket to poll list
                    _SocketAddToEpoll(sceEpollId, pSocket);
                    iPollCount += 1;
                }

                // release socket critical section
                NetCritLeave(&pSocket->RecvCrit);
            }
        }

        // release global critical section
        NetCritLeave(NULL);

        // any sockets?
        if (iPollCount > 0)
        {
            // poll for data (wait up to 50ms)
            iPollResult = sceNetEpollWait(sceEpollId, resultsList, iPollCount, 50*1000);
    
            if (iPollResult < 0)
            {
                NetPrintf(("dirtynetps4: sceNetEpollWait() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
    
                // stall for 50ms because experiment shows that next call to poll() may not block
                // internally if a socket is alreay in error.
                sceKernelUsleep(50*1000);
            }
        }
        else
        {
            // no sockets, so stall for 50ms
            sceKernelUsleep(50*1000);
        }
    }

    //clean up
    _SocketDestroyEpoll(sceEpollId);

    // indicate we are done
    NetPrintf(("dirtynetps4: receive thread exit\n"));
    pState->iRecvLife = 0;
}

/*F********************************************************************************/
/*!
    \Function    _SocketGetMacAddress

    \Description
        Attempt to retreive MAC address of the system.

    \Input *pState  - pointer to module state

    \Output
        uint8_t     - TRUE if MAC address found, FALSE otherwise

    \Version 10/24/2012 (jbrookes)
*/
/********************************************************************************F*/
static uint8_t _SocketGetMacAddress(SocketStateT *pState)
{
    SceNetEtherAddr EtherAddr;
    int32_t iResult;

    // get MAC address
    if ((iResult = sceNetGetMacAddress(&EtherAddr, 0)) != 0)
    {
        NetPrintf(("dirtynetps4: could not acquire MAC address (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        return(FALSE);
    }

    // copy it, return success
    ds_memcpy_s(pState->aMacAddr, sizeof(pState->aMacAddr), EtherAddr.data, sizeof(EtherAddr.data));
    return(TRUE);
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
    int32_t iResult;
    union SceNetCtlInfo CtlInfo;

    if (iInfo == 'addr')
    {
        if (pState->uLocalAddr == 0)
        {
            if ((iResult = sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_ADDRESS, &CtlInfo)) == 0)
            {
                pState->uLocalAddr = SocketInTextGetAddr(CtlInfo.ip_address);
            }
            else
            {
                NetPrintf(("dirtynetps4: sceNetCtrlGetInfo(IP_ADDRESS) failed (err=%s)\n", DirtyErrGetName(iResult)));
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
        _SocketNetCtlUpdate(pState);
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

        NetPrintf(("dirtynetps4: 'sdcf' selector used with invalid paramaters\n"));
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

        NetPrintf(("dirtynetps4: 'sdcu' selector used with invalid paramaters\n"));
        return(-1);
    }
    // return global debug output level
    if (iInfo == 'spam')
    {
        return(pState->iVerbose);
    }

    NetPrintf(("dirtynetps4: unhandled global SocketInfo() selector '%C'\n", iInfo));
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

    // init network stack and bring up interface
    if (iOption == 'conn')
    {
        NetPrintf(("dirtynetps4: setting network ~con state.\n"));
        pState->uConnStatus = '~con';

        if (pState->uNetCtlConnectionState == SCE_NET_CTL_STATE_IPOBTAINED)
        {
            // in this case we are already logged in
            // acquire address/mac address
            SocketInfo(NULL, 'addr', 0, NULL, 0);
            SocketInfo(NULL, 'macx', 0, NULL, 0);
            // mark us as online
            NetPrintf(("dirtynetps4: addr=%a mac=%02x:%02x:%02x:%02x:%02x:%02x\n", pState->uLocalAddr,
                pState->aMacAddr[0], pState->aMacAddr[1], pState->aMacAddr[2],
                pState->aMacAddr[3], pState->aMacAddr[4], pState->aMacAddr[5]));
            pState->uConnStatus = '+onl';
        }
        return(0);
    }
    // bring down interface
    if (iOption == 'disc')
    {
        NetPrintf(("dirtynetps4: setting network -off state.\n"));
        pState->bNetEventOccured = TRUE;
        pState->uConnStatus = '-off';
        return(0);
    }
    // handle any idle processing required
    if (iOption == 'idle')
    {
        return(0);
    }
    // set max udp packet size
    if (iOption == 'maxp')
    {
        NetPrintf(("dirtynetps4: setting max udp packet size to %d\n", iData1));
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
    // set the debug log verbosity level
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
            NetPrintfVerbose((pState->iVerbose, 1, "dirtynetps4: added port %d to virtual port list\n", iData1));
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
            NetPrintfVerbose((pState->iVerbose, 1, "dirtynetps4: removed port %d from virtual port list\n", iData1));
            pState->aVirtualPorts[iPort] = 0;
            return(0);
        }
    }
    // unhandled
    NetPrintf(("dirtynetps4: unhandled global SocketControl() option '%C'\n", iOption));
    return(-1);
}

/*** Public Functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function    SocketCreate

    \Description
        Create new instance of socket interface module.  Initializes all global
        resources and makes module ready for use.

    \Input iThreadPrio          - thread priority to start socket thread with
    \Input iThreadStackSize     - stack size to start threads with (in bytes)
    \Input iThreadCpuAffinity   - cpu affinity to start threads with, 0=SCE_KERNEL_CPUMASK_USER_ALL otherwise a mask of cores to use

    \Output
        int32_t                 - negative=error, zero=success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketCreate(int32_t iThreadPrio, int32_t iThreadStackSize, int32_t iThreadCpuAffinity)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    // Query mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // error if already started
    if (pState != NULL)
    {
        NetPrintf(("dirtynetps4: SocketCreate() called while module is already active\n"));
        return(-1);
    }

    // print version info
    NetPrintf(("dirtynetps4: DirtySDK v%d.%d.%d.%d.%d\n", DIRTYSDK_VERSION_YEAR, DIRTYSDK_VERSION_SEASON, DIRTYSDK_VERSION_MAJOR, DIRTYSDK_VERSION_MINOR, DIRTYSDK_VERSION_PATCH));

    // alloc and init state ref
    if ((pState = DirtyMemAlloc(sizeof(*pState), SOCKET_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetps4: unable to allocate module state\n"));
        return(-2);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->iMaxPacket = SOCKET_MAXUDPRECV;
    pState->iVerbose = 1;

    // startup network libs
    NetLibCreate(iThreadPrio, iThreadStackSize, iThreadCpuAffinity);

    // init network stack
    #ifndef DIRTYCODE_PS5
    if ((iResult = sceNetInit()) < 0)
    {
        if (iResult == (signed)SCE_NET_ERROR_EBUSY)  // this error means "already initialized"
        {
            NetPrintf(("dirtynetps4: warning -- sceNet already initialized; sceNetTerm() will not be called on destroy\n"));
            pState->bNetAlreadyInitialized = TRUE;
        }
        else
        {
            NetPrintf(("dirtynetps4: sceNetInit() failed err=%s\n", DirtyErrGetName(iResult)));
            return(-4);
        }
    }
    else
    {
        pState->bNetAlreadyInitialized = FALSE;
    }
    #endif

    // create hostname cache
    if ((pState->pHostnameCache = SocketHostnameCacheCreate(iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetps4: unable to create hostname cache\n"));
        SocketDestroy((uint32_t)(-1));
        return(-4);
    }

    // add our idle handler
    NetIdleAdd(&_SocketIdle, pState);

    // configure thread parameters
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "SocketRecv";
    ThreadConfig.iAffinity = iThreadCpuAffinity;
    ThreadConfig.iPriority = iThreadPrio;
    ThreadConfig.iVerbosity = pState->iVerbose;

    // create high-priority receive thread
    if ((iResult = DirtyThreadCreate(_SocketRecvThread, pState, &ThreadConfig)) < 0)
    {
        NetPrintf(("dirtynetps4: unable to create socket recv thread (err=%d)\n", iResult));
        pState->iRecvLife = 0;
        SocketDestroy(0);
        return(-6);
    }

    // wait for receive thread startup
    while (pState->iRecvLife == 0)
    {
        sceKernelUsleep(100);
    }

    // register the sceNetCtl callback
    pState->iNetEventCid = -1;
    pState->bNetEventOccured = TRUE;
    if ((iResult = sceNetCtlRegisterCallback(_SocketNetCtlCallback, pState, &pState->iNetEventCid)) != 0)
    {
        NetPrintf(("dirtynetps4: sceNetCtlRegisterCallback() failed (err=%s)\n", DirtyErrGetName(iResult)));
        return(-5);
    }

    // save state
    _Socket_pState = pState;
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
    #ifndef DIRTYCODE_PS5
    int32_t iResult;
    #endif

    // error if not active
    if (pState == NULL)
    {
        NetPrintf(("dirtynetps4: SocketDestroy() called while module is not active\n"));
        return(-1);
    }

    NetPrintf(("dirtynetps4: shutting down\n"));

    // remove sceNetCtl callback
    sceNetCtlUnregisterCallback(pState->iNetEventCid);

    // kill idle callbacks
    NetIdleDel(&_SocketIdle, pState);

    // let any idle event finish
    NetIdleDone();

    // tell receive thread to quit (if running)
    if (pState->iRecvLife == 1)
    {
        pState->iRecvLife = 2;
        // wait for thread to terminate
        while (pState->iRecvLife > 0)
        {
            sceKernelUsleep(1*1000);
        }
    }

    // close any remaining sockets
    NetCritEnter(NULL);
    while (pState->pSockList != NULL)
    {
        SocketClose(pState->pSockList);
    }
    NetCritLeave(NULL);

    // clear the kill list
    _SocketIdle(pState);

    // destroy hostname cache
    if (pState->pHostnameCache != NULL)
    {
        SocketHostnameCacheDestroy(pState->pHostnameCache);
    }

    // shut down network libs
    NetLibDestroy(0);

    // shut down sony networking (must come after thread exit and socket cleanup)
    #ifndef DIRTYCODE_PS5
    if ((pState->bNetAlreadyInitialized == FALSE) && ((iResult = sceNetTerm()) < 0))
    {
        NetPrintf(("dirtynetps4: sceNetTerm() failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
    #endif

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

    // close sce socket if allocated
    if (iSocket >= 0)
    {
        // close socket
        if (sceNetSocketClose(iSocket) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetSocketClose(%d) failed (err=%s)\n", iSocket, DirtyErrGetName(sce_net_errno)));
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
        SocketT pointer to the ref is returned, or it can be an actual ps4 socket ref,
        in which case a SocketT is created for the ps4 socket ref.

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
    if (sceNetGetsockopt(uSockRef, SCE_NET_SOL_SOCKET, SCE_NET_SO_TYPE, &iProto, &iProtoSize) == 0)
    {
        // create the socket
        pSock = _SocketOpen(uSockRef, AF_INET, iProto, 0, 0);

        // update local and remote addresses
        SocketInfo(pSock, 'bind', 0, &pSock->LocalAddr, sizeof(pSock->LocalAddr));
        SocketInfo(pSock, 'peer', 0, &pSock->RemoteAddr, sizeof(pSock->RemoteAddr));

        // mark it as imported
        pSock->bImported = TRUE;
    }
    else
    {
        NetPrintf(("dirtynetps4: sceNetGetsockopt(SCE_NET_SO_TYPE) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
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
    int32_t iResult=0;

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
    else if (iHow == (SOCK_NOSEND|SOCK_NORECV))
    {
        iHow = SHUT_RDWR;
    }

    // do the shutdown
    if (sceNetShutdown(pSocket->uSocket, iHow) < 0)
    {
        iResult = sce_net_errno;
        NetPrintf(("dirtynetps4: sceNetShutdown(%d) failed (err=%s)\n", pSocket->uSocket, DirtyErrGetName(iResult)));
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
    struct SceNetSockaddr sceSocketAddr;
    struct sockaddr BindAddr;
    int32_t iResult;

    // make sure socket is valid
    if (pSocket->uSocket < 0)
    {
        NetPrintf(("dirtynetps4: attempt to bind invalid socket\n"));
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    /* bind to specific address returned by sceNetGetCtlInfo(SCE_NET_CTL_INFO_IP_ADDRESS).  we do this
       because in some rare cases we have seen binds to INADDR_ANY bind to the non-default adapter */
    if ((SockaddrInGetAddr(pName) == INADDR_ANY) && (pState->uLocalAddr != 0))
    {
        ds_memcpy_s(&BindAddr, sizeof(BindAddr), pName, sizeof(*pName));
        SockaddrInSetAddr(&BindAddr, pState->uLocalAddr);
        pName = &BindAddr;
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
                if (pSocket->bVirtual && (pSocket->uVirtualport != 0))
                {
                    NetPrintf(("dirtynetps4: [%p] failed to bind socket to %u which was already bound to port %u virtual\n", pSocket, uPort, pSocket->uVirtualport));
                    NetCritLeave(&pSocket->RecvCrit);
                    return(pSocket->iLastError = SOCKERR_INVALID);
                }

                // close winsock socket
                NetPrintf(("dirtynetps4: [%p] making socket bound to port %d virtual\n", pSocket, uPort));
                if (pSocket->uSocket != INVALID_SOCKET)
                {
                    sceNetShutdown(pSocket->uSocket, SCE_NET_SHUT_RDWR);
                    sceNetSocketClose(pSocket->uSocket);
                    pSocket->uSocket = INVALID_SOCKET;
                }
                /* increase socket queue size; this protects virtual sockets from having data pushed into
                   them and overwriting previous data that hasn't been read yet */
                pSocket->pRecvQueue = SocketPacketQueueResize(pSocket->pRecvQueue, 4, pState->iMemGroup, pState->pMemGroupUserData);
                // mark socket as virtual
                pSocket->uVirtualport = uPort;
                pSocket->bVirtual = TRUE;

                // release socket critical section
                NetCritLeave(&pSocket->RecvCrit);
                return(0);
            }
        }
    }

    // do the bind
    _SockAddrToSceAddr(&sceSocketAddr, pName);
    if ((iResult = sceNetBind(pSocket->uSocket, &sceSocketAddr, iNameLen)) < 0)
    {
        NetPrintf(("dirtynetps4: sceNetBind() to port %d failed (err=%s)\n", SockaddrInGetPort(pName), DirtyErrGetName(sce_net_errno)));
    }
    else if (SockaddrInGetPort(&pSocket->LocalAddr) == 0)
    {
        iNameLen = sizeof(pSocket->LocalAddr);
        _SockAddrToSceAddr(&sceSocketAddr, &pSocket->LocalAddr);
        iResult = sceNetGetsockname(pSocket->uSocket, &sceSocketAddr, (socklen_t *)&iNameLen);
        _SceAddrToSockAddr(&pSocket->LocalAddr, &sceSocketAddr);
        NetPrintf(("dirtynetps4: sceNetBind(port=0) succeeded, local address=%a:%d.\n",
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
    struct SceNetSockaddr sceSocketAddr;
    struct sockaddr BindAddr;

    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: connecting to %a:%d\n", SockaddrInGetAddr(pName), SockaddrInGetPort(pName)));

    // first we execute an explicit bind; see note in SocketBind()
    SockaddrInit(&BindAddr, AF_INET);
    SocketBind(pSocket, &BindAddr, sizeof(BindAddr));

    // do the connect
    pSocket->iOpened = 0;
    _SockAddrToSceAddr(&sceSocketAddr, pName);
    if (((iResult = sceNetConnect(pSocket->uSocket, &sceSocketAddr, iNameLen)) < 0) && (sce_net_errno != SCE_NET_EINPROGRESS))
    {
        NetPrintf(("dirtynetps4: sceNetConnect() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
    }
    // if connect succeeded (usually for udp sockets) or attempting to establish a non-blocking connection
    // save correct address
    else
    {
        ds_memcpy_s(&pSocket->RemoteAddr, sizeof(pSocket->RemoteAddr), pName, sizeof(*pName));
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
    if ((iResult = sceNetListen(pSocket->uSocket, iBacklog)) < 0)
    {
        NetPrintf(("dirtynetps4: sceNetListen() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
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
    struct  SceNetSockaddr sceSocketAddr;

    pSocket->iLastError = SOCKERR_INVALID;

    // make sure we have an INET socket
    if ((pSocket->uSocket == INVALID_SOCKET) || (pSocket->iFamily != AF_INET))
    {
        return(NULL);
    }

    // make sure turn parm is valid
    if ((pAddr != NULL) && (*pAddrLen < (signed)sizeof(struct sockaddr)))
    {
        return(NULL);
    }

    // perform inet accept
    if (pAddrLen)
    {
        *pAddrLen = sizeof(*pAddr);
    }

    _SockAddrToSceAddr(&sceSocketAddr, pAddr);
    iResult = sceNetAccept(pSocket->uSocket, &sceSocketAddr, (socklen_t *)pAddrLen);
    _SceAddrToSockAddr(pAddr, &sceSocketAddr);
    if (iResult > 0)
    {
        // Allocate socket structure and install in list
        pOpen = _SocketOpen(iResult, pSocket->iFamily, pSocket->iType, pSocket->iProto, 1);
        pSocket->iLastError = SOCKERR_NONE;
    }
    else
    {
        pSocket->iLastError = _SocketTranslateError(iResult);

        if (sce_net_errno != SCE_NET_EWOULDBLOCK)
        {
            NetPrintf(("dirtynetps4: accept() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
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
        int32_t     - standard network error code (SOCKERR_xxx)

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketSendto(SocketT *pSocket, const char *pBuf, int32_t iLen, int32_t iFlags, const struct sockaddr *pTo, int32_t iToLen)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iResult = -1;
    struct SceNetSockaddr sceSocketAddr;

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
        NetPrintf(("dirtynetps4: attempting to send to %a:%d on invalid socket\n", uAddr, uPort));
        #endif
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // handle optional data rate throttling
    if ((iLen = SocketRateThrottle(&pSocket->SendRate, pSocket->iType, iLen, "send")) == 0)
    {
        return(0);
    }

    // use appropriate version
    if (pTo == NULL)
    {
        if ((iResult = _SocketTranslateError(sceNetSend(pSocket->uSocket, pBuf, iLen, 0))) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetSend() failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
    }
    else
    {
        // do the send
        #if SOCKET_VERBOSE
        NetPrintf(("dirtynetps4: sending %d bytes to %a:%d\n", iLen, SockaddrInGetAddr(pTo), SockaddrInGetPort(pTo)));
        #endif
        _SockAddrToSceAddr(&sceSocketAddr, pTo);
        if ((iResult = _SocketTranslateError(sceNetSendto(pSocket->uSocket, pBuf, iLen, 0, &sceSocketAddr, iToLen))) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetSendto(%a:%d) failed (err=%s)\n", SockaddrInGetAddr(pTo), SockaddrInGetPort(pTo), DirtyErrGetName(sce_net_errno)));
        }
    }

    // translate error
    pSocket->iLastError = iResult;
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

    // clear "bHasdata" hint
    pSocket->bHasdata = 0;

    // handle rate throttling, if enabled
    if ((iLen = SocketRateThrottle(&pSocket->RecvRate, pSocket->iType, iLen, "recv")) == 0)
    {
        return(0);
    }
    // handle socked been killed
    if (pSocket->pKill != NULL)
    {
        pSocket->iLastError = SOCKERR_INVALID;
        return(pSocket->iLastError);
    }

    // sockets marked for async recv had actual receive operation take place in the thread
    if (pSocket->bAsyncRecv == TRUE)
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
            iErrno = SCE_NET_EWOULDBLOCK;
        }

        // when data is obtained from the packet queue, we lose visibility on system socket errors
        pSocket->iLastError = SOCKERR_NONE;

        // release socket receive critical section
        NetCritLeave(&pSocket->RecvCrit);
    }
    else // non-async recvthread socket
    {
        // do direct recv call
        if (((iRecv = _SocketRecvfrom(pSocket, pBuf, iLen, pFrom, pFromLen)) < 0) && ((iErrno = sce_net_errno) != SCE_NET_EAGAIN))
        {
            NetPrintf(("dirtynetps4: _SocketRecvfrom() failed on a SOCK_STREAM socket (err=%s)\n", DirtyErrGetName(iErrno)));
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
            'ethr'/'macx' - local ethernet address (returned in pBuf), 0=success, negative=error
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
        \endverbatim

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen)
{
    int32_t iResult;

    // always zero results by default
    if (pBuf != NULL)
    {
        ds_memclr(pBuf, iLen);
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
                SockaddrInSetPort((struct sockaddr *)pBuf, pSocket->uVirtualport);
            }
            else
            {
                sceNetGetsockname(pSocket->uSocket, pBuf, (socklen_t *)&iLen);
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
            sceNetGetpeername(pSocket->uSocket, pBuf, (socklen_t *)&iLen);
        }
        return(0);
    }

    // get packet queue info
    if ((iInfo == 'pdrp') || (iInfo == 'pmax') || (iInfo == 'pnum') || (iInfo == 'psiz'))
    {
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
        return(pSocket->bHasdata);
    }

    // return last socket error
    if (iInfo == 'serr')
    {
        return(pSocket->iLastError);
    }

    // return ps4 socket ref
    if (iInfo == 'sock')
    {
        return(pSocket->uSocket);
    }

    // return socket status
    if (iInfo == 'stat')
    {
        SceNetEpollEvent eventPollIn, eventPollOut;
        SceNetId eId;

        // if not a connected socket, return TRUE
        if (pSocket->iType != SOCK_STREAM)
        {
            return(1);
        }

        // create poll descriptor
        if ((eId = sceNetEpollCreate("DirtyNet_SocketInfoEpoll", 0)) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollCreate failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            pSocket->iOpened = -1;
            return(pSocket->iOpened);
        }

        // set up poll events
        ds_memclr(&eventPollIn, sizeof(eventPollIn));
        eventPollIn.events = (pSocket->iOpened == 0) ? SCE_NET_EPOLLOUT : SCE_NET_EPOLLIN;
        if (sceNetEpollControl(eId, SCE_NET_EPOLL_CTL_ADD, pSocket->uSocket, &eventPollIn) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollControl failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
            sceNetEpollDestroy(eId);
            pSocket->iOpened = -1;
            return(pSocket->iOpened);
        }

        /* $$todo$$ this function should be refactored to be structured like the stat on other platforms.
           this helps code sharing between the unix based implementations and it makes more logical sense
           when trying to review the code */

        // execute non-blocking epoll check
        if ((iResult = sceNetEpollWait(eId, &eventPollOut, 1, 0)) > 0)
        {
            if (pSocket->iOpened == 0)
            {
                if (eventPollOut.events & SCE_NET_EPOLLOUT)
                {
                    SceNetSocklen_t iOptLen = sizeof(SceNetSocklen_t);
                    int32_t iError;
                    if (((iResult = sceNetGetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_ERROR, &iError, &iOptLen)) == 0) && (iError == 0))
                    {
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: connection open\n"));
                        pSocket->iOpened = 1;
                    }
                    else if (iResult < 0)
                    {
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: sceNetGetsockopt() failed (err=%s)\n", DirtyErrGetName(iResult)));
                        pSocket->iOpened = -1;
                    }
                    else
                    {
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: connection failed (err=%s)\n", DirtyErrGetName(iError)));
                        pSocket->iOpened = -1;
                    }
                }
                else if (eventPollOut.events & (SCE_NET_EPOLLHUP|SCE_NET_EPOLLERR))
                {
                    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: read exception on connect\n"));
                    pSocket->iOpened = -1;
                }
            }
            /* if previously connected, make sure connect still valid. we do this when not doing async receive for two reasons:
               1. there is a race condition between the epoll waking up and querying the bytes available. if the bytes are read on the
                  receive thread between the epoll and the recv then it would think the socket is closed because the socket has already
                  been drained.
               2. our reasoning behind using the async receive thread could be the cost of the recv call. the async receive thread
               will already set the correct state on the socket thus we can skip the query and return iOpened back to the user */
            else if (!pSocket->bAsyncRecv && (pSocket->iOpened > 0))
            {
                if (eventPollOut.events & SCE_NET_EPOLLIN)
                {
                    if (sceNetRecv(pSocket->uSocket, (char*)&iData, 1, SCE_NET_MSG_PEEK) <= 0)
                    {
                        //sceNetRecv Error
                        NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: connection closed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
                        pSocket->iLastError = SOCKERR_CLOSED;
                        pSocket->iOpened = -1;
                    }
                }
                else if (eventPollOut.events & (SCE_NET_EPOLLHUP|SCE_NET_EPOLLERR))
                {
                    NetPrintfVerbose((pSocket->iVerbose, 0, "dirtynetps4: connection failure\n"));
                    pSocket->iOpened = -1;
                }
            }        
            /* if we still have packets in the queue, tell the caller that we are still open. this makes sure they read the
               complete stream of data */
            else if (pSocket->bAsyncRecv && (pSocket->iOpened < 0) && (SocketInfo(pSocket, 'pnum', 0, NULL, 0) != 0))
            {
                return(1);
            }
        }
        else if (iResult < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollWait failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }

        // remove poll from control block (is this required?)
        if (sceNetEpollControl(eId, SCE_NET_EPOLL_CTL_DEL, pSocket->uSocket, NULL) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollControl failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
        // clean up epoll
        if (sceNetEpollDestroy(eId) < 0)
        {
            NetPrintf(("dirtynetps4: sceNetEpollControl failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
        // return connect status
        return(pSocket->iOpened);
    }


    // unhandled option?
    NetPrintf(("dirtynetps4: unhandled SocketInfo() option '%C'\n", iInfo));
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
int32_t SocketCallback(SocketT *pSocket, int32_t iMask, int32_t iIdle, void *pRef, int32_t (*pProc)(SocketT *pSock, int32_t iFlags, void *pRef))
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
            'conn' - init network stack
            'disc' - bring down network stack
            'idle' - perform any network connection related processing
            'maxp' - set max udp packet size
            'maxr' - set max recv rate (bytes/sec; zero=uncapped)
            'maxs' - set max send rate (bytes/sec; zero=uncapped)
            'nbio' - set nonblocking/blocking mode (TCP only, iData1=TRUE (nonblocking) or FALSE (blocking))
            'ndly' - set TCP_NODELAY state for given stream socket (iData1=zero or one)
            'pdev' - set simulated packet deviation
            'plat' - set simulated packet latency
            'plos' - set simulated packet loss
            'poll' - block waiting on input from socket list (iData1=ms to block)
            'pque' - set socket packet queue depth
            'push' - push data into given socket receive buffer (iData1=size, pData2=data ptr, pData3=sockaddr ptr)
            'radr' - set SO_REUSEADDR On the specified socket
            'rbuf' - set socket recv buffer size
            'sbuf' - set socket send buffer size
            'scbk' - enable/disable "send callbacks usage" on specified socket (defaults to enable)
            'sdcb' - set/unset send callback (iData1=TRUE for set - FALSE for unset, pData2=callback, pData3=callref)
            'soli' - set SO_LINGER on the specified socket, iData1 is timeout in seconds
            'spam' - set the debug log verbosity level
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

    // handle the global controls
    if (pSocket == NULL)
    {
        return(_SocketControlGlobal(iOption, iData1, pData2, pData3));
    }

    // set async recv enable
    if (iOption == 'arcv')
    {
        // set socket async recv flag
        pSocket->bAsyncRecv = iData1 ? TRUE : FALSE;
        return(0);
    }

    // set max recv rate
    if (iOption == 'maxr')
    {
        NetPrintf(("dirtynetps4: setting max recv rate to %d bytes/sec\n", iData1));
        pSocket->RecvRate.uMaxRate = iData1;
        return(0);
    }
    // set max send rate
    if (iOption == 'maxs')
    {
        NetPrintf(("dirtynetps4: setting max send rate to %d bytes/sec\n", iData1));
        pSocket->SendRate.uMaxRate = iData1;
        return(0);
    }
    // enable/disable "send callbacks usage" on specified socket (defaults to enable)
    if (iOption == 'scbk')
    {
        if (pSocket->bSendCbs != (iData1?TRUE:FALSE))
        {
            NetPrintf(("dirtynetps4: send callbacks usage changed from %s to %s for socket ref %p\n", (pSocket->bSendCbs?"ON":"OFF"), (iData1?"ON":"OFF"), pSocket));
            pSocket->bSendCbs = (iData1?TRUE:FALSE);
        }
        return(0);
    }
    // set the debug log verbosity level
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
        int32_t iVal = sceKernelFcntl(pSocket->uSocket, SCE_KERNEL_F_GETFL, SCE_KERNEL_O_NONBLOCK);
        iVal = iData1 ? (iVal | SCE_KERNEL_O_NONBLOCK) : (iVal & ~SCE_KERNEL_O_NONBLOCK);
        iResult = sceKernelFcntl(pSocket->uSocket, SCE_KERNEL_F_SETFL, iVal);
        pSocket->iLastError = _SocketTranslateError(iResult);
        NetPrintf(("dirtynetps4: setting socket:0x%x to %s mode %s (LastError=%d).\n", pSocket, iData1 ? "nonblocking" : "blocking", iResult ? "failed" : "succeeded", pSocket->iLastError));
        return(pSocket->iLastError);
    }
    // if a stream socket, set TCP_NODELAY state
    if ((iOption == 'ndly') && (pSocket->iType == SOCK_STREAM))
    {
        if ((iResult = sceNetSetsockopt(pSocket->uSocket, SCE_NET_IPPROTO_TCP, SCE_NET_TCP_NODELAY, &iData1, sizeof(iData1))) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_IPPROTO_TCP, SCE_NET_TCP_NODELAY) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
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
            NetPrintf(("dirtynetps4: request to push %d bytes of data discarded (max=%d)\n", iData1, SOCKET_MAXUDPRECV));
            NetCritLeave(&pSocket->RecvCrit);
            return(-1);
        }

        // add packet to queue
        SocketPacketQueueAdd(pSocket->pRecvQueue, (uint8_t *)pData2, iData1, (struct sockaddr *)pData3);
        // remember we have data
        pSocket->bHasdata = 1;

        // release socket critical section
        NetCritLeave(&pSocket->RecvCrit);

        // see if we should issue callback
        if ((pSocket->pCallback != NULL) && (pSocket->iCallMask & CALLB_RECV))
        {
            pSocket->pCallback(pSocket, 0, pSocket->pCallRef);
        }
        return(0);
    }
    // set SCE_NET_SO_REUSEADDR
    if (iOption == 'radr')
    {
        if ((iResult = sceNetSetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_REUSEADDR, (const char *)&iData1, sizeof(iData1))) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_SOL_SOCKET, SCE_NET_SO_REUSEADDR) failed (err=%s)\n", DirtyErrGetName(sce_net_errno)));
        }
        pSocket->iLastError = _SocketTranslateError(iResult);
        return(pSocket->iLastError);
    }
    // set socket receive buffer size
    if ((iOption == 'rbuf') || (iOption == 'sbuf'))
    {
        int32_t iOldSize, iNewSize;
        int32_t iSockOpt = (iOption == 'rbuf') ? SCE_NET_SO_RCVBUF : SCE_NET_SO_SNDBUF;
        socklen_t uOptLen = 4;

        // get current buffer size
        sceNetGetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, iSockOpt, (char *)&iOldSize, &uOptLen);

        // set new size
        if ((iResult = sceNetSetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, iSockOpt, (const char *)&iData1, sizeof(iData1))) != 0)
        {
            NetPrintf(("dirtynetps4: sceNetSetsockopt(SCE_NET_SOL_SOCKET, %s) failed (err=%s)\n", ((iOption == 'rbuf') ? "SCE_NET_SO_RCVBUF" : "SCE_NET_SO_SNDBUF"), DirtyErrGetName(sce_net_errno)));
        }
        if ((pSocket->iLastError = _SocketTranslateError(iResult)) == SOCKERR_NONE)
        {
            // save new buffer size
            if (iOption == 'rbuf')
            {
                pSocket->iRbufsize = iData1;
            }
            else
            {
                pSocket->iSbufsize = iData1;
            }
        }

        // get new size
        sceNetGetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, iSockOpt, (char *)&iNewSize, &uOptLen);
        NetPrintf(("dirtynetps4: sceNetGetsockopt(%s) changed buffer size from %d to %d\n", (iOption == 'rbuf') ? "SCE_NET_SO_RCVBUF" : "SCE_NET_SO_SNDBUF",
            iOldSize, iNewSize));

        return(pSocket->iLastError);
    }
    if (iOption == 'soli')
    {
        struct linger LingerOptions;
        LingerOptions.l_onoff = TRUE;
        LingerOptions.l_linger = iData1;
        pSocket->iLastError = _SocketTranslateError(sceNetSetsockopt(pSocket->uSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_LINGER, &LingerOptions, sizeof(LingerOptions)));
        return(pSocket->iLastError);
    }
    // unhandled
    NetPrintf(("dirtynetps4: unhandled control option '%C'\n", iOption));
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
    SocketLookupPrivT *pPriv, *pHostRef;
    int32_t iAddr;

    NetPrintf(("dirtynetps4: looking up address for host '%s'\n", pText));

    // dont allow negative timeouts
    if (iTimeout < 0)
    {
        return(NULL);
    }

    // create new structure
    pPriv = DirtyMemAlloc(sizeof(*pPriv), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    ds_memclr(pPriv, sizeof(*pPriv));

    // setup callbacks
    pPriv->Host.Done = &_SocketLookupDone;
    pPriv->Host.Free = &_SocketLookupFree;
    // copy over the target address
    ds_strnzcpy(pPriv->Host.name, pText, sizeof(pPriv->Host.name));
    // initialize sce IDs to invalid/unset
    pPriv->iSceEpollId = pPriv->iSceMemPoolId = pPriv->iSceResolverId = -1;

    // look for refcounted lookup
    if ((pHostRef = (SocketLookupPrivT *)SocketHostnameAddRef(&pState->pHostList, &pPriv->Host, TRUE)) != NULL)
    {
        // the one we found is good, we can use that and not make a new one
        if (pHostRef->iSceResolverId != -1)
        {
            DirtyMemFree(pPriv, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
            return(&pHostRef->Host);
        }
        // the one we found is already being torn down
        else
        {
            NetPrintf(("dirtynetps4: adding new lookup since current matching ref is being destroyed.\n"));
            // undo the refcount done by SocketHostnameAddRef(,,True)
            pHostRef->Host.refcount -= 1;
            // add the new lookup entry, force to not use a ref of an exisiting one
            SocketHostnameAddRef(&pState->pHostList, &pPriv->Host, FALSE);
        }
    }

    // check for dot notation
    if (((iAddr = SocketInTextGetAddr(pText)) != 0) || ((iAddr = SocketHostnameCacheGet(pState->pHostnameCache, pText, /*pState->iVerbose*/ 1)) != 0))
    {
        // we've got a dot-notation address
        pPriv->Host.addr = iAddr;
        pPriv->Host.done = 1;
        // return completed record
        return(&pPriv->Host);
    }

    // start an epoll dns resolve
    _SocketAddDnsToEpoll(pPriv);

    // return the host reference
    return(&pPriv->Host);
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

    \Version 01/23/2013 (amakoukji)
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

