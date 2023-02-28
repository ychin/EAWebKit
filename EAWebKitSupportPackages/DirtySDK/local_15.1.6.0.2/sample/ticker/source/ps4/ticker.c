/*H*************************************************************************************/
/*!

    \File    ticker.c

    \Description
        This application demonstrates the PS4 Internet access capability using the
        Dirtysock stack. This version uses DHCP to acquire a dynamic IP address,
        DNS address and gateway address and then uses HTTP to get information
        from the Internet.

    \Notes
        None.

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2003.  ALL RIGHTS RESERVED.

    \Version    1.0      12/10/2000 (GWS) Initial version
    \Version    2.0      11/27/2002 (GWS) Revived from the dead
    \Version    2.1      02/23/2003 (JLB) Rewrote to use demo lib
    \Version    2.2      03/06/2003 (JLB) Updated to use netconn
    \Version    3.0      10/05/2005 (JLB) PS3 version
    \Version    3.1      11/19/2008 (mclouatre) Adding user data concept to mem group support
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsysmodule.h>

// dirtysock includes
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protohttp.h"

#include "DirtySDK/misc/weblog.h"

#include "tickerplatform.h"

/*** Defines ***************************************************************************/

#define TICKER_WEBLOG_ENABLED (DIRTYCODE_DEBUG && TRUE)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables


// Public variables


/*** Private Functions ******************************************************************/

/*F********************************************************************************/
/*!
    \Function _TickerLoadSysUtils

    \Description
        Load required Sony PRXs

    \Version 01/24/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _TickerLoadSysUtils(void)
{
    int32_t iResult;

    if ((iResult = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_AUTH)) != SCE_OK)
    {
        NetPrintf(("ticker: sceSysmoduleLoadModule(SCE_SYSMODULE_NP_AUTH) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ( sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG) != SCE_OK ) 
    {
        NetPrintf(("t2host: sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
}

/*F********************************************************************************/
/*!
    \Function _TickerUnloadSysUtils

    \Description
        Unload Sony PRXs

    \Version 01/24/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _TickerUnloadSysUtils(void)
{
    int32_t iResult;

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_AUTH)) != SCE_OK)
    {
        NetPrintf(("ticker: sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_AUTH) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    if ((iResult = sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG)) != SCE_OK)
    {
        NetPrintf(("t2host: sceSysmoduleUnloadModule(SCE_SYSMODULE_ERROR_DIALOG) failed (err=%s)\n", DirtyErrGetName(iResult)));
    }
}



/*** Public Functions ******************************************************************/


int main(int32_t argc, char *argv[])
{
    int32_t iAddr, iLen, iStatus, iTimeout, iCount;
    ProtoHttpRefT *pHttp;
    char strBuffer[1024];
    #if TICKER_WEBLOG_ENABLED
    WebLogRefT *pWebLog;
    int32_t iVerbose = 1;
    #endif

    #if TICKER_WEBLOG_ENABLED
    // create weblog module
    pWebLog = WebLogCreate(8192);
    #if 0
    // set to post to dev server
    WebLogConfigure(pWebLog, "eggplant.online.ea.com:8001", NULL);
    #endif
    // set weblog protohttp debugging to a high level
    WebLogControl(pWebLog, 'spam', iVerbose, 0, NULL);
    // hook in to netprintf debug output
    #if DIRTYCODE_LOGGING
    NetPrintfHook(WebLogDebugHook, pWebLog);
    #endif
    // start logging
    WebLogStart(pWebLog);
    #endif

    _TickerLoadSysUtils();

    PlatformInit();

    // start network
    NetConnStartup("");

    PlatformFinalizeInit();
    PlatformTick();

    // bring up the interface
    NetConnConnect(NULL, NULL, 0);

    // wait for network interface activation
    for (iTimeout = NetTick()+15*1000 ; ; )
    {
        // update network
        NetConnIdle();
        PlatformTick();

        // get current status
        iStatus = NetConnStatus('conn', 0, NULL, 0);
        if ((iStatus == '+onl') || ((iStatus >> 24) == '-'))
        {
            break;
        }

        // check for timeout
        if (iTimeout < (signed)NetTick())
        {
            NetPrintf(("ticker: timeout waiting for interface activation\n"));
            return(-1);
        }

        // give time to other threads
        NetConnSleep(500);
    }

    // check result code
    if ((iStatus = NetConnStatus('conn', 0, NULL, 0)) == '+onl')
    {
        NetPrintf(("ticker: interface active\n"));
    }
    else if ((iStatus >> 14) == '-')
    {
        NetPrintf(("ticker: error bringing up interface\n"));
        return(-11);
    }

    // broadband check (should return TRUE if broadband, else FALSE)
    NetPrintf(("iftype=%d\n", NetConnStatus('type', 0, NULL, 0)));
    NetPrintf(("broadband=%s\n", NetConnStatus('bbnd', 0, NULL, 0) ? "TRUE" : "FALSE"));

    // acquire and display address
    iAddr = NetConnStatus('addr', 0, NULL, 0);
    NetPrintf(("addr=%a\n", iAddr));

    // display mac address
    NetPrintf(("mac=%s\n", NetConnMAC()));

    // setup http module
    pHttp = ProtoHttpCreate(4096);

    // just keep working
    for ( iTimeout = NetTick()-1, iLen=-1, iCount = 0; iCount < 8; )
    {
        // see if its time to query
        if ((iTimeout != 0) && (NetTick() > (unsigned)iTimeout))
        {
            ProtoHttpGet(pHttp, "http://quote.yahoo.com/d/quotes.csv?s=^DJI,^SPC,^IXIC,ERTS,SNE,AOL,YHOO,^AORD,^N225,^FTSE&f=sl1c1&e=.csv", FALSE);
            iTimeout = NetTick()+20*1000;
            iLen = 0;
            iCount += 1; // count the attempt
        }

        // update protohttp
        ProtoHttpUpdate(pHttp);
        PlatformTick();

        // read incoming data into buffer
        if (iLen == 0)
        {
            if ((iLen = ProtoHttpRecvAll(pHttp, strBuffer, sizeof(strBuffer)-1)) > 0)
            {
                // null-terminate response
                strBuffer[iLen] = '\0';

                // print response
                NetPrintf(("ticker: received response\n"));
                NetPrintf(("%s", strBuffer));
            }
        }

        #if TICKER_WEBLOG_ENABLED
        WebLogUpdate(pWebLog);
        #endif

        NetConnIdle();
    }

    NetPrintf(("ticker: done\n"));

    // shut down HTTP
    ProtoHttpDestroy(pHttp);

    #if TICKER_WEBLOG_ENABLED
    // stop the logging
    WebLogStop(pWebLog);
    // give it five seconds to flush the remaining data and end the transaction gracefully
    for (iCount = 0; iCount < (5*1000); iCount += 100)
    {
        NetConnIdle();
        WebLogUpdate(pWebLog);
        NetConnSleep(100);
    }
    // unhook netprintf debug output *before* we destroy WebLog
    #if DIRTYCODE_LOGGING
    NetPrintfHook(NULL, NULL);
    #endif
    // destroy module
    WebLogDestroy(pWebLog);
    #endif

    // disconnect from the network
    NetConnDisconnect();

    // shutdown the network connections && destroy the dirtysock code
    NetConnShutdown(FALSE);

    PlatformShutdown();

    _TickerUnloadSysUtils();

    return(0);
}

void *DirtyMemAlloc(int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    return(malloc(iSize));
}

void DirtyMemFree(void *pMem, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    free(pMem);
}

