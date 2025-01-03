/*H********************************************************************************/
/*!
    \File netconnps4.h

    \Description
        Definitions for the netconnps4 module.

    \Copyright
        Copyright (c) 2013 Electronic Arts

    \Version 08/06/2013 (tcho) First Version
*/
/********************************************************************************H*/

#ifndef _netconnps4_h
#define _netconnps4_h

/*!
\Moduledef NetConnPS4 NetConnPS4
\Modulemember DirtySock
*/
//@{

/*** Include files ****************************************************************/

#include <np/np_common.h>

#include "DirtySDK/platform.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

typedef struct NetConnNpUserStatusT
{
    SceUserServiceUserId    UserServiceId;      //!< user's userservice id
#ifndef DIRTYCODE_PS5
    SceNpId                 NpId;               //!< user's np id
#else
    uint32_t                NpId;
#endif
    SceNpState              NpState;            //!< user's np state
} NetConnNpUserStatusT;

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

//@}

#endif // _netconnps4_h
