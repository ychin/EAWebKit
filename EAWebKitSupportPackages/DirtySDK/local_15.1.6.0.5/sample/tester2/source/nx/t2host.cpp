/*H*************************************************************************************/
/*!
    \File t2host.c

    \Description
        Tester2 Host Application Framework

    \Copyright
        Copyright (c) Electronic Arts 2004.  ALL RIGHTS RESERVED.

    \Version    1.0 03/22/2005 (jfrank) First Version
*/
/*************************************************************************************H*/

/*** Include files *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <nn/init.h>
#include <nn/os.h>
#include <nn/socket.h>
#include <nn/fs.h>

#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
#include "EASystemEventMessageDispatcher/SystemEventMessageDispatcher.h"
#include "EAControllerUserPairing/Messages.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonformat.h"

#include "libsample/zlib.h"
#include "libsample/zmemtrack.h"

#include "testerhostcore.h"
#include "testercomm.h"
#include "t2nxrender.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/


/*** Type Definitions ******************************************************************/
// DirtyCoreAllocator is a bare bones memory allocator that piggy-backs on top of DirtyMem functionality
class DirtyCoreAllocator : public EA::Allocator::ICoreAllocator
{
    private:
        int32_t m_iMemModule;
        int32_t m_iMemGroup;
        void    *m_pMemGroupUserData;

    public:
        DirtyCoreAllocator() :
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

        void Free(void *block, size_t size = 0)
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
        static const int32_t iMaxLocalUsers = NETCONN_MAXLOCALUSERS;
        const EA::User::IEAUser *aLocalUsers[iMaxLocalUsers];
};



/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// tester host module
TesterHostCoreT *g_pHostCore;
static volatile uint8_t _bContinue = TRUE;

/*** Private Functions *****************************************************************/


/*F********************************************************************************/
/*!
    \Function _T2HostDisplayOutput

    \Description
        Take input from TesterConsole and display it somewhere

    \Input *pBuf    - string containing the debug output to display
    \Input  iLen    - length of buffer
    \Input  iRefcon - user-specified parameter (unused)
    \Input *pRefptr - user-specified parameter (window pointer)

    \Output None

    \Version 04/13/2005 (jfrank)
*/
/********************************************************************************F*/
static void _T2HostDisplayOutput(const char *pBuf, int32_t iLen, int32_t iRefcon, void *pRefptr)
{
    //printf("%s",pBuf);
}


/*F********************************************************************************/
/*!
    \Function _T2HostCommandlineProcess

    \Description
        Clear the console

    \Input  *argz   - environment
    \Input   argc   - num args
    \Input **argv   - arg list

    \Output  int32_t    - standard return code

    \Version 04/07/2005 (jfrank)
*/
/********************************************************************************F*/
static int32_t _TestHostProcessInput(char *pCommandLine, int32_t iCommandLen, int32_t *pCommandOff)
{
    struct pollfd PollFd;
    int32_t iResult;
    char cInput = 0;

    // poll for input, blocking up to 20ms
    PollFd.fd = 0;
    PollFd.events = POLLIN;
    if ((iResult = nn::socket::Poll(&PollFd, 1, 20)) < 0)
    {
        NetPrintf(("t2host: poll() error %d trying to get input from stdin\n", errno));
    }

    // did we get any input?
    if (PollFd.revents & POLLIN)
    {
        if ((iResult = (int32_t)read(0, &cInput, sizeof(cInput))) > 0)
        {
            pCommandLine[*pCommandOff] = cInput;
            *pCommandOff += 1;
        }
        else if (iResult < 0)
        {
            NetPrintf(("t2host: read() error %d trying to read input from stdin\n", errno));
        }
    }

    // no update
    return(cInput == '\n');
}

/*F********************************************************************************/
/*!
    \Function _T2HostCmdClear

    \Description
        Clear the console

    \Input  *argz   - environment
    \Input   argc   - num args
    \Input **argv   - arg list

    \Output  int32_t    - standard return code

    \Version 04/07/2005 (jfrank)
*/
/********************************************************************************F*/
static int32_t _T2HostCmdClear(ZContext *argz, int32_t argc, char **argv)
{
    if (argc < 1)
    {
        ZPrintf("   clear the display.\n");
        ZPrintf("   usage: %s\n", argv[0]);
    }
    else
    {
#if 0
        // clear the console
        //TesterConsoleT *pConsole;
        if ((pConsole = (TesterConsoleT *)TesterRegistryGetPointer("CONSOLE")) != NULL)
        {
            TesterConsoleClear(pConsole);
        }
#endif
    }
    return(0);
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

    \Version 04/05/2005 (jfrank)
*/
/********************************************************************************F*/
static int32_t _T2HostCmdExit(ZContext *argz, int32_t argc, char **argv)
{
    _bContinue = FALSE;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _T2HostRegisterModules

    \Description
        Register client commands (local commands, like exit, history, etc.)

    \Input None

    \Output None

    \Version 04/05/2005 (jfrank)
*/
/********************************************************************************F*/
static void _T2HostRegisterModules(void)
{
    TesterHostCoreRegister(g_pHostCore, "exit", &_T2HostCmdExit);
    TesterHostCoreRegister(g_pHostCore, "clear", &_T2HostCmdClear);
}

/*F********************************************************************************/
/*!
    \Function _T2HostNetConnect

    \Description
        Connect network using "net" commands, wait for network to initialize.

    \Input *pCore   - TesterHostCoreT ref

    \Version 09/11/2018 (tcho)
*/
/********************************************************************************F*/
static void _T2HostNetConnect(TesterHostCoreT *pCore)
{
    int32_t iStatus;
    int32_t iTimeout;

    // connect network by using net module
    TesterHostCoreDispatch(pCore, "net connect");

    //wait for the activation of the network interface
    for (iTimeout = NetTick() + 20 * 1000; ; )
    {
        // update network
        NetConnIdle();

        // get current status
        iStatus = NetConnStatus('conn', 0, NULL, 0);
        if ((iStatus == '+onl') || ((iStatus >> 24) == '-'))
        {
            break;
        }

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

int T2main(int32_t argc, char *argv[])
{
    EA::Pairing::EAControllerUserPairingServer *pEAcup = NULL;
    EA::SEMD::SystemEventMessageDispatcher *pSEMD = NULL;
    EA::SEMD::SystemEventMessageDispatcherSettings semdSettings;

    T2IEAMessageHandler *pEAmsghdlr = NULL;
    char strParams[512], strCommandLine[256] = "", strBase[256] = "", strHostName[128] = "";
    char strStartupParams[256] = "-servicename=tester2";
    int32_t iArg, iCommandOff = 0;
    uint8_t bInteractive = TRUE;

    // intialize Event Dispatcher settings
    semdSettings.useCompanionHttpd = false;
    semdSettings.useCompanionUtil  = false;
    semdSettings.useSystemService  = true;
    semdSettings.useUserService    = true;
    pSEMD = EA::SEMD::SystemEventMessageDispatcher::CreateInstance(semdSettings, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    
    // start the memtracker before we do anything else
    ZMemtrackStartup();

    ZPrintf(("\nStarting T2Host.\n\n"));


    int32_t iMemModule = 'tren';
    int32_t iMemGroup;
    void *pMemGroupUserData;
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // start rendering
    T2Render t2render(iMemModule, iMemGroup, pMemGroupUserData);
    t2render.T2RenderInit();

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
        if (!strncmp(argv[iArg], "-notty", 6))
        {
            bInteractive = FALSE;
            ZPrintf("t2host: notty mode enabled\n");
        }
        if (!strncmp(argv[iArg], "-singlethread", 13))
        {
            ds_strnzcat(strStartupParams, " -singlethreaded", sizeof(strStartupParams));
            ZPrintf("t2host: single-threaded mode enabled\n");
        }
    }

    // create the module
    JsonInit(strParams, sizeof(strParams), 0);
    JsonAddStr(strParams, "INPUTFILE", TESTERCOMM_HOSTINPUTFILE);
    JsonAddStr(strParams, "OUTPUTFILE", TESTERCOMM_HOSTOUTPUTFILE);
    JsonAddStr(strParams, "CONTROLDIR", strBase);
    JsonAddStr(strParams, "HOSTNAME", strHostName);

    // startup dirtysdk (must come before TesterHostCoreCreate() if we are using socket comm)
    if (NetConnStartup(strStartupParams) < 0)
    {
        ZPrintf("t2host: netconnstartup failed\n");
        t2render.T2RenderTerm();
        return(0);
    }

    // create eacup
    pEAcup = new EA::Pairing::EAControllerUserPairingServer(pSEMD, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    pEAmsghdlr = new T2IEAMessageHandler();
    pEAcup->Init();
    pEAcup->AddMessageHandler(pEAmsghdlr, EA::Pairing::E_USER_ADDED, false, 0);
    pEAcup->AddMessageHandler(pEAmsghdlr, EA::Pairing::E_USER_REMOVED, false, 0);

    // fetch and add user to netconn
    EA::User::UserList eacupUserList = pEAcup->GetUsers();
    EA::User::UserList::iterator i;

    for (i = eacupUserList.begin(); i != eacupUserList.end(); i++)
    {
        const EA::User::IEAUser *pLocalUser = *i;
        pLocalUser->AddRef();
        pEAmsghdlr->AddLocalUser(pLocalUser);
    }

    g_pHostCore = TesterHostCoreCreate(JsonFinish(strParams));
     _T2HostNetConnect(g_pHostCore);

    NetPrintf(("t2host: ipaddr=%a\n", NetConnStatus('addr', 0, NULL, 0)));
    NetPrintf(("t2host: macaddr=%s\n", NetConnMAC()));

    // define the function which will show stuff on the screen
    TesterHostCoreDisplayFunc(g_pHostCore, _T2HostDisplayOutput, 0, NULL);

    // register basic functions
    _T2HostRegisterModules();

    ZPrintf("T2Host NX Application Successfully started.\n");

    // check for command-line command
    for (iArg = 1; iArg < argc; iArg += 1)
    {
        // don't include -base or -connect arg, if present
        if (!strncmp(argv[iArg], "-base=", 6) || !strncmp(argv[iArg], "-connect=", 9) || !strncmp(argv[iArg], "-notty", 6) || !strncmp(argv[iArg], "-singlethread", 13))
        {
            continue;
        }
        // add to command-line
        ds_strnzcat(strCommandLine, argv[iArg], sizeof(strCommandLine));
        ds_strnzcat(strCommandLine, " ", sizeof(strCommandLine));
    }
    ZPrintf("> %s\n", strCommandLine);
    TesterHostCoreDispatch(g_pHostCore, strCommandLine);
    strCommandLine[0] = '\0';

    while (_bContinue)
    {
        // check for input
        if (bInteractive && _TestHostProcessInput(strCommandLine, sizeof(strCommandLine), &iCommandOff))
        {
            strCommandLine[iCommandOff - 1] = '\0'; // remove lf
            TesterHostCoreDispatch(g_pHostCore, strCommandLine);
            strCommandLine[0] = '\0';
            iCommandOff = 0;
        }

        // pump the host core module
        TesterHostCoreUpdate(g_pHostCore, 1);

        // give time to zlib
        ZTask();
        ZCleanup();

        // give time to network
        pSEMD->Tick();
        NetConnIdle();
        pEAcup->Tick();

        // render
        t2render.T2RenderUpdate();

        // sleep for a short while
        fflush(stdout);
        ZSleep(20);
    }

    // code is unreachable for now
    TesterHostCoreDisconnect(g_pHostCore);
    TesterHostCoreDestroy(g_pHostCore);

    // shut down the network
    NetConnShutdown(0);

    t2render.T2RenderTerm();

    ZMemtrackShutdown();

    ZPrintf("\nQuitting T2Host.\n\n");

    return(0);
}

extern "C" void nninitStartup()
{
    const size_t ApplicationMemorySize = 512 * 1024 * 1024;
    const size_t MallocMemorySize = 256 * 1024 * 1024;
    nn::Result result = nn::os::SetMemoryHeapSize(ApplicationMemorySize);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    uintptr_t address;
    result = nn::os::AllocateMemoryBlock(&address, MallocMemorySize);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    nn::init::InitializeAllocator(reinterpret_cast<void*>(address), MallocMemorySize);
}

extern "C" void nnMain()
{
    T2main(nn::os::GetHostArgc(), nn::os::GetHostArgv());
}

