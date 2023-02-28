/*H********************************************************************************/
/*!
    \File    userapixboxone.h

    \Description
        This file is the extension for Xbox One specific features for the user api

    \Copyright
        Copyright (c) Electronic Arts 2017

    \Version 06/07/17 (eesponda)
*/
/********************************************************************************H*/

#ifndef _userapixboxone_h
#define _userapixboxone_h

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

/*** Type Definitions *************************************************************/

typedef struct UserApiAccessibilityT
{
    uint8_t     bTTS;               //!< Text to Speech enabled
    uint8_t     bSTT;               //!< Speech to Text enabled
    int16_t     iPersonaGender;     //!< Voice Gender
    char        strPersonaId[512];  //!< Voice Id
    char        strPersonaName[32]; //!< Voice Name
    char        strPersonaLang[8];  //!< Voice Language
} UserApiAccessibilityT;

#endif // _userapixboxone_h
