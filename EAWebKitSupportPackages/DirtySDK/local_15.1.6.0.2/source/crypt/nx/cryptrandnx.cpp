/*H********************************************************************************/
/*!
    \File cryptrandnx.cpp

    \Description
        Cryptographic random number generator for NX.

    \Copyright
        Copyright (c) 2019 Electronic Arts Inc.

    \Version 02/14/2019 (tcho)
*/
/********************************************************************************H*/
/*** Include files ****************************************************************/

#include "cryptrandpriv.h"
#include <nn/crypto.h>

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/crypt/cryptrand.h"
#include "cryptrandcommon.h"

/*** Type Definitions *************************************************************/

//! internal rand state
typedef struct CryptRandRefT
{
    CryptRandCommonT Common;    //!< common state
    int32_t iMemGroup;          //!< memgroup id
    void *pMemGroupUserData;    //!< user data associated with memgroup
} CryptRandRefT;

/*** Variables ********************************************************************/
//! global rand state
static CryptRandRefT *_CryptRand_pState = NULL;

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _CryptRandGetEntropy

    \Description
        Get random data from the entropy pool

    \Input *pBuffer - [out] random data
    \Input iBufSize - size of random data

    \Output
        int32_t     - always zero as GenerateCryptographicallyRandomBytes does not return errors

    \Version 02/14/2019 (tcho)
*/
/********************************************************************************F*/
static int32_t _CryptRandGetEntropy(uint8_t *pBuffer, int32_t iBufSize)
{
    nn::crypto::GenerateCryptographicallyRandomBytes(pBuffer, iBufSize);
    return(0);
}

/*** Public Functions *************************************************************/
/*F********************************************************************************/
/*!
    \Function CryptRandInit

    \Description
        Initialize CryptRand module

    \Version 12/05/2012 (jbrookes)
*/
/********************************************************************************F*/
int32_t CryptRandInit(void)
{
    CryptRandRefT *pState;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // query memgroup info
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate state memory
    if ((pState = (CryptRandRefT *)DirtyMemAlloc(sizeof(*pState), CRYPTRAND_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("cryptrandnx: failed to allocate state\n"));
        return(-1);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->Common.GetEntropy = &_CryptRandGetEntropy;
    _CryptRand_pState = pState;

    // initialize common state
    if (CryptRandCommonInit(&pState->Common) != 0)
    {
        CryptRandShutdown();
        return(-1);
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function CryptRandShutdown

    \Description
        Shut down the CryptRand module

    \Version 12/05/2012 (jbrookes)
*/
/********************************************************************************F*/
void CryptRandShutdown(void)
{
    CryptRandRefT *pState = _CryptRand_pState;

    // free memory and reset state pointer
    DirtyMemFree(pState, CRYPTRAND_MEMID, pState->iMemGroup, pState->pMemGroupUserData);
    _CryptRand_pState = NULL;
}

/*F********************************************************************************/
/*!
    \Function CryptRandGet

    \Description
        Get random data

    \Input *pBuffer - [out] random data
    \Input iBufSize - size of random data

    \Version 12/05/2012 (jbrookes)
*/
/********************************************************************************F*/
void CryptRandGet(uint8_t *pBuffer, int32_t iBufSize)
{
    CryptRandCommonGet(&_CryptRand_pState->Common, pBuffer, iBufSize);
}
