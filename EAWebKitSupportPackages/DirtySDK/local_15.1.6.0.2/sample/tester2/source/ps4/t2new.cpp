/*H*************************************************************************************/
/*!

    \File t2new.cpp

    \Description
        Tester2 Host Application Framework

    \Copyright
        Copyright (c) Electronic Arts 2004.  ALL RIGHTS RESERVED.

    \Version    1.0 12/07/2013 (amakoukji) First Version
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdlib.h>
#include "EASTL/allocator.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private Functions *****************************************************************/

/*** Public Functions ******************************************************************/

// Extremely simple definitions of new for EA Base
void* operator new[](size_t size, unsigned long, unsigned long, char const*, int, unsigned int, char const*, int)
{
    return operator new(size);
}
void* operator new[](size_t size, char const*, int, unsigned int, char const*, int)
{
    return operator new(size);
}

