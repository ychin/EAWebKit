/*H********************************************************************************/
/*!
    \File testermoduleshost.c

    \Description
        PS3 specific module startup.

    \Copyright
        Copyright (c) 2006 Electronic Arts Inc.

    \Version 10/15/2012 (akirchner) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"
#include "testermodules.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function TesterModulesRegisterAllCommands

    \Description
        Register all ps4-specific modules

    \Input *pState - module state
    
    \Output 0=success, error code otherwise

    \Version 10/15/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t TesterModulesRegisterPlatformCommands(TesterModulesT *pState)
{
    TesterModulesRegister(pState, "voice",   &CmdVoice);
    return(TESTERMODULES_ERROR_NONE);
}
