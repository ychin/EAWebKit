/*
Copyright (C) 2010, 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAWebKitPlatformSocketAPI.h
// By Arpit Baldeva 
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITPLATFORMSOCKETAPI_H
#define EAWEBKIT_EAWEBKITPLATFORMSOCKETAPI_H

#include <EABase/eabase.h>
#include <stddef.h> //for size_t

	//http://msdn.microsoft.com/en-us/library/ms741394(VS.85).aspx 
	typedef int socklen_t;
	typedef int platform_ssize_t;

struct sockaddr;
struct pollfd;
namespace EA
{
namespace WebKit
{
enum ePlatformSocketError
{
	eSockErrNone		= 0,
	eSockErrClosed		= -1,
	eSockErrNotConn		= -2,
	eSockErrBlocked		= -3,
	eSockErrAddress		= -4,
	eSockErrUnreach		= -5,
	eSockErrRefused		= -6, 
	eSockErrOther		= -7,
	eSockErrNoMem		= -8,
	eSockErrNoRSRC		= -9,
	eSockErrUnsupport	= -10,
	eSockErrInvalid		= -11,
	eSockErrAddrInUse	= -12,
	eSockErrConnReset	= -13,
	eSockErrCount		= -14
};

}
}
typedef int					(*PLATFORM_ACCEPT)			(int, struct sockaddr*, socklen_t*);
typedef int					(*PLATFORM_BIND)			(int, struct sockaddr*, socklen_t);
typedef int					(*PLATFORM_CONNECT)			(int, struct sockaddr*, socklen_t);
typedef struct hostent*		(*PLATFORM_GETHOSTBYADDR)	(const char*,	socklen_t, int);
typedef struct hostent*		(*PLATFORM_GETHOSTBYNAME)	(const char*);
typedef int					(*PLATFORM_DNSLOOKUP)		(const char*, void*, void**, unsigned int*);
typedef int					(*PLATFORM_GETPEERNAME)		(int,struct sockaddr*, socklen_t*); 
typedef int					(*PLATFORM_GETSOCKOPT)		(int, int, int, void*, socklen_t*);
typedef int					(*PLATFORM_LISTEN)			(int, int);
typedef platform_ssize_t	(*PLATFORM_RECV)			(int, void*, size_t, int);
typedef platform_ssize_t	(*PLATFORM_RECVFROM)		(int, void*, size_t, int, struct sockaddr*, socklen_t*);
typedef platform_ssize_t	(*PLATFORM_SEND)			(int, void*, size_t, int);
typedef platform_ssize_t	(*PLATFORM_SENDTO)			(int, void*, size_t, int, struct sockaddr*, socklen_t);
typedef int					(*PLATFORM_SETSOCKOPT)		(int, int, int, void*, socklen_t);
typedef int					(*PLATFORM_SHUTDOWN)		(int, int);
typedef int					(*PLATFORM_SOCKET)			(int, int, int);
typedef int					(*PLATFORM_CLOSE)			(int);
typedef int					(*PLATFORM_POLL)			(struct pollfd*, int, int);
typedef int					(*PLATFORM_GETLASTERROR)	(int);

namespace EA
{
namespace WebKit
{
struct PlatformSocketAPI
{
	PLATFORM_ACCEPT			accept;
	PLATFORM_BIND			bind;
	PLATFORM_CONNECT		connect;
	PLATFORM_GETHOSTBYADDR	gethostbyaddr;
	PLATFORM_GETHOSTBYNAME	gethostbyname;
	PLATFORM_DNSLOOKUP		dnslookup;
	PLATFORM_GETPEERNAME	getpeername;
	PLATFORM_GETSOCKOPT		getsockopt;
	PLATFORM_LISTEN			listen;
	PLATFORM_RECV			recv;
	PLATFORM_RECVFROM		recvfrom;
	PLATFORM_SEND			send;
	PLATFORM_SENDTO			sendto;
	PLATFORM_SETSOCKOPT		setsockopt;
	PLATFORM_SHUTDOWN		shutdown;
	PLATFORM_SOCKET			socket;
	PLATFORM_CLOSE			close;
	PLATFORM_POLL			poll;
	PLATFORM_GETLASTERROR	getlasterror;
};

typedef struct PlatformSocketAPI PlatformSocketAPI;
}
}

#endif //EAWEBKIT_EAWEBKITPLATFORMSOCKETAPI_H