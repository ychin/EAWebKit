/*H*************************************************************************************/
/*!
    \File dirtyeventmanagerps4.h

    \Description
        DirtyEventManagerps4 wraps the EASystemEventMessageDispatcher functionality
        for delegating callbacks from Sony's UserService

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 1.0 07/11/2013 (amakoukji)  First Version
*/
/*************************************************************************************H*/

#ifndef _dirtyeventmanagerps4_h
#define _dirtyeventmanagerps4_h

/*!
\Moduledef DirtyEventManagerPS4 DirtyEventManagerPS4
\Modulemember DirtySock
*/
//@{

/*** Include files *********************************************************************/

#include <user_service.h>
#include <system_service.h>

#include "DirtySDK/platform.h"

#ifndef DIRTYCODE_PS5
#include <companion_httpd/companion_httpd_types.h>
#endif

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef void (DirtyEventManagerUserServiceCallbackT)(void *pUserData, const SceUserServiceEvent *pEvent);
typedef void (DirtyEventManagerSystemServiceCallbackT)(void *pUserData, const SceSystemServiceEvent *pEvent);
#ifndef DIRTYCODE_PS5
typedef void (DirtyEventManagerCompanionHttpdCallbackT)(void *pUserData, const SceCompanionHttpdEvent *pEvent);
typedef void (DirtyEventManagerCompanionUtilCallbackT)(void *pUserData, const SceCompanionUtilEvent *pEvent);
#else
typedef void (DirtyEventManagerCompanionHttpdCallbackT)(void *pUserData, const void *pEvent);
typedef void (DirtyEventManagerCompanionUtilCallbackT)(void *pUserData, const void *pEvent);
#endif

/*** Functions *************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

// initialize the module
int32_t DirtyEventManagerInit(uint8_t bCheckTick);

// shutdown the module
int32_t DirtyEventManagerShutdown(void);

// register user service callback
int32_t DirtyEventManagerRegisterUserService(DirtyEventManagerUserServiceCallbackT *pCallback, void *pUserData);

// register system service callback
int32_t DirtyEventManagerRegisterSystemService(DirtyEventManagerSystemServiceCallbackT *pCallback, void *pUserData);

// register companion httpd callback
int32_t DirtyEventManagerRegisterCompanionHttpd(DirtyEventManagerCompanionHttpdCallbackT *pCallback, void *pUserData);

// register companion util callback
int32_t DirtyEventManagerRegisterCompanionUtil(DirtyEventManagerCompanionUtilCallbackT *pCallback, void *pUserData);

// unregister user service callback
int32_t DirtyEventManagerUnregisterUserService(DirtyEventManagerUserServiceCallbackT *pCallback);

// unregister system service callback
int32_t DirtyEventManagerUnregisterSystemService(DirtyEventManagerSystemServiceCallbackT *pCallback);

// unregister companion httpd callback
int32_t DirtyEventManagerUnregisterCompanionHttpd(DirtyEventManagerCompanionHttpdCallbackT *pCallback);

// unregister companion util callback
int32_t DirtyEventManagerUnregisterCompanionUtil(DirtyEventManagerCompanionUtilCallbackT *pCallback);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtyeventmanagerps4_h

