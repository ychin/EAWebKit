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

#include "samplecode/pch.h"

#include "appnotify.h"
#include "assert.h"

#include <XGameRuntimeInit.h>
#include <xsapi-c/services_c.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
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

/*** Function Prototype ***********************************************************/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*** Variables ********************************************************************/
// private variables
static bool _T2Host_bActive = true;
HANDLE PlmSuspendCompleteT = nullptr;
HANDLE PlmSignalResumeT = nullptr;

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

    PostQuitMessage(0);

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
    \Function WinMain

    \Description
        Entry Point for T2 Host

    \Input  args        not used
    
    \Output int32_t     0 for success, 1 for error

    \Version 19/12/2019 (migallant, combination of previous version by tcho 
        and Microsoft samples)
*/
/********************************************************************************F*/
int32_t APIENTRY WinMain(HINSTANCE inst, HINSTANCE prev, char *pCmdline, int32_t iShow)
{

    HRESULT hResult;
    XblInitArgs InitArgs;

    // initialize game runtime
    if ((hResult = XGameRuntimeInitialize()) != S_OK)
    {
        NetPrintf(("t2host: error initializing game runtime (err=%s)\n", DirtyErrGetName(hResult)));
        return(1);
    }

    ds_memclr(&InitArgs, sizeof(InitArgs));
    InitArgs.scid = T2HOST_SCID;
    if (FAILED(hResult = XblInitialize(&InitArgs)))
    {
        ZPrintf("t2host: error initializing the xbl library (err=%s)\n", DirtyErrGetName(hResult));
        return(1);
    }

    // Default main thread to CPU 0
    SetThreadAffinityMask(GetCurrentThread(), 0x1);

    Sample sample = Sample();

    // Register class and create window
    PAPPSTATE_REGISTRATION hPLM = {};
    {
        // Register class
        WNDCLASSEXA WcexT = {};
        WcexT.cbSize = sizeof(WNDCLASSEXA);
        WcexT.style = CS_HREDRAW | CS_VREDRAW;
        WcexT.lpfnWndProc = WndProc;
        WcexT.hInstance = inst;
        WcexT.lpszClassName = u8"DirtySDKWindow";
        WcexT.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        if (!RegisterClassExA(&WcexT))
        {
            return(1);
        }

        // Create window
        HWND hwnd = CreateWindowExA(0, u8"DirtySDKWindow", u8"SampleCore", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, nullptr, nullptr, inst,
            nullptr);
        if (!hwnd)
        {
            return(1);
        }
        
        ShowWindow(hwnd, iShow);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&sample));

        sample.Initialize(hwnd);

        PlmSuspendCompleteT = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        PlmSignalResumeT = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        if (!PlmSuspendCompleteT || !PlmSignalResumeT)
        {
            return(1);
        }

        if (RegisterAppStateChangeNotification([](BOOLEAN quiesced, PVOID context)
        {
            if (quiesced)
            {
                ResetEvent(PlmSuspendCompleteT);
                ResetEvent(PlmSignalResumeT);

                // To ensure we use the main UI thread to process the notification, we self-post a message
                PostMessage(reinterpret_cast<HWND>(context), WM_USER, 0, 0);

                // To defer suspend, you must wait to exit this callback
                (void)WaitForSingleObject(PlmSuspendCompleteT, INFINITE);
            }
            else
            {
                SetEvent(PlmSignalResumeT);
            }
        }, hwnd, &hPLM))
            return(1);
    }

    sample.Run(&sample);

    sample.Uninitialize();

    UnregisterAppStateChangeNotification(hPLM);

    CloseHandle(PlmSuspendCompleteT);
    CloseHandle(PlmSignalResumeT);

    XGameRuntimeUninitialize();

    return(0);
}

/*F********************************************************************************/
/*!
    \Function WndProc

    \Description
        Windows Procedure called to process the messages from the window

    \Input  args   not used

    \Output LRESULT

    \Version 12/05/2019 (from Microsoft Samples)
*/
/********************************************************************************F*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Sample* sample = reinterpret_cast<Sample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_USER:
        if (sample)
        {
            sample->OnSuspending();

            // Complete deferral
            SetEvent(PlmSuspendCompleteT);

            (void)WaitForSingleObject(PlmSignalResumeT, INFINITE);

            sample->OnResuming();
        }
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
