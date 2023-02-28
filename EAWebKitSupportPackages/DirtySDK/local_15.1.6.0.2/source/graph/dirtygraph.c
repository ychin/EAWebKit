/*H********************************************************************************/
/*!
    \File dirtygraph.c

    \Description
        Routines for decoding an encoded graphics image.

    \Copyright
        Copyright (c) 2006 Electronic Arts Inc.

    \Version 03/09/2006 (jbrookes) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"

#include "DirtySDK/graph/dirtygraph.h"
#include "DirtySDK/graph/dirtygif.h"
#include "DirtySDK/graph/dirtyjpg.h"
#include "DirtySDK/graph/dirtypng.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

struct DirtyGraphRefT
{
    //! module memory group
    int32_t iMemGroup;
    void *pMemGroupUserData;
    
    DirtyJpgStateT *pJpg;
    DirtyPngStateT *pPng;
    DirtyGifHdrT GifHdr;
    DirtyJpgHdrT JpgHdr;
    DirtyPngHdrT PngHdr;
};

/*** Variables ********************************************************************/

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function DirtyGraphCreate

    \Description
        Create the DirtyGraph module.

    \Output
        DirtyGraphStateT *  - module state, or NULL if unable to create

    \Version 03/09/2006 (jbrookes) First Version
*/
/********************************************************************************F*/
DirtyGraphRefT *DirtyGraphCreate(void)
{
    DirtyGraphRefT *pDirtyGraph;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    
    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
    
    // allocate and init module state
    if ((pDirtyGraph = DirtyMemAlloc(sizeof(*pDirtyGraph), DIRTYGRAPH_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtygraph: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pDirtyGraph, sizeof(*pDirtyGraph));
    pDirtyGraph->iMemGroup = iMemGroup;
    pDirtyGraph->pMemGroupUserData = pMemGroupUserData;

    // allocate jpeg module state
    if ((pDirtyGraph->pJpg = DirtyJpgCreate()) == NULL)
    {
        NetPrintf(("dirtygraph: unable to allocate jpg module state\n"));
    }

    // allocate png module state
    if ((pDirtyGraph->pPng = DirtyPngCreate()) == NULL)
    {
        NetPrintf(("dirtygraph: unable to allocate png module state\n"));
    }
    
    // return module state ref
    return(pDirtyGraph);
}

/*F********************************************************************************/
/*!
    \Function DirtyGraphDestroy

    \Description
        Destroy the DirtyGraph module.

    \Input *pDirtyGraph - pointer to module state

    \Output
        None.

    \Version 03/09/2006 (jbrookes)
*/
/********************************************************************************F*/
void DirtyGraphDestroy(DirtyGraphRefT *pDirtyGraph)
{
    // destroy jpeg module?
    if (pDirtyGraph->pJpg != NULL)
    {
        DirtyJpgDestroy(pDirtyGraph->pJpg);
    }
    // destroy png module?
    if (pDirtyGraph->pPng != NULL)
    {
        DirtyPngDestroy(pDirtyGraph->pPng);
    }
    // free module state
    DirtyMemFree(pDirtyGraph, DIRTYGRAPH_MEMID, pDirtyGraph->iMemGroup, pDirtyGraph->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function DirtyGraphDecodeHeader

    \Description
        Decode an image header

    \Input *pDirtyGraph - pointer to module state
    \Input *pInfo       - [out] storage for image info
    \Input *pImageData  - pointer to input image info
    \Input uImageLen    - size of input image
    
    \Output
        int32_t         - negative=error, else success

    \Version 03/09/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGraphDecodeHeader(DirtyGraphRefT *pDirtyGraph, DirtyGraphInfoT *pInfo, const uint8_t *pImageData, uint32_t uImageLen)
{
    int32_t iError, iResult=-1;
    
    // init info structure
    ds_memclr(pInfo, sizeof(*pInfo));
    pInfo->pImageData = pImageData;
    pInfo->uLength = uImageLen;

    // try and identify image    
    if (DirtyGifIdentify(pImageData, uImageLen))
    {
        if ((iError = DirtyGifParse(&pDirtyGraph->GifHdr, pImageData, pImageData+uImageLen)) == 0)
        {
            pInfo->uType = DIRTYGRAPH_IMAGETYPE_GIF;
            pInfo->iWidth = pDirtyGraph->GifHdr.uWidth;
            pInfo->iHeight = pDirtyGraph->GifHdr.uHeight;
            iResult = 0;
        }
        else
        {
            NetPrintf(("dirtygraph: error %d parsing gif image\n", iError));
        }
        
    }
    else if ((pDirtyGraph->pJpg != NULL) && DirtyJpgIdentify(pDirtyGraph->pJpg, pImageData, uImageLen))
    {
        if ((iError = DirtyJpgDecodeHeader(pDirtyGraph->pJpg, &pDirtyGraph->JpgHdr, pImageData, uImageLen)) == DIRTYJPG_ERR_NONE)
        {
            pInfo->uType = DIRTYGRAPH_IMAGETYPE_JPG;
            pInfo->iWidth = pDirtyGraph->JpgHdr.uWidth;
            pInfo->iHeight = pDirtyGraph->JpgHdr.uHeight;
            iResult = 0;
        }
        else
        {
            NetPrintf(("dirtygraph: error %d parsing jpg image\n", iError));
        }
    }
    else if ((pDirtyGraph->pPng != NULL) && DirtyPngIdentify(pImageData, uImageLen))
    {
        if ((iError = DirtyPngParse(pDirtyGraph->pPng, &pDirtyGraph->PngHdr, pImageData, pImageData+uImageLen)) == DIRTYPNG_ERR_NONE)
        {
            pInfo->uType = DIRTYGRAPH_IMAGETYPE_PNG;
            pInfo->iWidth = pDirtyGraph->PngHdr.uWidth;
            pInfo->iHeight = pDirtyGraph->PngHdr.uHeight;
            iResult = 0;
        }
        else
        {
            NetPrintf(("dirtygraph: error %d parsing png image\n", iError));
        }
    }
    else
    {
        NetPrintf(("dirtygraph: cannot parse image of unknown type\n"));
        pInfo->uType = DIRTYGRAPH_IMAGETYPE_UNKNOWN;
        pInfo->pImageData = NULL;
        pInfo->uLength = 0;
    }

    // return result to caller    
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function DirtyGraphDecodeImage

    \Description
        Decode an image into the given output buffer.

    \Input *pDirtyGraph - pointer to module state
    \Input *pInfo       - image information (filled in by DirtyGraphDecodeHeader)
    \Input *pImageBuf   - [out] pointer to buffer to store decoded image
    \Input iBufWidth    - width of output buffer
    \Input iBufHeight   - height of output buffer
    
    \Output
        int32_t         - negative=error, else success

    \Version 03/09/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t DirtyGraphDecodeImage(DirtyGraphRefT *pDirtyGraph, DirtyGraphInfoT *pInfo, uint8_t *pImageBuf, int32_t iBufWidth, int32_t iBufHeight)
{
    if (pInfo->uType == DIRTYGRAPH_IMAGETYPE_GIF)
    {
        return(DirtyGifDecodeImage32(&pDirtyGraph->GifHdr, pImageBuf, iBufWidth, iBufHeight, TRUE));
    }
    else if (pInfo->uType == DIRTYGRAPH_IMAGETYPE_JPG)
    {
        return(DirtyJpgDecodeImage(pDirtyGraph->pJpg, &pDirtyGraph->JpgHdr, pImageBuf, iBufWidth, iBufHeight));
    }
    else if (pInfo->uType == DIRTYGRAPH_IMAGETYPE_PNG)
    {
        return(DirtyPngDecodeImage(pDirtyGraph->pPng, &pDirtyGraph->PngHdr, pImageBuf, iBufWidth, iBufHeight));
    }
    else
    {
        NetPrintf(("dirtygraph: cannot decode image of unknown type\n"));
        return(-1);
    }
}
