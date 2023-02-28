/*H*************************************************************************************/
/*!

    \File T2Host.c

    \Description
        Tester2 Host Application Framework

    \Copyright
        Copyright (c) Electronic Arts 2004.  ALL RIGHTS RESERVED.

    \Version    1.0 10/15/2012 (akirchner) First Version
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsysmodule.h>
#include <error_dialog.h>
#include <sce_random.h>

#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
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
#include "ps5/t2ps5render.h"

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

void T2ErrorHandler(int32_t iError);

/*** Variables *************************************************************************/

static TesterHostCoreT *g_pHostCore;
static volatile int iContinue=1;
static uint8_t aDirtySdkSampleSecret[] = {0x82, 0xcd, 0x3f, 0x20, 0xb7, 0x5a, 0xb5, 0x54, 0x2d, 0x0d, 0xfe, 0x04, 0xca, 0xcd, 0x54, 0x55, 0x8f, 0x81, 0xd8, 0xb4, 0x99, 0x44, 0x87, 0x17, 0x71, 0x8f, 0xef, 0xe6, 0x12, 0xb3, 0x2e, 0xdc, 0x36, 0x74, 0x5e, 0x85, 0xc4, 0xd2, 0x11, 0x74, 0xa8, 0xc1, 0x73, 0x07, 0xbf, 0xac, 0xd9, 0x85, 0x2d, 0x7d, 0xf3, 0xc1, 0x98, 0x7d, 0xbe, 0xd8, 0xa6, 0x47, 0xbf, 0x92, 0x13, 0xdb, 0x0d, 0x6e, 0x58, 0xc2, 0xaa, 0x66, 0xe6, 0x80, 0xcb, 0x16, 0xa4, 0xff, 0xe0, 0xf2, 0xab, 0x61, 0x5a, 0xea, 0x1c, 0xff, 0x59, 0x7c, 0x07, 0xa2, 0xd6, 0x69, 0x62, 0x9f, 0xa6, 0xd2, 0x2e, 0xa4, 0x15, 0x57, 0xc1, 0xf7, 0xfc, 0x1c, 0x3b, 0x37, 0x50, 0xdc, 0x90, 0x18, 0xe7, 0x85, 0xb8, 0x6e, 0xb8, 0xe0, 0x5f, 0x2b, 0x8e, 0x55, 0xea, 0x6e, 0xbb, 0x7b, 0x16, 0x06, 0x2a, 0xb2, 0x0f, 0xc3, 0x10, 0xfb };

/* The following global variables enable allocation of more than 256k of memory with malloc in the
   ps4 SDK version 820. This is needed because T2Host ends up breaking because it needes to
   allocate more than 256K, and fails */
size_t sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT;
unsigned int sceLibcHeapExtendedAlloc = 1;

/*** Private Functions *****************************************************************/

/*F********************************************************************************/
/*!
    \Function _T2HostLoadSysUtils

    \Description
        Load required Sony PRXs

    \Version 02/04/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _T2HostLoadSysUtils(void)
{
    int32_t iResult;

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_VOICE)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_VOICE) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_AUTH)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_NP_AUTH) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
    
    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG)) != SCE_OK ) 
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT)) != SCE_OK ) 
    {
        NetPrintf(("sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT) failed (err=%s)\n", iResult));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS)) != SCE_OK)
    {
        NetPrintf(("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS) failed (err=%s)\n", iResult));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM)) != SCE_OK)
    {
        NetPrintf(("sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
}

/*F********************************************************************************/
/*!
    \Function _T2HostUnloadSysUtils

    \Description
        Unload Sony PRXs

    \Version 02/04/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _T2HostUnloadSysUtils(void)
{
    int32_t iResult;

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_VOICE)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_VOICE) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_AUTH)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_AUTH) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_APP_CONTENT)) != SCE_OK)
    {
        NetPrintf(("sceSysmoduleUnloadModule(SCE_SYSMODULE_APP_CONTENT) failed (err=%s)\n", iResult));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_RANDOM)) != SCE_OK)
    {
        NetPrintf(("sceSysmoduleUnloadModule(SCE_SYSMODULE_RANDOM) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_MESSAGE_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_MESSAGE_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_COMMERCE)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_COMMERCE) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
}

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
        NetConnSleep(500);
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

//Use to handle Sony Error Code according to TRC4034
void T2ErrorHandler(int32_t iError)
{
    int32_t iRet = 0;
    SceErrorDialogParam param;
    sceErrorDialogParamInitialize( &param );
    param.errorCode = iError;

    if ((iRet = sceErrorDialogInitialize()) != SCE_OK ) 
    {
        if (iRet != SCE_ERROR_DIALOG_ERROR_ALREADY_INITIALIZED)
        {
            NetPrintf(("t2host: Fail to initialize Error Dialog\n"));
            return;
        }
    }

    sceErrorDialogOpen(&param);
}

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
/*F********************************************************************************/
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

    EA::SEMD::SystemEventMessageDispatcher *pDispatcher = EA::SEMD::SystemEventMessageDispatcher::CreateInstance(semdSettings, EA::Allocator::ICoreAllocator::GetDefaultAllocator());

    // load required system utilities
    _T2HostLoadSysUtils();

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
            ds_strnzcpy(strHostName, &argv[iArg][9], sizeof(strHostName));
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

    DirtyErrAppCallbackSet(&T2ErrorHandler);
    NetConnStartup("-userserviceprio=SCE_KERNEL_PRIO_FIFO_DEFAULT -servicename=tester2 -titleid=NPXX53581_00");

    // initialize sce user service
    SceUserServiceInitializeParams sceUserServiceInitParams;
    ds_memclr(&sceUserServiceInitParams, sizeof(sceUserServiceInitParams));
    sceUserServiceInitParams.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
    sceUserServiceInitialize(&sceUserServiceInitParams);

    // instantiate EACUP server
    gpEasmd = EA::SEMD::SystemEventMessageDispatcher::GetInstance();
    gpEacup = new EA::Pairing::EAControllerUserPairingServer(gpEasmd, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    gpEAmsghdlr = new T2IEAMessageHandler();
    gpEacup->Init();
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_ADDED, false, 0);
    gpEacup->AddMessageHandler(gpEAmsghdlr, EA::Pairing::E_USER_REMOVED, false, 0);
    EA::User::UserList eacupUserList = gpEacup->GetUsers();

    // tell netconn about the first set of users
    EA::User::UserList::iterator i; 
    for(i = eacupUserList.begin(); i != eacupUserList.end(); i++)
    {
        const EA::User::IEAUser *pLocalUser = *i;
        pLocalUser->AddRef();
        gpEAmsghdlr->AddLocalUser(pLocalUser);
    }

    // setup the title secret
    NetConnControl('npsc', sizeof(aDirtySdkSampleSecret), 0, aDirtySdkSampleSecret, NULL);

    // create tester2 host core
    g_pHostCore = TesterHostCoreCreate(JsonFinish(strParams));

    // connect to the network (blocking; waits for connection success)
    _T2HostNetConnect(g_pHostCore);

    // register some basic commands
    TesterHostCoreRegister(g_pHostCore, "exit", _T2HostCmdExit);

    // init app structure
    T2App.pCore = g_pHostCore;

    ZPrintf("t2host: entering polling loop\n");

    // start rendering
    int32_t iMemModule = 'tren';
    int32_t iMemGroup;
    void *pMemGroupUserData;
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
    T2RenderInit(iMemModule, iMemGroup, pMemGroupUserData);

    // burn some time
    while(iContinue)
    {
        pDispatcher->Tick();
        T2RenderUpdate();
        _T2HostUpdate(&T2App);
        gpEasmd->Tick();
        gpEacup->Tick();
    }

    T2RenderTerm();

    // clean up and exit
    TesterHostCoreDisconnect(g_pHostCore);
    TesterHostCoreDestroy   (g_pHostCore);

    // free EACUP server
    gpEacup->Shutdown();
    delete gpEacup;
    delete gpEAmsghdlr;
    pDispatcher->DestroyInstance();

    // shut down the network
    NetConnShutdown(0);

    // terminate sce user service
    sceUserServiceTerminate();

    // terminate Error Dialog
    sceErrorDialogTerminate();

    _T2HostUnloadSysUtils();

    // this must come last so everything else has a chance to free memory before it does its auditing
    ZMemtrackShutdown();

    ZPrintf("t2host: exit\n");
    return(0);
}
