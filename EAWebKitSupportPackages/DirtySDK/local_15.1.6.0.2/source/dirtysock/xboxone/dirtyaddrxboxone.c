/*H********************************************************************************/
/*!
    \File dirtyaddrxboxone.c

    \Description
        Platform-specific address functions.

    \Copyright
        Copyright (c) Electronic Arts 2013. ALL RIGHTS RESERVED.

    \Version 04/17/2013 (mclouatre)
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/dirtyaddr.h"
#include "DirtySDK/util/binary7.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

static const uint8_t _DirtyAddr_aHexMap[16] = "0123456789abcdef";

// hex decoding tables (128=invalid character)
static const uint8_t _DirtyAddr_aHexDecode[256] =
{
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,128,128,128,128,128,128,
    128, 10, 11, 12, 13, 14, 15,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128, 10, 11, 12, 13, 14, 15,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128
};

/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _ishex

    \Description
        Determine if the given character is a hex digit.

    \Input c    - character to check

    \Output
        int32_t     - TRUE if c is a hex digit, else FALSE

    \Version 05/13/2005 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ishex(int32_t c)
{
    if (((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) || ((c >= '0') && (c <= '9')))
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/*F********************************************************************************/
/*!
    \Function _DirtyAddrSetBinary

    \Description
        Wrapper to use tagfield to set binary or binary7 data

    \Input *pBuf        - [out] output buffer
    \Input iBufSize     - size of output buffer
    \Input *pInput      - input data to encode
    \Input uInputSize   - size of input data
    \Input bBinary7     - true=use binary7 encoding

    \Output
        int32_t        - length of encoded string data

    \Version 09/01/2006 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyAddrSetBinary(char *pBuf, int32_t iBufSize, const uint8_t *pInput, uint32_t uInputSize, uint32_t bBinary7)
{
    const char *pBegin = pBuf;

    if (bBinary7)
    {
        // mark as binary7
        *pBuf++ = '^';

        // add the binary7 data
        Binary7Encode((uint8_t *)pBuf, iBufSize, pInput, uInputSize, TRUE);

    }
    else
    {
        // mark as binary
        *pBuf++ = '$';

        // add the binary data
        for (; uInputSize > 0; uInputSize -= 1, pInput += 1)
        {
            *pBuf++ = _DirtyAddr_aHexMap[*pInput >> 4];
            *pBuf++ = _DirtyAddr_aHexMap[*pInput & 15];
        }
        *pBuf++ = '\0';
    }

    return((signed)strlen(pBegin));
}

/*F********************************************************************************/
/*!
    \Function _DirtyAddrGetBinary

    \Description
        Wrapper to decode the binary data using normal binary or binary7

    \Input *pBuf        - [out] output buffer
    \Input iBufSize     - size of output buffer
    \Input *pInput      - input data to encode

    \Version 01/09/2017 (eesponda)
*/
/********************************************************************************F*/
static void _DirtyAddrGetBinary(uint8_t *pBuf, int32_t iBufSize, const uint8_t *pInput)
{
    if (*pInput++ == '$')
    {
        int32_t iIndex;

        for (iIndex = 0; (pInput[0] >= '0') && (pInput[1] >= '0') && (iIndex < iBufSize); pInput += 2, iIndex += 1)
        {
            pBuf[iIndex] = (_DirtyAddr_aHexDecode[pInput[0]] << 4) | (_DirtyAddr_aHexDecode[pInput[1]]);
        }
    }
    else
    {
        Binary7Decode((uint8_t *)pBuf, iBufSize, pInput);
    }
}

/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function DirtyAddrToHostAddr

    \Description
        Convert from DirtyAddrT to native format.

    \Input *pOutput     - [out] storage for native format address
    \Input iBufLen      - length of output buffer
    \Input *pAddr       - source address to convert

    \Output
        uint32_t    - TRUE if successful, else FALSE

    \Notes
        On Xbox One, a DirtyAddrT is a string-encoded network-order XUID.qwUserID
        field.  This function converts that input to a binary-encoded host-order
        XUID.qwUserID.

    \Version 04/09/2004 (jbrookes)
*/
/********************************************************************************F*/
uint32_t DirtyAddrToHostAddr(void *pOutput, int32_t iBufLen, const DirtyAddrT *pAddr)
{
    const char *pAddrText;
    ULONGLONG qwUserID;
    int32_t iChar;
    unsigned char cNum;

    // make sure our buffer is big enough
    if (iBufLen < sizeof(qwUserID))
    {
        NetPrintf(("dirtyaddr: output buffer too small\n"));
        return(FALSE);
    }

    // convert from string to qword
    for (iChar = 0, qwUserID = 0, pAddrText = &pAddr->strMachineAddr[1]; _ishex(pAddrText[iChar]); iChar++)
    {
        // make room for next entry
        qwUserID <<= 4;

        // add in character
        cNum = pAddrText[iChar];
        cNum -= ((cNum >= 0x30) && (cNum < 0x3a)) ? '0' : 0x57;
        qwUserID |= (ULONGLONG)cNum;
    }

    // copy to output
    ds_memcpy(pOutput, &qwUserID, sizeof(qwUserID));
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyAddrFromHostAddr

    \Description
        Convert from native format to DirtyAddrT.

    \Input *pAddr       - [out] storage for output DirtyAddrT
    \Input *pInput      - pointer to native format address

    \Output
        uint32_t    - TRUE if successful, else FALSE

    \Notes
        On Xbox One, a DirtyAddrT is a string-encoded network-order XUID.qwUserID
        field.  This function converts that abinary-encoded host-order
        XUID.qwUserID to a DirtyAddrT.

    \Version 04/09/2004 (jbrookes)
*/
/********************************************************************************F*/
uint32_t DirtyAddrFromHostAddr(DirtyAddrT *pAddr, const void *pInput)
{
    ULONGLONG qwUserID;
    int32_t iChar;

    // make sure output buffer is okay
    if ((pInput == NULL) || (((uint64_t)pInput & 0x3) != 0))
    {
        return(FALSE);
    }

    // get userID
    qwUserID = *(ULONGLONG *)pInput;

    // set up address
    ds_memclr(pAddr, sizeof(*pAddr));
    for (iChar = (sizeof(qwUserID) * 2); iChar >= 1; iChar--)
    {
        pAddr->strMachineAddr[iChar] = _DirtyAddr_aHexMap[qwUserID & 0xf];
        qwUserID >>= 4;
    }
    pAddr->strMachineAddr[0] = '$';
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyAddrGetLocalAddr

    \Description
        Get the local address in DirtyAddr form.

    \Input *pAddr       - [out] storage for output DirtyAddrT

    \Output
        uint32_t    - TRUE if successful, else FALSE

    \Version 09/29/2004 (jbrookes)
*/
/********************************************************************************F*/
uint32_t DirtyAddrGetLocalAddr(DirtyAddrT *pAddr)
{
    ULONGLONG Xuid;
    if (NetConnStatus('xuid', 0, &Xuid, sizeof(Xuid)) < 0)
    {
        return(FALSE);
    }
    DirtyAddrFromHostAddr(pAddr, &Xuid);
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyAddrGetInfoXboxOne

    \Description
        Extract Xbox One-specific fields from the given DirtyAddr

    \Input *pDirtyAddr                  - dirtyaddr to decode
    \Input *pXuid                       - [out] storage for decoded XUID
    \Input *pSecureDeviceAddressBlob    - [out] storage for decode SecureDeviceAddress blob
    \Input *pBlobSize                   - [out] size of decoded SecureDeviceAddress blob (in bytes)

    \Output
        uint8_t                         - TRUE=success, FALSE=failure

    \Notes
        Any undesired outputs may be specified as NULL

    \Version 04/17/2013 (mclouatre)
*/
/********************************************************************************F*/
uint8_t DirtyAddrGetInfoXboxOne(const DirtyAddrT *pDirtyAddr, void *pXuid, void *pSecureDeviceAddressBlob, int32_t *pBlobSize)
{
    const char *pAddr = pDirtyAddr->strMachineAddr;
    ULONGLONG *_pXuid = (ULONGLONG *)pXuid, TempXuid;
    uint8_t *_pSecureDeviceAddressBlob = (uint8_t *)pSecureDeviceAddressBlob;
    uint8_t aTempBlob[512];
    int32_t *_pBlobSize = (int32_t *)pBlobSize, iBlobSize;
    int32_t iIndex;

    // allow NULL inputs
    if (_pXuid == NULL) _pXuid = &TempXuid;
    if (_pSecureDeviceAddressBlob == NULL) _pSecureDeviceAddressBlob = aTempBlob;
    if (_pBlobSize == NULL) _pBlobSize = &iBlobSize;

    // validate that we can extract the data for the address
    if (strlen(pAddr) == 0)
    {
        return(FALSE);
    }

    // extract XUID
    DirtyAddrToHostAddr(_pXuid, sizeof(*_pXuid), pDirtyAddr);
    iIndex = 17;

    // extract SecureDeviceAddress blob size
    _DirtyAddrGetBinary((uint8_t *)_pBlobSize, sizeof(*_pBlobSize), (const uint8_t *)pAddr+iIndex);
    iIndex += 9;

    // extract SecureDeviceAddress blob
    _DirtyAddrGetBinary(_pSecureDeviceAddressBlob, *_pBlobSize, (const uint8_t *)pAddr+iIndex);
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyAddrSetInfoXboxOne

    \Description
        Set Xbox One-specific fields into the given DirtyAddr

    \Input *pDirtyAddr               - [out] storage for dirtyaddr
    \Input *pXuid                    - XUID to encode
    \Input *pSecureDeviceAddressBlob - SecureDeviceAddress blob to encode
    \Input iBlobSize                 - size of Secure device address code (in bytes)

    \Version 04/17/2013 (mclouatre)
*/
/********************************************************************************F*/
void DirtyAddrSetInfoXboxOne(DirtyAddrT *pDirtyAddr, const void *pXuid, const void *pSecureDeviceAddressBlob, int32_t iBlobSize)
{
    char *pAddr = pDirtyAddr->strMachineAddr;
    ULONGLONG *_pXuid = (ULONGLONG *)pXuid;
    int32_t iIndex;

    // clear output data
    ds_memclr(pDirtyAddr, sizeof(*pDirtyAddr));

    // set XUID
    DirtyAddrFromHostAddr(pDirtyAddr, _pXuid);

    iIndex = (signed)strlen(pAddr);

    // append SecureDeviceAddres blob size
    iIndex += _DirtyAddrSetBinary(pAddr+iIndex, sizeof(*pDirtyAddr)-iIndex, (const uint8_t *)&iBlobSize, sizeof(iBlobSize), FALSE);

    // append SecureDeviceAddres blob
    iIndex += _DirtyAddrSetBinary(pAddr+iIndex, sizeof(*pDirtyAddr)-iIndex, (const uint8_t *)pSecureDeviceAddressBlob, iBlobSize, TRUE);
}

