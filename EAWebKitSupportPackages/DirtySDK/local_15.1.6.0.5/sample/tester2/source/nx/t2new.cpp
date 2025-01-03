/*H*************************************************************************************/
/*!
    \File t2new.cpp

    \Description
        Tester2 Host Application Framework

    \Copyright
        Copyright (c) Electronic Arts 2014.  ALL RIGHTS RESERVED.

    \Version    1.0 01/24/2014 (mclouatre) First Version
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

//EASTL New operator
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return new char[size];
}

//EASTL New operator
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return new char[size];
}