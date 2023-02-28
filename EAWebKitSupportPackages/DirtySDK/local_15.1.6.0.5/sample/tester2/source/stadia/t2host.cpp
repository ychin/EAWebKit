/*H*************************************************************************************/
/*!

    \File t2host.cpp

    \Description
        Tester2 Host Application Framework

    \Copyright
        Copyright (c) Electronic Arts 2020.  ALL RIGHTS RESERVED.

    \Version 01/28/2020 (eesponda)
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <EABase/eabase.h>

EA_DISABLE_CLANG_WARNING(-Wstrict-prototypes)
#include <ggp_c/ggp.h>
EA_RESTORE_CLANG_WARNING()

#include "IEAController/IEAController.h"
EA_DISABLE_CLANG_WARNING(-Wsign-compare)
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
EA_RESTORE_CLANG_WARNING()
#include "EAControllerUserPairing/Messages.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonformat.h"
#include "EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h"
#include "libsample/zlib.h"
#include "libsample/zmemtrack.h"
#include "testerhostcore.h"
#include "testercomm.h"
#include "libsample/zfile.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct T2HostAppT
{
    TesterHostCoreT *pCore;
} T2HostAppT;

class T2IEAMessageHandler : public EA::Messaging::IHandler
{
public:
    T2IEAMessageHandler()
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
EA::Pairing::EAControllerUserPairingServer *gpEacup;
EA::SEMD::SystemEventMessageDispatcher *gpEasmd;
T2IEAMessageHandler *gpEAmsghdlr;

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

static TesterHostCoreT *g_pHostCore;
static volatile int iContinue=1;

/*** Private Functions *****************************************************************/

/*F********************************************************************************/
/*!
    \Function _T2HostCmdExit

    \Description
        Quit

    \Input  *argz   - environment
    \Input   argc   - num args
    \Input **argv   - arg list

    \Output  int32_t    - standard return code

    \Version 05/25/2006 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _T2HostCmdExit(ZContext *argz, int32_t argc, char **argv)
{
    if (argc >= 1)
    {
        iContinue = 0;
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _T2HostUpdate

    \Description
        Update tester2 processing

    \Input  *argz   - environment
    \Input   argc   - num args
    \Input **argv   - arg list

    \Output  int32_t    - standard return code

    \Version 05/25/2006 (tdills)
*/
/********************************************************************************F*/
static void _T2HostUpdate(T2HostAppT *pApp)
{
    // pump the hostcore module
    TesterHostCoreUpdate(pApp->pCore, 1);

    // give time to zlib
    ZTask();
    ZCleanup();

    // give time to network
    NetConnIdle();
}

/*F********************************************************************************/
/*!
    \Function _T2HostNetStart

    \Description
        Connect network using "net" commands, wait for network to initialize.

    \Input *pCore   - TesterHostCoreT ref

    \Output
        None.

    \Version 08/11/2006 (jbrookes)
*/
/********************************************************************************F*/
static void _T2HostNetConnect(TesterHostCoreT *pCore)
{
    int32_t iStatus, iTimeout;

    // connect the network using "net" module
    TesterHostCoreDispatch(pCore, "net connect");

    // wait for network interface activation
    for (iTimeout = NetTick()+15*1000 ; ; )
    {
        // update network
        NetConnIdle();
        // get current status
        iStatus = NetConnStatus('conn', 0, NULL, 0);
        if ((iStatus == '+onl') || ((iStatus >> 24) == '-'))
        {
            break;
        }

        // check for timeout
        if (iTimeout < (signed)NetTick())
        {
            NetPrintf(("t2host: timeout waiting for interface activation\n"));
            break;
        }

        // give time to other threads
        NetConnSleep(1);
    }

    // check result code
    if ((iStatus = NetConnStatus('conn', 0, NULL, 0)) == '+onl')
    {
        NetPrintf(("t2host: interface active\n"));
    }
    else if ((iStatus >> 14) == '-')
    {
        NetPrintf(("t2host: error bringing up interface\n"));
    }
}

/*** Public Functions ******************************************************************/

/*F********************************************************************************/
/*!
    \Function main

    \Description
        Main routine for PS3 T2Host application.

    \Input argc     - argument count
    \Input *argv[]  - argument list

    \Output int32_t - zero

    \Version 10/15/2012 (akirchner)
*/
int main(int32_t argc, char *argv[])
{
    char strBase    [128] = "";
    char strHostName[128] = "";
    char strParams  [512];

    T2HostAppT T2App;
    int32_t    iArg;

    EA::SEMD::SystemEventMessageDispatcherSettings semdSettings;
    semdSettings.useCompanionHttpd = false;
    semdSettings.useCompanionUtil = false;
    semdSettings.useSystemService = true;
    semdSettings.useUserService = true;
    EA::SEMD::SystemEventMessageDispatcher::CreateInstance(semdSettings, EA::Allocator::ICoreAllocator::GetDefaultAllocator());

    /* the documentation says to call GgpInitialize before doing any interaction but calling it causes some weird behavior with the 
       EACUP. none of the library calls seem to fail without the init call */

    // get arguments
    for (iArg = 0; iArg < argc; iArg++)
    {
        if (!strncmp(argv[iArg], "-path=", 6))
        {
            ds_strnzcpy(strBase, &argv[iArg][6], sizeof(strBase));
            ZPrintf("t2host: base path=%s\n", strBase);
        }
        if (!strncmp(argv[iArg], "-connect=", 9))
        {
            ZPrintf("t2host: connect=%s\n", strHostName);
        }
    }

    // start the memtracker before we do anything else
    ZMemtrackStartup();

    // create the module
    JsonInit(strParams, sizeof(strParams), 0);

    JsonAddStr(strParams, "INPUTFILE", TESTERCOMM_HOSTINPUTFILE);
    JsonAddStr(strParams, "OUTPUTFILE", TESTERCOMM_HOSTOUTPUTFILE);
    JsonAddStr(strParams, "CONTROLDIR", strBase);
    JsonAddStr(strParams, "HOSTNAME", strHostName);

    // instantiate EACUP server
    gpEasmd = EA::SEMD::SystemEventMessageDispatcher::GetInstance();
    gpEacup = new EA::Pairing::EAControllerUserPairingServer(gpEasmd, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    gpEAmsghdlr = new T2IEAMessageHandler();
    gpEacup->Init();
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_ADDED, false, 0);
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_REMOVED, false, 0);
    EA::User::UserList eacupUserList = gpEacup->GetUsers();

    NetConnStartup("-servicename=tester2");

    // tell netconn about the first set of users
    for (const EA::User::IEAUser *pLocalUser : eacupUserList)
    {
        pLocalUser->AddRef();
        gpEAmsghdlr->AddLocalUser(pLocalUser);
    }

    // create tester2 host core
    g_pHostCore = TesterHostCoreCreate(JsonFinish(strParams));

    // on stadia the users take time to get paired. dirtysdk requires the users to
    // be added before we can connect. here we will let EACUP process until to give it
    // a better chance of success
    for (int32_t iTimeout = NetTick()+5*1000 ; ; )
    {
        gpEasmd->Tick();
        gpEacup->Tick();

        NetConnIdle();

        // check for user being paired
        if (gpEacup->GetUsers().size() > 0)
        {
            break;
        }

        // check for timeout
        if (iTimeout < (signed)NetTick())
        {
            break;
        }

        // give time to other threads
        NetConnSleep(16);
    }

    // connect to the network (blocking; waits for connection success)
    _T2HostNetConnect(g_pHostCore);

    // register some basic commands
    TesterHostCoreRegister(g_pHostCore, "exit", _T2HostCmdExit);

    // init app structure
    T2App.pCore = g_pHostCore;

    ZPrintf("t2host: entering polling loop\n");

    // burn some time
    while (iContinue)
    {
        _T2HostUpdate(&T2App);
        gpEasmd->Tick();
        gpEacup->Tick();
    }

    // clean up and exit
    TesterHostCoreDisconnect(g_pHostCore);
    TesterHostCoreDestroy   (g_pHostCore);

    // shut down the network
    NetConnShutdown(0);

    // free EACUP server
    gpEacup->RemoveMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_ADDED);
    gpEacup->RemoveMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_REMOVED);
    delete gpEAmsghdlr;
    gpEacup->Shutdown();
    delete gpEacup;

    ZMemtrackShutdown();

    gpEasmd->DestroyInstance();

    ZPrintf("t2host: exit\n");

    return(0);
}
