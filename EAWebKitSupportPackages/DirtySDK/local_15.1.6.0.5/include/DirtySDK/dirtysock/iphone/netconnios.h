/*H********************************************************************************/
/*!
    \File netconnios.h

    \Description
        Contain ios specific functions

    \Copyright
        Copyright (c) 2013 Electronic Arts

    \Version 10/18/2010 (tcho) added to deal with Apple Objective C API
 */
/********************************************************************************H*/

#ifndef _netconnios_h
#define _netconnios_h

/*!
\Moduledef NetConnDefs NetConnDefs
\Modulemember DirtySock
*/
//@{
/*** Include files ****************************************************************/
#include "DirtySDK/platform.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/
#define NETCONN_IOS_VERSION_EQUAL_TO(v)                    ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define NETCONN_IOS_VERSION_GREATER_THAN(v)                ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define NETCONN_IOS_VERSION_GREATER_THAN_OR_EQUAL_TO(v)    ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define NETCONN_IOS_VERSION_LESS_THAN(v)                   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define NETCONN_IOS_VERSION_LESS_THAN_OR_EQUAL_TO(v)       ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

/*** Type Definitions *************************************************************/
typedef struct NetConnCommonRefT NetConnCommonRefT; //! forward declaration

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

int32_t NetConnStartupIos(const char *pParams);
int32_t NetConnStatusIos(NetConnCommonRefT *pCommonRef, int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize);
int32_t NetConnShutdownIos(uint32_t uShutdownFlags);

#ifdef __cplusplus
}
#endif

//@}

#endif // _netconnios_h
