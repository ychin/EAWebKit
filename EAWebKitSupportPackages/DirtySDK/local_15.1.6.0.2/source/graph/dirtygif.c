/*H********************************************************************************/
/*!
    \File dirtygif.c

    \Description
        Routines to decode a GIF into a raw image and palette.  These routines
        were written from scratch based on the published specifications and visual
        inspection of some public-domain decoders.

    \Copyright
        Copyright (c) 2003-2005 Electronic Arts Inc.

    \Version 11/13/2003 (jbrookes) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/graph/dirtygif.h"

/*** Defines **********************************************************************/

#define DIRTYGIF_MAXBITS    (12)                            //!< max LZW code size
#define DIRTYGIF_MAXCODE    ((1 << DIRTYGIF_MAXBITS) - 1)   //!< max code value

/*** Macros ***********************************************************************/

//! macro to validate that enough space exists in the file for a particular header
#define DIRTYGIF_Validate(_pData, _pEnd, _size, _retcode)   \
{                                                           \
    if (((_pEnd) - (_pData)) < (_size))                     \
    {                                                       \
        return(_retcode);                                   \
    }                                                       \
}

#define DIRTYGIF_ClrCode(_iSize)  (1 << (iSize) + 0)
#define DIRTYGIF_EndCode(_iSize)  (1 << (iSize) + 1)
#define DIRTYGIF_NewCode(_iSize)  (1 << (iSize) + 2)

/*** Type Definitions *************************************************************/

//! decoder state
typedef struct DirtyGifDecoderT
{
    int32_t iCurCodeSize;                       //!< current code size, in bits
    int32_t iClearCode;                         //!< value of a clear code
    int32_t iEndingCode;                        //!< value of an ending code
    int32_t iNewCodes;                          //!< first available code
    int32_t iTopSlot;                           //!< highest code for current code size
    int32_t iSlot;                              //!< last read code

    int32_t iBytesLeft;                         //!< number of bytes left in block
    int32_t iBitsLeft;                          //!< number of bits left in block
    uint8_t uCurByte;                           //!< current byte
    const uint8_t *pByteStream;                 //!< pointer to byte stream

    uint8_t uSuffixTable[DIRTYGIF_MAXCODE+1];   //!< suffix table
    uint32_t uPrefixTable[DIRTYGIF_MAXCODE+1];  //!< prefix table
    
    uint8_t uStack[DIRTYGIF_MAXCODE+1];         //!< code stack
    uint8_t *pStackPtr;                         //!< current stack pointer
    uint8_t *pStackEnd;                         //!< end of stack

    int32_t iCode;                              //!< most recent table code
    int32_t iCodeBits;                          //!< number of bits used for code
    int32_t iCode1;
    int32_t iCode0;                                      
    int32_t iCodeRaw;                           //!< raw code read from data

    const uint8_t *pCodeData;                   //!< current image data pointer
    const uint8_t *pEndCodeData;                //!< end of image data
    int32_t uHeight;                            //!< image height
    int32_t uWidth;                             //!< image width
    uint8_t *pScanLine;                         //!< start of current scan line                 
    uint8_t *pScanLineEnd;                      //!< end of current scan line
    uint8_t *pBufferEnd;                        //!< end of current buffer line
    int32_t iPass;                              //!< pass for interlaced images
    int32_t iIndex;                             //!< which row within pass
    int32_t iStep;                              //!< interlace step
    int32_t iBufHeight;                         //!< output buffer height
    uint8_t *pImageData;                        //!< output image buffer (byte array)

} DirtyGifDecoderT;

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

static const int32_t stepsize[] = { 8, 8, 4, 2, 0, 1, 0 };
static const int32_t startrow[] = { 0, 4, 2, 1, 0, 0, 0 };


/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _DirtyGifParseColorTable

    \Description
        Parse the color table in GIF header.

    \Input *pGifHdr     - pointer to GIF header
    \Input *pGifData    - pointer to current location parsing GIF data
    \Input *pGifEnd     - pointer past end of GIF data
    \Input uBits        - byte containing color table info

    \Version 11/13/2003 (jbrookes)
*/
/********************************************************************************F*/
static const uint8_t *_DirtyGifParseColorTable(DirtyGifHdrT *pGifHdr, const uint8_t *pGifData, const uint8_t *pGifEnd, uint32_t uBits)
{
    if (uBits & 0x80)
    {
        // get number of colors
        pGifHdr->uNumColors = 2 << (uBits & 0x7);

        // validate color table
        if ((pGifEnd - pGifData) < (signed)(pGifHdr->uNumColors*3))
        {
            return(NULL);
        }

        // ref color table
        pGifHdr->pColorTable = pGifData;
        pGifData += pGifHdr->uNumColors*3;
    }

    return(pGifData);
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifParseHeader

    \Description
        Parse the GIF header.

    \Input *pGifHdr     - [out] pointer to GIF header to fill in
    \Input *pGifData    - pointer to GIF data
    \Input *pGifEnd     - pointer past the end of GIF data

    \Output
        int32_t         - nonnegative=success, negative=error

    \Version 11/13/2003 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyGifParseHeader(DirtyGifHdrT *pGifHdr, const uint8_t *pGifData, const uint8_t *pGifEnd)
{
    // iClearCode header
    ds_memclr(pGifHdr, sizeof(*pGifHdr));

    // validate and skip signature
    DIRTYGIF_Validate(pGifData, pGifEnd, 6, -1);
    if (memcmp(pGifData, "GIF87a", 6) && memcmp(pGifData, "GIF89a", 6))
    {
        return(-2);
    }
    pGifData += 6;

    // validate logical screen descriptor
    DIRTYGIF_Validate(pGifData, pGifEnd, 7, -3);
    
    // parse global color table info from logical screen descriptor
    if ((pGifData = _DirtyGifParseColorTable(pGifHdr, pGifData+7, pGifEnd, pGifData[4])) == NULL)
    {
        return(-4);
    }

    // process extensions, if any
    while(((pGifEnd - pGifData) > 2) && (pGifData[0] == 0x21))
    {
        uint8_t uKind = pGifData[1];
        pGifData += 2;
        
        // validate extension
        DIRTYGIF_Validate(pGifData, pGifEnd, pGifData[0]+1, -5);
        
        // parse alpha extension
        if ((uKind == 0xf9) && (pGifData[1] & 0x1))
        {
            pGifHdr->uTransColor = pGifData[4];
            pGifHdr->bHasAlpha = 1;
        }

        // parse any other extensions and ignore them
        while(pGifData[0] != 0)
        {
            // validate extension
            DIRTYGIF_Validate(pGifData, pGifEnd, pGifData[0]+1, -6);
            pGifData += pGifData[0]+1;
        }

        pGifData++;
    }

    // validate image descriptor
    DIRTYGIF_Validate(pGifData, pGifEnd, 10, -7);
    if (pGifData[0] != 0x2c)
    {
        return(-8);
    }
    
    // get width and height from image descriptor
    pGifHdr->uWidth = pGifData[5] | pGifData[6] << 8;
    pGifHdr->uHeight = pGifData[7] | pGifData[8] << 8;

    // determine if it is an interlaced image
    pGifHdr->bInterlaced = (pGifData[9] & 0x40) != 0;

    // parse local color table info from image descriptor
    if ((pGifData = _DirtyGifParseColorTable(pGifHdr, pGifData+10, pGifEnd, pGifData[9])) == NULL)
    {
        return(-8);
    }

    // save pointer to image data
    pGifHdr->pImageData = pGifData;
    pGifHdr->pImageEnd = pGifEnd;

    // return success
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifResetDecoder

    \Description
        Reset the LZW decoder.

    \Input *pDecoder    - pointer to decoder state

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static void _DirtyGifResetDecoder(DirtyGifDecoderT *pDecoder)
{
    pDecoder->iCurCodeSize = pDecoder->iCodeBits + 1;
    pDecoder->iSlot = pDecoder->iNewCodes;
    pDecoder->iTopSlot = 1 << pDecoder->iCurCodeSize;
    pDecoder->iCode = 0;
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifInitDecoder

    \Description
        Init the LZW decoder.

    \Input *pDecoder    - pointer to decoder state
    \Input *pGifHdr     - pointer to GIF header
    \Input *pImageData  - pointer to output pixel buffer
    \Input iStep        - output step size
    \Input iHeight      - output buffer height

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyGifInitDecoder(DirtyGifDecoderT *pDecoder, DirtyGifHdrT *pGifHdr, uint8_t *pImageData, int32_t iStep, int32_t iHeight)
{
    ds_memclr(pDecoder, sizeof(*pDecoder));
    pDecoder->pCodeData = pGifHdr->pImageData;
    pDecoder->pEndCodeData = pGifHdr->pImageEnd;
    pDecoder->uHeight = pGifHdr->uHeight;
    pDecoder->uWidth = pGifHdr->uWidth;
    pDecoder->iStep = iStep;
    pDecoder->iBufHeight = iHeight;
    pDecoder->pImageData = pImageData;

    // set starting imaging location
    pDecoder->iPass = (pGifHdr->bInterlaced ? 0 : 5);
    pDecoder->iIndex = startrow[pDecoder->iPass];
    pDecoder->pScanLine = NULL;

    // get the initial code length
    if (pDecoder->pCodeData == pDecoder->pEndCodeData)
    {
       return(-2);
    }
    pDecoder->iCodeBits = *pDecoder->pCodeData++;
    if ((pDecoder->iCodeBits < 2) || (pDecoder->iCodeBits > 9))
    {
        return(-2);
    }

    // setup for decoding
    pDecoder->iBytesLeft = pDecoder->iBitsLeft = 0;
    pDecoder->iClearCode = 1 << pDecoder->iCodeBits;
    pDecoder->iEndingCode = pDecoder->iClearCode + 1;
    pDecoder->iNewCodes = pDecoder->iEndingCode + 1;
    _DirtyGifResetDecoder(pDecoder);

    // protect against missing iClearCode code
    pDecoder->iCode0 = pDecoder->iCode1 = 0;

    // init stack
    pDecoder->pStackPtr = pDecoder->uStack;
    pDecoder->pStackEnd = pDecoder->uStack+sizeof(pDecoder->uStack);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifGetByte

    \Description
        Get next byte from current block.

        If the current block is exhausted, this function advances to the next block, and
        returns the first byte from that block.

    \Input *pDecoder    - pointer to decoder state

    \Output
        int32_t         - zero

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyGifGetByte(DirtyGifDecoderT *pDecoder)
{
    // have we consumed all bytes in this block?
    if (pDecoder->iBytesLeft <= 0)
    {
        // get byte length of next block
        pDecoder->iBytesLeft = *pDecoder->pCodeData++;
        
        // validate block
        DIRTYGIF_Validate(pDecoder->pCodeData, pDecoder->pEndCodeData, pDecoder->iBytesLeft, -1);
        
        // point to bytestream for this block
        pDecoder->pByteStream = pDecoder->pCodeData;
        
        // skip decode pointer past block
        pDecoder->pCodeData += pDecoder->iBytesLeft;
    }

    // get next byte from block
    pDecoder->uCurByte = *pDecoder->pByteStream++;
    pDecoder->iBytesLeft--;
    
    // return success to caller
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifGetNextCode

    \Description
        Get next code from the current block.

    \Input *pDecoder    - pointer to decoder state

    \Output
        int32_t         - TRUE if we should continue decoding, else FALSE if we've
                          reached the end code.

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyGifGetNextCode(DirtyGifDecoderT *pDecoder)
{
    // out of bits?
    if (pDecoder->iBitsLeft == 0)
    {
        if (_DirtyGifGetByte(pDecoder) < 0)
        {
            return(-1);
        }
        
        pDecoder->iBitsLeft += 8;
    }

    // add from bit bucket
    pDecoder->iCodeRaw = pDecoder->uCurByte >> (8 - pDecoder->iBitsLeft);

    // fill out the code
    while (pDecoder->iCurCodeSize > pDecoder->iBitsLeft)
    {
        if (_DirtyGifGetByte(pDecoder) < 0)
        {
            return(-1);
        }
        
        pDecoder->iCodeRaw |= pDecoder->uCurByte << pDecoder->iBitsLeft;
        pDecoder->iBitsLeft += 8;
    }
    
    // update bits left, and mask to code range
    pDecoder->iBitsLeft -= pDecoder->iCurCodeSize;
    pDecoder->iCodeRaw &= (1 << pDecoder->iCurCodeSize) - 1;

    // check for end of data
    return((pDecoder->iCodeRaw == pDecoder->iEndingCode) ? 0 : 1);
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifUpdateDecoder

    \Description
        Update decoder state.

    \Input *pDecoder    - pointer to decoder state

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static void _DirtyGifUpdateDecoder(DirtyGifDecoderT *pDecoder)
{
    // set up for decode
    pDecoder->iCode = pDecoder->iCodeRaw;

    // if we get a bogus code use the last valid code read instead
    if (pDecoder->iCode >= pDecoder->iSlot)
    {
        pDecoder->iCode = pDecoder->iCode0;
        if (pDecoder->pStackPtr < pDecoder->pStackEnd)
        {
            *pDecoder->pStackPtr++ = (uint8_t)pDecoder->iCode1;
        }
    }

    // push characters onto stack
    while (pDecoder->iCode >= pDecoder->iNewCodes)
    {
        if (pDecoder->pStackPtr >= pDecoder->pStackEnd)
        {
            // overflow error
            break;
        }
        *pDecoder->pStackPtr++ = pDecoder->uSuffixTable[pDecoder->iCode];
        pDecoder->iCode = pDecoder->uPrefixTable[pDecoder->iCode];
    }

    // push last char onto the uStack
    if (pDecoder->pStackPtr < pDecoder->pStackEnd)
    {
        *pDecoder->pStackPtr++ = (uint8_t)pDecoder->iCode;
    }

    // set up new prefix and uSuffixTable
    if (pDecoder->iSlot < pDecoder->iTopSlot)
    {
        pDecoder->iCode1 = pDecoder->iCode;
        pDecoder->uSuffixTable[pDecoder->iSlot] = (uint8_t)pDecoder->iCode1;
        pDecoder->uPrefixTable[pDecoder->iSlot++] = pDecoder->iCode0;
        pDecoder->iCode0 = pDecoder->iCodeRaw;
    }

    // if required iSlot number is greater than bit size allows, increase bit size (up to 12 bits)
    if ((pDecoder->iSlot >= pDecoder->iTopSlot)  && (pDecoder->iCurCodeSize < 12))
    {
        pDecoder->iTopSlot <<= 1;
        pDecoder->iCurCodeSize++;
    } 
}

/*F********************************************************************************/
/*!
    \Function _DirtyGifUpdateBitmap

    \Description
        Write decoded string to output bitmap buffer.

    \Input *pDecoder    - pointer to decoder state
    \Input bVflip       - if TRUE, flip image vertically

    \Output
        int32_t         - one to continue, zero to abort

    \Version 11/20/2003 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _DirtyGifUpdateBitmap(DirtyGifDecoderT *pDecoder, uint32_t bVflip)
{
    uint8_t cPixel;

    // pop decoded string off stack into output buffer
    for ( ; pDecoder->pStackPtr > pDecoder->uStack; )
    {
        // see if we need to calc scanline start
        if (pDecoder->pScanLine == NULL)
        {
            // if we've hit the output buffer height, quit early
            if (pDecoder->iIndex == pDecoder->iBufHeight)
            {
                return(0);
            }
            
            // calc the new line start
            if (bVflip)
            {
                pDecoder->pScanLine = pDecoder->pImageData + ((pDecoder->uHeight-1) * pDecoder->iStep);
                if (pDecoder->iIndex < pDecoder->uHeight)
                {
                    pDecoder->pScanLine -= pDecoder->iStep*((pDecoder->uHeight-1)-pDecoder->iIndex);
                }
            }
            else
            {
                pDecoder->pScanLine = pDecoder->pImageData;
                if (pDecoder->iIndex < pDecoder->uHeight)
                {
                    pDecoder->pScanLine += pDecoder->iStep*((pDecoder->uHeight-1)-pDecoder->iIndex);
                }
            }
            
            pDecoder->pScanLineEnd = pDecoder->pScanLine+pDecoder->uWidth;
            pDecoder->pBufferEnd = pDecoder->pScanLine+pDecoder->iStep;
        }

        // read pixel from stack
        cPixel = *(--pDecoder->pStackPtr);
        
        // put translated pixel into output buffer
        if (pDecoder->pScanLine < pDecoder->pBufferEnd)
        {
            *pDecoder->pScanLine = cPixel;
        }
        pDecoder->pScanLine += 1;

        // see if we are at end of scanline
        if (pDecoder->pScanLine == pDecoder->pScanLineEnd)
        {
            // if width and step size differ, zero fill extra
            if (pDecoder->uWidth < pDecoder->iStep)
            {
                *pDecoder->pScanLine++ = 0x00;
            }
            
            // see if we are done with this pass
            if (((pDecoder->iIndex += stepsize[pDecoder->iPass]) >= pDecoder->uHeight) && (stepsize[pDecoder->iPass] > 0))
            {
                pDecoder->iIndex = startrow[++pDecoder->iPass];
            }
            
            // invalidate the scanline pointer
            pDecoder->pScanLine = NULL;
        }
    }
    
    // normal return condition
    return(1);
}


/*** Public Functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function DirtyGifIdentify

    \Description
        Identify if input image is a GIF image.

    \Input *pImageData  - pointer to image data
    \Input uImageLen    - size of image data

    \Output
        int32_t         - TRUE if a GIF, else FALSE

    \Version 03/09/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGifIdentify(const uint8_t *pImageData, uint32_t uImageLen)
{
    // make sure we have enough data
    if (uImageLen < 6)
    {
        return(0);
    }
    // see of we're a GIF
    if (memcmp(pImageData, "GIF87a", 6) && memcmp(pImageData, "GIF89a", 6))
    {
        return(0);
    }
    return(1);
}

/*F********************************************************************************/
/*!
    \Function DirtyGifParse

    \Description
        Parse GIF header.

    \Input *pGifHdr     - [out] pointer to GIF header to fill in
    \Input *pGifData    - pointer to GIF data
    \Input *pGifEnd     - pointer past the end of GIF data

    \Version 11/13/2003 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGifParse(DirtyGifHdrT *pGifHdr, const uint8_t *pGifData, const uint8_t *pGifEnd)
{
    return(_DirtyGifParseHeader(pGifHdr, pGifData, pGifEnd));
}

/*F********************************************************************************/
/*!
    \Function DirtyGifDecodePalette

    \Description
        Decode a GIF palette into an RGBA palette.

    \Input *pGifHdr     - pointer to GIF header
    \Input *pPalette    - [out] pointer to output for RGBA palette
    \Input *pPaletteEnd - pointer past end of RGBA output buffer
    \Input uAlpha       - alpha value to use for normal pixels

    \Version 11/13/2003 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGifDecodePalette(DirtyGifHdrT *pGifHdr, uint8_t *pPalette, uint8_t *pPaletteEnd, uint8_t uAlpha)
{
    const uint8_t *pColorTable;
    uint8_t *pPalStart = pPalette;

    // validate parameters
    if ((pGifHdr->pColorTable == NULL) || (pPalette == NULL))
    {
        return(-1);
    }
    
    // extract palette colors
    for (pColorTable = pGifHdr->pColorTable; pPalette < pPaletteEnd; pPalette += 4, pColorTable += 3)
    {
        pPalette[0] = pColorTable[0];
        pPalette[1] = pColorTable[1];
        pPalette[2] = pColorTable[2];
        pPalette[3] = uAlpha;
    }
    
    // handle alpha transparency
    if (pGifHdr->bHasAlpha)
    {
        uint8_t *pAlphaColor = pPalStart + (pGifHdr->uTransColor * 4);
        if (pAlphaColor < pPaletteEnd)
        {
            pAlphaColor[3] = 0x00;
        }
    }
    
    return(0);
}

/*F********************************************************************************/
/*!
    \Function DirtyGifDecodeImage

    \Description
        Decode a GIF image into an 8bit paletteized bitmap.
        
    \Input *pGifHdr     - pointer to header describing gif to decode
    \Input *pImageData  - [out] pointer to buffer to write decoded image data to
    \Input iBufWidth    - width of output buffer
    \Input iBufHeight   - height of output buffer
    \Input bVflip       - if TRUE, flip image vertically

    \Output
        int32_t         - positive=number of bytes decoded, negative=error

    \Version 11/13/2003 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGifDecodeImage(DirtyGifHdrT *pGifHdr, uint8_t *pImageData, int32_t iBufWidth, int32_t iBufHeight, uint32_t bVflip)
{
    DirtyGifDecoderT Decoder;
    DirtyGifDecoderT *pDecoder = &Decoder;
    
    // init the decoder
    if (_DirtyGifInitDecoder(pDecoder, pGifHdr, pImageData, iBufWidth, iBufHeight) < 0)
    {
        return(-1);
    }

    // decode the image
    for (;;)
    {
        // get next code
        if (_DirtyGifGetNextCode(pDecoder) <= 0)
        {
            break;
        }

        // iClearCode code?
        if (pDecoder->iCodeRaw == pDecoder->iClearCode)
        {
            // reset the decoder
            _DirtyGifResetDecoder(pDecoder);
            
            // get code following iClearCode code
            if (_DirtyGifGetNextCode(pDecoder) <= 0)
            {
                break;
            }

            // if code is out of range, set code=0 to protect against broken encoders
            if (pDecoder->iCodeRaw >= pDecoder->iSlot)
            {
                pDecoder->iCodeRaw = 0;
            }

            // update
            pDecoder->iCode0 = pDecoder->iCode1 = pDecoder->iCodeRaw;

            // push the single value onto stack
            if (pDecoder->pStackPtr < pDecoder->pStackEnd)
            {
                *pDecoder->pStackPtr++ = (uint8_t)pDecoder->iCodeRaw;
            }
        }
        else
        {
            // decode code to stack and update decoder
            _DirtyGifUpdateDecoder(pDecoder);
        }

        // write any decoded codes into bitmap
        if (_DirtyGifUpdateBitmap(pDecoder, bVflip) == 0)
        {
            // buffer is too small in height - bail early
            break;
        }
    }

    // number number of bytes processed
    return((int32_t)(pDecoder->pCodeData - pGifHdr->pImageData));
}

/*F********************************************************************************/
/*!
    \Function DirtyGifDecodeImage32

    \Description
        Decode a GIF image into a 32bit ARGB direct-color bitmap

    \Input *pGifHdr     - pointer to header describing gif to decode
    \Input *pImageData  - [out] pointer to buffer to write decoded image data to
    \Input iBufWidth    - width of output buffer in pixels
    \Input iBufHeight   - height of output buffer in pixels
    \Input bVflip       - if TRUE, flip image vertically

    \Output
        int32_t         - positive=number of bytes decoded, negative=error

    \Version 03/09/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGifDecodeImage32(DirtyGifHdrT *pGifHdr, uint8_t *pImageData, int32_t iBufWidth, int32_t iBufHeight, uint32_t bVflip)
{
    uint8_t aPaletteData[256][4];
    uint8_t *p8BitImage, *pSrc, *pDst;
    int32_t i8BitSize, iWidth, iHeight;
    uint32_t uIndex;
    int32_t iError;

    // first, decode palette info
    if ((iError = DirtyGifDecodePalette(pGifHdr, (uint8_t *)aPaletteData, (uint8_t *)aPaletteData+sizeof(aPaletteData), 0xff)) < 0)
    {
        return(iError);
    }

    // calculate size of decoded 8bit image
    i8BitSize = iBufWidth * iBufHeight;

    // now, decode 8bit image into end of buffer
    p8BitImage = pImageData + (iBufWidth*iBufHeight*4) - i8BitSize;

    // decode the image
    if ((iError = DirtyGifDecodeImage(pGifHdr, p8BitImage, iBufWidth, iBufHeight, bVflip)) < 0)
    {
        return(iError);
    }

    // now translate the 8bit image to 32bits
    for (pSrc=p8BitImage, pDst=pImageData, iHeight=0; iHeight < iBufHeight; iHeight++)
    {
        for (iWidth = 0; iWidth < iBufWidth; iWidth++, pDst += 4)
        {
            uIndex = *pSrc++;

            // output in ARGB order
            pDst[0] = aPaletteData[uIndex][3];
            pDst[1] = aPaletteData[uIndex][0];
            pDst[2] = aPaletteData[uIndex][1];
            pDst[3] = aPaletteData[uIndex][2];
        }
    }

    // return success
    return(0);
}
