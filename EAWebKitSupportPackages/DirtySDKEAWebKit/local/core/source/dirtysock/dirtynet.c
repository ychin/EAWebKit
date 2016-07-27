/*
Copyright (C) 1999-2003,2009-2010 Electronic Arts, Inc.  All rights reserved.

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

    \File    dirtynet.c

*/
/*************************************************************************************************H*/


/*** Include files *********************************************************************/

#include <string.h>

#include "dirtysock.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables


// Public variables


/*** Private Functions *****************************************************************/

 

/*** Public Functions ******************************************************************/



int32_t SockaddrCompare(struct sockaddr *addr1, struct sockaddr *addr2)
{
    int32_t len = sizeof(*addr1)-sizeof(addr1->sa_family);

    if (addr1->sa_family != addr2->sa_family)
        return(addr1->sa_family-addr2->sa_family);

    if (addr1->sa_family == AF_INET)
        len = 2+4;

    return(memcmp(addr1->sa_data, addr2->sa_data, len));
}

int32_t SockaddrInSetAddrText(struct sockaddr *addr, const char *s)
{
    int32_t i;
    unsigned char *ipaddr = (unsigned char *)(addr->sa_data+2);

    for (i = 0; i < 4; ++i, ++s) {
        ipaddr[i] = 0;
        while ((*s >= '0') && (*s <= '9'))
            ipaddr[i] = (ipaddr[i]*10) + (*s++ & 15);
        if ((i < 3) && (*s != '.')) {
            ipaddr[0] = ipaddr[1] = 0; ipaddr[2] = ipaddr[3] = 0;
            return(-1);
        }
    }

    return(0);
}

char *SockaddrInGetAddrText(struct sockaddr *addr, char *str, int32_t len)
{
    int32_t i;
	char *s = str;

    if (len <= 0)
        return(NULL);
    if (len < 16) {
        *s = 0;
        return(NULL);
    }

    for (i = 2; i < 6; ++i) {
        int32_t val = (unsigned char)addr->sa_data[i];
        if (val > 99) {
            *s++ = (char)('0'+(val/100));
            val %= 100;
            *s++ = (char)('0'+(val/10));
            val %= 10;
        }
        if (val > 9) {
            *s++ = (char)('0'+(val/10));
            val %= 10;
        }
        *s++ = (char)('0'+val);
        if (i < 5) {
            *s++ = '.';
        }
    }

    *s = 0;
    return(str);
}

uint16_t SocketHtons(uint16_t addr)
{
    unsigned char netw[2];

    memcpy((char *)netw, (char *)&addr, sizeof(addr));

    return((netw[0]<<8)|(netw[1]<<0));
}

uint32_t SocketHtonl(uint32_t addr)
{
    unsigned char netw[4];

    memcpy((char *)netw, (char *)&addr, sizeof(addr));

    return((((((netw[0]<<8)|netw[1])<<8)|netw[2])<<8)|netw[3]);
}

uint16_t SocketNtohs(uint16_t addr)
{
    unsigned char netw[2];

    netw[1] = (unsigned char)addr;
    addr >>= 8;
    netw[0] = (unsigned char)addr;

    memcpy((char *)&addr, (char *)netw, sizeof(addr));

    return(addr);
}

uint32_t SocketNtohl(uint32_t addr)
{
    unsigned char netw[4];

    netw[3] = (unsigned char)addr;
    addr >>= 8;
    netw[2] = (unsigned char)addr;
    addr >>= 8;
    netw[1] = (unsigned char)addr;
    addr >>= 8;
    netw[0] = (unsigned char)addr;

    memcpy((char *)&addr, (char *)netw, sizeof(addr));

    return(addr);
}

const char *SocketInAddrGetText(int32_t addr)
{
    static char _addrtext[20];
    struct sockaddr sa;

    SockaddrInSetAddr(&sa,addr);
    SockaddrInGetAddrText(&sa,_addrtext,sizeof(_addrtext));

    return _addrtext;
}

char *SocketInAddrGetText2(uint32_t addr, char *str, int32_t len)
{
    struct sockaddr sa;
    SockaddrInSetAddr(&sa,addr);
    return(SockaddrInGetAddrText(&sa, str, len));
}

int32_t SocketInTextGetAddr(const char *pAddrText)
{
    struct sockaddr SockAddr;
    int32_t iAddr = 0;

    if (SockaddrInSetAddrText(&SockAddr, pAddrText) == 0)
    {
        iAddr = SockaddrInGetAddr(&SockAddr);
    }
    return(iAddr);
}

int32_t SockaddrInParse(struct sockaddr *sa, const char *pParse)
{
    int32_t iReturn = 0, iPort;
    uint32_t uAddr;

    SockaddrInit(sa, AF_INET);

    iReturn = SockaddrInParse2(&uAddr, &iPort, NULL, pParse);

    SockaddrInSetAddr(sa, uAddr);
    SockaddrInSetPort(sa, iPort);

    return(iReturn);
}

int32_t SockaddrInParse2(uint32_t *pAddr, int32_t *pPort, int32_t *pPort2, const char *pParse)
{
    int32_t iReturn = 0;
    uint32_t uVal;

    while ((*pParse > 0) && (*pParse <= ' '))
    {
        ++pParse;
    }

	for (uVal = 0; ((*pParse >= '0') && (*pParse <= '9')) || (*pParse == '.'); ++pParse)
    {
		if (*pParse != '.')
        {
			uVal = (uVal - (uVal & 255)) + ((uVal & 255) * 10) + (*pParse & 15);
        }
		else
        {
            uVal <<= 8;
        }
	}
    if ((*pAddr = uVal) != 0)
    {
        iReturn |= 1;
    }

	while ((*pParse != ':') && (*pParse != 0))
    {
		++pParse;
    }

    uVal = 0;
	if (*pParse == ':')
    {
		for (++pParse; (*pParse >= '0') && (*pParse <= '9'); ++pParse)
        {
			uVal = (uVal * 10) + (*pParse & 15);
        }
        iReturn |= 2;
	}
    *pPort = (int32_t)uVal;

    if (pPort2 != NULL)
    {
        uVal = 0;
	    if (*pParse == ':')
        {
		    for (++pParse; (*pParse >= '0') && (*pParse <= '9'); ++pParse)
            {
			    uVal = (uVal * 10) + (*pParse & 15);
            }
            iReturn |= 4;
	    }
        *pPort2 = (int32_t)uVal;
    }

    return(iReturn);
}
