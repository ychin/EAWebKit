/*H********************************************************************************/
/*!

    \File    dirtyuserxboxone.c

    \Description
        Platform-specific user functions.

    \Copyright
        Copyright (c) Electronic Arts 2013.    ALL RIGHTS RESERVED.

    \Version 05/02/13 (amakoukji) First Version from dirtyuserps4
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyuser.h"
#include "DirtySDK/util/binary7.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function DirtyUserToNativeUser

    \Description
        Convert a DirtyUserT to native format.

    \Input *pOutput     - [out] storage for native format user
    \Input iBufLen      - length of output buffer
    \Input *pUser       - source user to convert

    \Output
        uint32_t        - TRUE if successful, else FALSE

    \Notes
        On Xbox One, a DirtyUserT is a string-encoded XUID, which 
        is a 64-bit unsigned integer.
        This function converts that input into an XUID.

    \Version 04/25/2013 (mclouatre)
*/
/********************************************************************************F*/
uint32_t DirtyUserToNativeUser(void *pOutput, int32_t iBufLen, const DirtyUserT *pUser)
{
    // make sure output buffer is big enough
    if (iBufLen < (signed)sizeof(uint64_t))
    {
        return(FALSE);
    }
    // make sure the encoding is correct
    if (*pUser->strNativeUser != '^')
    {
        return(FALSE);
    }

    // decode contents of pUser
    Binary7Decode((uint8_t *)pOutput, iBufLen, (const uint8_t *)pUser->strNativeUser+1);

    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyUserFromNativeUser

    \Description
        Convert native format user data to a DirtyUserT.

    \Input *pUser       - [out] storage for output DirtyUserT
    \Input *pInput      - pointer to native format user

    \Output
        uint32_t        - TRUE if successful, else FALSE

    \Notes
        On Xbox One, a DirtyUserT is a string-encoded XUID, which 
        is a 64-bit unsigned integer.
        This function converts an XUID into a DirtyUsertT.

    \Version 04/25/2013 (mclouatre)
*/
/********************************************************************************F*/
uint32_t DirtyUserFromNativeUser(DirtyUserT *pUser, const void *pInput)
{
    uint64_t *pXuId = (uint64_t *)pInput;

    // clear output data
    ds_memclr(pUser, sizeof(*pUser));

    // encode input xuid into pUser
    pUser->strNativeUser[0] = '^';
    Binary7Encode((uint8_t *)pUser->strNativeUser+1, sizeof(*pUser)-1, (uint8_t *)pXuId, sizeof(*pXuId), TRUE);

    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function DirtyUserGetLocalUser

    \Description
        Convert a DirtyUserT to native format.

    \Input *pUser       - [out] storage for output DirtyUserT
    \Input *iUserIndex  - local user index

    \Output
        uint32_t        - TRUE if successful, else FALSE

    \Version 04/25/2013 (mclouatre)
*/
/********************************************************************************F*/
uint32_t DirtyUserGetLocalUser(DirtyUserT *pUser, int32_t iUserIndex)
{
    // to be completed
    return(FALSE);
}

/*F********************************************************************************/
/*!
    \Function DirtyUserCompare

    \Description
        Compare two DirtyUserT users for equality.

    \Input *pUser1 - user 1
    \Input *pUser2 - user 2

    \Output
        int32_t    - TRUE if successful, else FALSE

    \Version 06/19/2013 (amakoukji)
*/
/********************************************************************************F*/
int32_t DirtyUserCompare(DirtyUserT *pUser1, DirtyUserT *pUser2)
{
    int32_t iResult = strcmp((pUser1)->strNativeUser, (pUser2)->strNativeUser);

    if (iResult == 0)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}


