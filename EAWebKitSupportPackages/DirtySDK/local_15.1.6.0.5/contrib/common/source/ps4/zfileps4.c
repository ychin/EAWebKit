/*H********************************************************************************/
/*!
    \File zfilepc.c

    \Description
        Basic file operations (open, read, write, close, size).

    \Notes
        None.

    \Copyright
        Copyright (c) Electronic Arts 2004. ALL RIGHTS RESERVED.

    \Version 1.0 11/18/1004 (jbrookes)  First Version
    \Version 1.1 03/16/2005 (jfrank)    Updates for common sample libraries
    \Version 1.1 15/10/2012 (akirchner) Initial port to PS4
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock/dirtyerr.h"

#include "zfile.h"
#include "zlib.h"
#include "zmem.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

static int32_t _bInitialized = FALSE;
static FILE * _FileHandles[FOPEN_MAX];

/*** Private Functions ************************************************************/

static void _ZFileInit(void)
{
    // only initialize once
    if (_bInitialized == FALSE)
    {
        ds_memclr(_FileHandles, sizeof(_FileHandles));
        _bInitialized = TRUE;
    }
}

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function ZFileOpen

    \Description
        Open a file.

    \Input *pFileName   - file name to open
    \Input uFlags       - flags dictating how to open the file

    \Output
        int32_t             - file descriptor, or -1 if there was an error

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
ZFileT ZFileOpen(const char *pFileName, uint32_t uFlags)
{
    int32_t iFileId;

    if ((pFileName == NULL) || (pFileName[0] == '\0'))
    {
        return(ZFILE_INVALID);
    }

    _ZFileInit();

    for (iFileId = 0; iFileId < FOPEN_MAX; iFileId++)
    {
        if (_FileHandles[iFileId] == 0)
        {
            char strMode[16] = "";

            ds_memclr(strMode, sizeof(strMode));

            if (uFlags & ZFILE_OPENFLAG_APPEND)
            {
                strcat(strMode, "a");

                if (uFlags & ZFILE_OPENFLAG_RDONLY)
                {
                    strcat(strMode, "+");
                }
            }
            else if (uFlags & ZFILE_OPENFLAG_RDONLY)
            {
                strcat(strMode, "r");

                if (uFlags & ZFILE_OPENFLAG_WRONLY)
                {
                    strcat(strMode, "+");
                }
            }
            else if (uFlags & ZFILE_OPENFLAG_WRONLY)
            {
                strcat(strMode, "w");
            }
            if (uFlags & ZFILE_OPENFLAG_BINARY)
            {
                strcat(strMode, "b");
            }

            if ((_FileHandles[iFileId] = fopen(pFileName, strMode)) != NULL)
            {
                return(iFileId);
            }
            else
            {
                return(ZFILE_INVALID);
            }
        }
    }

    return(ZFILE_INVALID);
}

/*F********************************************************************************/
/*!
    \Function ZFileClose

    \Description
        Close a file

    \Input iFileId  - file descriptor

    \Output    int32_t  - return value from fclose()

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileClose(ZFileT iFileId)
{
    return(fclose((FILE *)_FileHandles[iFileId]));
}

/*F********************************************************************************/
/*!
    \Function ZFileRead

    \Description
        Read from a file.

    \Input *pData   - pointer to buffer to read to
    \Input iSize    - amount of data to read
    \Input iFileId  - file descriptor

    \Output
        Number of bytes read

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileRead(ZFileT iFileId, void *pData, int32_t iSize)
{
    int32_t iPos, iResult;

    iResult = fread(pData, 1, iSize, _FileHandles[iFileId]);
    iPos = ftell(_FileHandles[iFileId]);

    if (feof(_FileHandles[iFileId]))
    {
        return(iPos);
    }
    else if(ferror(_FileHandles[iFileId]))
    {
        return(0);
    }
    else
    {
        return((int32_t)iResult);
    }
}

/*F********************************************************************************/
/*!
    \Function ZFileWrite

    \Description
        Write to a file.

    \Input *pData   - pointer to buffer to write from
    \Input iSize    - amount of data to write
    \Input iFileId  - file descriptor

    \Output
        Number of bytes written

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileWrite(ZFileT iFileId, void *pData, int32_t iSize)
{
    int32_t iResult;

    if ((iFileId < 0)||(iFileId >= FOPEN_MAX))
    {
        return(0);
    }

    if ((pData == NULL) || (iSize == 0))
    {
        return(0);
    }

    iResult = fwrite(pData, iSize, 1, _FileHandles[iFileId]);

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ZFileSeek

    \Description
        Seek to location in file.

    \Input iFileId  - file id to seek
    \Input iOffset  - offset to seek to
    \Input uFlags   - seek mode (ZFILE_SEEKFLAG_*)

    \Output
        int64_t         - resultant seek location, or -1 on error

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int64_t ZFileSeek(ZFileT iFileId, int64_t iOffset, uint32_t uFlags)
{
    int64_t iResult;
    int32_t iFlags=0;

    if (uFlags == ZFILE_SEEKFLAG_CUR)
    {
        iFlags = SEEK_CUR;
    }
    else if (uFlags == ZFILE_SEEKFLAG_END)
    {
        iFlags = SEEK_END;
    }
    else if (uFlags == ZFILE_SEEKFLAG_SET)
    {
        iFlags = SEEK_SET;
    }

    fseek(_FileHandles[iFileId], iOffset, iFlags);

    iResult = ftell(_FileHandles[iFileId]);

    return((iResult >= 0) ? iResult : -1);
}

/*F********************************************************************************/
/*!
    \Function ZFileDelete

    \Description
        Delete a file.

    \Input *pFileName - filename of file to delete

    \Output int32_t - 0=success, error code otherwise

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileDelete(const char *pFileName)
{
    int32_t iResult;

    if (pFileName == NULL)
    {
        return(ZFILE_ERROR_FILENAME);
    }

    iResult = remove(pFileName);
    if (iResult == 0)
    {
        return(ZFILE_ERROR_NONE);
    }
    else
    {
        return(ZFILE_ERROR_FILEDELETE);
    }
}

/*F********************************************************************************/
/*!
    \Function ZFileStat

    \Description
        Get File Stat information on a file/dir.

    \Input *pFileName - filename/dir to stat
    \Input *pStat

    \Output int32_t - 0=success, error code otherwise

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileStat(const char *pFileName, ZFileStatT *pFileStat)
{
    return(ZFILE_ERROR_NONE);
}

/*F********************************************************************************/
/*!
    \Function ZFileRename

    \Description
        Rename a file.

    \Input *pOldname - old name
    \Input *pNewname - new name

    \Output int32_t - 0=success, error code otherwise

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileRename(const char *pOldname, const char *pNewname)
{
    int32_t iResult;

    // check for error conditions
    if ((pOldname == NULL) || (pNewname == NULL))
    {
        return(ZFILE_ERROR_NULLPOINTER);
    }

    // rename the file
    iResult = rename(pOldname, pNewname);

    return((iResult == 0) ? ZFILE_ERROR_NONE : ZFILE_ERROR_FILERENAME);
}

/*F********************************************************************************/
/*!
    \Function ZFileMkdir

    \Description
        Make a directory, recursively

    \Input *pPathName   - directory path to create

    \Output
        int32_t         - 0=success, error code otherwise

    \Version 1.0 15/10/2012 (akirchner) First Version
*/
/********************************************************************************F*/
int32_t ZFileMkdir(const char *pPathName)
{
    return(ZFILE_ERROR_NONE);
}
