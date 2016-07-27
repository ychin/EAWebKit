/*
Copyright (C) 1999-2004,2009-2010 Electronic Arts, Inc.  All rights reserved.

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

/*H*************************************************************************************/
/*!
    \File dirtynetwin.c
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

 #define WIN32_LEAN_AND_MEAN 1           // avoid extra stuff
 #include <windows.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>

#include "dirtylib.h"
#include "dirtymem.h"
#include "dirtyvers.h"
#include "dirtynet.h"


#include "platformsocketapi.h"
extern PlatformSocketAPICallbacks gPlatformSocketAPICallbacks;

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

struct SocketT
{
    int32_t family;             
    int32_t type;               
    int32_t proto;              

    int8_t opened;              
    uint8_t shutdown;           
    uint32_t socket;            

    struct sockaddr_in local;   
    struct sockaddr_in remote;  
};

typedef struct SocketStateT
{
    SocketAddrMapT aSockAddrMap[32];    
    SocketNameMapT aSockNameMap[32];    
    int32_t iDnsRemap;                  

    int32_t iMemGroup;                  
    void *pMemGroupUserData;            

    int32_t iVersion;                   
} SocketStateT;

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables

static SocketStateT *_Socket_pState = NULL;

// Public variables


/*** Private Functions *****************************************************************/


#if DIRTYCODE_DEBUG
static const char *_DisplayAddrMapEntry(SocketAddrMapT *pAddrMap)
{
    char strSrcAddr[20], strMskAddr[20], strDstAddr[20];
    static char strOutput[64];

    if (pAddrMap->uRemap == 0)
    {
        return("[no entry]");
    }
    
    strnzcpy(strSrcAddr, SocketInAddrGetText(pAddrMap->uAddr), sizeof(strSrcAddr));
    strnzcpy(strMskAddr, SocketInAddrGetText(pAddrMap->uMask), sizeof(strMskAddr));
    strnzcpy(strDstAddr, SocketInAddrGetText(pAddrMap->uRemap), sizeof(strDstAddr));
    
    snzprintf(strOutput, sizeof(strOutput), "[%s/%s:%d] -> [%s:%d]", strSrcAddr,
        strMskAddr, pAddrMap->uSrcPort, strDstAddr, pAddrMap->uDstPort);
    return(strOutput);
}
#endif

#if DIRTYCODE_DEBUG
static const char *_DisplayNameMapEntry(SocketNameMapT *pNameMap)
{
    char strDstAddr[32];
    static char strOutput[128];
    
    if (pNameMap->uRemap != 0)
    {
        strnzcpy(strDstAddr, SocketInAddrGetText(pNameMap->uRemap), sizeof(strDstAddr));
    }
    else if (pNameMap->strRemap[0] != '\0')
    {
        strnzcpy(strDstAddr, pNameMap->strRemap, sizeof(strDstAddr));
    }
    else
    {
        return("[no entry]");
    }
    
    snzprintf(strOutput, sizeof(strOutput), "[%s:%d] -> [%s:%d]", pNameMap->strDnsName,
        pNameMap->uSrcPort, strDstAddr, pNameMap->uDstPort);
    return(strOutput);
}
#endif

static uint32_t _MapNameMatch(const char *pName, const char *pMatch)
{
    const char *pNameEnd, *pTmpName, *pMatchEnd, *pTmpMatch;
    uint32_t uResult;
    
    if (*pMatch == '\0')
    {
        return(0);
    }
    
    pMatchEnd = pMatch + strlen(pMatch)-1;

    if (*pMatch == '*')
    {
        pNameEnd = pName + strlen(pName)-1;
    
        for (pTmpName = pNameEnd, pTmpMatch = pMatchEnd; (pTmpName != pName) && (pTmpMatch != pMatch); pTmpName -= 1, pTmpMatch -= 1)
        {
            if (*pTmpName != *pTmpMatch)
            {
                break;
            }
        }

        if (*pTmpMatch == '*')
        {
            pTmpName += 1;
            pTmpMatch += 1;
        }

        uResult = !strcmp(pTmpName, pTmpMatch);
    }
    else if (*pMatchEnd == '*') 
    {
        for (pTmpName = pName, pTmpMatch = pMatch; (*pTmpName != '\0') && (*pTmpMatch != '\0'); pTmpName += 1, pTmpMatch += 1)
        {
            if (*pTmpName != *pTmpMatch)
            {
                break;
            }
        }
        
        uResult = *pTmpMatch == '*' ? 1 : 0;
    }
    else 
    {
        uResult = !strcmp(pName, pMatch);
    }

    return(uResult);
}

static const SocketNameMapT *_MapName(SocketStateT *pState, const char *pName)
{
    const SocketNameMapT *pMap;
    int32_t iMap;
    
    for (iMap = 0, pMap = NULL; iMap < sizeof(pState->aSockNameMap)/sizeof(pState->aSockNameMap[0]); iMap += 1)
    {
        if (_MapNameMatch(pName, pState->aSockNameMap[iMap].strDnsName))
        {
            pMap = &pState->aSockNameMap[iMap];
            NetPrintf(("dirtynetwin: name remap %s", pName));
            NetPrintf((" to %s\n", (pMap->uRemap != 0) ? SocketInAddrGetText(pMap->uRemap) : pMap->strRemap));
            break;
        }
    }
    return(pMap);
} 


static const struct sockaddr *_MapAddress(struct sockaddr *pTemp, const struct sockaddr *pAddr)
{
    SocketStateT *pState = _Socket_pState;
    const SocketAddrMapT *pMap = pState->aSockAddrMap;
    uint32_t uAddr, uSrcPort, uDstPort;
    const SocketNameMapT *pNameMap;
    char strAddrText[16];
    int32_t iMap;

    uAddr = SockaddrInGetAddr(pAddr);
    uSrcPort = SockaddrInGetPort(pAddr);

    SockaddrInGetAddrText((struct sockaddr *)pAddr, strAddrText, sizeof(strAddrText));
    if ((pNameMap = _MapName(pState, strAddrText)) != NULL)
    {
        if (pNameMap->uRemap != 0)
        {
            uAddr = pNameMap->uRemap;
        }
        else
        {
            NetPrintf(("dirtynetwin: warning -- could not map address %s using name map, as remap address is not yet specified\n", strAddrText));
        }
    }

    for (iMap = 0; iMap < sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0]); iMap += 1)
    {
        pMap = &pState->aSockAddrMap[iMap];

        if (pMap->uRemap == 0)
        {
            continue;
        }        
        if (pMap->uAddr != (uAddr & pMap->uMask)) 
        {
            continue;
        }
        if ((pMap->uSrcPort != 0) && (uSrcPort != 0) && (pMap->uSrcPort != uSrcPort))
        {
            continue;
        }

        uDstPort = (pMap->uDstPort != 0) ? pMap->uDstPort : uSrcPort;

        NetPrintf(("dirtynetwin: addr remap %s:%d", SocketInAddrGetText(uAddr), uSrcPort));
        NetPrintf((" to %s:%d\n", SocketInAddrGetText(pMap->uRemap), uDstPort));

        memcpy(pTemp, pAddr, sizeof(*pTemp));
        SockaddrInSetAddr(pTemp, pMap->uRemap);
        SockaddrInSetPort(pTemp, uDstPort);
        SockaddrInSetMisc(pTemp, pMap->uServiceId);
        pAddr = pTemp;
        break;
    }
    return(pAddr);
}

static int32_t _XlatError0(int32_t err, int32_t wsaerr)
{
    if (err < 0)
    {
        err = wsaerr;
        if ((err == WSAEWOULDBLOCK) || (err == WSA_IO_PENDING))
            err = SOCKERR_NONE;
        else if ((err == WSAENETUNREACH) || (err == WSAEHOSTUNREACH))
            err = SOCKERR_UNREACH;
        else if (err == WSAENOTCONN)
            err = SOCKERR_NOTCONN;
        else if (err == WSAECONNREFUSED)
            err = SOCKERR_REFUSED;
        else if (err == WSAEINVAL)
            err = SOCKERR_INVALID;
        else if (err == WSAECONNRESET)
            err = SOCKERR_CONNRESET;
        else
            err = SOCKERR_OTHER;
    }
    return(err);
}

static int32_t _XlatError(int32_t err)
{
    return(_XlatError0(err, WSAGetLastError()));
}

static SocketT *_SocketOpen(SOCKET s, int32_t af, int32_t type, int32_t proto)
{
    SocketStateT *pState = _Socket_pState;
    uint32_t uTrue = 1;
    SocketT *sock;

    if (s == INVALID_SOCKET)
    {
        if ((s = socket(af, type, proto)) == INVALID_SOCKET)
        {
            NetPrintf(("dirtynetwin: error %d creating socket\n", WSAGetLastError()));
            return(NULL);
        }
    }

    sock = DirtyMemAlloc(sizeof(*sock), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    memset(sock, 0, sizeof(*sock));
    sock->socket = s;

    ioctlsocket(s, FIONBIO, &uTrue);

    sock->family = af;
    sock->type = type;
    sock->proto = proto;

    return(sock);
}

static int32_t _SocketClose(SocketT *sock, uint32_t bShutdown)
{
    SocketStateT *pState = _Socket_pState;

    if ((bShutdown == TRUE) && (sock->socket != INVALID_SOCKET))
    {
        shutdown(sock->socket, 2);
        closesocket(sock->socket);
    }
    sock->socket = INVALID_SOCKET;
    sock->opened = 0;

	DirtyMemFree(sock, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    return(0);
}

/*** Public Functions ******************************************************************/


int32_t SocketCreate(int32_t iThreadPrio)
{
    SocketStateT *pState = _Socket_pState;
    WSADATA data;
	int32_t iResult;
	int32_t iMemGroup;
    void *pMemGroupUserData;
    
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    if (pState != NULL)
    {
        NetPrintf(("dirtynetwin: SocketCreate() called while module is already active\n"));
        return(-1);
    }

    NetPrintf(("dirtynetwin: DirtySock SDK v%d.%d.%d.%d\n",
        (DIRTYVERS>>24)&0xff, (DIRTYVERS>>16)&0xff,
        (DIRTYVERS>> 8)&0xff, (DIRTYVERS>> 0)&0xff));

    if ((pState = DirtyMemAlloc(sizeof(*pState), SOCKET_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtynetpc: unable to allocate module state\n"));
        return(-2);
    }
    memset(pState, 0, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;

    _Socket_pState = pState;
    
    memset(&data, 0, sizeof(data));
    // startup winsock (would like 2.0 if available)
    iResult = WSAStartup(MAKEWORD(2, 0), &data);

    pState->iVersion = (LOBYTE(data.wVersion)<<8)|(HIBYTE(data.wVersion)<<0);
   
    return(0);
}

int32_t SocketDestroy(uint32_t uFlags)
{
    SocketStateT *pState = _Socket_pState;
    
    if (pState == NULL)
    {
        NetPrintf(("dirtynetpc: SocketDestroy() called while module is not active\n"));
        return(-1);
    }

    NetPrintf(("dirtynetpc: shutting down\n"));

    DirtyMemFree(pState, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    _Socket_pState = NULL;

    if (uFlags == 0)
    {
        WSACleanup();
    }
    
    return(0);
}

SocketT *SocketOpen(int32_t af, int32_t type, int32_t proto)
{
    return(_SocketOpen(INVALID_SOCKET, af, type, proto));
}

int32_t SocketClose(SocketT *sock)
{
    return(_SocketClose(sock, TRUE));
}

int32_t SocketShutdown(SocketT *pSocket, int32_t iHow)
{
    if (pSocket->socket == INVALID_SOCKET)
    {
        return(0);
    }

    if (iHow == SOCK_NOSEND)
    {
        iHow = SD_SEND;
    }
    else if (iHow == SOCK_NORECV)
    {
        iHow = SD_RECEIVE;
    }
    else if (iHow == (SOCK_NOSEND|SOCK_NORECV))
    {
        iHow = SD_BOTH;
    }
    
    pSocket->shutdown |= iHow;
    shutdown(pSocket->socket, iHow);

    return(0);
}

int32_t SocketBind(SocketT *sock, const struct sockaddr *name, int32_t namelen)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iResult;

    iResult = _XlatError(bind(sock->socket, name, namelen));
    
    return(iResult);
}

int32_t SocketConnect(SocketT *sock, struct sockaddr *name, int32_t namelen)
{
    SocketStateT *pState = _Socket_pState;
    struct sockaddr temp;
    int32_t iResult;

    sock->opened = 0;
    
    SockaddrInit(&temp, AF_INET);

    if (((iResult = SocketBind(sock, &temp, sizeof(temp))) < 0) && (iResult != SOCKERR_INVALID))
    {
        return(iResult);
    }

    iResult = _XlatError(connect(sock->socket, _MapAddress(&temp, name), namelen));

    return(iResult);
}

int32_t SocketListen(SocketT *sock, int32_t backlog)
{
    return(_XlatError(listen(sock->socket, backlog)));
}

SocketT *SocketAccept(SocketT *sock, struct sockaddr *addr, int32_t *addrlen)
{
    SocketT *open = NULL;
    SOCKET incoming;
    uint32_t nonblock = 1;

    if (sock->socket == INVALID_SOCKET)
    {
        return(NULL);
    }

    if ((addr != NULL) && (*addrlen < sizeof(struct sockaddr)))
    {
        return(NULL);
    }

    if (sock->family == AF_INET)
    {
        incoming = accept(sock->socket, addr, addrlen);
        if (incoming != INVALID_SOCKET)
        {
            open = _SocketOpen(incoming, sock->family, sock->type, sock->proto);
        }
    }

    return(open);
}

int32_t SocketSendto(SocketT *pSocket, const char *buf, int32_t len, int32_t flags, const struct sockaddr *to, int32_t tolen)
{
    SocketStateT *pState = _Socket_pState;
    struct sockaddr temp;
    int32_t retn;

    if (pSocket->socket == INVALID_SOCKET)
    {
        NetPrintf(("dirtynetwin: attempting to send on invalid socket\n"));
        return(SOCKERR_INVALID);
    }

    if (to == NULL)
    {
        retn = send(pSocket->socket, buf, len, 0);
    }
    else
    {
        retn = sendto(pSocket->socket, buf, len, 0, _MapAddress(&temp, to), tolen);
    }

    return(_XlatError(retn));
}

int32_t SocketRecvfrom(SocketT *pSock, char *pBuf, int32_t iLen, int32_t iFlags, struct sockaddr *pFrom, int32_t *pFromLen)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iRecv = 0, iRecvErr;

	{
        if (pSock->socket == INVALID_SOCKET)
        {
            return(SOCKERR_INVALID);
        }

        if (pFrom != NULL)
        {
            if ((iRecv = recvfrom(pSock->socket, pBuf, iLen, 0, pFrom, pFromLen)) > 0)
            {
                SockaddrInSetMisc(pFrom, NetTick());
            }
        }
        else
        {
            iRecv = recv(pSock->socket, pBuf, iLen, 0);
        }
        
        iRecvErr = WSAGetLastError();
    }

    iRecv = (iRecv == 0) ? SOCKERR_CLOSED : _XlatError0(iRecv, iRecvErr);

    return(iRecv);
}

int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen)
{
    SocketStateT *pState = _Socket_pState;
    
    if ( (pBuf != NULL) && (iInfo != 'ladr') )
    {
        memset(pBuf, 0, iLen);
    }

    if (pSocket->socket == INVALID_SOCKET)
    {
        return(SOCKERR_OTHER);
    }

    if (iInfo == 'stat')
    {
        int32_t iErr;
        fd_set fdwrite;
        fd_set fdexcept;
        struct timeval tv;
        struct sockaddr peeraddr;

        if (pSocket->opened == 0)
        {
            FD_ZERO(&fdwrite);
            FD_ZERO(&fdexcept);
            FD_SET(pSocket->socket, &fdwrite);
            FD_SET(pSocket->socket, &fdexcept);
            tv.tv_sec = tv.tv_usec = 0;
            if (select(1, NULL, &fdwrite, &fdexcept, &tv) != 0)
            {
                if (fdexcept.fd_count > 0)
                {
                    NetPrintf(("dirtynetwin: connection failed\n"));
                    pSocket->opened = -1;
                }
                else if (fdwrite.fd_count > 0)
                {
                    NetPrintf(("dirtynetwin: connection open\n"));
                    pSocket->opened = 1;
                }
            }
        }

        if (pSocket->opened > 0)
        {
            iLen = sizeof(peeraddr);
            iErr = _XlatError(getpeername(pSocket->socket, &peeraddr, &iLen));
            if (iErr == SOCKERR_NOTCONN)
            {
                NetPrintf(("dirtynetwin: connection closed\n"));
                pSocket->opened = -1;
            }
        }

        return(pSocket->opened);
    }

    NetPrintf(("SocketInfo: invalid query for [%c%c%c%c]\n", (unsigned char)(iInfo>>24),
		(unsigned char)(iInfo>>16), (unsigned char)(iInfo>>8), (unsigned char)(iInfo>>0)));
    return(-1);
}

int32_t SocketControl(SocketT *pSocket, int32_t iOption, int32_t iData1, void *pData2, void *pData3)
{
    SocketStateT *pState = _Socket_pState;
    int32_t iMapEntry;

    if (iOption == 'madr')
    {
        struct sockaddr TempAddr;
        const struct sockaddr *pAddr;
        pAddr = _MapAddress(&TempAddr, pData2);
        memcpy(pData3, pAddr, sizeof(*pAddr));
        return(0);
    }
    if (iOption == 'xclr')
    {
        memset(pState->aSockNameMap, 0, sizeof(pState->aSockNameMap));
        memset(pState->aSockAddrMap, 0, sizeof(pState->aSockAddrMap));
        return(0);
    }
    if (iOption == 'xdns')
    {
        pState->iDnsRemap = iData1;
        return(0);
    }
    if (iOption == 'xmap')
    {
        iData1 *= sizeof(pState->aSockAddrMap[0]);
        if (iData1 > sizeof(pState->aSockAddrMap))
        {
            NetPrintf(("dirtynetwin: clamping addr map to %d entries\n", sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0])));
            iData1 = sizeof(pState->aSockAddrMap);
        }
        memcpy(pState->aSockAddrMap, pData2, iData1);
        return(0);
    }
    if (iOption == 'xnam')
    {
        iData1 *= sizeof(pState->aSockNameMap[0]);
        if (iData1 > sizeof(pState->aSockNameMap))
        {
            NetPrintf(("dirtynetwin: clamping name map to %d entries\n", sizeof(pState->aSockNameMap)/sizeof(pState->aSockNameMap[0])));
            iData1 = sizeof(pState->aSockNameMap);
        }
        memcpy(pState->aSockNameMap, pData2, iData1);
        return(0);
    }
    if (iOption == '+xam')
    {
        for (iMapEntry = 0; iMapEntry < sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0]); iMapEntry++)
        {
            if (pState->aSockAddrMap[iMapEntry].uRemap == 0)
            {
                memcpy(&pState->aSockAddrMap[iMapEntry], pData2, sizeof(pState->aSockAddrMap[0]));
                NetPrintf(("dirtynetwin: add addr map entry [%d] %s\n", iMapEntry, _DisplayAddrMapEntry(&pState->aSockAddrMap[iMapEntry])));
                return(iMapEntry);
            }
        }
        NetPrintf(("dirtynetwin: unable to add addr map entry %s; table full\n", _DisplayAddrMapEntry((SocketAddrMapT *)pData2)));
        return(-1);
    }
    if (iOption == '+xnm')
    {
        for (iMapEntry = 0; iMapEntry < sizeof(pState->aSockNameMap)/sizeof(pState->aSockNameMap[0]); iMapEntry++)
        {
            if (pState->aSockNameMap[iMapEntry].strDnsName[0] == '\0')
            {
                memcpy(&pState->aSockNameMap[iMapEntry], pData2, sizeof(pState->aSockNameMap[0]));
                NetPrintf(("dirtynetwin: add name map entry [%d] %s\n", iMapEntry, _DisplayNameMapEntry(&pState->aSockNameMap[iMapEntry])));
                return(iMapEntry);
            }                
        }
        NetPrintf(("dirtynetwin: unable to add name map entry %s; table full\n", _DisplayNameMapEntry((SocketNameMapT *)pData2)));
        return(-1);
    }
    if (iOption == '-xam')
    {
        int32_t iDelEntry = -1;
        if (pData2 != NULL)
        {
            for (iMapEntry = 0; iMapEntry < sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0]); iMapEntry++)
            {
                if (!memcmp(&pState->aSockAddrMap[iMapEntry], pData2, sizeof(pState->aSockAddrMap[0])))
                {
                    iDelEntry = iMapEntry;
                    break;
                }
            }
            if (iMapEntry == sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0]))
            {
                NetPrintf(("dirtynetwin: could not find addr map entry %s to delete\n", _DisplayAddrMapEntry((SocketAddrMapT *)pData2)));
            }
        }
        else if ((iData1 >= 0) && (iData1 < sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0])))
        {
            iDelEntry = iData1;
        }
        else
        {
            NetPrintf(("dirtynetwin: could not delete addr map entry with index %d; invalid range\n", iData1));
        }
        if (iDelEntry != -1)
        {
            NetPrintf(("dirtynetwin: del addr map entry [%d] %s\n", iDelEntry, _DisplayAddrMapEntry(&pState->aSockAddrMap[iDelEntry])));
            memset(&pState->aSockAddrMap[iDelEntry], 0, sizeof(pState->aSockAddrMap[0]));
            return(iDelEntry);
        }
    }
    if (iOption == '-xnm')
    {
        int32_t iDelEntry = -1;
        if (pData2 != NULL)
        {
            for (iMapEntry = 0; iMapEntry < sizeof(pState->aSockNameMap)/sizeof(pState->aSockNameMap[0]); iMapEntry++)
            {
                if (!memcmp(&pState->aSockNameMap[iMapEntry], pData2, sizeof(pState->aSockNameMap[0])))
                {
                    iDelEntry = iMapEntry;
                    break;
                }
            }
            if (iMapEntry == sizeof(pState->aSockAddrMap)/sizeof(pState->aSockAddrMap[0]))
            {
                NetPrintf(("dirtynetwin: could not find name map entry %s to delete\n", _DisplayNameMapEntry((SocketNameMapT *)pData2)));
            }
        }
        else if ((iData1 >= 0) && (iData1 < sizeof(pState->aSockNameMap)/sizeof(pState->aSockNameMap[0])))
        {
            iDelEntry = iData1;
        }
        else
        {
            NetPrintf(("dirtynetwin: could not delete name map entry with index %d; invalid range\n", iData1));
        }
        if (iDelEntry != -1)
        {
            NetPrintf(("dirtynetwin: del name map entry [%d] %s\n", iDelEntry, _DisplayNameMapEntry(&pState->aSockNameMap[iDelEntry])));
            memset(&pState->aSockNameMap[iDelEntry], 0, sizeof(pState->aSockNameMap[0]));
            return(iDelEntry);
        }
    }
    NetPrintf(("SocketControl: invalid query for [%c%c%c%c]\n", (unsigned char)(iOption>>24),
		(unsigned char)(iOption>>16), (unsigned char)(iOption>>8), (unsigned char)(iOption>>0)));
    return(-1);
}



static int32_t SocketLookupDone(HostentT *host)
{
    return(host->done);
}

static void SocketLookupFree(HostentT *host)
{
    SocketStateT *pState = _Socket_pState;

    if (InterlockedExchange(&host->thread, 1) != 0)
    {
        DirtyMemFree(host, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }
}

static int32_t SocketLookupThread(HostentT *host)
{
    SocketStateT *pState = _Socket_pState;
    struct hostent *winhost;
    unsigned char *ipaddr;

    winhost = gethostbyname(host->name);
    if (winhost != NULL)
    {
        ipaddr = winhost->h_addr_list[0];
        host->addr = (ipaddr[0]<<24)|(ipaddr[1]<<16)|(ipaddr[2]<<8)|(ipaddr[3]<<0);
        host->done = 1;
    }
    else
    {
        host->done = -1;
    }

    if (InterlockedExchange(&host->thread, 1) != 0)
    {
        DirtyMemFree(host, SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    }

    return(0);
}

HostentT *SocketLookup(const char *text, int32_t timeout)
{
    SocketStateT *pState = _Socket_pState;
    int32_t i, j;
    const char *s;
    HostentT *host;
    DWORD pid;
    HANDLE thread;

    if (timeout < 0)
    {
        return(NULL);
    }

    host = DirtyMemAlloc(sizeof(*host), SOCKET_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    memset(host, 0, sizeof(*host));

    host->Done = &SocketLookupDone;
    host->Free = &SocketLookupFree;

    for (s = text; *s != 0; ++s)
    {
        if ((*s != '.') && ((*s < '0') || (*s > '9')))
        {
            break;
        }
    }
    if (*s == 0)
    {
        s = text;
        for (i = 0; i < 4; ++i)
        {
            for (j = 0; (*s >= '0') && (*s <= '9');)
            {
                j = (j * 10) + (*s++ & 15);
            }
            if ((*s != ((i < 3) ? '.' : 0)) || (j > 255))
            {
                host->done = -2;
                host->addr = 0;
                break;
            }
            host->addr = (host->addr << 8) | j;
            host->done = 1;
            ++s;
        }
        host->thread = 1;
        return(host);
    }

    strnzcpy(host->name, text, sizeof(host->name));

    thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&SocketLookupThread, (LPVOID)host, 0, &pid);
    if (thread != NULL)
    {
        CloseHandle(thread);
    }

    return(host);
}






