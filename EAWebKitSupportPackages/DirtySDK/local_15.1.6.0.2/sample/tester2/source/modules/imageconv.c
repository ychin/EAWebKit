/*H********************************************************************************/
/*!
    \File imageconv.c

    \Description
        Test DirtyGraph image conversion routines.

    \Copyright
        Copyright (c) 2006 Electronic Arts Inc.

    \Version 02/23/2006 (jbrookes) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#pragma warning(push,0)
#include <windows.h>
#pragma warning(pop)

#include <string.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/graph/dirtygraph.h"
#include "DirtySDK/graph/dirtygif.h"
#include "DirtySDK/graph/dirtyjpg.h"
#include "DirtySDK/graph/dirtypng.h"

#include "libsample/zfile.h"
#include "libsample/zlib.h"
#include "libsample/zmem.h"

#include "testermodules.h"

#define RUNLIBJPEG  (FALSE)

#if RUNLIBJPEG
extern BITMAPINFO *jpeg_read_dibitmap(char *fname, char *errbuf, long errlen, long *cmpsize);
#endif

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

static DirtyGraphRefT *pDirtyGraph;

/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _CmdImgConvDecodeImage

    \Description
        Read input image, and decode it to a 32bit ARGB file using DirtyGraph

    \Input *pInputFile  - input file data
    \Input uInputSize   - size of input file data
    \Input *pWidth      - [out] storage for width of input image
    \Input *pHeight     - [out[ storage for height of input image
    
    \Output
        uint8_t *       - pointer to output 32bit ARGB image, or NULL

    \Version 02/23/2006 (jbrookes)
*/
/********************************************************************************F*/
static uint8_t *_CmdImgConvDecodeImage(const uint8_t *pInputFile, uint32_t uInputSize, int32_t *pWidth, int32_t *pHeight)
{
    DirtyGraphInfoT ImageInfo;
    uint8_t *p32Bit;
    int32_t iError;
    const char *_strImageTypes[] = { "unknown", "gif", "jpg", "png" };

    // create module state
    if (pDirtyGraph == NULL)
    {
        pDirtyGraph = DirtyGraphCreate();
    }

    // parse the image header
    if ((iError = DirtyGraphDecodeHeader(pDirtyGraph, &ImageInfo, pInputFile, uInputSize)) < 0)
    {
        ZPrintf("imgconv: error %d trying to parse image\n", iError);
        DirtyGraphDestroy(pDirtyGraph);
        pDirtyGraph = NULL;
        return(NULL);
    }

    // identify image type
    ZPrintf("imgconv: parsed %s image\n", _strImageTypes[ImageInfo.uType]);
    
    // allocate space for 32bit raw image
    if ((p32Bit = ZMemAlloc((ImageInfo.iWidth) * (ImageInfo.iHeight) * 4)) == NULL)
    {
        ZPrintf("imgconv: could not allocate memory for decoded image\n");
        DirtyGraphDestroy(pDirtyGraph);
        pDirtyGraph = NULL;
        return(NULL);
    }
    
    // decode the image
    if ((iError = DirtyGraphDecodeImage(pDirtyGraph, &ImageInfo, p32Bit, ImageInfo.iWidth, ImageInfo.iHeight)) < 0)
    {
        ZPrintf("imgconv: error %d trying to decode image\n", iError);
        DirtyGraphDestroy(pDirtyGraph);
        pDirtyGraph = NULL;
        ZMemFree(p32Bit);
        return(NULL);
    }

    // store image dimensions image
    *pWidth = ImageInfo.iWidth;
    *pHeight = ImageInfo.iHeight;

    // return 32bit image buffer
    return(p32Bit);
}

/*F********************************************************************************/
/*!
    \Function _CmdImgConvSwizzleLineBMP

    \Description
        Swizzle scanline in place.

    \Input *pScan       - scanline to swizzled
    \Input iWidth       - width of scanline
    
    \Output
        None

    \Version 03/07/2007 (jbrookes)
*/
/********************************************************************************F*/
static void _CmdImgConvSwizzleLineBMP(uint8_t *pScan, int32_t iWidth)
{
    int32_t iCurW;
    uint8_t uTmp;

    // do the swap and swizzle in one pass
    for (iCurW = 0; iCurW < iWidth; iCurW += 1, pScan += 4)
    {
        // swap a and b
        uTmp = pScan[0];            // save a
        pScan[0] = pScan[3];        // b->a
        pScan[3] = uTmp;            // a->b

        // swap r and g
        uTmp = pScan[1];            // save r
        pScan[1] = pScan[2];        // g->r
        pScan[2] = uTmp;            // r->g
    }
}

/*F********************************************************************************/
/*!
    \Function _CmdImgConvSwapAndSwizzleLineBMP

    \Description
        Swap scanlines pointed to by pScanLo and pScanHi and swizzle in-place.

    \Input *pScanLo     - lo scanline to swap&swizzle
    \Input *pScanHi     - hi scanline to swap&swizzle
    \Input iWidth       - width of scanline
    
    \Output
        None

    \Version 03/07/2007 (jbrookes)
*/
/********************************************************************************F*/
static void _CmdImgConvSwapAndSwizzleLineBMP(uint8_t *pScanLo, uint8_t *pScanHi, int32_t iWidth)
{
    int32_t iCurW;
    uint8_t uTmp;

    // do the swap and swizzle in one pass
    for (iCurW = 0; iCurW < iWidth; iCurW += 1, pScanLo += 4, pScanHi += 4)
    {
        // swap a(hi) and b(lo)
        uTmp = pScanHi[0];          // save a(hi)
        pScanHi[0] = pScanLo[3];    // b(lo)->a(hi)
        pScanLo[3] = uTmp;          // a(hi)->b(lo)

        // swap b(hi) and a(lo)
        uTmp = pScanHi[3];          // save b(hi)
        pScanHi[3] = pScanLo[0];    // a(lo)->b(hi)
        pScanLo[0] = uTmp;          // b(hi)->a(lo)

        // swap r(hi) and g(lo)
        uTmp = pScanHi[1];          // save r(hi)
        pScanHi[1] = pScanLo[2];    // g(lo)->r(hi)
        pScanLo[2] = uTmp;          // r(hi)->g(lo)

        // swap g(hi) and r(lo)
        uTmp = pScanHi[2];          // save g(hi)
        pScanHi[2] = pScanLo[1];    // r(lo)->g(hi)
        pScanLo[1] = uTmp;          // g(hi)->r(lo)
    }
}

/*F********************************************************************************/
/*!
    \Function _CmdImgConvSaveBMP

    \Description
        Save input 32bit ARGB image as a 32bit BMP file

    \Input *pFilename   - filename of file to save
    \Input *pImageData  - input image data
    \Input iWidth       - width of input image
    \Input iHeight      - height of input image
    
    \Output
        int32_t         - ZFileClose() result

    \Version 02/23/2006 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _CmdImgConvSaveBMP(const char *pFileName, uint8_t *pImageData, int32_t iWidth, int32_t iHeight)
{
    BITMAPFILEHEADER BitmapFileHeader;
    BITMAPINFOHEADER BitmapInfoHeader;
    uint8_t *pScanHi, *pScanLo;
    ZFileT iFileId;
    int32_t iCurH;

    // open the file for writing
    if ((iFileId = ZFileOpen(pFileName, ZFILE_OPENFLAG_WRONLY|ZFILE_OPENFLAG_CREATE|ZFILE_OPENFLAG_BINARY)) < 0)
    {
        return(-1);
    }

    // vflip image and convert from ARGB to BGRA in one pass
    for (iCurH = 0; iCurH < (iHeight/2); iCurH++)
    {
        // ref scanlines to swap and swizzle
        pScanLo = pImageData + (iCurH*iWidth*4);
        pScanHi = pImageData + ((iHeight-iCurH-1)*iWidth*4);

        // do the swap and swizzle
        _CmdImgConvSwapAndSwizzleLineBMP(pScanLo, pScanHi, iWidth);
    }
    // if height is odd, swizzle the center scanline
    if (iHeight & 1)
    {
        // ref scanlines to swap and swizzle
        pScanLo = pImageData + (iCurH*iWidth*4);

        // do the swap and swizzle
        _CmdImgConvSwizzleLineBMP(pScanLo, iWidth);
    }

    // format bitmap header
    ds_memclr(&BitmapFileHeader, sizeof(BitmapFileHeader));
    BitmapFileHeader.bfType = 'MB';
    BitmapFileHeader.bfSize = sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+(iWidth*iHeight*4);
    BitmapFileHeader.bfOffBits = sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader);
   
    // write fileheader to output file
    if (ZFileWrite(iFileId, &BitmapFileHeader, sizeof(BitmapFileHeader)) < 0)
    {
        ZFileClose(iFileId);
        return(-1);
    }
   
    // format bitmapinfo header
    ds_memclr(&BitmapInfoHeader, sizeof(BitmapInfoHeader));
    BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitmapInfoHeader.biWidth = iWidth;
    BitmapInfoHeader.biHeight = iHeight;
    BitmapInfoHeader.biPlanes = 1;
    BitmapInfoHeader.biBitCount = 32;
    BitmapInfoHeader.biCompression = BI_RGB;
    BitmapInfoHeader.biSizeImage = iWidth*iHeight*4;
    BitmapInfoHeader.biXPelsPerMeter = 0;
    BitmapInfoHeader.biYPelsPerMeter = 0;
    BitmapInfoHeader.biClrUsed = 0;
    BitmapInfoHeader.biClrImportant = 0;
    
    // write infoheader to output file
    if (ZFileWrite(iFileId, &BitmapInfoHeader, sizeof(BitmapInfoHeader)) < 0)
    {
        ZFileClose(iFileId);
        return(-1);
    }
    
    // write pixel data to output file
    if (ZFileWrite(iFileId, pImageData, iWidth*iHeight*4) < 0)
    {
        ZFileClose(iFileId);
        return(-1);
    }

    // close the file
    return(ZFileClose(iFileId));    
}

/*F********************************************************************************/
/*!
    \Function _CmdImgConvSaveRAW

    \Description
        Save input 32bit ARGB image as a 24bit RAW image.

    \Input *pFilename   - filename of file to save
    \Input *pImageData  - input image data
    \Input iWidth       - width of input image
    \Input iHeight      - height of input image
    
    \Output
        int32_t         - ZFileSave() result

    \Version 02/23/2006 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _CmdImgConvSaveRAW(const char *pFileName, uint8_t *pImageData, int32_t iWidth, int32_t iHeight)
{
    uint8_t *pSrc, *pDst;
    int32_t iW, iH;
    
    // convert 32bit ARGB image to 24bit RGB image (inline)
    for (pSrc=pImageData, pDst=pImageData, iH=0; iH < iHeight; iH++)
    {
        for (iW = 0; iW < iWidth; iW++)
        {
            pDst[0] = pSrc[1];
            pDst[1] = pSrc[2];
            pDst[2] = pSrc[3];
            pSrc += 4;
            pDst += 3;
        }
    }

    // write the file
    return(ZFileSave(pFileName, (const char *)pImageData, iWidth*iHeight*3, ZFILE_OPENFLAG_WRONLY|ZFILE_OPENFLAG_CREATE|ZFILE_OPENFLAG_BINARY));
}


/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function CmdStream

    \Description
        Create the Module module.

    \Input *argz    - environment
    \Input argc     - number of args
    \Input *argv[]  - argument list
    
    \Output int32_t - standard return code

    \Version 02/23/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t CmdImgConv(ZContext *argz, int32_t argc, char *argv[])
{
    int32_t iInputSize, iWidth, iHeight, iResult;
    uint8_t *pInputFile;
    uint8_t *p32BitImage;
    const char *pInputName;
    
    // usage
    if (argc == 1)
    {
        if (pDirtyGraph != NULL)
        {
            // destroy state

            DirtyGraphDestroy(pDirtyGraph);
            pDirtyGraph = NULL;
            ZPrintf("%s: graph instance destroyed\n", argv[0]);
        }
        else
        {
            ZPrintf("usage: %s <inputfile> <outputfile>\n", argv[0]);
        }
        return(0);
    }
    pInputName = argv[1];
    
    // open input file for reading
    if ((pInputFile = (uint8_t *)ZFileLoad(pInputName, &iInputSize, ZFILE_OPENFLAG_RDONLY|ZFILE_OPENFLAG_BINARY)) == NULL)
    {
        ZPrintf("%s: unable to open input file '%s'\n", argv[0], pInputName);
        return(0);
    }

    #if RUNLIBJPEG
    // first read it with libjpeg for comparison
    if (ds_stristr(pInputName, ".jpg"))
    {
        char strError[256];
        jpeg_read_dibitmap((char *)pInputName, strError, sizeof(strError), NULL);
    }
    #endif
   
    // convert to 32bit raw image
    if ((p32BitImage = _CmdImgConvDecodeImage(pInputFile, iInputSize, &iWidth, &iHeight)) == NULL)
    {
        return(0);
    }
    
    // save output
    if ((argc >= 3) && (p32BitImage != NULL))
    {
        // save output image based on type
        if (ds_stristr(argv[2], ".raw"))
        {
            iResult = _CmdImgConvSaveRAW(argv[2], p32BitImage, iWidth, iHeight);
        }
        else if (ds_stristr(argv[2], ".bmp"))
        {
            iResult = _CmdImgConvSaveBMP(argv[2], p32BitImage, iWidth, iHeight);
        }
        else
        {
            ZPrintf("%s: output filetype unrecognized\n");
            iResult = -1;
        }

        // success?
        if (iResult >= 0)
        {
            ZPrintf("%s: saved output image %s\n", argv[0], argv[2]);
        }
        else
        {
            ZPrintf("%s: error writing output file '%s'\n", argv[0], argv[2]);
        }
    }
    
    // dispose of source image
    ZMemFree(pInputFile);

    // dispose of buffer
    ZMemFree(p32BitImage);
    return(0);
}
