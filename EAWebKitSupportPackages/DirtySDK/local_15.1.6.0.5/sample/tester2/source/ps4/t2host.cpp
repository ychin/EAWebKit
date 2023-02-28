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

#ifdef DIRTY_NPTOOLKIT
#include "np_toolkit.h"
#endif

#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
#include "EAControllerUserPairing/Messages.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonformat.h"
#include "DirtySDK/dirtysock/ps4/dirtyeventmanagerps4.h"
#include "EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h"
#include "libsample/zlib.h"
#include "libsample/zmemtrack.h"
#include "testerhostcore.h"
#include "testercomm.h"
#include "libsample/zfile.h"
#include "render.h"



/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct T2HostAppT
{
    TesterHostCoreT *pCore;
    T2Render render;
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
#ifdef DIRTY_NPTOOLKIT
void T2_NpToolkitCallback( const sce::Toolkit::NP::Event& event);
#endif

void T2ErrorHandler(int32_t iError);

/*** Variables *************************************************************************/

static TesterHostCoreT *g_pHostCore;
static volatile int iContinue=1;
static uint8_t aDirtySdkSampleSecret[] = {0x2a, 0x00, 0x27, 0xfa, 0x77, 0x7c, 0xdc, 0x31, 0xfa, 0x65, 0xc2, 0x55, 0x6b, 0xa3, 0x31, 0xef, 0x10, 0x39, 0x83, 0x7a, 0xa0, 0xfe, 0xf8, 0xa7, 0x70, 0xbf, 0x68, 0x05, 0x9b, 0x1a, 0xb1, 0x9b, 0x67, 0xfc, 0x8a, 0x1f, 0xfe, 0x43, 0x7b, 0xd8, 0x82, 0xf7, 0xbf, 0xad, 0x52, 0x12, 0xa8, 0x19, 0x03, 0x94, 0x17, 0x18, 0xb4, 0x92, 0x44, 0xb4, 0x47, 0x3a, 0xdd, 0x29, 0x2c, 0x29, 0x87, 0x2a, 0xf5, 0x83, 0x90, 0xdd, 0xec, 0x9d, 0x81, 0x87, 0xff, 0xbc, 0x29, 0x25, 0xd7, 0xf3, 0x48, 0x3b, 0xb4, 0xae, 0xa8, 0x14, 0xf8, 0xa7, 0xa9, 0x10, 0x6a, 0x9f, 0xe0, 0xcf, 0xd3, 0x65, 0xec, 0xe7, 0x99, 0x08, 0x4d, 0x0f, 0xb9, 0x22, 0x4b, 0x4f, 0x57, 0x0e, 0x79, 0x58, 0x7d, 0x82, 0x26, 0x41, 0x90, 0x8e, 0x06, 0xc6, 0x44, 0x72, 0x57, 0x3c, 0x7a, 0x7b, 0xf7, 0x1b, 0x49, 0x9a, 0x8f, 0xa6};

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
    
    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_INVITATION_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_INVITATION_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ( sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG) != SCE_OK ) 
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ( sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT) != SCE_OK ) 
    {
        NetPrintf(("sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT) failed (err=%s)\n", iResult));
    }

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM)) != SCE_OK)
    {
        NetPrintf(("sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM) failed (err=%s)\n", DirtyErrGetName(iResult)));
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

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_INVITATION_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_INVITATION_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
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

#ifdef DIRTY_NPTOOLKIT

/*F********************************************************************************/
/*!
    \Function T2_NpToolkitCallback

    \Description
        Callback function for User Events

    \Input event    A sce::Toolkit::NP::Event

    \Output
        None.

    \Version 08/06/2013 (tcho)
*/
/********************************************************************************F*/
void T2_NpToolkitCallback( const sce::Toolkit::NP::Event& event)
{
    if (event.event == sce::Toolkit::NP::Event::UserEvent::loggedIn || event.event == sce::Toolkit::NP::Event::UserEvent::loggedOut)
    {       
        NetConnNpUserStatusT UserStatus;
        ds_memclr(&UserStatus, sizeof(UserStatus));
        UserStatus.iUserId = event.userInformation.userId;
        UserStatus.npId = event.userInformation.npId;
        UserStatus.state = event.userInformation.state;
            
        NetConnControl('stat', sizeof(UserStatus), 0, &UserStatus, NULL);
    }
}

#endif


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

#ifdef DIRTY_NPTOOLKIT
    SceNpTitleId s_npTitleId;
    SceNpTitleSecret s_npTitleSecret;

    sce::Toolkit::NP::NpTitleId nptTitleId;
    ds_memclr(&s_npTitleId, sizeof(s_npTitleId));
    strncpy(s_npTitleId.id, "NPXX51027_00", strlen("NPXX51027_00"));
    ds_memclr(&s_npTitleSecret, sizeof(s_npTitleSecret));
    ds_memcpy(&s_npTitleSecret.data, aDirtySdkSampleSecret, SCE_NP_TITLE_SECRET_SIZE);
    nptTitleId.setTitleSecret(s_npTitleId, s_npTitleSecret);

    sce::Toolkit::NP::Parameters params(T2_NpToolkitCallback, nptTitleId);
    if ( sceSysmoduleLoadModule(SCE_SYSMODULE_XML) != SCE_OK ) 
    {
        NetPrintf(("T2 Host: Failed to load SCE_SYSMODULE_XML module!\n"));
        return(-1);
    }

    int ret = sce::Toolkit::NP::Interface::init(params);
    if (ret != SCE_OK) 
    {
        NetPrintf(("T2 Host: Failed to initialize NP tool kit Interface !\n"));
        return(-1);
    }

    // startup dirtysdk (must come before TesterHostCoreCreate() if we are using socket comm)
    NetConnStartup("-userserviceprio=SCE_KERNEL_PRIO_FIFO_DEFAULT -servicename=tester2 -titleid=NPXX51027_00 -nptoolkit");
#else
    NetConnStartup("-userserviceprio=SCE_KERNEL_PRIO_FIFO_DEFAULT -servicename=tester2 -titleid=NPXX51027_00");
#endif

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

    T2RenderInit(&T2App.render);

    // burn some time
    while(iContinue)
    {
        pDispatcher->Tick();
        _T2HostUpdate(&T2App);
        T2RenderUpdate(&T2App.render);
        gpEasmd->Tick();
        gpEacup->Tick();
    }

    T2RenderTerm(&T2App.render);

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

    // terminate sce user service
    sceUserServiceTerminate();

    // terminate Error Dialog
    sceErrorDialogTerminate();

    ZMemtrackShutdown();

    _T2HostUnloadSysUtils();

    pDispatcher->DestroyInstance();

    ZPrintf("t2host: exit\n");

    return(0);
}
