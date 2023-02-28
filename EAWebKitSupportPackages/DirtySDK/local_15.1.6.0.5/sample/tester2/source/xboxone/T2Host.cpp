/*H********************************************************************************/
/*!
    \File T2Host.cpp

    \Description
        Main file for Tester2 Host Application.

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 11/01/2012 (akirchner) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

#include <string>
#include <sstream>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonformat.h"

#include "libsample/zlib.h"
#include "libsample/zmemtrack.h"

#include "testerhostcore.h"
#include "testerregistry.h"
#include "testercomm.h"

#include "SampleCore.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

// private variables
static bool _T2Host_bActive = true;

// public variables
TesterHostCoreT *g_pHostCore;   //!< global host core pointer


/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _T2HostCmdExit

    \Description
        Quit

    \Input  *argz   - environment
    \Input   argc   - num args
    \Input **argv   - arg list
    
    \Output  int32_t    - standard return code

    \Version 05/03/2005 (jfrank)
*/
/********************************************************************************F*/
static int32_t _T2HostCmdExit(ZContext *argz, int32_t argc, char **argv)
{
    if (argc >= 1)
    {
        _T2Host_bActive = false;
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _T2HostNetConnect

    \Description
        Connect network using "net" commands, wait for network to initialize.

    \Input *pCore   - TesterHostCoreT ref

    \Version 12/18/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _T2HostNetConnect(TesterHostCoreT *pCore)
{
    int32_t iStatus, iTimeout;

    // connect the network using "net" module
    TesterHostCoreDispatch(pCore, "net connect");

    // wait for network interface activation
    for (iTimeout = NetTick()+20*1000 ; ; )
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

/*F********************************************************************************/
/*!
    \Function _T2HostRegisterModules

    \Description
        Register client commands (local commands, like exit, history, etc.)

    \Input None
    
    \Output None

    \Version 05/03/2005 (jfrank)
*/
/********************************************************************************F*/
static void _T2HostRegisterModules(void)
{
    TesterHostCoreRegister(g_pHostCore, "exit", &_T2HostCmdExit);
}


/*** Public Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function InitializeT2

    \Description
        Initialize state before running the sample.

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
void InitializeT2()
{
    char strParams[512];

    ZPrintf("\nStarting T2Host.\n\n");

    ZMemtrackStartup();

    if (NetConnStartup("-servicename=tester2") < 0)
    {
        ZPrintf("\\nFailed to start T2Host.\n\n");
        ZMemtrackShutdown();
        return;
    }

    // create the module
    JsonInit(strParams, sizeof(strParams), 0);
    JsonAddInt(strParams, "LOCALECHO", 1);
    g_pHostCore = TesterHostCoreCreate(JsonFinish(strParams));

    // register local modules
    _T2HostRegisterModules();

    ZPrintf("\nStarted T2Host.\n\n");
}


/*F********************************************************************************/
/*!
    \Function ConnectT2

    \Description
        Call NetConnConnect

    \Version 23/01/2014 (mclouatre)
*/
/********************************************************************************F*/
void ConnectT2()
{
    // connect to the network
    _T2HostNetConnect(g_pHostCore);
}


/*F********************************************************************************/
/*!
    \Function UpdateT2

    \Description
        Called once per frame to update data

   \Output
        bool    - TRUE to continue processing, FALSE to quit

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
bool UpdateT2(void)
{
    // pump the host core module
    TesterHostCoreUpdate(g_pHostCore, 1);

    // give time to zlib
    ZTask();
    ZCleanup();

    // give time to network
    NetConnIdle();

    // return active status
    return(_T2Host_bActive);
}

/*F********************************************************************************/
/*!
    \Function UninitializeT2

    \Description
        Uninitialize T2 Host (Shutdown T2 Host)

    \Input  None
    
    \Output None

    \Version 16/05/2013 (tcho)
*/
/********************************************************************************F*/
void UninitializeT2()
{
    // clean up and exit
    TesterHostCoreDisconnect(g_pHostCore);
    TesterHostCoreDestroy(g_pHostCore);

    // shut down the network
    NetConnShutdown(0);

    ZMemtrackShutdown();

    ZPrintf("\nQuitting T2Host.\n\n");
}

/*F********************************************************************************/
/*!
    \Function Main

    \Description
        Entry Point for T2 Host

    \Input  args   not used
    
    \Output None

    \Version 16/05/2013 (tcho)
*/
/********************************************************************************F*/
int main(Platform::Array<Platform::String^>^ args)
{
    auto sampleFactory = ref new SampleFactory();
    Windows::ApplicationModel::Core::CoreApplication::Run(sampleFactory);
}


