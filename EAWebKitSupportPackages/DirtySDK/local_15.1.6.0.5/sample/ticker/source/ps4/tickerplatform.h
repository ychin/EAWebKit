/*H*************************************************************************************/
/*!

    \File    tickerplatform.h

    \Description
        Header file to wrap SystemEventMessageDispatcher/EACUP functionality for PS4

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2013.  ALL RIGHTS RESERVED.

    \Version    1.0      22/070/2013 (amakoukji) Initial version
*/
/*************************************************************************************H*/

/*** Public Functions ******************************************************************/

#ifndef _tickerplatform_h
#define _tickerplatform_h

#ifdef __cplusplus
extern "C" {
#endif
void PlatformInit(void);
void PlatformFinalizeInit(void);
void PlatformTick(void);
void PlatformShutdown(void);

#ifdef __cplusplus
}
#endif

#endif // _tickerplatform_h