/*
Copyright (C) 2002,2004,2005,2009-2010 Electronic Arts, Inc.  All rights reserved.

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

/*H**************************************************************************/
/*!

    \File protossl.c
*/
/***************************************************************************H*/

/*** Include files ***********************************************************/

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>

#include "dirtysock.h"
#include "dirtymem.h"
#include "protossl.h"
#include "cryptmd2.h"
#include "cryptmd5.h"
#include "cryptsha1.h"
#include "cryptarc4.h"
#include "cryptrsa.h"

#include "lobbybase64.h"

/*** Defines ***************************************************************************/

#define DEBUG_ANY_CERT  (DIRTYCODE_DEBUG && 0)  
#define DEBUG_RAW_DATA  (DIRTYCODE_DEBUG && 0)  
#define DEBUG_ENC_PERF  (DIRTYCODE_DEBUG && 0) 
#define DEBUG_MSG_LIST  (DIRTYCODE_DEBUG && 0)  

#define SSL_MIN_PACKET      5                               
#define SSL_CRYPTO_PAD      1024                            
#define SSL_RAW_PACKET      16384                           
#define SSL_RCVMAX_PACKET   (SSL_RAW_PACKET+SSL_CRYPTO_PAD) 
#define SSL_SNDMAX_PACKET   (SSL_RAW_PACKET)                
#define SSL_SNDLIM_PACKET   (SSL_SNDMAX_PACKET-384)         

#define SSL3_VERSION        0x0300      

#define SSL3_REC_CIPHER         20  
#define SSL3_REC_ALERT          21  
#define SSL3_REC_HANDSHAKE      22  
#define SSL3_REC_APPLICATION    23  

#define SSL3_MSG_CLIENT_HELLO   1   
#define SSL3_MSG_SERVER_HELLO   2   
#define SSL3_MSG_CERTIFICATE    11  
#define SSL3_MSG_SERVER_KEY     12  
#define SSL3_MSG_CERT_REQ       13  
#define SSL3_MSG_SERVER_DONE    14  
#define SSL3_MSG_CERT_VERIFY    15  
#define SSL3_MSG_CLIENT_KEY     16  
#define SSL3_MSG_FINISHED       20  

#define SSL3_MAC_NULL           0   
#define SSL3_MAC_MD5            16  
#define SSL3_MAC_SHA            20  

#define SSL3_KEY_NULL           0   
#define SSL3_KEY_RSA            1   

#define SSL3_ENC_NULL           0   
#define SSL3_ENC_RC4            1   

#define SSL3_ALERT_LEVEL_WARNING   1
#define SSL3_ALERT_LEVEL_FATAL     2

#define SSL3_ALERT_DESC_CLOSE_NOTIFY              0
#define SSL3_ALERT_DESC_UNEXPECTED_MESSAGE        10
#define SSL3_ALERT_DESC_BAD_RECORD_MAC            20
#define SSL3_ALERT_DESC_DECOMPRESSION_FAILURE     30
#define SSL3_ALERT_DESC_HANDSHAKE_FAILURE         40
#define SSL3_ALERT_DESC_NO_CERTIFICATE            41
#define SSL3_ALERT_DESC_BAD_CERTFICIATE           42
#define SSL3_ALERT_DESC_UNSUPPORTED_CERTIFICATE   43
#define SSL3_ALERT_DESC_CERTIFICATE_REVOKED       44
#define SSL3_ALERT_DESC_CERTIFICATE_EXPIRED       45
#define SSL3_ALERT_DESC_CERTIFICATE_UNKNOWN       46
#define SSL3_ALERT_DESC_ILLEGAL_PARAMETER         47

#define SSL_CERT_X509   1

#define ST_IDLE         0
#define ST_ADDR         1
#define ST_CONN         2
#define ST_WAIT         3
#define ST3_SEND_HELLO  20
#define ST3_RECV_HELLO  21
#define ST3_SEND_KEY    22
#define ST3_SEND_CHANGE 23
#define ST3_SEND_FINISH 24
#define ST3_RECV_CHANGE 25
#define ST3_RECV_FINISH 26
#define ST3_SECURE      30
#define ST_UNSECURE     31
#define ST_FAIL         0x1000
#define ST_FAIL_DNS     0x1001
#define ST_FAIL_CONN    0x1002
#define ST_FAIL_CERT    0x1003
#define ST_FAIL_SETUP   0x1004
#define ST_FAIL_SECURE  0x1005

#define ASN_CLASS_UNIV      0x00
#define ASN_CLASS_APPL      0x40
#define ASN_CLASS_CONT      0x80
#define ASN_CLASS_PRIV      0xc0

#define ASN_PRIMITIVE       0x00
#define ASN_CONSTRUCT       0x20

#define ASN_TYPE_INTEGER    0x02
#define ASN_TYPE_BITSTRING  0x03
#define ASN_TYPE_OCTSTRING  0x04
#define ASN_TYPE_NULL       0x05
#define ASN_TYPE_OBJECT     0x06
#define ASN_TYPE_UTF8STR    0x0c
#define ASN_TYPE_SEQN       0x10
#define ASN_TYPE_SET        0x11
#define ASN_TYPE_PRINTSTR   0x13
#define ASN_TYPE_T61        0x14
#define ASN_TYPE_IA5        0x16
#define ASN_TYPE_UTCTIME    0x17
#define ASN_TYPE_GENERALIZEDTIME 0x18

enum {
    ASN_OBJ_NONE = 0,
    ASN_OBJ_COUNTRY,
    ASN_OBJ_STATE,
    ASN_OBJ_CITY,
    ASN_OBJ_ORGANIZATION,
    ASN_OBJ_UNIT,
    ASN_OBJ_COMMON,
    ASN_OBJ_RSA_PKCS_KEY,
    ASN_OBJ_RSA_PKCS_MD2,
    ASN_OBJ_RSA_PKCS_MD5,
    ASN_OBJ_RSA_PKCS_SHA1
};

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct ProtoSSLCertIdentT
{
    char strCountry[32];
    char strState[32];
    char strCity[32];
    char strOrg[32];
    char strUnit[256];
    char strCommon[32];
} ProtoSSLCertIdentT;

typedef struct ProtoSSLCACertT
{
    ProtoSSLCertIdentT Subject;     

    int32_t iKeyModSize;           
    const uint8_t *pKeyModData;     

    int32_t iKeyExpSize;            
    uint8_t KeyExpData[16];         

    int32_t iMemGroup;              
    void   *pMemGroupUserData;      

    struct ProtoSSLCACertT *pNext;  
} ProtoSSLCACertT;


typedef struct X509CertificateT
{
    ProtoSSLCertIdentT Issuer;      
    ProtoSSLCertIdentT Subject;     

    char strGoodFrom[32];           
    char strGoodTill[32];           

    int32_t iSerialSize;            
    uint8_t SerialData[32];         

    int32_t iSigType;               
    int32_t iSigSize;               
    uint8_t SigData[256];           

    int32_t iKeyType;               
    int32_t iKeyModSize;            
    uint8_t KeyModData[256];        
    int32_t iKeyExpSize;            
    uint8_t KeyExpData[16];         

    int32_t iHashSize;              
    uint8_t HashData[CRYPTSHA1_HASHSIZE];
} X509CertificateT;


typedef struct CipherSuiteT
{
    uint8_t uIdent[2];              
    uint8_t uKey;                   
    uint8_t uEnc;                   
    uint8_t uMac;                   
} CipherSuiteT;


typedef struct SecureStateT
{
    uint32_t uTimer;                

    uint32_t uSendSeqn;             
    int32_t iSendProg;              
    int32_t iSendSize;              

    uint32_t uRecvSeqn;             
    int32_t iRecvProg;              
    int32_t iRecvSize;              
    int32_t iRecvBase;              
    int32_t iRecvDone;              

    const CipherSuiteT *pCipher;   

    uint8_t ClientRandom[32];       
    uint8_t ServerRandom[32];       

    uint8_t PreMasterKey[48];       
    uint8_t MasterKey[48];         

    uint8_t KeyBlock[128];         
    uint8_t *pServerMAC;            
    uint8_t *pClientMAC;            
    uint8_t *pServerKey;            
    uint8_t *pClientKey;            

    CryptMD5T HandshakeMD5;         
    CryptSha1T HandshakeSHA;        
    CryptArc4T ReadArc4;            
    CryptArc4T WriteArc4;           

    X509CertificateT Cert;          

    uint8_t SendData[SSL_SNDMAX_PACKET];   
    uint8_t RecvData[SSL_RCVMAX_PACKET];   
} SecureStateT;

struct ProtoSSLRefT
{
    SocketT *pSock;                 
    HostentT *pHost;                

    int32_t iMemGroup;              
    void *pMemGroupUserData;        

    char strHost[256];              
    struct sockaddr PeerAddr;       

    int32_t iState;                 
    int32_t iClosed;                
    SecureStateT *pSecure;          
    
    uint8_t bAllowAnyCert;          
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

uint32_t _ProtoSSL_Random[4] = { 0, 0, 0, 0 };

const static uint8_t _SSL3_Pad1[48] =
{
    0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
    0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
    0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36
};

const static uint8_t _SSL3_Pad2[48] =
{
    0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,
    0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,
    0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c,0x5c
};

const static CipherSuiteT _SSL3_CipherSuite[] =
{
    { { 0, 5 }, SSL3_KEY_RSA, SSL3_ENC_RC4, SSL3_MAC_SHA }, 
    { { 0, 4 }, SSL3_KEY_RSA, SSL3_ENC_RC4, SSL3_MAC_MD5 }, 
};

const static struct
{
    int32_t iType;              
    int32_t iSize;              
    uint8_t strData[16];        
}
_SSL_ObjectList[] =
{
    { ASN_OBJ_COUNTRY, 3, { 0x55, 0x04, 0x06 } },
    { ASN_OBJ_CITY, 3, { 0x55, 0x04, 0x07 } },
    { ASN_OBJ_STATE, 3, { 0x55, 0x04, 0x08 } },
    { ASN_OBJ_ORGANIZATION, 3, { 0x55, 0x04, 0x0a } },
    { ASN_OBJ_UNIT, 3, { 0x55, 0x04, 0x0b } },
    { ASN_OBJ_COMMON, 3, { 0x55, 0x04, 0x03 } },
    { ASN_OBJ_RSA_PKCS_KEY, 9, { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01 } },
    { ASN_OBJ_RSA_PKCS_MD2, 9, { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x02 } },
    { ASN_OBJ_RSA_PKCS_MD5, 9, { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04 } },
    { ASN_OBJ_RSA_PKCS_SHA1, 9, { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05 } },
    { ASN_OBJ_NONE, 0, { 0 } }
};

const static uint8_t JunkModulus[] =
{
    0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc
};

static ProtoSSLCACertT _ProtoSSL_CACerts[] =
{
    { { "UnInit", "UnInit", "UnInit", "UnInit",
        "UnInit", "UnInit" },
      128, JunkModulus,
      1, { 0x03 }, 
      0, NULL, NULL },
    
};

/*** Private functions ******************************************************/


#if DIRTYCODE_DEBUG
static void  _DebugAlert(ProtoSSLRefT *pState, uint8_t uAlertLevel, uint8_t uAlertType)
{
    SecureStateT *pSecure = pState->pSecure;
    NetPrintf(("protossl: ALERT: level=%d, type=%d\n", uAlertLevel, uAlertType));
    NetPrintMem(_SSL3_Pad1, sizeof(_SSL3_Pad1), "_SSL3_Pad1");
    NetPrintMem(_SSL3_Pad2, sizeof(_SSL3_Pad2), "_SSL3_Pad2");
    NetPrintf(("protossl: pSecure->uSendSeqn=%d\n", pSecure->uSendSeqn));
    NetPrintf(("protossl: pSecure->pCipher->uMac=%d\n", pSecure->pCipher->uMac));
    NetPrintf(("protossl: pSecure->pCipher->uEnc=%d\n", pSecure->pCipher->uEnc));
}
#else
#define _DebugAlert(_pState, _uAlertLevel, _uAlertType)
#endif

static int32_t _WildcardMatchNoCase(const char *pString1, const char *pString2)
{
    int32_t r;
    char c1, c2;

    do {
        c1 = *pString1++;
        if ((c1 >= 'A') && (c1 <= 'Z'))
            c1 ^= 32;
        c2 = *pString2;
        if ((c2 >= 'A') && (c2 <= 'Z'))
            c2 ^= 32;
        if ((c2 == '*') && c1 != ('.'))
        {
            r = _WildcardMatchNoCase(pString1, pString2+1);
            if (r == 0)
            {
                break;
            }
            r = 0;
        }
        else
        {
            pString2 += 1;
            r = c1-c2;
        }
    } while ((c1 != 0) && (c2 != 0) && (r == 0));

    return(r);
}

static void _GenerateRandom(uint8_t *pRandomBuf, int32_t iRandomLen, CryptArc4T *pArc4)
{
    int32_t iCount;

    if (_ProtoSSL_Random[0] == 0)
    {
        _ProtoSSL_Random[0] = NetTick();
    }

    _ProtoSSL_Random[1] += NetTick();

    _ProtoSSL_Random[2] += 1;

    for (iCount = 0; iCount < 32; ++iCount)
    {
        _ProtoSSL_Random[3] += (&iCount)[iCount];
    }

    if (pRandomBuf != NULL)
    {
        CryptArc4Init(pArc4, (uint8_t *)&_ProtoSSL_Random, sizeof(_ProtoSSL_Random), 3);
        CryptArc4Apply(pArc4, pRandomBuf, iRandomLen);
    }
}

static int32_t _ResetState(ProtoSSLRefT *pState, int32_t iSecure)
{
    SecureStateT *pSecure;

    if (pState->pSock != NULL)
    {
        SocketClose(pState->pSock);
        pState->pSock = NULL;
    }

    if (pState->pHost != NULL)
    {
        pState->pHost->Free(pState->pHost);
        pState->pHost = NULL;
    }

    pState->iState = ST_IDLE;
    pState->iClosed = 1;

    if (!iSecure && (pState->pSecure != NULL))
    {
        DirtyMemFree(pState->pSecure, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        pState->pSecure = NULL;
    }

    if (iSecure && (pState->pSecure == NULL))
    {
        pState->pSecure = DirtyMemAlloc(sizeof(*pState->pSecure), PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
        if (pState->pSecure != NULL)
        {
            memset(pState->pSecure, 0, sizeof(*pState->pSecure));
        }
    }

    if ((pSecure = pState->pSecure) != NULL)
    {
        _GenerateRandom(NULL, 0, &pState->pSecure->ReadArc4);

        pSecure->iSendProg = 0;
        pSecure->iSendSize = 0;
        pSecure->uSendSeqn = 0;

        pSecure->iRecvProg = 0;
        pSecure->iRecvSize = 0;
        pSecure->uRecvSeqn = 0;

        CryptMD5Init(&pSecure->HandshakeMD5);
        CryptSha1Init(&pSecure->HandshakeSHA);
    }

    return((iSecure && !pSecure) ? SOCKERR_NOMEM : SOCKERR_NONE);
}

static int32_t _SendPacket(ProtoSSLRefT *pState, uint8_t uType, const void *pHeadPtr, int32_t iHeadLen, const void *pBodyPtr, int32_t iBodyLen)
{
    SecureStateT *pSecure = pState->pSecure;
    uint8_t *pSend;
    int32_t iSize;

    pSecure->SendData[0] = uType;
    pSecure->SendData[1] = (uint8_t)(SSL3_VERSION>>8);
    pSecure->SendData[2] = (uint8_t)(SSL3_VERSION>>0);

    pSend = pSecure->SendData+5;
    iSize = 0;

    memcpy(pSend+iSize, pHeadPtr, iHeadLen);
    iSize += iHeadLen;

    memcpy(pSend+iSize, pBodyPtr, iBodyLen);
    iSize += iBodyLen;

    if (uType == SSL3_REC_HANDSHAKE)
    {
        CryptMD5Update(&pSecure->HandshakeMD5, pSend, iSize);
        CryptSha1Update(&pSecure->HandshakeSHA, pSend, iSize);
    }

    if ((pState->iState >= ST3_SEND_FINISH) && (pState->iState <= ST3_SECURE) && (pSecure->pCipher != NULL))
    {
        uint8_t MacTemp[20];

        struct {
            uint8_t uSeqn[8];
            uint8_t uType;
            uint8_t uLength[2];
        } MacData;

        MacData.uSeqn[0] = 0;
        MacData.uSeqn[1] = 0;
        MacData.uSeqn[2] = 0;
        MacData.uSeqn[3] = 0;
        MacData.uSeqn[4] = (uint8_t)((pSecure->uSendSeqn>>24)&255);
        MacData.uSeqn[5] = (uint8_t)((pSecure->uSendSeqn>>16)&255);
        MacData.uSeqn[6] = (uint8_t)((pSecure->uSendSeqn>>8)&255);
        MacData.uSeqn[7] = (uint8_t)((pSecure->uSendSeqn>>0)&255);
        MacData.uType = pSecure->SendData[0];
        MacData.uLength[0] = (uint8_t)(iSize>>8);
        MacData.uLength[1] = (uint8_t)(iSize>>0);

        if (pSecure->pCipher->uMac == SSL3_MAC_MD5)
        {
            CryptMD5T MD5Context;
            CryptMD5Init(&MD5Context);
            CryptMD5Update(&MD5Context, pSecure->pClientMAC, 16);
            CryptMD5Update(&MD5Context, _SSL3_Pad1, 48);
            CryptMD5Update(&MD5Context, (char *)&MacData, sizeof(MacData));
            CryptMD5Update(&MD5Context, pSend, iSize);
            CryptMD5Final(&MD5Context, MacTemp, 16);

            CryptMD5Init(&MD5Context);
            CryptMD5Update(&MD5Context, pSecure->pClientMAC, 16);
            CryptMD5Update(&MD5Context, _SSL3_Pad2, 48);
            CryptMD5Update(&MD5Context, MacTemp, 16);
            CryptMD5Final(&MD5Context, pSend+iSize, 16);
            iSize += 16;
        }

        if (pSecure->pCipher->uMac == SSL3_MAC_SHA)
        {
            CryptSha1T SHAContext;
            CryptSha1Init(&SHAContext);
            CryptSha1Update(&SHAContext, pSecure->pClientMAC, 20);
            CryptSha1Update(&SHAContext, _SSL3_Pad1, 40);
            CryptSha1Update(&SHAContext, (char *)&MacData, sizeof(MacData));
            CryptSha1Update(&SHAContext, pSend, iSize);
            CryptSha1Final(&SHAContext, MacTemp, 20);

            CryptSha1Init(&SHAContext);
            CryptSha1Update(&SHAContext, pSecure->pClientMAC, 20);
            CryptSha1Update(&SHAContext, _SSL3_Pad2, 40);
            CryptSha1Update(&SHAContext, MacTemp, 20);
            CryptSha1Final(&SHAContext, pSend+iSize, 20);
            iSize += 20;
        }

        #if DEBUG_MSG_LIST
        NetPrintf(("protossl: _SendPacket (secure enc=%d mac=%d): type=%d, size=%d, seq=%d\n",
            pSecure->pCipher->uEnc, pSecure->pCipher->uMac, pSecure->SendData[0], iSize, pSecure->uSendSeqn));
        #endif
        #if (DEBUG_RAW_DATA > 1)
        NetPrintMem(pSecure->SendData, iSize+5, "_SendPacket");
        #endif

        if (pSecure->pCipher->uEnc == SSL3_ENC_RC4)
        {
            CryptArc4Apply(&pSecure->WriteArc4, pSend, iSize);
        }
    }
    else
    {
        #if DEBUG_MSG_LIST
        NetPrintf((" protossl: _SendPacket (unsecure): type=%d, size=%d, seq=%d\n", pSecure->SendData[0], iSize, pSecure->uSendSeqn));
        #endif
        #if (DEBUG_RAW_DATA > 1)
        NetPrintMem(pSecure->SendData, iSize+5, "_SendPacket");
        #endif
    }
        
    pSecure->SendData[3] = (uint8_t)(iSize>>8);
    pSecure->SendData[4] = (uint8_t)(iSize>>0);

    pSecure->iSendProg = 0;
    pSecure->iSendSize = iSize+5;

    pSecure->uSendSeqn += 1;
    return(0);
}

static int32_t _RecvPacket(ProtoSSLRefT *pState)
{
    int32_t iSize;
    SecureStateT *pSecure = pState->pSecure;

    pSecure->iRecvBase = pSecure->iRecvSize;

    if (pSecure->iRecvProg != pSecure->iRecvSize)
    {
        NetPrintf(("protossl: _RecvPacket: no packet queued\n"));
        return(-1);
    }

    if ((pSecure->RecvData[0] < SSL3_REC_CIPHER) || (pSecure->RecvData[0] > SSL3_REC_APPLICATION))
    {
        NetPrintf(("protossl: _RecvPacket: unknown record type %d\n", pSecure->RecvData[0]));
        return(-2);
    }

    if ((pSecure->RecvData[1] != (SSL3_VERSION>>8)) || (pSecure->RecvData[2] != (SSL3_VERSION&255)))
    {
        NetPrintf(("protossl: _RecvPacket: unsupported version 0x%02x%02x\n", pSecure->RecvData[1], pSecure->RecvData[2]));
        return(-3);
    }

    iSize = (pSecure->RecvData[3]<<8)|pSecure->RecvData[4];
    if (pSecure->iRecvSize != iSize+5)
    {
        NetPrintf(("protossl: _RecvPacket: invalid length (%d != %d)\n", pSecure->iRecvSize, iSize+5));
        return(-4);
    }

    pSecure->iRecvBase = 5;

    if ((pState->iState >= ST3_RECV_FINISH) && (pState->iState <= ST3_SECURE) && (pSecure->pCipher != NULL))
    {
        uint8_t MacTemp[20];

        struct {
            uint8_t uSeqn[8];
            uint8_t uType;
            uint8_t uLength[2];
        } MacData;

        if (pSecure->pCipher->uEnc == SSL3_ENC_RC4)
        {
            CryptArc4Apply(&pSecure->ReadArc4, pSecure->RecvData+pSecure->iRecvBase, pSecure->iRecvSize-pSecure->iRecvBase);
        }

        #if DEBUG_MSG_LIST
        NetPrintf(("protossl: _RecvPacket (secure enc=%d mac=%d): type=%d, size=%d, seq=%d\n",
            pSecure->pCipher->uEnc, pSecure->pCipher->uMac, pSecure->RecvData[0], iSize, pSecure->uRecvSeqn));
        #endif
        #if (DEBUG_RAW_DATA > 1)
        NetPrintMem(pSecure->RecvData, pSecure->iRecvSize, "_RecvPacket");
        #endif

        iSize = pSecure->iRecvSize-pSecure->iRecvBase;
        if (iSize < pSecure->pCipher->uMac)
        {
            NetPrintf(("protossl: _RecvPacket: no room for mac (%d < %d)\n", iSize, pSecure->pCipher->uMac));
            return(-4);
        }
        iSize -= pSecure->pCipher->uMac;

        pSecure->iRecvProg = pSecure->iRecvSize = pSecure->iRecvBase+iSize;

        MacData.uSeqn[0] = 0;
        MacData.uSeqn[1] = 0;
        MacData.uSeqn[2] = 0;
        MacData.uSeqn[3] = 0;
        MacData.uSeqn[4] = (uint8_t)((pSecure->uRecvSeqn>>24)&255);
        MacData.uSeqn[5] = (uint8_t)((pSecure->uRecvSeqn>>16)&255);
        MacData.uSeqn[6] = (uint8_t)((pSecure->uRecvSeqn>>8)&255);
        MacData.uSeqn[7] = (uint8_t)((pSecure->uRecvSeqn>>0)&255);
        MacData.uType = pSecure->RecvData[0];
        MacData.uLength[0] = (uint8_t)(iSize>>8);
        MacData.uLength[1] = (uint8_t)(iSize>>0);

        if (pSecure->pCipher->uMac == SSL3_MAC_MD5)
        {
            CryptMD5T MD5Context;
            CryptMD5Init(&MD5Context);
            CryptMD5Update(&MD5Context, pSecure->pServerMAC, 16);
            CryptMD5Update(&MD5Context, _SSL3_Pad1, 48);
            CryptMD5Update(&MD5Context, (char *)&MacData, sizeof(MacData));
            CryptMD5Update(&MD5Context, pSecure->RecvData+pSecure->iRecvBase, pSecure->iRecvSize-pSecure->iRecvBase);
            CryptMD5Final(&MD5Context, MacTemp, 16);

            CryptMD5Init(&MD5Context);
            CryptMD5Update(&MD5Context, pSecure->pServerMAC, 16);
            CryptMD5Update(&MD5Context, _SSL3_Pad2, 48);
            CryptMD5Update(&MD5Context, MacTemp, 16);
            CryptMD5Final(&MD5Context, MacTemp, 16);

            if (memcmp(MacTemp, pSecure->RecvData+pSecure->iRecvSize, 16) != 0)
            {
                NetPrintf(("protossl: _RecvPacket: bad MD5 MAC\n"));
                return(-5);
            }
        }

        if (pSecure->pCipher->uMac == SSL3_MAC_SHA)
        {
            CryptSha1T SHAContext;
            CryptSha1Init(&SHAContext);
            CryptSha1Update(&SHAContext, pSecure->pServerMAC, 20);
            CryptSha1Update(&SHAContext, _SSL3_Pad1, 40);
            CryptSha1Update(&SHAContext, (char *)&MacData, sizeof(MacData));
            CryptSha1Update(&SHAContext, pSecure->RecvData+pSecure->iRecvBase, pSecure->iRecvSize-pSecure->iRecvBase);
            CryptSha1Final(&SHAContext, MacTemp, 20);

            CryptSha1Init(&SHAContext);
            CryptSha1Update(&SHAContext, pSecure->pServerMAC, 20);
            CryptSha1Update(&SHAContext, _SSL3_Pad2, 40);
            CryptSha1Update(&SHAContext, MacTemp, 20);
            CryptSha1Final(&SHAContext, MacTemp, 20);

            if (memcmp(MacTemp, pSecure->RecvData+pSecure->iRecvSize, 20) != 0)
            {
                NetPrintf(("protossl: _RecvPacket: bad SHA MAC\n"));
                return(-6);
            }
        }
    }
    else
    {
        #if DEBUG_MSG_LIST
        NetPrintf(("protossl: _RecvPacket (unsecure): type=%d, size=%d, seq=%d\n", pSecure->RecvData[0], iSize, pSecure->uRecvSeqn));
        #endif
        #if (DEBUG_RAW_DATA > 1)
        NetPrintMem(pSecure->RecvData, pSecure->iRecvSize, "_RecvPacket");
        #endif
    }

    if ((pSecure->RecvData[0] == SSL3_REC_HANDSHAKE) && (pState->iState < ST3_RECV_FINISH))
    {
        CryptMD5Update(&pSecure->HandshakeMD5, pSecure->RecvData+pSecure->iRecvBase, pSecure->iRecvSize-pSecure->iRecvBase);
        CryptSha1Update(&pSecure->HandshakeSHA, pSecure->RecvData+pSecure->iRecvBase, pSecure->iRecvSize-pSecure->iRecvBase);
    }

    pSecure->uRecvSeqn += 1;
    pSecure->iRecvDone = 1;
    return(0);
}

static const void *_RecvHandshake(ProtoSSLRefT *pState, uint8_t uType)
{
    int32_t iSize;
    SecureStateT *pSecure = pState->pSecure;
    uint8_t *pRecv = pSecure->RecvData+pSecure->iRecvBase;

    if (pRecv[0] != uType)
    {
        #if DEBUG_RAW_DATA
        NetPrintf(("protossl: _RecvHandshake: got type %d instead of %d\n", pRecv[0], uType));
        #endif
        return(NULL);
    }

    iSize = (pRecv[1]<<16)|(pRecv[2]<<8)|(pRecv[3]<<0);
    if (pSecure->iRecvBase+4+iSize > pSecure->iRecvSize)
    {
        NetPrintf(("protossl: _RecvHandshake: packet at base %d is too long (%d vs %d)\n", pSecure->iRecvBase,
            iSize, pSecure->iRecvSize-pSecure->iRecvBase-4));
        return(NULL);
    }

    pSecure->iRecvBase += iSize+4;

    return(pRecv+4);
}

#if DIRTYCODE_DEBUG
static void _DebugPrintCertIdent(const ProtoSSLCertIdentT *pIdent, const char *pName)
{
    NetPrintf(("protossl:  %s: C=%s, ST=%s, L=%s, O=%s, OU=%s, CN=%s\n", pName,
        pIdent->strCountry, pIdent->strState, pIdent->strCity,
        pIdent->strOrg, pIdent->strUnit, pIdent->strCommon));
}
#else
#define _DebugPrintCertIdent(__pCert, __pMessage)
#endif

#if DIRTYCODE_DEBUG
static void _DebugPrintCert(const X509CertificateT *pCert, const char *pMessage)
{
    NetPrintf(("protossl: %s\n", pMessage));
    _DebugPrintCertIdent(&pCert->Issuer, " issuer");
    _DebugPrintCertIdent(&pCert->Subject, "subject");
    NetPrintf(("protossl: keytype: %d\n", pCert->iKeyType));
    NetPrintf(("protossl: keyexp size: %d\n", pCert->iKeyExpSize));
    NetPrintf(("protossl: keymod size: %d\n", pCert->iKeyModSize));
    NetPrintf(("protossl: sigtype: %d\n", pCert->iSigType));
    NetPrintf(("protossl: sigsize: %d\n", pCert->iSigSize));
}
#else
#define _DebugPrintCert(__pCert, __pMessage)
#endif

static const uint8_t *_FindPEMSignature(const uint8_t *pCertData, int32_t iCertSize, const char *pSigText)
{
    int32_t iSigLen = (int32_t)strlen(pSigText);
    int32_t iCertIdx;
    
    for (iCertIdx = 0; iCertIdx < iCertSize; iCertIdx += 1)
    {
        if ((pCertData[iCertIdx] == *pSigText) && ((iCertSize - iCertIdx) >= iSigLen))
        {
            if (!strncmp(pCertData+iCertIdx, pSigText, iSigLen))
            {
                return(pCertData+iCertIdx);
            }
        }
    }
    return(NULL);
}

static int32_t _FindPEMCertificateData(const uint8_t *pCertData, int32_t iCertSize, const uint8_t **pCertBeg, const uint8_t **pCertEnd)
{
    static const char _strPemBeg[] = "-----BEGIN CERTIFICATE-----";
    static const char _strPemEnd[] = "-----END CERTIFICATE-----";
    static const char _str509Beg[] = "-----BEGIN X509 CERTIFICATE-----";
    static const char _str509End[] = "-----END X509 CERTIFICATE-----";
    
    if (((*pCertBeg = _FindPEMSignature(pCertData, iCertSize, _strPemBeg)) != NULL) &&
        ((*pCertEnd = _FindPEMSignature(*pCertBeg, pCertData+iCertSize-*pCertBeg, _strPemEnd)) != NULL))
    {
        *pCertBeg += strlen(_strPemBeg);
        return(*pCertEnd-*pCertBeg);
    }
    else if (((*pCertBeg = _FindPEMSignature(pCertData, iCertSize, _str509Beg)) != NULL) &&
             ((*pCertEnd = _FindPEMSignature(*pCertBeg, pCertData+iCertSize-*pCertBeg, _str509End)) != NULL))
    {
        *pCertBeg += strlen(_str509Beg);
        return(*pCertEnd-*pCertBeg);
    }
    return(0);
}

static int32_t _CompareIdent(const ProtoSSLCertIdentT *pIdent1, const ProtoSSLCertIdentT *pIdent2)
{
    return(strcmp(pIdent1->strCountry, pIdent2->strCountry) ||
           strcmp(pIdent1->strState, pIdent2->strState) ||
           strcmp(pIdent1->strCity, pIdent2->strCity) ||
           strcmp(pIdent1->strOrg, pIdent2->strOrg) ||
           strcmp(pIdent1->strUnit, pIdent2->strUnit) ||
           strcmp(pIdent1->strCommon, pIdent2->strCommon)
           );
}

static const unsigned char *_ParseHeader(const unsigned char *pData, const unsigned char *pLast, int32_t *pType, int32_t *pSize)
{
    int32_t iCnt;
    uint32_t uLen;

    if (pSize != NULL)
    {
        *pSize = 0;
    }
    if (pType != NULL)
    {
        *pType = 0;
    }
    
    if (pData == NULL)
    {
        return(NULL);
    }

    if (pData == pLast)
    {
        return(NULL);
    }

    if (pType != NULL)
    {
        *pType = *pData;
    }
    pData += 1;

    if (pData == pLast)
    {
        return(NULL);
    }

    uLen = *pData++;
    if (uLen > 127)
    {
        iCnt = (uLen & 127);
        for (uLen = 0; iCnt > 0; --iCnt)
        {
            if (pData == pLast)
            {
                return(NULL);
            }
            uLen = (uLen << 8) | *pData++;
        }
    }
    if (pSize != NULL)
    {
        *pSize = uLen;
    }

    return(pData);
}

static int32_t _ParseObject(const unsigned char *pData, int32_t iSize)
{
    int32_t iType = 0;
    int32_t iIndex;

    for (iIndex = 0; _SSL_ObjectList[iIndex].iType != ASN_OBJ_NONE; ++iIndex)
    {
        if ((iSize >= _SSL_ObjectList[iIndex].iSize) && (memcmp(pData, _SSL_ObjectList[iIndex].strData, _SSL_ObjectList[iIndex].iSize) == 0))
        {
            iType = _SSL_ObjectList[iIndex].iType;
            break;
        }
    }

    return(iType);
}
 
static void _ParseString(const unsigned char *pData, int32_t iSize, char *pString, int32_t iLength)
{
    for (; (iSize > 0) && (iLength > 1); --iSize, --iLength)
    {
        *pString++ = *pData++;
    }
    if (iLength > 0)
    {
        *pString = 0;
    }
}

static void _ParseStringMulti(const unsigned char *pData, int32_t iSize, char *pString, int32_t iLength)
{
    for (; (*pString != '\0') && (iLength > 1); --iLength)
    {
        pString += 1;
    }
    for (; (iSize > 0) && (iLength > 1); --iSize, --iLength)
    {
        *pString++ = *pData++;
    }
    if (iLength > 0)
    {
        *pString = '\0';
    }
}

static int32_t _ParseCertificate(X509CertificateT *pCert, const uint8_t *pData, int32_t iSize)
{
    int32_t iType;
    int32_t iObjType;
    const uint8_t *pInfData;
    const uint8_t *pInfSkip;
    const uint8_t *pSigSkip;
    const uint8_t *pIssSkip;
    const uint8_t *pSubSkip;
    const uint8_t *pKeySkip;
    const uint8_t *pLast = pData+iSize;
    int32_t iKeySize;
    const uint8_t *pKeyData;

    memset(pCert, 0, sizeof(*pCert));

    pData = _ParseHeader(pData, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not parse base sequence (iType=%d)\n", iType));
        return(-1);
    }

    pData = _ParseHeader(pInfData = pData, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not parse info sequence (iType=%d)\n", iType));
        return(-2);
    }
    pInfSkip = pData+iSize;

    if (*pData != ASN_TYPE_INTEGER+ASN_PRIMITIVE)
    {
        pData = _ParseHeader(pData, pLast, NULL, &iSize);
        if (pData == NULL)
        {
            NetPrintf(("protossl: _ParseCertificate: could not skip non-integer tag\n"));
            return(-3);
        }
        pData += iSize;
    }

    pData = _ParseHeader(pData, pInfSkip, &iType, &iSize);
    if ((pData == NULL) || (iSize < 0) || ((unsigned)iSize > sizeof(pCert->SerialData)))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get certificate serial number (iSize=%d)\n", iSize));
        return(-4);
    }
    pCert->iSerialSize = iSize;
    memcpy(pCert->SerialData, pData, iSize);
    pData += iSize;

    pData = _ParseHeader(pData, pInfSkip, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get signature algorithm (iType=%d)\n", iType));
        return(-5);
    }
    pSigSkip = pData+iSize;

    pData = _ParseHeader(pData, pInfSkip, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_OBJECT+ASN_PRIMITIVE))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get signature algorithm type (iType=%d)\n", iType));
        return(-6);
    }

    pCert->iSigType = _ParseObject(pData, iSize);
    if (pCert->iSigType == ASN_OBJ_NONE)
    {
        NetPrintMem(pData, iSize, "protossl: unsupported signature algorithm");
        return(-7);
    }
    pData += iSize;

    pData = _ParseHeader(pSigSkip, pInfSkip, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get issuer (iType=%d)\n", iType));
        return(-8);
    }
    pIssSkip = pData+iSize;

    iObjType = 0;
    while ((pData = _ParseHeader(pData, pIssSkip, &iType, &iSize)) != NULL)
    {
        if ((iType != ASN_TYPE_SEQN+ASN_CONSTRUCT) && (iType != ASN_TYPE_SET+ASN_CONSTRUCT))
        {
            if (iType == ASN_TYPE_OBJECT+ASN_PRIMITIVE)
            {
                iObjType = _ParseObject(pData, iSize);
            }
            if ((iType == ASN_TYPE_PRINTSTR+ASN_PRIMITIVE) || (iType == ASN_TYPE_UTF8STR+ASN_PRIMITIVE) || (iType == ASN_TYPE_T61+ASN_PRIMITIVE))
            {
                if (iObjType == ASN_OBJ_COUNTRY)
                {
                    _ParseString(pData, iSize, pCert->Issuer.strCountry, sizeof(pCert->Issuer.strCountry));
                }
                if (iObjType == ASN_OBJ_STATE)
                {
                    _ParseString(pData, iSize, pCert->Issuer.strState, sizeof(pCert->Issuer.strState));
                }
                if (iObjType == ASN_OBJ_CITY)
                {
                    _ParseString(pData, iSize, pCert->Issuer.strCity, sizeof(pCert->Issuer.strCity));
                }
                if (iObjType == ASN_OBJ_ORGANIZATION)
                {
                    _ParseString(pData, iSize, pCert->Issuer.strOrg, sizeof(pCert->Issuer.strOrg));
                }
                if (iObjType == ASN_OBJ_UNIT)
                {
                    if (pCert->Issuer.strUnit[0] != '\0')
                    {
                        strnzcat(pCert->Issuer.strUnit, ", ", sizeof(pCert->Issuer.strUnit));
                    }
                    _ParseStringMulti(pData, iSize, pCert->Issuer.strUnit, sizeof(pCert->Issuer.strUnit));
                }
                if (iObjType == ASN_OBJ_COMMON)
                {
                    _ParseString(pData, iSize, pCert->Issuer.strCommon, sizeof(pCert->Issuer.strCommon));
                }
                iObjType = 0;
            }
            pData += iSize;
        }
    }

    pData = _ParseHeader(pIssSkip, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get validity info (iType=%d)\n", iType));
        return(-9);
    }

    pData = _ParseHeader(pData, pLast, &iType, &iSize);
    if ((pData == NULL) || ((iType != ASN_TYPE_UTCTIME) && (iType != ASN_TYPE_GENERALIZEDTIME)))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get from date (iType=%d)\n", iType));
        return(-10);
    }
    _ParseString(pData, iSize, pCert->strGoodFrom, sizeof(pCert->strGoodFrom));
    pData += iSize;

    pData = _ParseHeader(pData, pLast, &iType, &iSize);
    if ((pData == NULL) || ((iType != ASN_TYPE_UTCTIME) && (iType != ASN_TYPE_GENERALIZEDTIME)))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get to date (iType=%d)\n", iType));
        return(-11);
    }
    _ParseString(pData, iSize, pCert->strGoodTill, sizeof(pCert->strGoodTill));
    pData += iSize;

    pData = _ParseHeader(pData, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not subject (iType=%d)\n", iType));
        return(-12);
    }
    pSubSkip = pData+iSize;

    iObjType = 0;
    while ((pData = _ParseHeader(pData, pSubSkip, &iType, &iSize)) != NULL)
    {
        if ((iType != ASN_TYPE_SEQN+ASN_CONSTRUCT) && (iType != ASN_TYPE_SET+ASN_CONSTRUCT))
        {
            if (iType == ASN_TYPE_OBJECT+ASN_PRIMITIVE)
            {
                iObjType = _ParseObject(pData, iSize);
            }
            if ((iType == ASN_TYPE_PRINTSTR+ASN_PRIMITIVE) || (iType == ASN_TYPE_UTF8STR+ASN_PRIMITIVE) || (iType == ASN_TYPE_T61+ASN_PRIMITIVE))
            {
                if (iObjType == ASN_OBJ_COUNTRY)
                {
                    _ParseString(pData, iSize, pCert->Subject.strCountry, sizeof(pCert->Subject.strCountry));
                }
                if (iObjType == ASN_OBJ_STATE)
                {
                    _ParseString(pData, iSize, pCert->Subject.strState, sizeof(pCert->Subject.strState));
                }
                if (iObjType == ASN_OBJ_CITY)
                {
                    _ParseString(pData, iSize, pCert->Subject.strCity, sizeof(pCert->Subject.strCity));
                }
                if (iObjType == ASN_OBJ_ORGANIZATION)
                {
                    _ParseString(pData, iSize, pCert->Subject.strOrg, sizeof(pCert->Subject.strOrg));
                }
                if (iObjType == ASN_OBJ_UNIT)
                {
                    if (pCert->Subject.strUnit[0] != '\0')
                    {
                        strnzcat(pCert->Subject.strUnit, ", ", sizeof(pCert->Subject.strUnit));
                    }
                    _ParseStringMulti(pData, iSize, pCert->Subject.strUnit, sizeof(pCert->Subject.strUnit));
                }
                if (iObjType == ASN_OBJ_COMMON)
                {
                    _ParseString(pData, iSize, pCert->Subject.strCommon, sizeof(pCert->Subject.strCommon));
                }
                iObjType = 0;
            }
            pData += iSize;
        }
    }

    pData = _ParseHeader(pSubSkip, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get public key (iType=%d)\n", iType));
        return(-13);
    }

    pData = _ParseHeader(pData, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get key algorithm sequence (iType=%d)\n", iType));
        return(-14);
    }
    pKeySkip = pData+iSize;

    pData = _ParseHeader(pData, pKeySkip, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_OBJECT+ASN_PRIMITIVE))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get public key algorithm (iType=%d)\n", iType));
        return(-15);
    }
    pCert->iKeyType = _ParseObject(pData, iSize);

    pData = _ParseHeader(pKeySkip, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_BITSTRING+ASN_PRIMITIVE) || (iSize < 1))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get actual public key (iType=%d, iSize=%d)\n", iType, iSize));
        return(-16);
    }

    pKeyData = pData+1;
    iKeySize = iSize-1;
    pData += iSize;

    pData = _ParseHeader(pInfSkip, pSigSkip, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get signature algorithm sequence (iType=%d)\n", iType));
        return(-18);
    }
    pSigSkip = pData+iSize;

    pData = _ParseHeader(pData, pSigSkip,  &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_OBJECT+ASN_PRIMITIVE))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get signature algorithm identifier (iType=%d)\n", iType));
        return(-19);
    }
    pCert->iSigType = _ParseObject(pData, iSize);

    pData = _ParseHeader(pSigSkip, pLast, &iType, &iSize);
    if ((pData == NULL) || (iType != ASN_TYPE_BITSTRING+ASN_PRIMITIVE) || (iSize < 1))
    {
        NetPrintf(("protossl: _ParseCertificate: could not get signature data (iType=%d, iSize=%d)\n", iType, iSize));
        return(-20);
    }
    if (iSize > (signed)(sizeof(pCert->SigData)+1))
    {
        NetPrintf(("protossl: _ParseCertificate: signature data is too large (sig=%d, max=%d)\n", iSize-1, sizeof(pCert->SigData)));
        return(-21);
    }
    pCert->iSigSize = iSize-1;
    memcpy(pCert->SigData, pData+1, iSize-1);
    pData += iSize;

    if (pCert->iKeyType == ASN_OBJ_RSA_PKCS_KEY)
    {
        int32_t iKeyModSize, iKeyExpSize;
        const uint8_t *pKeyModData, *pKeyExpData;

        pData = _ParseHeader(pKeyData, pKeyData+iKeySize, &iType, &iSize);
        if ((pData == NULL) || (iType != ASN_TYPE_SEQN+ASN_CONSTRUCT))
        {
            NetPrintf(("protossl: _ParseCertificate: could not get sequence (iType=%d)\n", iType));
            return(-22);
        }

        pData = _ParseHeader(pData, pKeyData+iKeySize, &iType, &iSize);
        if ((pData == NULL) || (iType != ASN_TYPE_INTEGER+ASN_PRIMITIVE) || (iSize < 4))
        {
            NetPrintf(("protossl: _ParseCertificate: could not get modulus (iType=%d, iSize=%d)\n", iType, iSize));
            return(-23);
        }

        iKeyModSize = iSize;
        pKeyModData = pData;

        if (*pKeyModData == 0)
        {
            pKeyModData += 1;
            iKeyModSize -= 1;
        }

        if ((unsigned)iKeyModSize > sizeof(pCert->KeyModData))
        {
            NetPrintf(("protossl: _ParseCertificate: modulus data is too large (mod=%d, max=%d)\n", iKeyModSize, sizeof(pCert->KeyModData)));
            return(-24);
        }
        pCert->iKeyModSize = iKeyModSize;
        memcpy(pCert->KeyModData, pKeyModData, iKeyModSize);
        pData += iSize;

        pData = _ParseHeader(pData, pKeyData+iKeySize, &iType, &iSize);
        if ((pData == NULL) || (iType != ASN_TYPE_INTEGER+ASN_PRIMITIVE) || (iSize < 1))
        {
            NetPrintf(("protossl: _ParseCertificate: could not get exponent (iType=%d, iSize=%d)\n", iType, iSize));
            return(-25);
        }

        iKeyExpSize = iSize;
        pKeyExpData = pData;

        if (*pKeyExpData == 0)
        {
            pKeyExpData += 1;
            iKeyExpSize -= 1;
        }

        if (iKeyExpSize > (signed)sizeof(pCert->KeyExpData))
        {
            NetPrintf(("protossl: _ParseCertificate: exponent data is too large (exp=%d, max=%d)\n", iKeyExpSize, sizeof(pCert->KeyExpData)));
            return(-26);
        }
        pCert->iKeyExpSize = iKeyExpSize;
        memcpy(pCert->KeyExpData, pKeyExpData, iKeyExpSize);
        pData += iSize;
    }

    switch (pCert->iSigType)
    {
        case ASN_OBJ_RSA_PKCS_MD2:
        {
            CryptMD2T MD2;
            CryptMD2Init(&MD2);
            CryptMD2Update(&MD2, pInfData, pInfSkip-pInfData);
            CryptMD2Final(&MD2, pCert->HashData, pCert->iHashSize = MD2_BINARY_OUT);
            break;
        }

        case ASN_OBJ_RSA_PKCS_MD5:
        {
            CryptMD5T MD5;
            CryptMD5Init(&MD5);
            CryptMD5Update(&MD5, pInfData, pInfSkip-pInfData);
            CryptMD5Final(&MD5, pCert->HashData, pCert->iHashSize = MD5_BINARY_OUT);
            break;
        }

        case ASN_OBJ_RSA_PKCS_SHA1:
        {
            CryptSha1T Sha1;
            CryptSha1Init(&Sha1);
            CryptSha1Update(&Sha1, pInfData, pInfSkip-pInfData);
            CryptSha1Final(&Sha1, pCert->HashData, pCert->iHashSize = CRYPTSHA1_HASHSIZE);
            break;
        }

        default:
            NetPrintf(("protossl: unknown signature algorithm, should never get here\n"));
            pCert->iHashSize = 0;
    }

    return(0);
}

static int32_t _VerifyCertificate(X509CertificateT *pCert, int32_t iSelfSigned)
{
    CryptRSAT RSA;
    int32_t iKeyModSize;
    int32_t iKeyExpSize;
    const uint8_t *pKeyModData;
    const uint8_t *pKeyExpData;
    const ProtoSSLCACertT *pCACert;

    if (iSelfSigned && (_CompareIdent(&pCert->Subject, &pCert->Issuer) == 0))
    {
        pKeyModData = pCert->KeyModData;
        iKeyModSize = pCert->iKeyModSize;
        pKeyExpData = pCert->KeyExpData;
        iKeyExpSize = pCert->iKeyExpSize;
        pCACert = NULL;
    }
    else
    {
        for (pCACert = _ProtoSSL_CACerts; (pCACert != NULL) && (_CompareIdent(&pCACert->Subject, &pCert->Issuer) != 0); pCACert = pCACert->pNext)
            ;
        if (pCACert == NULL)
        {
            _DebugPrintCert(pCert, "_VerifyCertificate() -- no CA available for this certificate");
            return(-28);
        }

        if (pCACert->iKeyModSize != pCert->iSigSize)
        {
            NetPrintf(("protossl: _VerifyCertificate() -- Cert/CA modulus size mismatch (got %d but expected %d)\n", pCert->iSigSize, pCACert->iKeyModSize));
            return(-29);
        }

        pKeyModData = pCACert->pKeyModData;
        iKeyModSize = pCACert->iKeyModSize;
        pKeyExpData = pCACert->KeyExpData;
        iKeyExpSize = pCACert->iKeyExpSize;
    }

    #if DEBUG_ENC_PERF
    uint32_t uTick = NetTick();
    #endif

    CryptRSAInit(&RSA, pKeyModData, iKeyModSize, pKeyExpData, iKeyExpSize);
    CryptRSAInitSignature(&RSA, pCert->SigData, pCert->iSigSize);
    CryptRSAEncrypt(&RSA);

    #if DEBUG_ENC_PERF
    NetPrintf(("protossl: SSL Perf: RSA Sig %dms\n", NetTick()-uTick));
    #endif

    if (memcmp(pCert->HashData, RSA.EncryptBlock+(pCert->iSigSize&65534)-pCert->iHashSize, pCert->iHashSize) != 0)
    {
        NetPrintf(("protossl: _VerifyCertificate: signature hash mismatch\n"));
        _DebugPrintCert(pCert, "failed cert");
        if (pCACert != NULL)
        {
            _DebugPrintCertIdent(&pCACert->Subject, "parent ca");
        }
        return(-30);
    }

    return(0);
}

static void _AddCertificate(X509CertificateT *pCert, int32_t iMemGroup, void *pMemGroupUserData)
{
    ProtoSSLCACertT *pCACert;
    int32_t iCertSize = sizeof(*pCACert) + pCert->iKeyModSize;

    for (pCACert = _ProtoSSL_CACerts; (pCACert != NULL) && (_CompareIdent(&pCACert->Subject, &pCert->Subject) != 0); pCACert = pCACert->pNext)
        ;
    if (pCACert != NULL)
    {
        _DebugPrintCert(pCert, "ignoring redundant add of CA cert");
        return;
    }

    for (pCACert = _ProtoSSL_CACerts; pCACert->pNext != NULL; pCACert = pCACert->pNext)
        ;

    if ((pCACert->pNext = (ProtoSSLCACertT *)DirtyMemAlloc(iCertSize, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) != NULL)
    {
        pCACert = pCACert->pNext;
        memset(pCACert, 0, iCertSize);
        
        memcpy(&pCACert->Subject, &pCert->Subject, sizeof(pCACert->Subject));
        
        pCACert->iKeyExpSize = pCert->iKeyExpSize;
        memcpy(pCACert->KeyExpData, pCert->KeyExpData, pCACert->iKeyExpSize);
        
        pCACert->iKeyModSize = pCert->iKeyModSize;
        pCACert->pKeyModData = (uint8_t *)pCACert + sizeof(*pCACert);
        memcpy((uint8_t *)pCACert->pKeyModData, pCert->KeyModData, pCACert->iKeyModSize);
        
        pCACert->iMemGroup = iMemGroup;

        _DebugPrintCert(pCert, "added new certificate authority");
    }
    else
    {
        _DebugPrintCert(pCert, "failed to allocate memory for cert");
    }
}

/*** Public functions ********************************************************/

ProtoSSLRefT *ProtoSSLCreate(void)
{
    ProtoSSLRefT *pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;
       
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);    
    
    if ((pState = DirtyMemAlloc(sizeof(*pState), PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protossl: could not allocate module state\n"));
        return(NULL);
    }
    memset(pState, 0, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;

    return(pState);
}

void ProtoSSLReset(ProtoSSLRefT *pState)
{
    _ResetState(pState, 0);
}

void ProtoSSLDestroy(ProtoSSLRefT *pState)
{
    _ResetState(pState, 0);
    DirtyMemFree(pState, PROTOSSL_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
}

ProtoSSLRefT *ProtoSSLAccept(ProtoSSLRefT *pState, int32_t iSecure, struct sockaddr *pAddr, int32_t *pAddrlen)
{
    ProtoSSLRefT *pClient;
    SocketT *pSocket;
    
    if (iSecure)
    {
        return(NULL);
    }
    pSocket = SocketAccept(pState->pSock, pAddr, pAddrlen);
    if (pSocket == NULL)
    {
        return(NULL);
    }
    pClient = ProtoSSLCreate();
    if (pClient == NULL)
    {
        SocketClose(pSocket);
        return(NULL);
    }
    pClient->pSock = pSocket;
    memcpy(&pClient->PeerAddr, pAddr, *pAddrlen);
    pClient->iState = ST_UNSECURE;
    return pClient;
}

int32_t ProtoSSLBind(ProtoSSLRefT *pState, const struct sockaddr *pAddr, int32_t iAddrlen)
{
    int32_t iResult;

    if (pState->pSock != NULL)
    {
        SocketClose(pState->pSock);
    }

    pState->pSock = SocketOpen(AF_INET, SOCK_STREAM, 0);
    if (pState->pSock == NULL)
    {
        return(SOCKERR_OTHER);
    }

    iResult = SocketBind(pState->pSock, pAddr, iAddrlen);
    return(iResult);
}

int32_t ProtoSSLConnect(ProtoSSLRefT *pState, int32_t iSecure, const char *pAddr, uint32_t uAddr, int32_t iPort)
{
    int32_t iIndex;
    int32_t iError;

    iError = _ResetState(pState, iSecure);
    if (iError != SOCKERR_NONE)
    {
        return(iError);
    }

    pState->pSock = SocketOpen(AF_INET, SOCK_STREAM, 0);
    if (pState->pSock == NULL)
    {
        return(SOCKERR_NORSRC);
    }

    SockaddrInit(&pState->PeerAddr, AF_INET);

    if (pAddr == NULL)
    {
        pAddr = "";
    }

    for (iIndex = 0; (pAddr[iIndex] != 0) && (pAddr[iIndex] != ':') && ((unsigned)iIndex < sizeof(pState->strHost)-1); ++iIndex)
    {
        pState->strHost[iIndex] = pAddr[iIndex];
    }
    pState->strHost[iIndex] = 0;

    SockaddrInSetAddrText(&pState->PeerAddr, pState->strHost);
    if (SockaddrInGetAddr(&pState->PeerAddr) == 0)
    {
        SockaddrInSetAddr(&pState->PeerAddr, uAddr);
    }

    if (pAddr[iIndex] == ':')
    {
        SockaddrInSetPort(&pState->PeerAddr, atoi(pAddr+iIndex+1));
    }
    else
    {
        SockaddrInSetPort(&pState->PeerAddr, iPort);
    }

    if (SockaddrInGetAddr(&pState->PeerAddr) == 0)
    {
        pState->pHost = SocketLookup(pState->strHost, 30*1000);
        pState->iState = ST_ADDR;
    }
    else
    {
        pState->iState = ST_CONN;
    }

    return(SOCKERR_NONE);
}

int32_t ProtoSSLListen(ProtoSSLRefT *pState, int32_t iBacklog)
{
    int32_t iResult;
    iResult = SocketListen(pState->pSock, iBacklog);
    return(iResult);
}

int32_t ProtoSSLDisconnect(ProtoSSLRefT *pState)
{
    if ((DEBUG_ENC_PERF) && (pState->pSecure != NULL) && (pState->pSecure->uTimer != 0))
    {
        NetPrintf(("protossl: SSL Perf: closed %dms\n", NetTick()-pState->pSecure->uTimer));
        pState->pSecure->uTimer = 0;
    }

    _ResetState(pState, (pState->pSecure != 0));
    return(SOCKERR_NONE);
}

static int32_t _ProtoSSLUpdateSendClientHello(ProtoSSLRefT *pState)
{
    int32_t iResult;
    uint8_t strHead[4];
    uint8_t strBody[256];
    uint8_t *pData = strBody;
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Send Client Hello\n"));
    #endif

    #if DEBUG_ENC_PERF
    pSecure->uTimer = NetTick();
    #endif

    *pData++ = (uint8_t)(SSL3_VERSION>>8);
    *pData++ = (uint8_t)(SSL3_VERSION>>0);

    _GenerateRandom(pSecure->ClientRandom, sizeof(pSecure->ClientRandom), &pSecure->ReadArc4);
    memcpy(pData, pSecure->ClientRandom, sizeof(pSecure->ClientRandom));
    pData += 32;

    *pData++ = 0;

    *pData++ = 0;
    *pData++ = (sizeof(_SSL3_CipherSuite)/sizeof(_SSL3_CipherSuite[0]))*2;
    for (iResult = 0; iResult < (signed)(sizeof(_SSL3_CipherSuite)/sizeof(_SSL3_CipherSuite[0])); ++iResult)
    {
        *pData++ = _SSL3_CipherSuite[iResult].uIdent[0];
        *pData++ = _SSL3_CipherSuite[iResult].uIdent[1];
    }

    *pData++ = 1;
    *pData++ = 0;

    strHead[0] = SSL3_MSG_CLIENT_HELLO;
    strHead[1] = 0;
    strHead[2] = (uint8_t)((pData-strBody)>>8);
    strHead[3] = (uint8_t)((pData-strBody)>>0);

    _SendPacket(pState, SSL3_REC_HANDSHAKE, strHead, 4, strBody, pData-strBody);
    return(ST3_RECV_HELLO);
}

static int32_t _ProtoSSLUpdateRecvServerHello(ProtoSSLRefT *pState, const uint8_t *pData)
{
    int32_t iIndex;
    SecureStateT *pSecure = pState->pSecure;
 
    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Server Hello\n"));
    #endif

    if ((pData[0] != (SSL3_VERSION>>8)) || (pData[1] != (SSL3_VERSION&255)))
    {
        NetPrintf(("protossl: _ServerHello: version %d,%d\n", pData[0], pData[1]));
        return(ST_FAIL_SETUP);
    }
    pData += 2;

    memcpy(pSecure->ServerRandom, pData, sizeof(pSecure->ServerRandom));
    pData += 32;

    pData += *pData+1;

    pSecure->pCipher = NULL;
    for (iIndex = 0; iIndex < (signed)(sizeof(_SSL3_CipherSuite)/sizeof(_SSL3_CipherSuite[0])); ++iIndex)
    {
        if ((pData[0] == _SSL3_CipherSuite[iIndex].uIdent[0]) && (pData[1] == _SSL3_CipherSuite[iIndex].uIdent[1]))
        {
            pSecure->pCipher = _SSL3_CipherSuite+iIndex;
            break;
        }
    }

    #if DEBUG_MSG_LIST
    if (pSecure->pCipher == NULL)
    {
        NetPrintf(("protossl: SSL Msg: No matching cipher (ident=%d,%d)\n", pData[0], pData[1]));
    }
    else
    {
        NetPrintf(("protossl: SSL Msg: Cipher %d,%d selected (enc=%d mac=%d)\n",
            pData[0], pData[1], pSecure->pCipher->uEnc, pSecure->pCipher->uMac));
    }
    #endif

    return(ST3_RECV_HELLO);
}

static int32_t _ProtoSSLUpdateRecvServerCert(ProtoSSLRefT *pState, const uint8_t *pData)
{
    int32_t iSize1;
    int32_t iSize2;
    int32_t iParse;
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Server Cert\n"));
    #endif

    iSize1 = (pData[0]<<16)|(pData[1]<<8)|(pData[2]<<0);
    iSize2 = (pData[3]<<16)|(pData[4]<<8)|(pData[5]<<0);


    if (iSize2 > iSize1-3)
    {
        NetPrintf(("protossl: _ServerCert: first certificate is larger than envelope (%d/%d)\n", iSize1, iSize2));
        return(ST_FAIL_SETUP);
    }

    iParse = _ParseCertificate(&pSecure->Cert, pData+6, iSize2);
    if (iParse < 0)
    {
        NetPrintf(("protossl: _ServerCert: x509 cert invalid (error=%d)\n", iParse));
        return(ST_FAIL_CERT);
    }

    if (!pState->bAllowAnyCert)
    {
        if (_WildcardMatchNoCase(pState->strHost, pSecure->Cert.Subject.strCommon) != 0)
        {
            NetPrintf(("protossl: _ServerCert: certificate not issued to this host (%s != %s)\n", pState->strHost, pSecure->Cert.Subject.strCommon));
            return(ST_FAIL_CERT);
        }

        iParse = _VerifyCertificate(&pSecure->Cert, FALSE);
        if (iParse < 0)
        {
            NetPrintf(("protossl: _ServerCert: x509 cert untrusted (error=%d)\n", iParse));
            return(ST_FAIL_CERT);
        }
    }

    return(ST3_RECV_HELLO);
}

static int32_t _ProtoSSLUpdateRecvServerKey(ProtoSSLRefT *pState, const uint8_t *pData)
{
    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Server Key\n"));
    #endif

    if (pData[0] != 1)
    {
        NetPrintf(("protossl: _ServerKey: wrong key alg %d\n", pData[0]));
        return(ST_FAIL_SETUP);
    }

    return(ST3_RECV_HELLO);
}

static int32_t _ProtoSSLUpdateRecvServerDone(ProtoSSLRefT *pState, const uint8_t *pData)
{
    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Server Done\n"));
    #endif

    return(ST3_SEND_KEY);
}

static int32_t _ProtoSSLUpdateSendClientKey(ProtoSSLRefT *pState)
{
    int32_t iLoop;
    CryptRSAT RSAContext;
    CryptMD5T MD5Context;
    CryptSha1T SHA1Context;
    uint8_t *pData;
    uint8_t strHead[4];
    uint8_t strBody[256];
    SecureStateT *pSecure = pState->pSecure;
    #if DEBUG_ENC_PERF
    uint32_t uTick = NetTick();
    #endif

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Send Client Key\n"));
    #endif

    _GenerateRandom(pSecure->PreMasterKey, sizeof(pSecure->PreMasterKey), &pSecure->ReadArc4);
    pSecure->PreMasterKey[0] = (uint8_t)(SSL3_VERSION>>8);
    pSecure->PreMasterKey[1] = (uint8_t)(SSL3_VERSION>>0);

    CryptRSAInit(&RSAContext, pSecure->Cert.KeyModData, pSecure->Cert.iKeyModSize,
        pSecure->Cert.KeyExpData, pSecure->Cert.iKeyExpSize);
    CryptRSAInitMaster(&RSAContext, pSecure->PreMasterKey, sizeof(pSecure->PreMasterKey));
    CryptRSAEncrypt(&RSAContext);
    #if DEBUG_ENC_PERF
    NetPrintf(("protossl: SSL Perf: RSA Key %dms\n", NetTick()-uTick));
    #endif

    for (iLoop = 0; iLoop < 3; ++iLoop)
    {
        CryptMD5Init(&MD5Context);
        CryptMD5Update(&MD5Context, pSecure->PreMasterKey, sizeof(pSecure->PreMasterKey));
        CryptSha1Init(&SHA1Context);
        strBody[0] = strBody[1] = strBody[2] = 'A'+iLoop;
        CryptSha1Update(&SHA1Context, strBody, iLoop+1);
        CryptSha1Update(&SHA1Context, pSecure->PreMasterKey, sizeof(pSecure->PreMasterKey));
        CryptSha1Update(&SHA1Context, pSecure->ClientRandom, sizeof(pSecure->ClientRandom));
        CryptSha1Update(&SHA1Context, pSecure->ServerRandom, sizeof(pSecure->ServerRandom));
        CryptSha1Final(&SHA1Context, strBody, 20);
        CryptMD5Update(&MD5Context, strBody, 20);
        CryptMD5Final(&MD5Context, pSecure->MasterKey+iLoop*16, 16);
    }

    #if DEBUG_RAW_DATA
    NetPrintMem(pSecure->PreMasterKey, sizeof(pSecure->PreMasterKey), "PreMaster");
    NetPrintMem(pSecure->MasterKey, sizeof(pSecure->MasterKey), "Master");
    #endif
    memset(pSecure->PreMasterKey, 0, sizeof(pSecure->PreMasterKey));

    for (iLoop = 0; iLoop < (signed)(sizeof(pSecure->KeyBlock)/16); ++iLoop)
    {
        CryptMD5Init(&MD5Context);
        CryptMD5Update(&MD5Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
        CryptSha1Init(&SHA1Context);
        memset(strBody, 'A'+iLoop, iLoop+1);
        CryptSha1Update(&SHA1Context, strBody, iLoop+1);
        CryptSha1Update(&SHA1Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
        CryptSha1Update(&SHA1Context, pSecure->ServerRandom, sizeof(pSecure->ServerRandom));
        CryptSha1Update(&SHA1Context, pSecure->ClientRandom, sizeof(pSecure->ClientRandom));
        CryptSha1Final(&SHA1Context, strBody, 20);
        CryptMD5Update(&MD5Context, strBody, 20);
        CryptMD5Final(&MD5Context, pSecure->KeyBlock+16*iLoop, 16);

    }
    #if DEBUG_RAW_DATA
    NetPrintMem(pSecure->KeyBlock, sizeof(pSecure->KeyBlock), "Key Block");
    #endif

    pData = pSecure->KeyBlock;
    pSecure->pClientMAC = pData;
    pData += pSecure->pCipher->uMac;
    pSecure->pServerMAC = pData;
    pData += pSecure->pCipher->uMac;
    pSecure->pClientKey = pData;
    pData += 16;
    pSecure->pServerKey = pData;
    pData += 16;

    strHead[0] = SSL3_MSG_CLIENT_KEY;
    strHead[1] = 0;
    strHead[2] = (uint8_t)(pSecure->Cert.iKeyModSize>>8);
    strHead[3] = (uint8_t)(pSecure->Cert.iKeyModSize>>0);

    _SendPacket(pState, SSL3_REC_HANDSHAKE, strHead, 4, RSAContext.EncryptBlock, pSecure->Cert.iKeyModSize);
    return(ST3_SEND_CHANGE);
}

static int32_t _ProtoSSLUpdateSendChange(ProtoSSLRefT *pState)
{
    uint8_t strHead[4];
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Send Cipher Change\n"));
    #endif

    CryptArc4Init(&pSecure->WriteArc4, pSecure->pClientKey, 16, 1);

    strHead[0] = 1;
    _SendPacket(pState, SSL3_REC_CIPHER, strHead, 1, NULL, 0);

    pSecure->uSendSeqn = 0;
    return(ST3_SEND_FINISH);
}

static int32_t _ProtoSSLUpdateSendAlert(ProtoSSLRefT *pState, int32_t iLevel, int32_t iDescription)
{
    uint8_t strHead[4];

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Send Alert: level=%d desc=%d\n", iLevel, iDescription));
    #endif

    strHead[0] = iLevel;
    strHead[1] = iDescription;
    _SendPacket(pState, SSL3_REC_ALERT, strHead, 2, NULL, 0);

    return(pState->iState);
}

static int32_t _ProtoSSLUpdateSendClientFinish(ProtoSSLRefT *pState)
{
    uint8_t strHead[4];
    uint8_t strBody[256];
    CryptMD5T MD5Context;
    CryptSha1T SHAContext;
    uint8_t MacTemp[20];
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Send Client Finished\n"));
    #endif

    memcpy(&MD5Context, &pSecure->HandshakeMD5, sizeof(MD5Context));
    CryptMD5Update(&MD5Context, "CLNT", 4);
    CryptMD5Update(&MD5Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptMD5Update(&MD5Context, _SSL3_Pad1, 48);
    CryptMD5Final(&MD5Context, MacTemp, 16);

    CryptMD5Init(&MD5Context);
    CryptMD5Update(&MD5Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptMD5Update(&MD5Context, _SSL3_Pad2, 48);
    CryptMD5Update(&MD5Context, MacTemp, 16);
    CryptMD5Final(&MD5Context, strBody+0, 16);
    #if DEBUG_RAW_DATA
    NetPrintMem(strBody+0, 16, "HS finish MD5");
    #endif

    memcpy(&SHAContext, &pSecure->HandshakeSHA, sizeof(SHAContext));
    CryptSha1Update(&SHAContext, "CLNT", 4);
    CryptSha1Update(&SHAContext, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptSha1Update(&SHAContext, _SSL3_Pad1, 40);
    CryptSha1Final(&SHAContext, MacTemp, 20);

    CryptSha1Init(&SHAContext);
    CryptSha1Update(&SHAContext, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptSha1Update(&SHAContext, _SSL3_Pad2, 40);
    CryptSha1Update(&SHAContext, MacTemp, 20);
    CryptSha1Final(&SHAContext, strBody+16, 20);
    #if DEBUG_RAW_DATA
    NetPrintMem(strBody+16, 20, "HS finish SHA");
    #endif

    strHead[0] = SSL3_MSG_FINISHED;
    strHead[1] = 0;
    strHead[2] = 0;
    strHead[3] = 16+20;

    _SendPacket(pState, SSL3_REC_HANDSHAKE, strHead, 4, strBody, 16+20);
    return(ST3_RECV_CHANGE);
}

static int32_t _ProtoSSLUpdateRecvChange(ProtoSSLRefT *pState, const uint8_t *pData)
{
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Cipher Change\n"));
    #endif

    CryptArc4Init(&pSecure->ReadArc4, pSecure->pServerKey, 16, 1);

    pSecure->uRecvSeqn = 0;

    return(ST3_RECV_FINISH);
}

static int32_t _ProtoSSLUpdateRecvServerFinish(ProtoSSLRefT *pState, const uint8_t *pData)
{
    CryptMD5T MD5Context;
    CryptSha1T SHAContext;
    uint8_t MacTemp[20];
    uint8_t strMD5[16];
    uint8_t strSHA[20];
    SecureStateT *pSecure = pState->pSecure;

    #if DEBUG_MSG_LIST
    NetPrintf(("protossl: SSL Msg: Process Client Finish\n"));
    #endif

    memcpy(&MD5Context, &pSecure->HandshakeMD5, sizeof(MD5Context));
    CryptMD5Update(&MD5Context, "SRVR", 4);
    CryptMD5Update(&MD5Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptMD5Update(&MD5Context, _SSL3_Pad1, 48);
    CryptMD5Final(&MD5Context, MacTemp, 16);

    CryptMD5Init(&MD5Context);
    CryptMD5Update(&MD5Context, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptMD5Update(&MD5Context, _SSL3_Pad2, 48);
    CryptMD5Update(&MD5Context, MacTemp, 16);
    CryptMD5Final(&MD5Context, strMD5, 16);

    memcpy(&SHAContext, &pSecure->HandshakeSHA, sizeof(SHAContext));
    CryptSha1Update(&SHAContext, "SRVR", 4);
    CryptSha1Update(&SHAContext, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptSha1Update(&SHAContext, _SSL3_Pad1, 40);
    CryptSha1Final(&SHAContext, MacTemp, 20);

    CryptSha1Init(&SHAContext);
    CryptSha1Update(&SHAContext, pSecure->MasterKey, sizeof(pSecure->MasterKey));
    CryptSha1Update(&SHAContext, _SSL3_Pad2, 40);
    CryptSha1Update(&SHAContext, MacTemp, 20);
    CryptSha1Final(&SHAContext, strSHA, 20);

    if ((memcmp(strMD5, pData+0, sizeof(strMD5)) != 0) ||
        (memcmp(strSHA, pData+16, sizeof(strSHA)) != 0))
    {
        NetPrintMem(strMD5, sizeof(strMD5), "Server Final MD5 Calc");
        NetPrintMem(pData+0, 16, "Server Final MD5 Recv");
        NetPrintMem(strSHA, sizeof(strSHA), "Server Final SHA Calc");
        NetPrintMem(pData+16, 20, "Server Final SHA Recv");
        return(ST_FAIL_SETUP);
    }

    #if DEBUG_ENC_PERF
    {
        uint32_t uTick = NetTick();
        NetPrintf(("protossl: SSL Perf: setup %dms\n", uTick-pSecure->uTimer));
        pSecure->uTimer = uTick;
    }
    #endif
    return(ST3_SECURE);
}

void ProtoSSLUpdate(ProtoSSLRefT *pState)
{
    int32_t iXfer;
    int32_t iResult;
    const uint8_t *pData;
    SecureStateT *pSecure = pState->pSecure;

    if (pState->iState == ST_ADDR)
    {
        if (pState->pHost->Done(pState->pHost))
        {
            if (pState->pHost->addr == 0)
            {
                pState->iState = ST_FAIL_DNS;
            }
            else
            {
                pState->iState = ST_CONN;
                SockaddrInSetAddr(&pState->PeerAddr, pState->pHost->addr);
            }
            pState->pHost->Free(pState->pHost);
            pState->pHost = NULL;
        }
    }

    if (pState->iState == ST_CONN)
    {
        iResult = SocketConnect(pState->pSock, &pState->PeerAddr, sizeof pState->PeerAddr);

        if(iResult == SOCKERR_NONE)
        {
            pState->iState = ST_WAIT;
        }
        else
        {
            pState->iState = ST_FAIL_CONN;
            pState->iClosed = 1;
        }
    }

    if (pState->iState == ST_WAIT)
    {
        iResult = SocketInfo(pState->pSock, 'stat', 0, NULL, 0);
        if (iResult > 0)
        {
            pState->iState = (pSecure ? ST3_SEND_HELLO : ST_UNSECURE);
            pState->iClosed = 0;
        }
        if (iResult < 0)
        {
            pState->iState = ST_FAIL_CONN;
            pState->iClosed = 1;
        }
    }

    while ((pState->pSock != NULL) && (pState->iState >= ST3_SEND_HELLO) && (pState->iState <= ST3_SECURE))
    {
        iXfer = 0;

        if (pSecure->iSendProg == pSecure->iSendSize)
        {
            if (pState->iState == ST3_SEND_HELLO)
            {
                pState->iState = _ProtoSSLUpdateSendClientHello(pState);
            }
            else if (pState->iState == ST3_SEND_KEY)
            {
                pState->iState = _ProtoSSLUpdateSendClientKey(pState);
            }
            else if (pState->iState == ST3_SEND_CHANGE)
            {
                pState->iState = _ProtoSSLUpdateSendChange(pState);
            }
            else if (pState->iState == ST3_SEND_FINISH)
            {
                pState->iState = _ProtoSSLUpdateSendClientFinish(pState);
            }
        }

        if (pSecure->iSendProg < pSecure->iSendSize)
        {
            iResult = SocketSend(pState->pSock, pSecure->SendData+pSecure->iSendProg, pSecure->iSendSize-pSecure->iSendProg, 0);
            if (iResult > 0)
            {
                pSecure->iSendProg += iResult;
                iXfer = 1;
            }
            if (pSecure->iSendProg == pSecure->iSendSize)
            {
                pSecure->iSendProg = pSecure->iSendSize = 0;
            }
        }

        if (pSecure->iRecvSize < SSL_MIN_PACKET)
        {
            iResult = SocketRecv(pState->pSock, pSecure->RecvData+pSecure->iRecvSize, SSL_MIN_PACKET-pSecure->iRecvSize, 0);
            if (iResult > 0)
            {
                pSecure->iRecvSize += iResult;
                pSecure->iRecvProg = pSecure->iRecvSize;
            }
            if (iResult < 0)
            {
                pState->iClosed = 1;
            }
        }
        if (pSecure->iRecvSize < SSL_MIN_PACKET)
        {
            break;
        }

        if (pSecure->iRecvSize == SSL_MIN_PACKET)
        {
            if ((pSecure->RecvData[1] != (SSL3_VERSION>>8)) || (pSecure->RecvData[2] != (SSL3_VERSION&255)))
            {
                pSecure->RecvData[3] = 0;
                pSecure->RecvData[4] = 0;
            }
            pSecure->iRecvSize += (pSecure->RecvData[3]<<8)|(pSecure->RecvData[4]<<0);
            if (pSecure->iRecvSize > SSL_RCVMAX_PACKET)
            {
                NetPrintf(("protossl: received oversized packet (%d/%d); terminating connection\n", pSecure->iRecvSize, SSL_RCVMAX_PACKET));
                pState->iClosed = 1;
                pSecure->iRecvProg = pSecure->iRecvSize = pSecure->iRecvBase = pSecure->iRecvDone = 0;
                pState->iState = ST_FAIL_SECURE;
            }
        }

        if (pSecure->iRecvProg < pSecure->iRecvSize)
        {
            iResult = SocketRecv(pState->pSock, pSecure->RecvData+pSecure->iRecvProg, pSecure->iRecvSize-pSecure->iRecvProg, 0);
            if (iResult > 0)
            {
                pSecure->iRecvProg += iResult;
                iXfer = 1;

                if (pSecure->iRecvProg == pSecure->iRecvSize)
                {
                    iResult = _RecvPacket(pState);
                }
            }
            if (iResult < 0)
            {
                pState->iClosed = 1;
                if ((DEBUG_ENC_PERF) && (pState->pSecure != NULL) && (pState->pSecure->uTimer != 0))
                {
                    NetPrintf(("protossl: SSL Perf: closed %dms\n", NetTick()-pState->pSecure->uTimer));
                    pState->pSecure->uTimer = 0;
                }
            }
        }

        if ((pSecure->iRecvProg == pSecure->iRecvSize) && (pSecure->iRecvBase < pSecure->iRecvSize))
        {
            if (pSecure->RecvData[0] == SSL3_REC_ALERT)
            {
                if ((pSecure->RecvData[5] == SSL3_ALERT_LEVEL_WARNING)
                        && (pSecure->RecvData[6] == SSL3_ALERT_DESC_CLOSE_NOTIFY))
                {
                    pState->iClosed = 1;
                    pSecure->iRecvProg = pSecure->iRecvSize = pSecure->iRecvBase = pSecure->iRecvDone = 0;
                    if ((DEBUG_ENC_PERF) && (pState->pSecure != NULL) && (pState->pSecure->uTimer != 0))
                    {
                        NetPrintf(("protossl: SSL Perf: closed %dms\n", NetTick()-pState->pSecure->uTimer));
                        pState->pSecure->uTimer = 0;
                    }
                }
                else
                {
                    _DebugAlert(pState, pSecure->RecvData[5], pSecure->RecvData[6]);
                    pState->iState = ST_FAIL_SETUP;
                }
            }
            else if (pState->iState == ST3_RECV_HELLO)
            {
                if ((pData = _RecvHandshake(pState, SSL3_MSG_SERVER_HELLO)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateRecvServerHello(pState, pData);
                }
                else if ((pData = _RecvHandshake(pState, SSL3_MSG_CERTIFICATE)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateRecvServerCert(pState, pData);
                }
                else if ((pData = _RecvHandshake(pState, SSL3_MSG_SERVER_KEY)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateRecvServerKey(pState, pData);
                }
                else if ((pData = _RecvHandshake(pState, SSL3_MSG_CERT_REQ)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateSendAlert(
                            pState, SSL3_ALERT_LEVEL_WARNING, SSL3_ALERT_DESC_NO_CERTIFICATE);
                }
                else if ((pData = _RecvHandshake(pState, SSL3_MSG_SERVER_DONE)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateRecvServerDone(pState, pData);
                }
                else
                {
                }
            }
            else if (pState->iState == ST3_RECV_CHANGE)
            {
                if (pSecure->RecvData[0] == SSL3_REC_CIPHER)
                {
                    pState->iState = _ProtoSSLUpdateRecvChange(pState, pSecure->RecvData+pSecure->iRecvBase);
                    pSecure->iRecvBase = pSecure->iRecvSize;
                }
                else
                {
                }
            }
            else if (pState->iState == ST3_RECV_FINISH)
            {
                if ((pData = _RecvHandshake(pState, SSL3_MSG_FINISHED)) != NULL)
                {
                    pState->iState = _ProtoSSLUpdateRecvServerFinish(pState, pData);
                }
                else
                {
                }
            }

            if (pSecure->iRecvBase >= pSecure->iRecvSize)
            {
                pSecure->iRecvProg = pSecure->iRecvSize = pSecure->iRecvBase = pSecure->iRecvDone = 0;
            }
        }

        if (iXfer == 0)
        {
            break;
        }
    }
}

int32_t ProtoSSLSend(ProtoSSLRefT *pState, const char *pBuffer, int32_t iLength)
{
    int32_t iResult = -1;
    SecureStateT *pSecure = pState->pSecure;

    if (iLength < 0)
    {
        iLength = (int32_t)strlen(pBuffer);
    }

    if (pState->iState == ST3_SECURE)
    {
        iResult = 0;

        if (pSecure->iSendSize == 0)
        {
            if (iLength > SSL_SNDLIM_PACKET)
            {
                iLength = SSL_SNDLIM_PACKET;
            }

            _SendPacket(pState, SSL3_REC_APPLICATION, NULL, 0, pBuffer, iLength);
            iResult = iLength;

            ProtoSSLUpdate(pState);
        }
    }

    if (pState->iState == ST_UNSECURE)
    {
        iResult = SocketSend(pState->pSock, pBuffer, iLength, 0);
    }

    return(iResult);
}

int32_t ProtoSSLRecv(ProtoSSLRefT *pState, char *pBuffer, int32_t iLength)
{
    int32_t iResult = -1;
    SecureStateT *pSecure = pState->pSecure;

    if (pState->iState == ST3_SECURE)
    {
        iResult = 0;

        if ((pSecure->iRecvProg == 0) || (pSecure->iRecvProg != pSecure->iRecvSize))
        {
            ProtoSSLUpdate(pState);
        }
        
        if (((pSecure->iRecvSize < SSL_MIN_PACKET) || (pSecure->iRecvProg < pSecure->iRecvSize)) &&
            (pState->iClosed))
        {
            iResult = SOCKERR_CLOSED;
        }
        else if ((pSecure->iRecvProg == pSecure->iRecvSize) && (pSecure->iRecvBase < pSecure->iRecvSize) &&
            (pSecure->RecvData[0] == SSL3_REC_APPLICATION) && (pSecure->iRecvDone != 0))
        {
            iResult = pSecure->iRecvSize-pSecure->iRecvBase;
            if (iResult > iLength)
            {
                iResult = iLength;
            }
            memcpy(pBuffer, pSecure->RecvData+pSecure->iRecvBase, iResult);
            pSecure->iRecvBase += iResult;

            if ((pSecure->iRecvBase >= pSecure->iRecvSize) && (pSecure->iRecvDone != 0))
            {
                pSecure->iRecvProg = pSecure->iRecvSize = pSecure->iRecvBase = pSecure->iRecvDone = 0;
            }
        }
    }

    if (pState->iState == ST_UNSECURE)
    {
        iResult = SocketRecv(pState->pSock, pBuffer, iLength, 0);
    }

    if ((iResult > 0) && (iResult < iLength))
    {
        pBuffer[iResult] = 0;
    }

    return(iResult);
}

int32_t ProtoSSLStat(ProtoSSLRefT *pState, int32_t iSelect, void *pBuffer, int32_t iLength)
{
    int32_t iResult = -1;
    
    if (iSelect == 'sock')
    {
        if ((pBuffer == NULL) || (iLength != sizeof(pState->pSock)))
        {
            return(-1);
        }
        memcpy(pBuffer, &pState->pSock, sizeof(pState->pSock));
        return(0);
    }

    if (pState->pSock != NULL)
    {
        iResult = SocketInfo(pState->pSock, iSelect, 0, pBuffer, iLength);
        
        if (iSelect == 'stat')
        {
            if (pState->iState >= ST_FAIL)
            {
                iResult = -1;
            }
            if (pState->iState == ST_ADDR)
            {
                iResult = 0;
            }
            if ((iResult > 0) && (pState->iState != ST_UNSECURE) && (pState->iState != ST3_SECURE))
            {
                iResult = 0;
            }
        }
    }
    return(iResult);
}

int32_t ProtoSSLControl(ProtoSSLRefT *pState, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iSelect == 'ncrt')
    {
        pState->bAllowAnyCert = (uint8_t)iValue;
        return(0);
    }
    return(-1);
}

int32_t ProtoSSLSetCACert(const uint8_t *pCertData, int32_t iCertSize)
{
    int32_t iResult;
    int32_t iCount = -1;
    X509CertificateT Cert;
    uint8_t *pCertBuffer = NULL;
    const int32_t _iMaxCertSize = 4096;
    const uint8_t *pCertBeg, *pCertEnd;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    if (_FindPEMCertificateData(pCertData, iCertSize, &pCertBeg, &pCertEnd) == 0)
    {
        pCertBeg = pCertData;
        pCertEnd = pCertData+iCertSize;
    }

    iCertSize -= pCertEnd-pCertData;
    pCertData = pCertEnd;

    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    if ((iResult = LobbyBase64Decode(pCertEnd-pCertBeg, pCertBeg, NULL)) > 0)
    {
        if (iResult > _iMaxCertSize)
        {
            return(-111);
        }
        if ((pCertBuffer = (uint8_t *)DirtyMemAlloc(_iMaxCertSize, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
        {
            return(-112);
        }
        LobbyBase64Decode(pCertEnd-pCertBeg, pCertBeg, pCertBuffer);
        pCertBeg = pCertBuffer;
        pCertEnd = pCertBeg+iResult;
    }

    if ((iResult = _ParseCertificate(&Cert, pCertBeg, pCertEnd-pCertBeg)) == 0)
    {
        if ((iResult = _VerifyCertificate(&Cert, TRUE)) == 0)
        {
            _AddCertificate(&Cert, iMemGroup, pMemGroupUserData);
            iCount = 1;
        }
    }

    while ((iResult == 0) && (iCertSize > 0) && (_FindPEMCertificateData(pCertData, iCertSize, &pCertBeg, &pCertEnd) != 0))
    {
        iCertSize -=  pCertEnd-pCertData;
        pCertData = pCertEnd;

        if (((iResult = LobbyBase64Decode(pCertEnd-pCertBeg, pCertBeg, NULL)) <= 0) || (iResult > _iMaxCertSize))
        {
            break;
        }
        LobbyBase64Decode(pCertEnd-pCertBeg, pCertBeg, pCertBuffer);

        if ((iResult = _ParseCertificate(&Cert, pCertBuffer, iResult)) < 0)
        {
            continue;
        }

        if ((iResult = _VerifyCertificate(&Cert, TRUE)) < 0)
        {
            continue;
        }

        _AddCertificate(&Cert, iMemGroup, pMemGroupUserData);
        iCount += 1;
    }
    
    if (pCertBuffer != NULL)
    {
        DirtyMemFree(pCertBuffer, PROTOSSL_MEMID, iMemGroup, pMemGroupUserData);
    }

    return(iCount);
}

void ProtoSSLClrCACerts(void)
{
    ProtoSSLCACertT *pCACert, *pCACert0=NULL;
    
    for (pCACert = _ProtoSSL_CACerts; (pCACert != NULL) && (pCACert->iMemGroup == 0); )
    {
        pCACert0 = pCACert;
        pCACert = pCACert->pNext;
    }
    if ((pCACert != NULL) && (pCACert0 != NULL))
    {
        pCACert0->pNext = NULL;
    
        for ( ; pCACert != NULL; )
        {
            pCACert0 = pCACert->pNext;
            DirtyMemFree(pCACert, PROTOSSL_MEMID, pCACert->iMemGroup, pCACert->pMemGroupUserData);
            pCACert = pCACert0;
        }
    }
}
