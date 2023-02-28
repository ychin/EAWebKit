/*H*************************************************************************************/
/*!
    \File dirtyeventmanagerps4.cpp

    \Description
        DirtyEventManagerps4 wraps the EASystemEventMessageDispatcher functionality
        for delegating callbacks from Sony's UserService

    \Copyright
        Copyright (c) / Electronic Arts 2013.  ALL RIGHTS RESERVED.

    \Version 1.0 07/11/2013 (amakoukji)  First Version
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/
#include "EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h"
#include "DirtySDK/dirtysock/ps4/dirtyeventmanagerps4.h"
#include "DirtySDK/dirtysock/dirtylib.h"

/*** Defines ***************************************************************************/

//! max number of supported delegates
#define DIRTYEVENTMANAGER_MAX_DELEGATES (100)

/*** Type Definitions ******************************************************************/

// DirtyEventHandler defines how to deal with the callback from EATech's SystemEventMessageDispatcher
// in our case we will simply be pushing the event forward to our own handler
struct DirtyEventHandler : EA::Messaging::IHandler
{
    virtual bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage);
};

/*** Variables *************************************************************************/

static EA::SEMD::SystemEventMessageDispatcher *_pSystemEventMessageDispatcher = NULL;                       //!< module state pointer
static DirtyEventHandler _Handler;

static DirtyEventManagerUserServiceCallbackT    *_aHandlerUserService[DIRTYEVENTMANAGER_MAX_DELEGATES];     //<! list of callbacks
static DirtyEventManagerSystemServiceCallbackT  *_aHandlerSystemService[DIRTYEVENTMANAGER_MAX_DELEGATES];   //<! list of callbacks
static DirtyEventManagerCompanionHttpdCallbackT *_aHandlerCompanionHttpd[DIRTYEVENTMANAGER_MAX_DELEGATES];  //<! list of callbacks
static DirtyEventManagerCompanionUtilCallbackT  *_aHandlerCompanionUtil[DIRTYEVENTMANAGER_MAX_DELEGATES];   //<! list of callbacks

static void *_aUserDataUserService[DIRTYEVENTMANAGER_MAX_DELEGATES];      //<! list of user objects
static void *_aUserDataSystemService[DIRTYEVENTMANAGER_MAX_DELEGATES];    //<! list of user objects
static void *_aUserDataCompanionHttpd[DIRTYEVENTMANAGER_MAX_DELEGATES];   //<! list of user objects
static void *_aUserDataCompanionUtil[DIRTYEVENTMANAGER_MAX_DELEGATES];    //<! list of user objects

/*** Private Functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function HandleMessage

    \Description
        Handles a message from the SEMD system

    \Input messageId    - type of message
    \Input *pMessage    - message specific data

    \Output
        bool            - true if handled, false otherwise

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
bool DirtyEventHandler::HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
{
    EA::SEMD::Message *pSEMDMessage = static_cast<EA::SEMD::Message *>(pMessage);
    int32_t iDelegate;

    if (messageId == EA::SEMD::kGroupUserService)
    {
        const SceUserServiceEvent *pUserServiceEvent = pSEMDMessage->GetData<const SceUserServiceEvent*>();

        // forward to our own handler
        for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
        {
            DirtyEventManagerUserServiceCallbackT *pCallback;
            if ((pCallback = _aHandlerUserService[iDelegate]) != NULL)
            {
                pCallback(_aUserDataUserService[iDelegate], pUserServiceEvent);
            }
        }
        return(true);
    }
    else if (messageId == EA::SEMD::kGroupSystemService)
    {
        const SceSystemServiceEvent *pSystemServiceEvent = pSEMDMessage->GetData<const SceSystemServiceEvent*>();

        // forward to our own handler
        for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
        {
            DirtyEventManagerSystemServiceCallbackT *pCallback;
            if ((pCallback = _aHandlerSystemService[iDelegate]) != NULL)
            {
                pCallback(_aUserDataSystemService[iDelegate], pSystemServiceEvent);
            }
        }
        return(true);
    }
    else if (messageId == EA::SEMD::kGroupCompanionUtil)
    {
        const SceCompanionUtilEvent *pCompanionUtilEvent = pSEMDMessage->GetData<const SceCompanionUtilEvent*>();

        // forward to our own handler
        for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
        {
            DirtyEventManagerCompanionUtilCallbackT *pCallback;
            if ((pCallback = _aHandlerCompanionUtil[iDelegate]) != NULL)
            {
                pCallback(_aUserDataCompanionUtil[iDelegate], pCompanionUtilEvent);
            }
        }
        return(true);
    }
    else if (messageId == EA::SEMD::kGroupCompanionHttpd)
    {
        const SceCompanionHttpdEvent *pCompanionHttpdEvent = pSEMDMessage->GetData<const SceCompanionHttpdEvent*>();

        // forward to our own handler
        for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
        {
            DirtyEventManagerCompanionHttpdCallbackT *pCallback;
            if ((pCallback = _aHandlerCompanionHttpd[iDelegate]) != NULL)
            {
                pCallback(_aUserDataCompanionHttpd[iDelegate], pCompanionHttpdEvent);
            }
        }
        return(true);
    }

    return(false);
}

/*** Public Functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerInit

    \Description
        Initializes the dirtyeventmanager module

    \Input bCheckTick   - if we should check whether the SEMD instance has been updated

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerInit(uint8_t bCheckTick)
{
    bool bAlreadyInitialized = (_pSystemEventMessageDispatcher != NULL);

    if ((_pSystemEventMessageDispatcher = EA::SEMD::SystemEventMessageDispatcher::GetInstance()) == NULL)
    {
        NetPrintf(("dirtyeventmanagerps4: error, SystemEventMessageDispatcher has not been initialized.\n"));
        return(-1);
    }

    if ((bCheckTick) && (_pSystemEventMessageDispatcher->HasTicked()))
    {
        NetPrintf(("dirtyeventmanagerps4: ************************************************************************************\n"));
        NetPrintf(("dirtyeventmanagerps4: error, EASEMD has ticked before DirtyEventManager was initialized. Events may have been lost.\n"));
        NetPrintf(("dirtyeventmanagerps4: *** PLEASE INITIALIZE THE DIRTYEVENTMANAGER EARLIER ***\n"));
        return(-1);
    }

    // if handlers are not already registered, register them
    if (!bAlreadyInitialized)
    {
        _pSystemEventMessageDispatcher->AddMessageHandler(&_Handler, EA::SEMD::kGroupUserService, false, EA::Messaging::kPriorityHigh);
        _pSystemEventMessageDispatcher->AddMessageHandler(&_Handler, EA::SEMD::kGroupSystemService, false, EA::Messaging::kPriorityHigh);
        _pSystemEventMessageDispatcher->AddMessageHandler(&_Handler, EA::SEMD::kGroupCompanionUtil, false, EA::Messaging::kPriorityHigh);
        _pSystemEventMessageDispatcher->AddMessageHandler(&_Handler, EA::SEMD::kGroupCompanionHttpd, false, EA::Messaging::kPriorityHigh);

        ds_memclr(_aHandlerUserService, sizeof(_aHandlerUserService));
        ds_memclr(_aHandlerSystemService, sizeof(_aHandlerSystemService));
        ds_memclr(_aHandlerCompanionHttpd, sizeof(_aHandlerCompanionHttpd));
        ds_memclr(_aHandlerCompanionUtil, sizeof(_aHandlerCompanionUtil));
        ds_memclr(_aUserDataUserService, sizeof(_aUserDataUserService));
        ds_memclr(_aUserDataSystemService, sizeof(_aUserDataSystemService));
        ds_memclr(_aUserDataCompanionHttpd, sizeof(_aUserDataCompanionHttpd));
        ds_memclr(_aUserDataCompanionUtil, sizeof(_aUserDataCompanionUtil));
    }

    return(0);
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerShutdown

    \Description
        Shuts down the dirtyeventmanager module

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerShutdown(void)
{
    if (_pSystemEventMessageDispatcher == NULL)
    {
        NetPrintf(("dirtyeventmanagerps4: DirtyEventManagerShutdown() called while module is inactive\n"));
        return(0);
    }

    // clean up the dispatcher
    _pSystemEventMessageDispatcher->RemoveMessageHandler(&_Handler, EA::SEMD::kGroupUserService);
    _pSystemEventMessageDispatcher->RemoveMessageHandler(&_Handler, EA::SEMD::kGroupSystemService);
    _pSystemEventMessageDispatcher->RemoveMessageHandler(&_Handler, EA::SEMD::kGroupCompanionUtil);
    _pSystemEventMessageDispatcher->RemoveMessageHandler(&_Handler, EA::SEMD::kGroupCompanionHttpd);

    // reset state
    _pSystemEventMessageDispatcher = NULL;
    ds_memclr(_aHandlerUserService, sizeof(_aHandlerUserService));
    ds_memclr(_aHandlerSystemService, sizeof(_aHandlerSystemService));
    ds_memclr(_aHandlerCompanionHttpd, sizeof(_aHandlerCompanionHttpd));
    ds_memclr(_aHandlerCompanionUtil, sizeof(_aHandlerCompanionUtil));
    ds_memclr(_aUserDataUserService, sizeof(_aUserDataUserService));
    ds_memclr(_aUserDataSystemService, sizeof(_aUserDataSystemService));
    ds_memclr(_aUserDataCompanionHttpd, sizeof(_aUserDataCompanionHttpd));
    ds_memclr(_aUserDataCompanionUtil, sizeof(_aUserDataCompanionUtil));

    return(0);
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerRegisterUserService

    \Description
        Register a callback for Sony's user service

    \Input *pCallback   - callback we are registering
    \Input *pUserData   - userdata we are passing along with the callback

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerRegisterUserService(DirtyEventManagerUserServiceCallbackT *pCallback, void *pUserData)
{
    int32_t iFirstAvailable = -1, iDelegate;

    // loop through all the available callback to find the first empty slot and check to see if the callback is already registered.
    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if ((iFirstAvailable < 0) && (_aHandlerUserService[iDelegate]  == NULL))
        {
            iFirstAvailable = iDelegate;
        }

        if (_aHandlerUserService[iDelegate] == pCallback)
        {
            NetPrintf(("dirtyeventmanagerps4: attempt to register duplicate handler [%p].\n", pCallback));
            return(-1);
        }
    }

    if ((iFirstAvailable >= 0) && (iFirstAvailable < DIRTYEVENTMANAGER_MAX_DELEGATES))
    {
        _aHandlerUserService[iFirstAvailable] = pCallback;
        _aUserDataUserService[iFirstAvailable] = pUserData;
        return(0);
    }
    else
    {
        NetPrintf(("dirtyeventmanagerps4: unable to register handler [%p], queue is full.\n", pCallback));
        return(-1);
    }
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerRegisterSystemService

    \Description
        Register a callback for Sony's system service

    \Input *pCallback   - callback we are registering
    \Input *pUserData   - userdata we are passing along with the callback

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerRegisterSystemService(DirtyEventManagerSystemServiceCallbackT *pCallback, void *pUserData)
{
    int32_t iFirstAvailable = -1, iDelegate;

    // loop through all the available callback to find the first empty slot and check to see if the callback is already registered.
    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if ((iFirstAvailable < 0) && (_aHandlerSystemService[iDelegate]  == NULL))
        {
            iFirstAvailable = iDelegate;
        }

        if (_aHandlerSystemService[iDelegate] == pCallback)
        {
            NetPrintf(("dirtyeventmanagerps4: attempt to register duplicate handler [%p].\n", pCallback));
            return(-1);
        }
    }

    if ((iFirstAvailable >= 0) && (iFirstAvailable < DIRTYEVENTMANAGER_MAX_DELEGATES))
    {
        _aHandlerSystemService[iFirstAvailable] = pCallback;
        _aUserDataSystemService[iFirstAvailable] = pUserData;
        return(0);
    }
    else
    {
        NetPrintf(("dirtyeventmanagerps4: unable to register handler [%p], queue is full.\n", pCallback));
        return(-1);
    }
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerRegisterCompanionHttpd

    \Description
        Register a callback for Sony's companion httpd

    \Input *pCallback   - callback we are registering
    \Input *pUserData   - userdata we are passing along with the callback

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerRegisterCompanionHttpd(DirtyEventManagerCompanionHttpdCallbackT *pCallback, void *pUserData)
{
    int32_t iFirstAvailable = -1, iDelegate;

    // loop through all the available callback to find the first empty slot and check to see if the callback is already registered.
    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if ((iFirstAvailable < 0) && (_aHandlerCompanionHttpd[iDelegate]  == NULL))
        {
            iFirstAvailable = iDelegate;
        }

        if (_aHandlerCompanionHttpd[iDelegate] == pCallback)
        {
            NetPrintf(("dirtyeventmanagerps4: attempt to register duplicate handler [%p].\n", pCallback));
            return(-1);
        }
    }

    if ((iFirstAvailable >= 0) && (iFirstAvailable < DIRTYEVENTMANAGER_MAX_DELEGATES))
    {
        _aHandlerCompanionHttpd[iFirstAvailable] = pCallback;
        _aUserDataCompanionHttpd[iFirstAvailable] = pUserData;
        return(0);
    }
    else
    {
        NetPrintf(("dirtyeventmanagerps4: unable to register handler [%p], queue is full.\n", pCallback));
        return(-1);
    }
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerRegisterCompanionUtil

    \Description
        Register a callback for Sony's companion util

    \Input *pCallback   - callback we are registering
    \Input *pUserData   - userdata we are passing along with the callback

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerRegisterCompanionUtil(DirtyEventManagerCompanionUtilCallbackT *pCallback, void *pUserData)
{
    int32_t iFirstAvailable = -1, iDelegate;

    // loop through all the available callback to find the first empty slot and check to see if the callback is already registered.
    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if ((iFirstAvailable < 0) && (_aHandlerCompanionUtil[iDelegate]  == NULL))
        {
            iFirstAvailable = iDelegate;
        }

        if (_aHandlerCompanionUtil[iDelegate] == pCallback)
        {
            NetPrintf(("dirtyeventmanagerps4: attempt to register duplicate handler [%p].\n", pCallback));
            return(-1);
        }
    }

    if ((iFirstAvailable >= 0) && (iFirstAvailable < DIRTYEVENTMANAGER_MAX_DELEGATES))
    {
        _aHandlerCompanionUtil[iFirstAvailable] = pCallback;
        _aUserDataCompanionUtil[iFirstAvailable] = pUserData;
        return(0);
    }
    else
    {
        NetPrintf(("dirtyeventmanagerps4: unable to register handler [%p], queue is full.\n", pCallback));
        return(-1);
    }
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerUnregisterUserService

    \Description
        Unregister a callback for Sony's user service

    \Input *pCallback   - callback we are unregistering

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerUnregisterUserService(DirtyEventManagerUserServiceCallbackT *pCallback)
{
    int32_t iDelegate;

    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if (pCallback == _aHandlerUserService[iDelegate])
        {
            _aHandlerUserService[iDelegate] = NULL;
            _aUserDataUserService[iDelegate] = NULL;
            return(0);
        }
    }

    NetPrintf(("dirtyeventmanagerps4: attempt to unregister unknown handler [%p]\n", pCallback));
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerUnregisterSystemService

    \Description
        Unregister a callback for Sony's system service

    \Input *pCallback   - callback we are unregistering

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerUnregisterSystemService(DirtyEventManagerSystemServiceCallbackT *pCallback)
{
    int32_t iDelegate;

    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if (pCallback == _aHandlerSystemService[iDelegate])
        {
            _aHandlerSystemService[iDelegate] = NULL;
            _aUserDataSystemService[iDelegate] = NULL;
            return(0);
        }
    }

    NetPrintf(("dirtyeventmanagerps4: attempt to unregister unknown handler [%p]\n", pCallback));
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerUnregisterCompanionHttpd

    \Description
        Unregister a callback for Sony's companion httpd

    \Input *pCallback   - callback we are unregistering

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerUnregisterCompanionHttpd(DirtyEventManagerCompanionHttpdCallbackT *pCallback)
{
    int32_t iDelegate;

    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if (pCallback == _aHandlerCompanionHttpd[iDelegate])
        {
            _aHandlerCompanionHttpd[iDelegate] = NULL;
            _aUserDataCompanionHttpd[iDelegate] = NULL;
            return(0);
        }
    }

    NetPrintf(("dirtyeventmanagerps4: attempt to unregister unknown handler [%p]\n", pCallback));
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function DirtyEventManagerUnregisterCompanionUtil

    \Description
        Unregister a callback for Sony's companion util

    \Input *pCallback   - callback we are unregistering

    \Output
        int32_t         - 0=success, <0=failure

    \Version 07/11/2013 (amakoukji)
*/
/*************************************************************************************F*/
int32_t DirtyEventManagerUnregisterCompanionUtil(DirtyEventManagerCompanionUtilCallbackT *pCallback)
{
    int32_t iDelegate;

    for (iDelegate = 0; iDelegate < DIRTYEVENTMANAGER_MAX_DELEGATES; ++iDelegate)
    {
        if (pCallback == _aHandlerCompanionUtil[iDelegate])
        {
            _aHandlerCompanionUtil[iDelegate] = NULL;
            _aUserDataCompanionUtil[iDelegate] = NULL;
            return(0);
        }
    }

    NetPrintf(("dirtyeventmanagerps4: attempt to unregister unknown handler [%p]\n", pCallback));
    return(-1);
}
