/*H********************************************************************************/
/*!
    \File dirtyerrnx.c

    \Description
        Dirtysock debug error routines. (This is derived from dirtyerrunix)

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 08/04/2018 (tcho) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/
#include <nn/socket/sys/errno.h>
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

#if DIRTYSOCK_ERRORNAMES
static DirtyErrT _DirtyErr_List[] =
{
    DIRTYSOCK_ErrorName(EPERM),             // 1   Operation not permitted
    DIRTYSOCK_ErrorName(ENOENT),            // 2   No such file or directory
    DIRTYSOCK_ErrorName(ESRCH),             // 3   No such process
    DIRTYSOCK_ErrorName(EINTR),             // 4   Interrupted system call
    DIRTYSOCK_ErrorName(EIO),               // 5   I/O error
    DIRTYSOCK_ErrorName(ENXIO),             // 6   No such device or address
    DIRTYSOCK_ErrorName(E2BIG),             // 7   Arg list too long
    DIRTYSOCK_ErrorName(ENOEXEC),           // 8   Exec format error
    DIRTYSOCK_ErrorName(EBADF),             // 9   Bad file number
    DIRTYSOCK_ErrorName(ECHILD),            // 10  No child processes
    //DIRTYSOCK_ErrorName(EAGAIN),          // 11  Try again
    DIRTYSOCK_ErrorName(EWOULDBLOCK),       // 11  Operation would block
    DIRTYSOCK_ErrorName(ENOMEM),            // 12  Out of memory
    DIRTYSOCK_ErrorName(EACCES),            // 13  Permission denied
    DIRTYSOCK_ErrorName(EFAULT),            // 14  Bad address
    DIRTYSOCK_ErrorName(ENOTBLK),           // 15  Block device required
    DIRTYSOCK_ErrorName(EBUSY),             // 16  Device or resource busy
    DIRTYSOCK_ErrorName(EEXIST),            // 17  File exists
    DIRTYSOCK_ErrorName(EXDEV),             // 18  Cross-device link
    DIRTYSOCK_ErrorName(ENODEV),            // 19  No such device
    DIRTYSOCK_ErrorName(ENOTDIR),           // 20  Not a directory
    DIRTYSOCK_ErrorName(EISDIR),            // 21  Is a directory
    DIRTYSOCK_ErrorName(EINVAL),            // 22  Invalid argument
    DIRTYSOCK_ErrorName(ENFILE),            // 23  File table overflow
    DIRTYSOCK_ErrorName(EMFILE),            // 24  Too many open files
    DIRTYSOCK_ErrorName(ENOTTY),            // 25  Not a typewriter
    DIRTYSOCK_ErrorName(ETXTBSY),           // 26  Text file busy
    DIRTYSOCK_ErrorName(EFBIG),             // 27  File too large
    DIRTYSOCK_ErrorName(ENOSPC),            // 28  No space left on device
    DIRTYSOCK_ErrorName(ESPIPE),            // 29  Illegal seek
    DIRTYSOCK_ErrorName(EROFS),             // 30  Read-only file system
    DIRTYSOCK_ErrorName(EMLINK),            // 31  Too many links
    DIRTYSOCK_ErrorName(EPIPE),             // 32  Broken pipe
    DIRTYSOCK_ErrorName(EDOM),              // 33  Math argument out of domain of func
    DIRTYSOCK_ErrorName(ERANGE),            // 34  Math result not representable
    DIRTYSOCK_ErrorName(EDEADLK),           // 35  Resource deadlock would occur
    DIRTYSOCK_ErrorName(ENAMETOOLONG),      // 36  File name too long
    DIRTYSOCK_ErrorName(ENOLCK),            // 37  No record locks available
    DIRTYSOCK_ErrorName(ENOSYS),            // 38  Function not implemented
    DIRTYSOCK_ErrorName(ENOTEMPTY),         // 39  Directory not empty
    DIRTYSOCK_ErrorName(ELOOP),             // 40  Too many symbolic links encountered
    DIRTYSOCK_ErrorName(ENOMSG),            // 42  No message of desired type
    DIRTYSOCK_ErrorName(EIDRM),             // 43  Identifier removed
    DIRTYSOCK_ErrorName(ENOSTR),            // 60  Device not a stream
    DIRTYSOCK_ErrorName(ENODATA),           // 61  No data available
    DIRTYSOCK_ErrorName(ETIME),             // 62  Timer expired
    DIRTYSOCK_ErrorName(ENOSR),             // 63  Out of streams resources
    DIRTYSOCK_ErrorName(EREMOTE),           // 66  Object is remote
    DIRTYSOCK_ErrorName(ENOLINK),           // 67  Link has been severed
    DIRTYSOCK_ErrorName(EPROTO),            // 71  Protocol error
    DIRTYSOCK_ErrorName(EMULTIHOP),         // 72  Multihop attempted
    DIRTYSOCK_ErrorName(EBADMSG),           // 74  Not a data message
    DIRTYSOCK_ErrorName(EOVERFLOW),         // 75  Value too large for defined data type
    DIRTYSOCK_ErrorName(EILSEQ),            // 84  Illegal byte sequence
    DIRTYSOCK_ErrorName(EUSERS),            // 87  Too many users
    DIRTYSOCK_ErrorName(ENOTSOCK),          // 88  Socket operation on non-socket
    DIRTYSOCK_ErrorName(EDESTADDRREQ),      // 89  Destination address required
    DIRTYSOCK_ErrorName(EMSGSIZE),          // 90  Message too long
    DIRTYSOCK_ErrorName(EPROTOTYPE),        // 91  Protocol wrong type for socket
    DIRTYSOCK_ErrorName(ENOPROTOOPT),       // 92  Protocol not available
    DIRTYSOCK_ErrorName(EPROTONOSUPPORT),   // 93  Protocol not supported
    DIRTYSOCK_ErrorName(ESOCKTNOSUPPORT),   // 94  Socket type not supported
    DIRTYSOCK_ErrorName(EOPNOTSUPP),        // 95  Operation not supported on transport endpoint
    DIRTYSOCK_ErrorName(EPFNOSUPPORT),      // 96  Protocol family not supported
    DIRTYSOCK_ErrorName(EAFNOSUPPORT),      // 97  Address family not supported by protocol
    DIRTYSOCK_ErrorName(EADDRINUSE),        // 98  Address already in use
    DIRTYSOCK_ErrorName(EADDRNOTAVAIL),     // 99  Cannot assign requested address
    DIRTYSOCK_ErrorName(ENETDOWN),          // 100 Network is down
    DIRTYSOCK_ErrorName(ENETUNREACH),       // 101 Network is unreachable
    DIRTYSOCK_ErrorName(ENETRESET),         // 102 Network dropped connection because of reset
    DIRTYSOCK_ErrorName(ECONNABORTED),      // 103 Software caused connection abort
    DIRTYSOCK_ErrorName(ECONNRESET),        // 104 Connection reset by peer
    DIRTYSOCK_ErrorName(ENOBUFS),           // 105 No buffer space available
    DIRTYSOCK_ErrorName(EISCONN),           // 106 Transport endpoint is already connected
    DIRTYSOCK_ErrorName(ENOTCONN),          // 107 Transport endpoint is not connected
    DIRTYSOCK_ErrorName(ESHUTDOWN),         // 108 Cannot send after transport endpoint shutdown
    DIRTYSOCK_ErrorName(ETOOMANYREFS),      // 109 Too many references: cannot splice
    DIRTYSOCK_ErrorName(ETIMEDOUT),         // 110 Connection timed out
    DIRTYSOCK_ErrorName(ECONNREFUSED),      // 111 Connection refused
    DIRTYSOCK_ErrorName(EHOSTDOWN),         // 112 Host is down
    DIRTYSOCK_ErrorName(EHOSTUNREACH),      // 113 No route to host
    DIRTYSOCK_ErrorName(EALREADY),          // 114 Operation already in progress
    DIRTYSOCK_ErrorName(EINPROGRESS),       // 115 Operation now in progress
    DIRTYSOCK_ErrorName(ESTALE),            // 116 Stale NFS file handle
    DIRTYSOCK_ErrorName(EDQUOT),            // 122 Quota exceeded

    // NULL terminate
    DIRTYSOCK_ErrorName(0)
};
#endif

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function DirtyErrNameList

    \Description
        This function takes as input a system-specific error code, and either
        resolves it to its define name if it is recognized or formats it as a hex
        number if not.

    \Input *pBuffer - [out] pointer to output buffer to store result
    \Input iBufSize - size of output buffer
    \Input uError   - error code to format
    \Input *pList   - error list to use

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
void DirtyErrNameList(char *pBuffer, int32_t iBufSize, uint32_t uError, const DirtyErrT *pList)
{
    #if DIRTYSOCK_ERRORNAMES
    int32_t iErr;

    for (iErr = 0; pList[iErr].uError != 0; iErr++)
    {
        if (pList[iErr].uError == uError)
        {
            ds_strnzcpy(pBuffer, pList[iErr].pErrorName, iBufSize);
            return;
        }
    }
    #endif

    ds_snzprintf(pBuffer, iBufSize, "0x%08x", uError);
}

/*F********************************************************************************/
/*!
    \Function DirtyErrName

    \Description
        This function takes as input a system-specific error code, and either
        resolves it to its define name if it is recognized or formats it as a hex
        number if not.

    \Input *pBuffer - [out] pointer to output buffer to store result
    \Input iBufSize - size of output buffer
    \Input uError   - error code to format

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
void DirtyErrName(char *pBuffer, int32_t iBufSize, uint32_t uError)
{
    #if DIRTYSOCK_ERRORNAMES
    DirtyErrNameList(pBuffer, iBufSize, uError, _DirtyErr_List);
    #endif
}

/*F********************************************************************************/
/*!
    \Function DirtyErrGetNameList

    \Description
        This function takes as input a system-specific error code, and either
        resolves it to its define name if it is recognized or formats it as a hex
        number if not.

    \Input uError   - error code to format
    \Input *pList   - error list to use

    \Output
        const char *- pointer to error name or error formatted in hex

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
const char *DirtyErrGetNameList(uint32_t uError, const DirtyErrT *pList)
{
    static char strName[8][64];
    static uint8_t uLast = 0;
    char *pName = strName[uLast++];
    if (uLast > 7) uLast = 0;
    DirtyErrNameList(pName, sizeof(strName[0]), uError, pList);
    return(pName);
}

/*F********************************************************************************/
/*!
    \Function DirtyErrGetName

    \Description
        This function takes as input a system-specific error code, and either
        resolves it to its define name if it is recognized or formats it as a hex
        number if not.

    \Input uError   - error code to format

    \Output
        const char *- pointer to error name or error formatted in hex

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
const char *DirtyErrGetName(uint32_t uError)
{
    static char strName[64];
    DirtyErrName(strName, sizeof(strName), uError);
    return(strName);
}
