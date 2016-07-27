/*
Copyright (C) 2005, 2009, 2010, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <unistd.h>

#include "dirtyplatform.h"


#include "dirtysock.h"
#include "dirtyvers.h"
#include "dirtymem.h"


/*** Defines **********************************************************************/

#define INVALID_SOCKET      (-1)

#define SOCKET_VERBOSE      (DIRTYCODE_DEBUG && FALSE)

/*** Type Definitions *************************************************************/

//! private socketlookup structure containing extra data
typedef struct SocketLookupPrivT
{
    //DO NOT CHANGE FOLLOWING STRUCTURE WITHOUT FOLLOWING THE CODE FIRST.
	HostentT            Host;      //!< MUST COME FIRST!

} SocketLookupPrivT;

//! dirtysock connection socket structure
struct SocketT
{
    int32_t family;             //!< protocol family
    int32_t type;               //!< protocol type
    int32_t proto;              //!< protocol ident

    int8_t  opened;             //!< negative=error, zero=not open (connecting), positive=open
    uint8_t unused;
    
    int32_t socket;             //!< unix socket ref

    struct sockaddr local;      //!< local address
    struct sockaddr remote;     //!< remote address

};



//! local state
typedef struct SocketStateT
{
    
    // module memory group
    int32_t  iMemGroup;                 //!< module mem group id
    void     *pMemGroupUserData;        //!< user data associated with mem group
    
    void     *pMemPool;                 //!< sys_net memory pool pointer

    int32_t  iSid;                      //!< state id    
    int32_t  iHandlerId;                //!< id of connection handler
    uint32_t uConnStatus;               //!< current connection status
    uint32_t uLocalAddr;                //!< local internet address for active interface
    uint8_t  aMacAddr[6];               //!< mac address for active interface
    
} SocketStateT;

/*** Variables ********************************************************************/

//! module state ref
static SocketStateT *_Socket_pState = NULL;

//! mempool size for sys_net
static int32_t      _Socket_iMemPoolSize = 128 * 1024;

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function    _XlatError

    \Description
        Translate a BSD error to dirtysock

    \Input iErr     - BSD error code (e.g EAGAIN)

    \Output
        int32_t         - dirtysock error

    \Version 06/21/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _XlatError(int32_t iErr)
{
	if (iErr < 0)
    {
        iErr = errno;
        if ((iErr == EWOULDBLOCK) || (iErr == EINPROGRESS))
            iErr = SOCKERR_NONE;
        else if (iErr == EHOSTUNREACH)
            iErr = SOCKERR_UNREACH;
        else if (iErr == ENOTCONN)
            iErr = SOCKERR_NOTCONN;
        else if (iErr == ECONNREFUSED)
            iErr = SOCKERR_REFUSED;
        else if (iErr == ECONNRESET)
            iErr = SOCKERR_CONNRESET;
        //else if ((iErr == EBADF) || (iErr == EPIPE))
        //    iErr = SOCKERR_BADPIPE;
        else
            iErr = SOCKERR_OTHER;
    }
    return(iErr);
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
    if ((iSocket = socket(iAddrFamily, iType, iProto)) >= 0)
    {
        uint32_t uTrue = 1;
        // if dgram, allow broadcast
        if (iType == SOCK_DGRAM)
        {
            setsockopt(iSocket, SOL_SOCKET, SO_BROADCAST, &uTrue, sizeof(uTrue));
        }
        // if a raw socket, set header include
        if (iType == SOCK_RAW)
        {
            setsockopt(iSocket, IPPROTO_IP, IP_HDRINCL, &uTrue, sizeof(uTrue));
        }
        // set nonblocking operation
        if (fcntl(iSocket, F_SETFL, O_NONBLOCK) < 0)
        {
            NetPrintf(("dirtynetunix: error trying to make socket non-blocking (err=%d)\n", errno));
        }
    }
    else
    {
        NetPrintf(("dirtynetunix: socket() failed (err=%d)\n", errno));
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
    \Input iType        - socket type (SOCK_STREAM, ...)
    \Input iProto       - protocol type for SOCK_RAW (unused by others)
    \Input iOpened      - 0=not open (connecting), 1=open

    \Output
        SocketT *   - socket reference

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static SocketT *_SocketOpen(int32_t iSocket, int32_t iAddrFamily, int32_t iType, int32_t iProto, int32_t iOpened)
{
    SocketStateT *pState = _Socket_pState;
    SocketT *pSocket;
    
    // allocate memory
    if ((pSocket = DirtyMemAlloc(sizeof(*pSocket), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetunix: unable to allocate memory for socket\n"));
        return(NULL);
    }
    memset(pSocket, 0, sizeof(*pSocket));
    
    if (iSocket == -1)
    {
        // create socket (ipv4)
        iAddrFamily = AF_INET;
        iSocket = _SocketCreateSocket(iAddrFamily, iType, iProto);
    }
    
    // set family/proto info
    pSocket->family = iAddrFamily;
    pSocket->type = iType;
    pSocket->proto = iProto;
    pSocket->socket = iSocket;
    pSocket->opened = iOpened;
       
        
    // return the socket
    return(pSocket);}

/*F********************************************************************************/
/*!
    \Function _SocketClose
    
    \Description
        Disposes of a SocketT, including disposal of the SocketT allocated memory.  Does
        NOT dispose of the unix socket ref.
    
    \Input *pSocket - socket to close
    
    \Output
        None.
            
    \Version 06/21/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _SocketClose(SocketT *pSocket)
{
    //SocketStateT *pState = _Socket_pState;
    
	pSocket->socket = INVALID_SOCKET;
    pSocket->opened = FALSE;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function    SocketLookupDone

    \Description
        Callback to determine if gethostbyname is complete.

    \Input *host    - pointer to host lookup record

    \Output
        int32_t         - zero=in progess, neg=done w/error, pos=done w/success

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
    \Function    SocketLookupFree

    \Description
        Release resources used by SocketLookup()

    \Input *host    - pointer to host lookup record

    \Output
        None.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
static void _SocketLookupFree(HostentT *pHost)
{
	SocketStateT *pState = _Socket_pState;
	DirtyMemFree(pHost, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function    _SocketRecvfrom

    \Description
        Receive data from a remote host on a datagram socket.

    \Input *pSocket - socket reference
    \Input *pBuf    - buffer to receive data
    \Input iLen     - length of recv buffer
    \Input *pFrom   - address data was received from (NULL=ignore)
    \Input *pFromLen- length of address

    \Output
        int32_t         - positive=data bytes received, else error

    \Version 1.0 09/10/04 (jbrookes) First Version
*/
/********************************************************************************F*/
static int32_t _SocketRecvfrom(SocketT *pSocket, char *pBuf, int32_t iLen, struct sockaddr *pFrom, int32_t *pFromLen)
{
    int32_t iResult;
    
    if (pFrom != NULL)
    {
        // do the receive
        if ((iResult = recvfrom(pSocket->socket, pBuf, iLen, 0, pFrom, (socklen_t *)pFromLen)) > 0)
        {
            // save recv timestamp
            SockaddrInSetMisc(pFrom, NetTick());
        }
    }
    else
    {
        iResult = recv(pSocket->socket, pBuf, iLen, 0);
    }
    
    
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    SocketCreate

    \Description
        Create new instance of socket interface module.  Initializes all global
        resources and makes module ready for use.

    \Input  iThreadPrio - thread priority to start socket thread with

    \Output
        int32_t         - negative=error, zero=success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketCreate(int32_t iThreadPrio)
{
    SocketStateT *pState = _Socket_pState;
	int32_t iMemGroup;
    void *pMemGroupUserData;
    
    // Query mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // error if already started
    if (pState != NULL)
    {
        NetPrintf(("dirtynetunix: SocketCreate() called while module is already active\n"));
        return(-1);
    }

    // print version info
    NetPrintf(("dirtynetunix: DirtySock SDK v%d.%d.%d.%d\n",
        (DIRTYVERS>>24)&0xff, (DIRTYVERS>>16)&0xff,
        (DIRTYVERS>> 8)&0xff, (DIRTYVERS>> 0)&0xff));

    // alloc and init state ref
    if ((pState = DirtyMemAlloc(sizeof(*pState), SOCKET_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetunix: unable to allocate module state\n"));
        return(-2);
    }
    memset(pState, 0, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    
    
    if ((pState->pMemPool = DirtyMemAlloc(_Socket_iMemPoolSize, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetunix: uanble to allocate %d byte memory pool for sys_net\n", _Socket_iMemPoolSize));
        DirtyMemFree(pState, SOCKET_MEMID, iMemGroup, pMemGroupUserData);
        return(-3);
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

    \Input uFlags   - shutdown flags

    \Output
        int32_t     - negative=error, zero=success

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketDestroy(uint32_t uFlags)
{
    SocketStateT *pState = _Socket_pState;
 //   int32_t iResult;
    
    // error if not active
    if (pState == NULL)
    {
        NetPrintf(("dirtynetunix: SocketDestroy() called while module is not active\n"));
        return(-1);
    }

    NetPrintf(("dirtynetunix: shutting down\n"));

	
    // dispose of sys_net memory pool
    DirtyMemFree(pState->pMemPool, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);

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
    \Input iType        - socket type (SOCK_STREAM, ...)
    \Input iProto       - protocol type for  (unused by others)

    \Output
        SocketT *   - socket reference

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

    \Input *pSocket - socket reference

    \Output
        int32_t         - zero

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketClose(SocketT *pSocket)
{
    int32_t iSocket = pSocket->socket;
    
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
        if (close(iSocket) < 0)
        {
            NetPrintf(("dirtynetunix: close() failed (err=%d)\n", errno));
        }
    }
    
    // success
    
    return(0);
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
        int32_t         - zero

    \Version 1.0 09/10/04 (jbrookes) First Version
*/
/********************************************************************************F*/
int32_t SocketShutdown(SocketT *pSocket, int32_t iHow)
{
    int32_t iResult=0;
    
    // only shutdown a connected socket
    if (pSocket->type != SOCK_STREAM)
    {
        return(SOCKERR_NONE);
    }
    
    // make sure socket ref is valid
    if (pSocket->socket == INVALID_SOCKET)
    {
        return(SOCKERR_NONE);
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
	if ((iResult = shutdown(pSocket->socket, iHow))< 0)
    {
        NetPrintf(("dirtynetunix: shutdown() failed (err=%d)\n", iResult));
    }
    return(_XlatError(iResult));
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
        int32_t         - standard network error code (SOCKERR_xxx)

    \Notes
        If either address or port is zero, then they are filled in automatically.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketBind(SocketT *pSocket, const struct sockaddr *pName, int32_t iNameLen)
{
    //SocketStateT *pState = _Socket_pState;
    //struct sockaddr SockAddr;
	int32_t iResult;
    
    // make sure socket is valid
    if (pSocket->socket < 0)
    {
        NetPrintf(("dirtynetunix: attempt to bind invalid socket\n"));
        return(SOCKERR_INVALID);
    }

    
    // save local address
    memcpy(&pSocket->local, pName, sizeof(pSocket->local));

    // do the bind
    if ((iResult = bind(pSocket->socket, pName, iNameLen)) < 0)
    {
        NetPrintf(("dirtynetunix: bind() to port %d failed (err=%d)\n", SockaddrInGetPort(pName), iResult));
    }
    return(_XlatError(iResult));
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
        int32_t         - standard network error code (NETERR_xxx)

    \Notes
        Only has real meaning for stream protocols. For a datagram protocol, this
        just sets the default remote host.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketConnect(SocketT *pSocket, struct sockaddr *pName, int32_t iNameLen)
{
    struct sockaddr SockAddr;
    int32_t iResult;
    
    NetPrintf(("dirtynetunix: connecting to %s:%d\n", SocketInAddrGetText(SockaddrInGetAddr(pName)), SockaddrInGetPort(pName)));
    
    // do the connect
    pSocket->opened = 0;
    iNameLen = sizeof(SockAddr);
    if (((iResult = connect(pSocket->socket, pName, iNameLen)) < 0) && _XlatError(iResult) != SOCKERR_NONE)//or in progress
    {
        NetPrintf(("dirtynetunix: connect() failed (err=%d)\n", iResult));
    }
    return (_XlatError(iResult));
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
        int32_t         - standard network error code (NETERR_xxx)

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketListen(SocketT *pSocket, int32_t iBacklog)
{
    // do the listen
    int32_t iResult;
    if ((iResult = listen(pSocket->socket, iBacklog)) < 0)
    {
        NetPrintf(("dirtynetunix: listen() failed (err=%d)\n", iResult));
    }
    return(_XlatError(iResult));
}

/*F********************************************************************************/
/*!
    \Function    SocketAccept

    \Description
        Accept an incoming connection attempt on a socket.

    \Input *sock    - socket reference to socket in listening state (see SocketListen())
    \Input *addr    - pointer to storage for address of the connecting entity, or NULL
    \Input *addrlen - pointer to storage for length of address, or NULL

    \Output
        SocketT *   - the accepted socket, or NULL if not available

    \Notes
        The integer pointed to by addrlen should on input contain the number of characters
        in the buffer addr.  On exit it will contain the number of characters in the
        output address.

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
SocketT *SocketAccept(SocketT *pSocket, struct sockaddr *pAddr, int32_t *pAddrLen)
{
    int32_t iAddrLen, iIncoming;
    struct sockaddr SockAddr;
    SocketT *pOpen = NULL;
    
    // make sure we have an INET socket
    if ((pSocket->socket == INVALID_SOCKET) || (pSocket->family != AF_INET))
    {
        return(NULL);
    }

    // make sure turn parm is valid
    if ((pAddr != NULL) && (*pAddrLen < (signed)sizeof(struct sockaddr)))
    {
        return(NULL);
    }
    
    // perform inet accept
    iAddrLen = sizeof(SockAddr);
    if ((iIncoming = accept(pSocket->socket, &SockAddr, &iAddrLen)) > 0)
    {
        // Allocate socket structure and install in list
        pOpen = _SocketOpen(iIncoming, pSocket->family, pSocket->type, pSocket->proto, 1);
    }
    else if (_XlatError(iIncoming) != SOCKERR_NONE)//would block
    {
        NetPrintf(("dirtynetunix: accept() failed (err=%d)\n", iIncoming));
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

    \Input *sock    - socket reference
    \Input *buf     - the data to be sent
    \Input len      - size of data
    \Input flags    - unused
    \Input *to      - the address to send to (NULL=use connection address)
    \Input tolen    - length of address

    \Output
        int32_t         - standard network error code (NETERR_xxx)

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketSendto(SocketT *pSocket, const char *pBuf, int32_t iLen, int32_t iFlags, const struct sockaddr *pTo, int32_t iToLen)
{
    //SocketStateT *pState = _Socket_pState;
    int32_t iResult = -1;

    // make sure socket ref is valid
    if (pSocket->socket < 0)
    {
        NetPrintf(("dirtynetunix: attempting to send on invalid socket\n"));
        return(SOCKERR_INVALID);
    }
    
    // use appropriate version
    if (pTo == NULL)
    {
        if ((iResult = send(pSocket->socket, (void*)pBuf, iLen, 0)) < 0)
        {
            NetPrintf(("dirtynetunix: send() failed (err=%d)\n", iResult));
        }
    }
    else
    {
        // do the send
        #if SOCKET_VERBOSE
        NetPrintf(("dirtynetunix: sending %d bytes to %s\n", iLen, SocketInAddrGetText(SockaddrInGetAddr(pTo))));
        #endif
        if ((iResult = sendto(pSocket->socket, (void*)pBuf, iLen, 0, pTo, iToLen)) < 0)
        {
            NetPrintf(("dirtynetunix: sendto() failed (err=%d)\n", iResult));
        }
    }

    // return bytes sent
    return(_XlatError(iResult));
}

/*F********************************************************************************/
/*!
    \Function    SocketRecvfrom

    \Description
        Receive data from a remote host. If socket is a connected stream, then data can
        only come from that source. A datagram socket can receive from any remote host.

    \Input *pSocket - socket reference
    \Input *pBuf    - buffer to receive data
    \Input iLen     - length of recv buffer
    \Input iFlags   - unused
    \Input *pFrom   - address data was received from (NULL=ignore)
    \Input *pFromLen- length of address

    \Output
        int32_t         - positive=data bytes received, else standard error code

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketRecvfrom(SocketT *pSocket, char *pBuf, int32_t iLen, int32_t iFlags, struct sockaddr *pFrom, int32_t *pFromLen)
{
    int32_t iRecv = -1;
    
    // receiving on a dgram socket?
    if ((pSocket->type == SOCK_DGRAM) || (pSocket->type == SOCK_RAW))
    {
        
    }
    else if (pSocket->type == SOCK_STREAM)
    {
        // make sure socket ref is valid
        if (pSocket->socket == INVALID_SOCKET)
        {
            return(SOCKERR_INVALID);
        }
        // do direct recv call
        if (((iRecv = _SocketRecvfrom(pSocket, pBuf, iLen, pFrom, pFromLen)) < 0) && (errno != EAGAIN))//or again
        {
            NetPrintf(("dirtynetunix: _SocketRecvfrom() failed (err=%d)\n", iRecv));
        }
    }

    // do error conversion
    iRecv = (iRecv == 0) ? SOCKERR_CLOSED : _XlatError(iRecv);
    
    // return the error code
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
        int32_t     - size of returned data or error code (negative value)

    \Notes
        iInfo can be one of the following:
        
        \verbatim
            'actv' - returns whether module is active or not
            'addr' - returns local address
            'audt' - prints verbose socket info (debug only)
            'bind' - local bind data
            'conn' - connection status
            'ethr' - local ethernet address
            'maxp' - return max packet size
            'peer' - peer info (only valid if connected)
            'plug' - plug status
            'stat' - socket status
        \endverbatim

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen)
{
    //SocketStateT *pState = _Socket_pState;
    //int32_t iResult;
    
    // always zero results by default
    if (pBuf != NULL)
    {
        memset(pBuf, 0, iLen);
    }

    // make sure there is a valid socket ref
    if (pSocket->socket == INVALID_SOCKET)
    {
        return(-2);
    }

    // return socket status
    if (iInfo == 'stat')
    {
        // if not connected, use select to determine connect
        if (pSocket->opened == 0)
        {
            
            struct pollfd PollFd;
        
            memset(&PollFd, 0, sizeof(PollFd));
            PollFd.fd = pSocket->socket;
            PollFd.events = POLLOUT|POLLERR;
            if (poll(&PollFd, 1, 0) != 0)
            {
                // if we got an exception, that means connect failed
                if (PollFd.revents & POLLERR)
                {
                    NetPrintf(("dirtynetunix: read exception on connect\n"));
                    pSocket->opened = -1;
                }
                
                // if socket is writable, that means connect succeeded
                if (PollFd.revents & POLLOUT)
                {
                    NetPrintf(("dirtynetunix: connection open\n"));
                    pSocket->opened = 1;
                }
            }
        }

        // if previously connected, make sure connect still valid
        if (pSocket->opened > 0)
        {
            struct sockaddr SockAddr;
            int32_t iAddrLen = sizeof(SockAddr);
            if (getpeername(pSocket->socket, &SockAddr, &iAddrLen) < 0)
            {
                NetPrintf(("dirtynetunix: connection closed\n"));
                pSocket->opened = -1;
            }
        }

        // return connect status
        return(pSocket->opened);
    }

    // unhandled option?
    NetPrintf(("dirtynetunix: unhandled SocketInfo() option '%c%c%c%c'\n",
        (unsigned char)(iInfo >> 24),
        (unsigned char)(iInfo >> 16),
        (unsigned char)(iInfo >>  8),
        (unsigned char)(iInfo >>  0)));
    return(-1);
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
            'conn' - init network stack
            'disc' - bring down network stack
            'ndly' - set TCP_NODELAY state for given stream socket (iData1=zero or one)
            'pool' - set sys_net mempool size (default=128k)
            'push' - push data into given socket receive buffer (iData1=size, pData2=data ptr, pData3=sockaddr ptr)
            'sdcb' - set send callback (pData2=callback, pData3=callref)
            'vadd' - add a port to virtual port list
            'vdel' - del a port from virtual port list
        \endverbatim

    \Version 06/20/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t SocketControl(SocketT *pSocket, int32_t iOption, int32_t iData1, void *pData2, void *pData3)
{
    //SocketStateT *pState = _Socket_pState;
    //int32_t iResult;

    if (iOption == 'pool')
    {
        NetPrintf(("dirtynetunix: setting netpool size=%d bytes\n", iData1));
        _Socket_iMemPoolSize = iData1;
        return(0);
    }
    
    
    // unhandled
    NetPrintf(("dirtynetunix: unhandled control option '%c%c%c%c'\n",
        (uint8_t)(iOption>>24), (uint8_t)(iOption>>16), (uint8_t)(iOption>>8), (uint8_t)iOption));
    return(-1);
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
    int32_t iAddr, iResult=0;
    HostentT *pHost;
    struct addrinfo Hints, *pList = NULL;
    
    NetPrintf(("dirtynetunix: looking up address for host '%s'\n", pText));

    // dont allow negative timeouts
    if (iTimeout < 0)
    {
        return(NULL);
    }

    // create new structure
    pPriv = DirtyMemAlloc(sizeof(*pPriv), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    memset(pPriv, 0, sizeof(*pPriv));
    pHost = &pPriv->Host;

    // setup callbacks
    pHost->Done = &_SocketLookupDone;
    pHost->Free = &_SocketLookupFree;

    // check for dot notation
    if ((iAddr = SocketInTextGetAddr(pText)) != 0)
    {
        // we've got a dot-notation address
        pHost->addr = iAddr;
        pHost->done = 1;
        // return completed record
        return(pHost);
    }

    // copy over the target address
    strnzcpy(pHost->name, pText, sizeof(pHost->name));
    pHost->sema = 1;
    
	(void)iResult;
	unsigned char *pAddr;
	
    // start lookup
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;
    
    
	if ((iResult = getaddrinfo(pHost->name, NULL, &Hints, &pList)) == 0)
    {
        // extract ip address
        pHost->addr = SocketNtohl(((struct sockaddr_in *)pList->ai_addr)->sin_addr.s_addr);
        
        // mark success
        NetPrintf(("dirtynetunix: %s=%a\n", pHost->name, pHost->addr));
        pHost->done = 1;
        
        // release memory
        freeaddrinfo(pList);
    }
    else
    {
        // unsuccessful
        NetPrintf(("dirtynetunix: getaddrinfo() failed err=%d\n", errno));
        pHost->done = -1;
    }
    

	pHost->sema = 0;
    
	// return the host reference
    return(pHost);
}
