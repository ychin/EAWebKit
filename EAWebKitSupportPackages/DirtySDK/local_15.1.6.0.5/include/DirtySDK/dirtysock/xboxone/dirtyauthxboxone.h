/*H********************************************************************************/
/*!
    \File dirtyauthxboxone.h

    \Description
        Xbox One Auth wrapper module

    \Copyright
        Copyright 2012 Electronic Arts

    \Version 09/04/2013 (amakoukji) First Version ported from Xenon
*/
/********************************************************************************H*/

#ifndef _dirtyauthxboxone_h
#define _dirtyauthxboxone_h

/*!
\Moduledef DirtyAuthXboxOne DirtyAuthXboxOne
\Modulemember DirtySock
*/
//@{

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

/*** Defines **********************************************************************/
// token validity period
#define DIRTYAUTH_DEFAULT_TOKEN_TIMEOUT (60 * 1000)   // 1 minute

#define DIRTYAUTH_SUCCESS       (0)     //!< success result
#define DIRTYAUTH_AVAILABLE     (0)     //!< alternative name for SUCCESS
#define DIRTYAUTH_PENDING       (-1)    //!< operation pending
#define DIRTYAUTH_ERROR         (-2)    //!< error
#define DIRTYAUTH_MEMORY        (-3)    //!< memory error
#define DIRTYAUTH_NOTSTARTED    (-4)    //!< not started
#define DIRTYAUTH_NOTFOUND      (-5)    //!< not found
#define DIRTYAUTH_AUTHERR       (-6)    //!< xauth error code
#define DIRTYAUTH_EXPIRED       (-7)    //!< token expired

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! module state ref
typedef struct DirtyAuthRefT DirtyAuthRefT;

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// create the module
DIRTYCODE_API DirtyAuthRefT *DirtyAuthCreate(uint8_t bSecureBypass);

// destroy the module
DIRTYCODE_API void DirtyAuthDestroy(DirtyAuthRefT *pDirtyAuth);

// get a token
DIRTYCODE_API int32_t DirtyAuthGetToken(int32_t iUserIndex, const char *pHost, uint8_t bForceNew);

// get a token with a given expiration time
DIRTYCODE_API int32_t DirtyAuthGetToken2(int32_t iUserIndex, const char *pHost, uint8_t bForceNew, uint32_t uTimeout);

// check for token availability
DIRTYCODE_API int32_t DirtyAuthCheckToken(int32_t iUserIndex, const char *pHost,int32_t *pTokenLen, char *pToken);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtyauthxboxone_h
