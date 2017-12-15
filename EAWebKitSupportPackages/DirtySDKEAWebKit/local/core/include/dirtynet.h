/*
Copyright (C) 2001-2002,2009, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

/*H*************************************************************************************************/
/*!

    \File    dirtynet.h


*/
/*************************************************************************************************H*/


#ifndef _dirtynet_h
#define _dirtynet_h

/*!
\Module DirtySock
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

// Constants

#if   defined(DIRTY_HAVE_SOCKET_HEADER)
 #include <sys/socket.h>
 #include <netinet/in.h>         
 #include <arpa/inet.h>         
#else
#define AF_INET 2               

#define SOCK_STREAM 1           
#define SOCK_DGRAM 2            
#define SOCK_RAW 3              

#define IPPROTO_IP     0
#define IPPROTO_ICMP   1
#define IPPROTO_IPV4   4
#define IPPROTO_TCP    6
#define IPPROTO_UDP   17
#define IPPROTO_RAW  255

#ifndef INADDR_ANY
#define INADDR_ANY          0x00000000
#endif
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK     0x7f000001
#endif

#endif /* !DIRTY_HAVE_SOCKET_HEADER */

#define SOCK_NORECV 1           
#define SOCK_NOSEND 2           

#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST    0xffffffff
#endif

#define CALLB_NONE 0           
#define CALLB_SEND 1            
#define CALLB_RECV 2          

#define SOCKLOOK_LOCALADDR      "*"


#define SOCKET_MAXUDPRECV       (1264)

#define SOCKET_MAXVIRTUALPORTS  (32)

#define SOCKERR_NONE 0          
#define SOCKERR_CLOSED -1       
#define SOCKERR_NOTCONN -2      
#define SOCKERR_BLOCKED -3      
#define SOCKERR_ADDRESS -4      
#define SOCKERR_UNREACH -5      
#define SOCKERR_REFUSED -6      
#define SOCKERR_OTHER   -7      
#define SOCKERR_NOMEM   -8     
#define SOCKERR_NORSRC  -9      
#define SOCKERR_UNSUPPORT -10 
#define SOCKERR_INVALID -11   
#define SOCKERR_ADDRINUSE -12 
#define SOCKERR_CONNRESET -13 
/*** Macros ****************************************************************************/

#define SockaddrInit(addr,fam)      { (addr)->sa_family = (fam); *((uint16_t *)&(addr)->sa_data[0]) = 0; *((uint32_t *)&(addr)->sa_data[2]) = 0; *((uint32_t *)&(addr)->sa_data[6]) = 0; *((uint32_t *)&(addr)->sa_data[10]) = 0; }

#define SockaddrInGetPort(addr)     ((((unsigned char)(addr)->sa_data[0])<<8)|(((unsigned char)(addr)->sa_data[1])<<0))

#define SockaddrInSetPort(addr,val) { (addr)->sa_data[0] = (unsigned char)((val)>>8); (addr)->sa_data[1] = (unsigned char)(val); }

#define SockaddrInGetAddr(addr)     (((((((unsigned char)((addr)->sa_data[2])<<8)|(unsigned char)((addr)->sa_data[3]))<<8)|(unsigned char)((addr)->sa_data[4]))<<8)|(unsigned char)((addr)->sa_data[5]))

#define SockaddrInSetAddr(addr,val) { uint32_t val2 = (val); (addr)->sa_data[5] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[4] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[3] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[2] = (unsigned char)val2; }

#define SockaddrInGetMisc(addr)     (((((((unsigned char)((addr)->sa_data[6])<<8)|(unsigned char)((addr)->sa_data[7]))<<8)|(unsigned char)((addr)->sa_data[8]))<<8)|(unsigned char)((addr)->sa_data[9]))

#define SockaddrInSetMisc(addr,val) { uint32_t val2 = (val); (addr)->sa_data[9] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[8] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[7] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[6] = (unsigned char)val2; }

#define SockaddrIsLoopback(addr)   (( ((addr)->sa_family == AF_INET) && ((addr)->sa_data[0] == 127) && ((addr)->sa_data[1] == 0) && ((addr)->sa_data[2] == 0) && ((addr)->sa_data[3] == 1) ))


/*** Type Definitions ******************************************************************/

typedef struct SocketT SocketT;

typedef struct HostentT
{
    int32_t done;                       
    uint32_t addr;                      

    int32_t (*Done)(struct HostentT *); 
    void (*Free)(struct HostentT *);    

    char name[64];                      
    int32_t sema;                       
    int32_t thread;                    
    void* pData;                        
    uint32_t timeout;                   
} HostentT;


typedef struct SocketAddrMapT
{
    uint32_t uAddr;                 
    uint32_t uMask;                 
    uint32_t uRemap;                
    uint32_t uServiceId;            
    uint16_t uSrcPort;              
    uint16_t uDstPort;              
} SocketAddrMapT;

typedef struct SocketNameMapT
{
    char strDnsName[64];    
    char strRemap[32];      
    uint32_t uRemap;        
    uint32_t uServiceId;    
    uint16_t uSrcPort;      
    uint16_t uDstPort;     
} SocketNameMapT;


#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_) && !defined(CS_UNDEFINED_STRING) && !defined(CS_UNDEFINED_STRING)
#define _WINSOCKAPI_
#define _WINSOCK2API_

struct sockaddr
{
    uint16_t sa_family;
    unsigned char sa_data[14];
};

struct in_addr
{
    uint32_t s_addr;
};

struct sockaddr_in
{
    uint16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    uint32_t sin_misc;
    char sin_zero[4];
};
#endif // !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)

typedef int32_t (SocketSendCallbackT)(SocketT *pSocket, int32_t iType, const uint8_t *pData, int32_t iDataSize, const struct sockaddr *pTo, void *pCallref); 

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int32_t SockaddrCompare(struct sockaddr *addr1, struct sockaddr *addr2);

int32_t SockaddrInSetAddrText(struct sockaddr *addr, const char *str);

char *SockaddrInGetAddrText(struct sockaddr *addr, char *str, int32_t len);

uint16_t SocketHtons(uint16_t addr);

uint32_t SocketHtonl(uint32_t addr);

uint16_t SocketNtohs(uint16_t addr);

uint32_t SocketNtohl(uint32_t addr);

const char *SocketInAddrGetText(int32_t addr);

char *SocketInAddrGetText2(uint32_t addr, char *str, int32_t len);

int32_t SocketInTextGetAddr(const char *addrtext);

int32_t SockaddrInParse(struct sockaddr *addr, const char *parse);

int32_t SockaddrInParse2(uint32_t *pAddr, int32_t *pPort, int32_t *pPort2, const char *parse);

int32_t SocketCreate(int32_t iThreadPrio);

int32_t SocketDestroy(uint32_t uFlags);

SocketT *SocketOpen(int32_t af, int32_t type, int32_t protocol);

int32_t SocketShutdown(SocketT *pSocket, int32_t how);

int32_t SocketClose(SocketT *pSocket);

SocketT *SocketImport(intptr_t uSockRef);

void SocketRelease(SocketT *pSocket);

int32_t SocketBind(SocketT *pSocket, const struct sockaddr *name, int32_t namelen);

int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen);

int32_t SocketControl(SocketT *pSocket, int32_t option, int32_t data1, void *data2, void *data3);

int32_t SocketListen(SocketT *pSocket, int32_t backlog);

SocketT *SocketAccept(SocketT *pSocket, struct sockaddr *addr, int32_t *addrlen);

int32_t SocketConnect(SocketT *pSocket, struct sockaddr *name, int32_t namelen);

int32_t SocketSendto(SocketT *pSocket, const char *buf, int32_t len, int32_t flags, const struct sockaddr *to, int32_t tolen);

#define SocketSend(_pSocket, _pBuf, iLen, iFlags)   SocketSendto(_pSocket, _pBuf, iLen, iFlags, NULL, 0)

int32_t SocketRecvfrom(SocketT *pSocket, char *buf, int32_t len, int32_t flags, struct sockaddr *from, int32_t *fromlen);

#define SocketRecv(_pSocket, pBuf, iLen, iFlags)   SocketRecvfrom(_pSocket, pBuf, iLen, iFlags, NULL, 0)

int32_t SocketCallback(SocketT *pSocket, int32_t flags, int32_t timeout, void *ref, int32_t (*proc)(SocketT *pSocket, int32_t flags, void *ref));

int32_t SocketHost(struct sockaddr *host, int32_t hostlen, const struct sockaddr *dest, int32_t destlen);

const char *SocketError(int32_t iErrno);

HostentT *SocketLookup(const char *text, int32_t timeout);

uint32_t SocketGetLocalAddr(void);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtynet_h


