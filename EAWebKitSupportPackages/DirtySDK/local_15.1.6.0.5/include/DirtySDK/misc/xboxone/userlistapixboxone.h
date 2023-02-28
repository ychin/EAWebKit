/*H********************************************************************************/
/*!
    \File    userlistapixboxone.h

    \Description
        This file is the extension for Xbox One specific features for the user list api

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 04/16/13 (amakoukji)
*/
/********************************************************************************H*/

#ifndef _userlistapixboxone_h
#define _userlistapixboxone_h

/*!
\Moduledef UserListApiXboxOne UserListApiXboxOne
\Modulemember User
*/
//@{

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! Friend Level Types
typedef enum UserListApiLevelXboxOneE
{
    USERLISTAPI_LEVEL_FRIEND,                   //!< regular friend
    USERLISTAPI_LEVEL_FAVORITE,                 //!< favorite friend
} UserListApiLevelXboxOneE;

typedef struct UserListApiFiltersT
{
    UserListApiLevelXboxOneE eLevel;          //!< friend level
} UserListApiFiltersT;

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

//@}

#endif // _userlistapixboxone_h