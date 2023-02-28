/*H********************************************************************************/
/*!
    \File dirtyerrnx.cpp

    \Description
        Dirtysock debug error routines. (This is derived from dirtyerrunix)

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 08/04/2018 (tcho) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/
#include <nn/socket.h>
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

#if DIRTYSOCK_ERRORNAMES
static DirtyErrT _DirtyErr_List[] =
{
    // Warning: although these look like the POSIX error macro names, they actually
    // have different values on NX (e.g. posix EAGAIN == 9976, EWOULDBLOCK == 9930 but
    // nn::socket::Errno::EAgain == 11 and nn::socket::Errno::EWouldBlock == 11).
    //
    DIRTYSOCK_ErrorName(nn::socket::Errno::EPerm),             // 1   Operation not permitted
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoEnt),            // 2   No such file or directory
    DIRTYSOCK_ErrorName(nn::socket::Errno::ESrch),             // 3   No such process
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIntr),             // 4   Interrupted system call
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIo),               // 5   I/O error
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENxIo),             // 6   No such device or address
    DIRTYSOCK_ErrorName(nn::socket::Errno::E2Big),             // 7   Arg list too long
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoExec),           // 8   Exec format error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadf),             // 9   Bad file number
    DIRTYSOCK_ErrorName(nn::socket::Errno::EChild),            // 10  No child processes
    //DIRTYSOCK_ErrorName(nn::socket::Errno::EAgain),          // 11  Try again
    DIRTYSOCK_ErrorName(nn::socket::Errno::EWouldBlock),       // 11  Operation would block
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoMem),            // 12  Out of memory
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAcces),            // 13  Permission denied
    DIRTYSOCK_ErrorName(nn::socket::Errno::EFault),            // 14  Bad address
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotBlk),           // 15  Block device required
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBusy),             // 16  Device or resource busy
    DIRTYSOCK_ErrorName(nn::socket::Errno::EExist),            // 17  File exists
    DIRTYSOCK_ErrorName(nn::socket::Errno::EXDev),             // 18  Cross-device link
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoDev),            // 19  No such device
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotDir),           // 20  Not a directory
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIsDir),            // 21  Is a directory
    DIRTYSOCK_ErrorName(nn::socket::Errno::EInval),            // 22  Invalid argument
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENFile),            // 23  File table overflow
    DIRTYSOCK_ErrorName(nn::socket::Errno::EMFile),            // 24  Too many open files
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotTy),            // 25  Not a typewriter
    DIRTYSOCK_ErrorName(nn::socket::Errno::ETxtBsy),           // 26  Text file busy
    DIRTYSOCK_ErrorName(nn::socket::Errno::EFBig),             // 27  File too large
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoSpc),            // 28  No space left on device
    DIRTYSOCK_ErrorName(nn::socket::Errno::ESPipe),            // 29  Illegal seek
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERofs),             // 30  Read-only file system
    DIRTYSOCK_ErrorName(nn::socket::Errno::EMLink),            // 31  Too many links
    DIRTYSOCK_ErrorName(nn::socket::Errno::EPipe),             // 32  Broken pipe
    DIRTYSOCK_ErrorName(nn::socket::Errno::EDom),              // 33  Math argument out of domain of func
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERange),            // 34  Math result not representable
    DIRTYSOCK_ErrorName(nn::socket::Errno::EDeadLock),         // 35  Resource deadlock would occur
    //DIRTYSOCK_ErrorName(nn::socket::Errno::EDeadLk),         // 35  Resource deadlock would occur
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENameTooLong),      // 36  File name too long
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoLck),            // 37  No record locks available
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoSys),            // 38  Function not implemented
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotEmpty),         // 39  Directory not empty
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELoop),             // 40  Too many symbolic links encountered
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoMsg),            // 42  No message of desired type
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIdrm),             // 43  Identifier removed
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIdrm),             // 43  Identifier removed
    DIRTYSOCK_ErrorName(nn::socket::Errno::EChrng),            // 44  Channel number out of range
    DIRTYSOCK_ErrorName(nn::socket::Errno::EL2NSync),          // 45  Level 2 not synchronized
    DIRTYSOCK_ErrorName(nn::socket::Errno::EL3Hlt),            // 46  Level 3 halted
    DIRTYSOCK_ErrorName(nn::socket::Errno::EL3Rst),            // 47  Level 3 reset
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELnrng),            // 48  Link number out of range
    DIRTYSOCK_ErrorName(nn::socket::Errno::EUnatch),           // 49  Protocol driver not attached
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoCsi),            // 50  No CSI structure available
    DIRTYSOCK_ErrorName(nn::socket::Errno::EL2Hlt),            // 51  Level 2 halted
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBade),             // 52  Invalid exchange
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadr),             // 53  Invalid request descriptor
    DIRTYSOCK_ErrorName(nn::socket::Errno::EXFull),            // 54  Exchange full
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoAno),            // 55  No anode
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadRqc),           // 56  Invalid request code
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadSsl),           // 57  Invalid slot ? (EBADSLT)
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBFont),            // 59  Bad font file format
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoStr),            // 60  Device not a stream
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoData),           // 61  No data available
    DIRTYSOCK_ErrorName(nn::socket::Errno::ETime),             // 62  Timer expired
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoSr),             // 63  Out of streams resources
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoNet),            // 64  Machine is not on the network
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoPkg),            // 65  Package not installed
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERemote),           // 66  Object is remote
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoLink),           // 67  Link has been severed
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAdv),              // 68  Advertise error
    DIRTYSOCK_ErrorName(nn::socket::Errno::ESrmnt),            // 69  Srmount error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EComm),             // 70  Communication error on send
    DIRTYSOCK_ErrorName(nn::socket::Errno::EProto),            // 71  Protocol error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EMultiHop),         // 72  Multihop attempted
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadMsg),           // 74  Not a data message
    DIRTYSOCK_ErrorName(nn::socket::Errno::EOverflow),         // 75  Value too large for defined data type
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotUnuq),          // 76  Name not unique on network
    DIRTYSOCK_ErrorName(nn::socket::Errno::EBadFd),            // 77  File descriptor in bad state
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERemChg),           // 78  Remote address changed
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELibAcc),           // 79  Can not access a needed shared library
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELibBad),           // 80  Accessing a corrupted shared library
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELibScn),           // 81  .lib section in a.out corrupted
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELibMax),           // 82  Attempting to link in too many shared libraries
    DIRTYSOCK_ErrorName(nn::socket::Errno::ELibExec),          // 83  Cannot exec a shared library directly
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIlSeq),            // 84  Illegal byte sequence
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERestart),          // 85  Interrupted system call should be restarted
    DIRTYSOCK_ErrorName(nn::socket::Errno::EStrPipe),          // 86  Streams pipe error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EUsers),            // 87  Too many users
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotSock),          // 88  Socket operation on non-socket
    DIRTYSOCK_ErrorName(nn::socket::Errno::EDestAddrReq),      // 89  Destination address required
    DIRTYSOCK_ErrorName(nn::socket::Errno::EMsgSize),          // 90  Message too long
    DIRTYSOCK_ErrorName(nn::socket::Errno::EPrototype),        // 91  Protocol wrong type for socket
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoProtoOpt),       // 92  Protocol not available
    DIRTYSOCK_ErrorName(nn::socket::Errno::EProtoNoSupport),   // 93  Protocol not supported
    DIRTYSOCK_ErrorName(nn::socket::Errno::ESocktNoSupport),   // 94  Socket type not supported
    DIRTYSOCK_ErrorName(nn::socket::Errno::EOpNotSupp),        // 95  Operation not supported on transport endpoint
    DIRTYSOCK_ErrorName(nn::socket::Errno::EPfNoSupport),      // 96  Protocol family not supported
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAfNoSupport),      // 97  Address family not supported by protocol
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAddrInUse),        // 98  Address already in use
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAddrNotAvail),     // 99  Cannot assign requested address
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENetDown),          // 100 Network is down
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENetUnreach),       // 101 Network is unreachable
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENetReset),         // 102 Network dropped connection because of reset
    DIRTYSOCK_ErrorName(nn::socket::Errno::EConnAborted),      // 103 Software caused connection abort
    DIRTYSOCK_ErrorName(nn::socket::Errno::EConnReset),        // 104 Connection reset by peer
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoBufs),           // 105 No buffer space available
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIsConn),           // 106 Transport endpoint is already connected
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotConn),          // 107 Transport endpoint is not connected
    DIRTYSOCK_ErrorName(nn::socket::Errno::EShutDown),         // 108 Cannot send after transport endpoint shutdown
    DIRTYSOCK_ErrorName(nn::socket::Errno::ETooManyRefs),      // 109 Too many references: cannot splice
    DIRTYSOCK_ErrorName(nn::socket::Errno::ETimedOut),         // 110 Connection timed out
    DIRTYSOCK_ErrorName(nn::socket::Errno::EConnRefused),      // 111 Connection refused
    DIRTYSOCK_ErrorName(nn::socket::Errno::EHostDown),         // 112 Host is down
    DIRTYSOCK_ErrorName(nn::socket::Errno::EHostUnreach),      // 113 No route to host
    DIRTYSOCK_ErrorName(nn::socket::Errno::EAlready),          // 114 Operation already in progress
    DIRTYSOCK_ErrorName(nn::socket::Errno::EInProgress),       // 115 Operation now in progress
    DIRTYSOCK_ErrorName(nn::socket::Errno::EStale),            // 116 Stale NFS file handle
    DIRTYSOCK_ErrorName(nn::socket::Errno::EUClean),           // 117 Structure needs cleaning
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotNam),           // 118 Not a XENIX named type file
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENAvail),           // 119 No XENIX semaphores available
    DIRTYSOCK_ErrorName(nn::socket::Errno::EIsNam),            // 120 Is a named type file
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERemoteIo),         // 121 Remote I/O error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EDQuot),            // 122 Quota exceeded
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoMedium),         // 123 No medium found
    DIRTYSOCK_ErrorName(nn::socket::Errno::EMediumType),       // 124 Wrong medium type
    DIRTYSOCK_ErrorName(nn::socket::Errno::ECanceled),         // 125 Operation canceled
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENoKey),            // 126 Required key not available
    DIRTYSOCK_ErrorName(nn::socket::Errno::EKeyExpired),       // 127 Key has expired
    DIRTYSOCK_ErrorName(nn::socket::Errno::EKeyRevoked),       // 128 Key has been revoked
    DIRTYSOCK_ErrorName(nn::socket::Errno::EKeyRejected),      // 129 Key was rejected by service
    DIRTYSOCK_ErrorName(nn::socket::Errno::EOwnerDead),        // 130 Owner died
    DIRTYSOCK_ErrorName(nn::socket::Errno::ENotRecoverable),   // 131 State not recoverable
    DIRTYSOCK_ErrorName(nn::socket::Errno::ERfKill),           // 132 Operation not possible due to RF-kill
    DIRTYSOCK_ErrorName(nn::socket::Errno::EHwPoison),         // 133 Memory page has hardware error
    DIRTYSOCK_ErrorName(nn::socket::Errno::EProcLim),          // 156 Too many processes

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
