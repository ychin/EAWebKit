/*H********************************************************************************/
/*!
    \File testermoduleshost.c

    \Description
        Xbox One specific module startup.

    \Copyright
        Copyright (c) 2013 Electronic Arts Inc.

    \Version 06/07/2013 (mclouatre) First Version
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
    \Function TesterModulesRegisterPlatformCommands

    \Description
        Register all modules for the Xbox One platform

    \Input *pState - module state

    \Output 0=success, error code otherwise

    \Version 06/07/2031 (mclouatre)
*/
/********************************************************************************F*/
int32_t TesterModulesRegisterPlatformCommands(TesterModulesT *pState)
{
    TesterModulesRegister(pState, "voice",   &CmdVoice);

    return(TESTERMODULES_ERROR_NONE);
}
