/*H********************************************************************************/
/*!
    \File dirtylibps4.c

    \Description
        Platform-specific support library for network code.  Supplies simple time,
        debug printing, and mutex functions.

    \Copyright
        Copyright (c) 2013 Electronic Arts Inc.

    \Version 01/17/2013 (amakoukji) First version; a vanilla port to ps4 from unix
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <net.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtythread.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

// idle thread state
static volatile int32_t g_idlelife = -1;

// Time Stamp Conuter Freuency
static uint64_t _NetLib_uTimeStampFreq = 0;

// static printf buffer to avoid using dynamic allocation
#if DIRTYCODE_LOGGING
static char _NetLib_aPrintfMem[4096];
#endif


/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _NetTickCount

    \Description
        Return millisecond/microsecond tick count, scale based on input.

    \Input
        uScale      - output scale (1000 for ms, 1000*1000 for us)

    \Output
        uint64_t    - tick count

    \Version 12/18/2013 (tcho) rdtsc version
*/
/********************************************************************************F*/
static uint64_t _NetTickCount(uint32_t uScale)
{
    uint64_t uCurUsec;

    if (_NetLib_uTimeStampFreq == 0)
    {
        _NetLib_uTimeStampFreq = sceKernelGetProcessTimeCounterFrequency();
    }

    // convert timestamp to microseconds/milliseconds based on output scale
    uCurUsec = sceKernelGetProcessTimeCounter();
    return((uCurUsec*uScale)/_NetLib_uTimeStampFreq);
}

/*F********************************************************************************/
/*!
    \Function _NetLibThread

    \Description
        Thread to handle special library tasks.

    \Input *pArg    - pointer to argument

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
static void _NetLibThread(void *pArg)
{
    char strThreadId[32];

    // get the thread id
    DirtyThreadGetThreadId(strThreadId, sizeof(strThreadId));

    // show we are running
    NetPrintf(("dirtylibps4: idle thread running (thid=%s)\n", strThreadId));
    g_idlelife = 1;

    // run while we have sema
    while (g_idlelife == 1)
    {
        // call idle functions
        NetIdleCall();

        // wait for next tick
        sceKernelUsleep(50*1000);
    }

    // report termination
    NetPrintf(("dirtylibps4: idle thread exiting\n"));

    // show we are dead
    g_idlelife = 0;
}


/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function NetLibCreate

    \Description
        Initialize the network library module.

    \Input iThreadPrio        - priority to start the _NetLibThread with
    \Input iThreadStackSize   - stack size to start the _NetLibThread with (in bytes)
    \Input iThreadCpuAffinity - cpu affinity to start the _NetLibThread with, 0=SCE_KERNEL_CPUMASK_USER_ALL otherwise a mask of cores to use

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
void NetLibCreate(int32_t iThreadPrio, int32_t iThreadStackSize, int32_t iThreadCpuAffinity)
{
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    // init common functionality
    NetLibCommonInit();

    // init idlelife tracker
    g_idlelife = -1;

    // configure thread parameters
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "NetLib";
    ThreadConfig.iPriority = iThreadPrio;
    ThreadConfig.iAffinity = iThreadCpuAffinity;
    ThreadConfig.iVerbosity = 1;

    // create a worker thread
    if ((iResult = DirtyThreadCreate(_NetLibThread, NULL, &ThreadConfig)) == 0)
    {
        // wait for thread startup
        while (g_idlelife == -1)
        {
            sceKernelUsleep(100);
        }
    }
    else
    {
        NetPrintf(("dirtylibps4: unable to start idle thread (err=%d)\n", iResult));
        g_idlelife = 0;
    }

    #if DIRTYCODE_LOGGING
    // set explicit printf buffer to avoid dynamic malloc() use
    setvbuf(stdout, _NetLib_aPrintfMem, _IOLBF, sizeof(_NetLib_aPrintfMem));
    #endif
}

/*F********************************************************************************/
/*!
    \Function NetLibDestroy

    \Description
        Destroy the network library module.

    \Input uShutdownFlags   - NET_SHUTDOWN_* flags

    \Version 01/23/2013 (amakoukji)
*/
/********************************************************************************F*/
void NetLibDestroy(uint32_t uShutdownFlags)
{
    // signal a shutdown when the thread is running
    if (g_idlelife == 1)
    {
        g_idlelife = 2;

        // wait for thread to terminate
        while (g_idlelife > 0)
        {
            sceKernelUsleep(1);
        }
    }

    // shut down common functionality
    NetLibCommonShutdown();
}

/*F********************************************************************************/
/*!
    \Function NetTick

    \Description
        Return some kind of increasing tick count with millisecond scale (does
        not need to have millisecond precision, but higher precision is better).

    \Output
        uint32_t    - millisecond tick count

    \Version 12/18/2013 (tcho) rdtsc version
*/
/********************************************************************************F*/
uint32_t NetTick(void)
{
    return((uint32_t)(_NetTickCount(1000)));
}

/*F********************************************************************************/
/*!
    \Function NetTickUsec

    \Description
        Return increasing tick count in microseconds.  Used for performance timing
        purposes.

    \Output
        uint64_t    - microsecond tick count

    \Version 01/30/2015 (jbrookes)
*/
/********************************************************************************F*/
uint64_t NetTickUsec(void)
{
    return(_NetTickCount(1000000));
}

/*F********************************************************************************/
/*!
    \Function NetLocalTime

    \Description
        This converts the input GMT time to the local time as specified by the
        system clock.  This function follows the re-entrant localtime_r function
        signature.

    \Input *pTm         - storage for localtime output
    \Input uElap        - GMT time

    \Output
        struct tm *     - pointer to localtime result

    \Version 04/23/2008 (jbrookes)
*/
/********************************************************************************F*/
struct tm *NetLocalTime(struct tm *pTm, uint64_t uElap)
{
    time_t uTimeT = (time_t)uElap;
    localtime_s(&uTimeT, pTm);
    return(pTm);
}

/*F********************************************************************************/
/*!
    \Function NetPlattimeToTime

    \Description
        This converts the input platform-specific time data structure to the
        generic time data structure.

    \Input *pTm         - generic time data structure to be filled by the function
    \Input *pPlatTime   - pointer to the platform-specific data structure

    \Output
        struct tm *     - NULL=failure; else pointer to user-provided generic time data structure

    \Notes
        pPlatTime is expected to point to a SceKernelTimeval format time

    \Version 05/08/2010 (mclouatre)
*/
/********************************************************************************F*/
struct tm *NetPlattimeToTime(struct tm *pTm, void *pPlatTime)
{
    SceKernelTimeval kernelTime = *(SceKernelTimeval *)pPlatTime;
    localtime_s(&kernelTime.tv_sec, pTm);
    return(pTm);
}

/*F********************************************************************************/
/*!
    \Function NetPlattimeToTimeMs
 
    \Description
        This function retrieves the current date time and fills in the
        generic time data structure prrovided. It has the option of returning millisecond
        which is not part of the generic time data structure

    \Input *pTm         - generic time data structure to be filled by the function
    \Input *pImSec      - output param for milisecond to be filled by the function (optional can be NULL)

    \Output
        struct tm *     - NULL=failure; else pointer to user-provided generic time data structure

    \Version 09/16/2014 (tcho)
*/
/********************************************************************************F*/
struct tm *NetPlattimeToTimeMs(struct tm *pTm , int32_t *pImSec)
{
    void *pPlatTime;
    int32_t iMsec;
    
    SceKernelTimeval kernalTime;
    sceKernelGettimeofday(&kernalTime);
    iMsec = kernalTime.tv_usec/1000;
    pPlatTime = (void *)&kernalTime;

    if (pImSec != NULL)
    {
        *pImSec = iMsec;
    }
   
    if (pTm == NULL)
    {
        return(NULL);
    }

    return(NetPlattimeToTime(pTm, pPlatTime));
}

/*F********************************************************************************/
/*!
    Function NetTime

    \Description
    This function replaces the standard library time() function. Main
    differences are the missing pointer parameter (not needed) and the uint64_t
    return value. The function returns 0 on unsupported platforms vs time which
    returns -1.

    \Output
        uint64_t    - number of elapsed seconds since Jan 1, 1970.

    \Version 01/12/2005 (gschaefer)
*/
/********************************************************************************F*/
uint64_t NetTime(void)
{
    return((uint64_t)time(NULL));
}
