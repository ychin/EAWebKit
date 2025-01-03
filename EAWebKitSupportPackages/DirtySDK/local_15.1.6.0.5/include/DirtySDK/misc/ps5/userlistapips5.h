/*H********************************************************************************/
/*!
    \File    userlistapips5.h

    \Description
        This file is the extension for ps4 specific features for the UserList API

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 01/22/2020 (tcho)
*/
/********************************************************************************H*/

#ifndef _userlistapips5_h
#define _userlistapips5_h

/*!
\Moduledef UserListApiPs5 UserListApiPs5
\Modulemember User
*/
//@{

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"
#include "DirtySDK/misc/userlistapi.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! Friend Level Types
typedef enum UserListApiLevelPS5E
{
    USERLISTAPI_LEVEL_FRIEND                    //!< regular friend
} UserListApiLevelPS5E;

//! Friend Level Types
typedef enum UserListApiAvatarSizeE
{
    USERLISTAPI_AVATAR_NONE,
    USERLISTAPI_AVATAR_SMALL,
    USERLISTAPI_AVATAR_MEDIUM,
    USERLISTAPI_AVATAR_LARGE,
    USERLISTAPI_AVATAR_EXTRA_LARGE
} UserListApiAvatarSizeE;

//! Friend Level Types
typedef enum UserListApiStatusE
{
    USERLISTAPI_STATUS_ALL,    //!< all friends
    USERLISTAPI_STATUS_ONLINE  //!< friends currently online
} UserListApiStatusE;

//! Order By
typedef enum UserListApiOrderByE
{
    USERLISTAPI_ORDERBY_NONE,
    USERLISTAPI_ORDERBY_ID,
    USERLISTAPI_ORDERBY_STATUS,
    USERLISTAPI_ORDERBY_ID_AND_STATUS
} UserListApiOrderByE;


struct UserListApiFiltersT
{
    DirtyUserT             *pFriend;      // target user for friend of friend lookup
    UserListApiLevelPS5E    eLevel;       // Only 'friend' currently exists
    UserListApiAvatarSizeE  eAvatarSize;  // S, M, L
    UserListApiStatusE      eStatus;      // 'all', 'online'
    UserListApiOrderByE     eOrder;       // 'none', 'id', 'status' or 'both'
};

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

//@}

#endif // _userlistapips4_h
