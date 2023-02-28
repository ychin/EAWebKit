/*H********************************************************************************/
/*!
    \File dirtygif.h

    \Description
        Routines to parse and decode a GIF file.

    \Copyright
        Copyright (c) 2003-2005 Electronic Arts Inc.

    \Version 11/13/2003 (jbrookes) First Version
*/
/********************************************************************************H*/

#ifndef _dirtygif_h
#define _dirtygif_h

/*!
\Moduledef DirtyGif DirtyGif
\Modulemember Graph
*/
//@{

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

/*** Defines **********************************************************************/

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! structure to represent a parsed gif
typedef struct DirtyGifHdrT
{
    // image info
    const uint8_t *pImageData;  //!< pointer to start of gif bits
    const uint8_t *pImageEnd;   //!< pointer to end of gif bits
    uint32_t uWidth;            //!< image width
    uint32_t uHeight;           //!< image height

    // palette info
    const uint8_t *pColorTable; //!< pointer to gif color table
    uint32_t uNumColors;        //!< number of color table entries

    // misc info
    uint8_t bInterlaced;        //!< flag indicating an interlaced image
    uint8_t bHasAlpha;          //!< flag indicating transparent color present
    uint8_t uTransColor;        //!< which color index is transparent
    uint8_t uPad;
} DirtyGifHdrT;

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// identify if image is a gif or not
DIRTYCODE_API int32_t DirtyGifIdentify(const uint8_t *pImageData, uint32_t uImageLen);

// parse GIF into something we can use
DIRTYCODE_API int32_t DirtyGifParse(DirtyGifHdrT *pGifHdr, const uint8_t *pGifData, const unsigned char *pGifEnd);

// decode a GIF palette to 32bit color table
DIRTYCODE_API int32_t DirtyGifDecodePalette(DirtyGifHdrT *pGifHdr, uint8_t *pPalette, unsigned char *pPaletteEnd, unsigned char uAlpha);

// decode a GIF image to 8bit paletted image
DIRTYCODE_API int32_t DirtyGifDecodeImage(DirtyGifHdrT *pGifHdr, uint8_t *pImageData, int32_t iBufWidth, int32_t iBufHeight, uint32_t bVflip);

// decode a GIF image to 32bit direct-color image
DIRTYCODE_API int32_t DirtyGifDecodeImage32(DirtyGifHdrT *pGifHdr, uint8_t *pImageData, int32_t iBufWidth, int32_t iBufHeight, uint32_t bVflip);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtygif_h
