/*H********************************************************************************/
/*!
    \File    userlistapips4.h

    \Description
        This file is the extension for ps4 specific features for the UserList API

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 04/16/13 (amakoukji)
*/
/********************************************************************************H*/

#ifndef _userlistapips4_h
#define _userlistapips4_h

/*!
\Moduledef UserListApiPs4 UserListApiPs4
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
typedef enum UserListApiLevelPS4E
{
    USERLISTAPI_LEVEL_FRIEND                    //!< regular friend
} UserListApiLevelPS4E;

//! Friend Level Types
typedef enum UserListApiAvatarSizeE
{
    USERLISTAPI_AVATAR_NONE,
    USERLISTAPI_AVATAR_SMALL,
    USERLISTAPI_AVATAR_MEDIUM,
    USERLISTAPI_AVATAR_LARGE
} UserListApiAvatarSizeE;

//! Friend Level Types
typedef enum UserListApiContextE
{
    USERLISTAPI_CONTEXT_ALL,                    //!< all friends
    USERLISTAPI_CONTEXT_ONLINE,                 //!< friends currently online
    USERLISTAPI_CONTEXT_INCONTEXT               //!< friends in the same game
} UserListApiContextE;

//! Friend Level Types
typedef enum UserListApiOrderByE
{
    USERLISTAPI_ORDERBY_NONE,
    USERLISTAPI_ORDERBY_ID,
    USERLISTAPI_ORDERBY_STATUS,
    USERLISTAPI_ORDERBY_ID_AND_STATUS
} UserListApiOrderByE;

//! Friend Level Types
typedef enum UserListApiPresenceE
{
    USERLISTAPI_PRESENCE_NONE,
    USERLISTAPI_PRESENCE_PRIMARY,
    USERLISTAPI_PRESENCE_PLATFORM,
    USERLISTAPI_PRESENCE_INCONTEXT
} UserListApiPresenceE;

typedef struct UserListApiFieldsT
{
    uint8_t bUser;
    uint8_t bRegion;
    uint8_t bNpId;
    uint8_t bAvatarUrl;
    uint8_t bPersonalDetail;
} UserListApiFieldsT;

typedef enum UserListApiDirectionE
{
    USERLISTAPI_DIRECTION_ASCENDING,
    USERLISTAPI_DIRECTION_DESCENDING
} UserListApiDirectionE;

struct UserListApiFiltersT 
{
    DirtyUserT             *pFriend;      // target user for friend of friend lookup
    UserListApiLevelPS4E    eLevel;       // Only 'friend' currently exists
    UserListApiAvatarSizeE  eAvatarSize;  // S, M, L
    UserListApiContextE     eContext;     // 'all', 'online' or 'incontext'
    UserListApiOrderByE     eOrder;       // 'none', 'id', 'status' or 'both'
    UserListApiDirectionE   eDirection;   //  order direction 'ascending', 'descending'
    UserListApiPresenceE    ePresence;    // 'none', 'Primary', 'Platform', 'incontext'
    UserListApiFieldsT      Fields;       // a list of fields to include in the result  
};

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

//@}

#endif // _userlistapips4_h
