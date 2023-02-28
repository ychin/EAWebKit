/*H********************************************************************************/
/*!
    \File netconnios.m

    \Description
        Contain ios specific functions

    \Copyright
        Copyright (c) 2013 Electronic Arts Inc.

    \Version 10/18/2013 (tcho) Added to deal with Apple Objective C API
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCNetworkReachability.h>
#include <UIKit/UIDevice.h>
#include <uuid/uuid.h>

#include "DirtySDK/dirtysock/netconndefs.h"
#include "DirtySDK/util/base64.h"
#include "DirtySDK/dirtysock.h"

#include "DirtySDK/dirtysock/iphone/netconnios.h"
#include "netconncommon.h"

/*** Variable *********************************************************************/
static SCNetworkReachabilityRef _NetConn_Reachability_Ref = NULL;

/*** Functions Prototype **********************************************************/

/*** Private Functions ************************************************************/
/*F********************************************************************************/
/*!
    \Function    _NetConnReachabilitySetup
 
    \Description
        Setup reachability module
 
    \Output
        int32_t     - return negative if error encountered
 
    \Version 7/28/2014 (tcho)
 */
/********************************************************************************F*/
static int32_t _NetConnReachabilitySetup()
{
    struct sockaddr_in zeroAddr;
    memset(&zeroAddr, 0, sizeof(zeroAddr));
    
    zeroAddr.sin_len = sizeof(zeroAddr);
    zeroAddr.sin_family = AF_INET;
    
    _NetConn_Reachability_Ref = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr *) &zeroAddr);
    
    if (_NetConn_Reachability_Ref == NULL)
    {
        NetPrintf(("netconnios: unable to create network reachability module.\n"));
        return(-1);
    }
    
    return 0;
}

/*F********************************************************************************/
/*!
    \Function    _NetConnReachabilityCleanUp
 
    \Description
        Cleanup reachability module
 
    \Output
        None
 
    \Version 7/28/2014 (tcho) First Version.
 */
/********************************************************************************F*/
static void _NetConnReachabilityCleanUp()
{
    if (_NetConn_Reachability_Ref != NULL)
    {
        CFRelease(_NetConn_Reachability_Ref);
        _NetConn_Reachability_Ref = NULL;
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetConnReachabilityCheckConnectivityLevel
 
    \Description
        Determine netwrok connectivity level
        For reference look at Apple's Reachability Example
        https://developer.apple.com/library/ios/samplecode/reachability/introduction/intro.html
 
    \Output
        uint32_t Return network interface type
 
    \Version 7/28/2014 (tcho)
 */
/********************************************************************************F*/
static uint32_t _NetConnReachabilityCheckInterfaceType()
{
    uint32_t uIfType = NETCONN_IFTYPE_NONE;
    
    if (_NetConn_Reachability_Ref != NULL)
    {
        SCNetworkReachabilityFlags nrFlags;
        
        if (SCNetworkReachabilityGetFlags(_NetConn_Reachability_Ref, &nrFlags))
        {
            if ((nrFlags & kSCNetworkReachabilityFlagsReachable) != 0)
            {
                if ((nrFlags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
                {
                    uIfType = NETCONN_IFTYPE_WIRELESS;
                }
            
                if (((nrFlags & kSCNetworkReachabilityFlagsConnectionOnDemand) != 0) || ((nrFlags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0))
                {
                    if ((nrFlags & kSCNetworkReachabilityFlagsInterventionRequired) == 0)
                    {
                        //Wifi Access
                        uIfType = NETCONN_IFTYPE_WIRELESS;
                    }
                }
            
                if ((nrFlags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
                {
                    //Cellular Access
                    uIfType = NETCONN_IFTYPE_CELL;
                }
            }
        }
    }
    else
    {
        NetPrintf(("netconnios: _NetConnReachibilityCheckConnectivityLevel() Reachability Module was not created.\n"));
    }

    return (uIfType);
}

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function    NetConnStartupIos
 
    \Description
      Ios NetConnStartup function.
 
    \Input pParams  - (Not used at the moment)
 
    \Output
        int32_t     - return negative when error encountered
 
    \Version 7/28/2014 (tcho)
 */
/********************************************************************************F*/
int32_t NetConnStartupIos(const char* pParams)
{
    if (_NetConnReachabilitySetup() != 0)
    {
        NetPrintf(("netconnios: unable to start Reachability.\n"));
        return(NETCONN_ERROR_PLATFORM_SPECIFIC);
    }
    
    return 0;
}

/*F********************************************************************************/
/*!
    \Function    NetConnShutdownIos
 
    \Description
        Ios NetConnShutdown function.
 
    \Input uShutdownFlags  - (Not used at the moment)
 
    \Output
        int32_t     - return negative when error encountered
 
    \Version 7/28/2014 (tcho) First Version.
 */
/********************************************************************************F*/
int32_t NetConnShutdownIos(uint32_t uShutdownFlags)
{
    _NetConnReachabilityCleanUp();
    
    return 0;
}


/*F********************************************************************************/
/*!
    \Function    NetConnStatusIos

    \Description
        Different selectors return different status attributes.

    \Input pRef     - common module state
    \Input iKind    - status selector ('open', 'conn', 'onln')
    \Input iData    - (optional) selector specific
    \Input *pBuf    - (optional) pointer to output buffer
    \Input iBufSize - (optional) size of output buffer

    \Output
        int32_t     - selector specific

    \Notes
        iKind can be one of the following:

        \verbatim
            hwid: Returns Apple Vendor Id base64 encoded. pBuf must be at least Base64EncodedSize(16) + 1 in length
            ncon: Returns TRUE if there is network connectivity or else returns FALSE
            type: Returns the current network interface type
        \endverbatim

    Unhandled selectors are passed through to NetConnCommonStatus()

    \Version 10/18/2013 (tcho) 
*/
/********************************************************************************F*/
int32_t NetConnStatusIos(NetConnCommonRefT *pCommonRef, int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize)
{
    //Returns a Apple device id for vendors
    if (iKind == 'hwid')
    {
        //Return Vendor Id for IOS 6.0 or greater
        if (NETCONN_IOS_VERSION_GREATER_THAN_OR_EQUAL_TO(@"6.0"))
        {
            //Note: UUID is a 16 byte char array
            if (pBuf != NULL && iBufSize >= (int32_t)(Base64EncodedSize(sizeof(uuid_t)) + 1))
            {
                //Grabs the current Device
                UIDevice *currentDevice = [UIDevice currentDevice];
                
                //Get the Id for Vendor Property
                NSUUID *vendorId = [currentDevice identifierForVendor];
                
                if (vendorId != nil)
                {
                    char tempBuf[sizeof(uuid_t)];
                    //Convert this into a null terminated string
                    [vendorId getUUIDBytes:*(uuid_t *)&tempBuf];

                    Base64Encode2(tempBuf, sizeof(tempBuf), pBuf, iBufSize);

                    return(0);
                }
                else
                {
                    NetPrintf(("netconnios: NetConnStatusIos('hwid') Cannot retrieve Vendor Id \n"));
                    return(-2);
                }
            }
            else
            {
                NetPrintf(("netconnios: NetConnStatus('hwid') Invalid Arguments \n"));
                return(-1);
            }
        }
        //Returns the mac address if IOS version is less than 6.0
        else
        {
            return(SocketInfo(NULL,'macx', 0, pBuf, iBufSize));
        }
    }
    else if (iKind == 'ncon')
    {
        if (_NetConnReachabilityCheckInterfaceType() != NETCONN_IFTYPE_NONE)
        {
            return (TRUE);
        }
        else
        {
            return (FALSE);
        }
    }
    else if (iKind == 'type')
    {
        return (_NetConnReachabilityCheckInterfaceType());
    }
    
    // pass unrecognized options to NetConnCommon
    return(NetConnCommonStatus(pCommonRef, iKind, iData, pBuf, iBufSize));
}
