/*H*************************************************************************************/
/*!

    \File    tickernew.cpp

    \Description
        Exists only to define new operators for EABase

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2003.  ALL RIGHTS RESERVED.

    \Version    1.0      12/07/2013 (amakoukji) Initial version
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdlib.h>
#include "EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h"
#include "tickerplatform.h"
#include "DirtySDK/dirtysock/dirtymem.h"

#include <user_service.h>
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
#include "EAControllerUserPairing/Messages.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

class TickerIEAMessageHandler : public EA::Messaging::IHandler
{
public:
    TickerIEAMessageHandler()
    {
        NetPrintf(("EACUP event handler: initializing\n"));
        ds_memclr(aLocalUsers, sizeof(aLocalUsers));
    }

    bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
    {
        bool bSuccess = true;
        const EA::User::IEAUser *pLocalUser;
        EA::Pairing::UserMessage *pUserMessage;

        switch (messageId)
        {
            case EA::Pairing::E_USER_ADDED:
                NetPrintf(("EACUP event handler: E_USER_ADDED\n"));
                pUserMessage = (EA::Pairing::UserMessage *)pMessage;
                pLocalUser = pUserMessage->GetUser();
                pLocalUser->AddRef();
                bSuccess = AddLocalUser(pLocalUser);
                break;

            case EA::Pairing::E_USER_REMOVED:
                NetPrintf(("EACUP event handler: E_USER_REMOVED\n"));
                pUserMessage = (EA::Pairing::UserMessage *)pMessage;
                pLocalUser = pUserMessage->GetUser();
                bSuccess = RemoveLocalUser(pLocalUser);
                pLocalUser->Release();
                break;

            default:
                NetPrintf(("EACUP event handler: unsupported event (%d)\n", messageId));
                bSuccess = false;
                break;
        }

        return(bSuccess);
    }

    bool AddLocalUser(const EA::User::IEAUser *pLocalUser)
    {
        bool bSuccess = false;
        int32_t iLocalUserIndex;

        for (iLocalUserIndex = 0; iLocalUserIndex < iMaxLocalUsers; iLocalUserIndex++)
        {
            if (aLocalUsers[iLocalUserIndex] == NULL)
            {
                aLocalUsers[iLocalUserIndex] = pLocalUser;
                if (NetConnAddLocalUser(iLocalUserIndex, pLocalUser) == 0)
                {
                    bSuccess = true;
                }
                break;
            }
        }

        if (!bSuccess)
        {
            NetPrintf(("E_USER_ADDED failed\n"));
        }

        return(bSuccess);
    }

    bool RemoveLocalUser(const EA::User::IEAUser * pLocalUser)
    {
        bool bSuccess = false;
        int32_t iLocalUserIndex;

        for (iLocalUserIndex = 0; iLocalUserIndex < iMaxLocalUsers; iLocalUserIndex++)
        {
            if (aLocalUsers[iLocalUserIndex] == pLocalUser)
            {
                if (NetConnRemoveLocalUser(iLocalUserIndex, pLocalUser) == 0)
                {
                    bSuccess = true;
                }
                aLocalUsers[iLocalUserIndex] = NULL;
                break;
            }
        }

        if (!bSuccess)
        {
            NetPrintf(("E_USER_REMOVED failed\n"));
        }

        return(bSuccess);
    }

private:
    static const int32_t iMaxLocalUsers = 16;
    const EA::User::IEAUser *aLocalUsers[iMaxLocalUsers];
};

// DirtyCoreAllocator is a bare bones memory allocator that piggy-backs on top of DirtyMem functionality
class DirtyCoreAllocator : public EA::Allocator::ICoreAllocator
{
private:
     int32_t m_iMemModule;
     int32_t m_iMemGroup;
     void    *m_pMemGroupUserData;

public:
    DirtyCoreAllocator():
    m_iMemModule(DIRTYEVENT_DISP_MEMID),
    m_iMemGroup(-1),
    m_pMemGroupUserData(NULL)
    {

    }

    ~DirtyCoreAllocator() {}

    void *Alloc(size_t size, const char *name, unsigned int flags)
    {
        if (m_iMemGroup == -1)
        {
            DirtyMemGroupQuery(&m_iMemGroup, &m_pMemGroupUserData);  
        }
        return(DirtyMemAlloc((int32_t)size, m_iMemModule, m_iMemGroup, m_pMemGroupUserData));
    }

    void *Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset = 0)
    {
        // Since we are using DirtyMemAlloc, which does not support alignment, just call the simplified Alloc
        return(Alloc(size, name, flags));
    }

    void Free(void *block, size_t size=0)
    {
        DirtyMemFree(block, m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
    }

};

ICOREALLOCATOR_INTERFACE_API EA::Allocator::ICoreAllocator *EA::Allocator::ICoreAllocator::GetDefaultAllocator()
{
    // NOTE: we can't just use another global allocator object, since we'd have an initialization race condition between the two globals.
    //   to avoid that, we use a static function var, which is init'd the first time the function is called.
    static DirtyCoreAllocator sTestAllocator; // static function members are only constructed on first function call
    return &sTestAllocator;
}



/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

static EA::SEMD::SystemEventMessageDispatcher *pgDispatcher = NULL;
static EA::Pairing::EAControllerUserPairingServer *gpEacup;
static TickerIEAMessageHandler *gpEAmsghdlr;

/*** Public Functions ******************************************************************/

// Extremely simple definitions of new for EA Base
void* operator new[](size_t size, unsigned long, unsigned long, char const*, int, unsigned int, char const*, int)
{
    return operator new(size);
}
void* operator new[](size_t size, char const*, int, unsigned int, char const*, int)
{
    return operator new(size);
}

extern "C" void PlatformInit(void)
{
    EA::SEMD::SystemEventMessageDispatcherSettings semdSettings;

    semdSettings.useCompanionHttpd = false;
    semdSettings.useCompanionUtil = false;
    semdSettings.useSystemService = true;
    semdSettings.useUserService = true;
    pgDispatcher = EA::SEMD::SystemEventMessageDispatcher::CreateInstance(semdSettings, EA::Allocator::ICoreAllocator::GetDefaultAllocator());

    // initialize sce user service
    SceUserServiceInitializeParams sceUserServiceInitParams;
    ds_memclr(&sceUserServiceInitParams, sizeof(sceUserServiceInitParams));
    sceUserServiceInitParams.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
    sceUserServiceInitialize(&sceUserServiceInitParams);

    // instantiate EACUP server
    gpEacup = new EA::Pairing::EAControllerUserPairingServer(pgDispatcher, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    gpEAmsghdlr = new TickerIEAMessageHandler();
    gpEacup->Init();
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_ADDED, false, 0);
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_REMOVED, false, 0);
}

extern "C" void PlatformFinalizeInit(void)
{
    // tell netconn about the first set of users
    EA::User::UserList eacupUserList = gpEacup->GetUsers();
    EA::User::UserList::iterator i; 
    for(i = eacupUserList.begin(); i != eacupUserList.end(); i++)
    {
        const EA::User::IEAUser *pLocalUser = *i;
        pLocalUser->AddRef();
        gpEAmsghdlr->AddLocalUser(pLocalUser);
    }
}


extern "C" void PlatformTick(void)
{
    if (pgDispatcher)
    {
        pgDispatcher->Tick();
    }

    gpEacup->Tick();
}

extern "C" void PlatformShutdown(void)
{
    // free EACUP server
    gpEacup->RemoveMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_ADDED);
    gpEacup->RemoveMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_REMOVED);
    delete gpEAmsghdlr;
    gpEacup->Shutdown();
    delete gpEacup;

    // terminate sce user service
    sceUserServiceTerminate();

    if (pgDispatcher)
    {
        pgDispatcher->DestroyInstance();
    }
}
