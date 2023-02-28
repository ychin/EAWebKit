/*H********************************************************************************/
/*!
    \File dirtyerrps4.c

    \Description
        Dirtysock debug error routines.

    \Copyright
        Copyright (c) 2012 Electronic Arts Inc.

    \Version 10/24/2012 (jbrookes) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <sdk_version.h>

// error includes
#include <app_content.h>
#include <common_dialog/error.h>
#include <libhttp/libhttp_error.h>
#include <libnet/errno.h>
#include <libnetctl.h>
#include <libssl.h>
#include <net.h>
#include <np.h>
#include <scebase_common.h>
#include <sys/sce_errno.h>
#include <user_service/user_service_error.h>
#include <voice/voice_types.h>
#include <audioin.h>
#include <audioout.h>
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

static DirtySockAppErrorCallback pAppErrorCallback = NULL;

#if DIRTYSOCK_ERRORNAMES
static DirtyErrT _DirtyErr_List[] =
{
    /*
        libnet/errno.h error codes
    */
    DIRTYSOCK_ErrorName(SCE_NET_EPERM),                     // 1
    DIRTYSOCK_ErrorName(SCE_NET_ENOENT),                    // 2
    DIRTYSOCK_ErrorName(SCE_NET_ESRCH),                     // 3
    DIRTYSOCK_ErrorName(SCE_NET_EINTR),                     // 4
    DIRTYSOCK_ErrorName(SCE_NET_EIO),                       // 5
    DIRTYSOCK_ErrorName(SCE_NET_ENXIO),                     // 6
    DIRTYSOCK_ErrorName(SCE_NET_E2BIG),                     // 7
    DIRTYSOCK_ErrorName(SCE_NET_ENOEXEC),                   // 8
    DIRTYSOCK_ErrorName(SCE_NET_EBADF),                     // 9
    DIRTYSOCK_ErrorName(SCE_NET_ECHILD),                    // 10
    DIRTYSOCK_ErrorName(SCE_NET_EDEADLK),                   // 11
    DIRTYSOCK_ErrorName(SCE_NET_ENOMEM),                    // 12
    DIRTYSOCK_ErrorName(SCE_NET_EACCES),                    // 13
    DIRTYSOCK_ErrorName(SCE_NET_EFAULT),                    // 14
    DIRTYSOCK_ErrorName(SCE_NET_ENOTBLK),                   // 15
    DIRTYSOCK_ErrorName(SCE_NET_EBUSY),                     // 16
    DIRTYSOCK_ErrorName(SCE_NET_EEXIST),                    // 17
    DIRTYSOCK_ErrorName(SCE_NET_EXDEV),                     // 18
    DIRTYSOCK_ErrorName(SCE_NET_ENODEV),                    // 19
    DIRTYSOCK_ErrorName(SCE_NET_ENOTDIR),                   // 20
    DIRTYSOCK_ErrorName(SCE_NET_EISDIR),                    // 21
    DIRTYSOCK_ErrorName(SCE_NET_EINVAL),                    // 22
    DIRTYSOCK_ErrorName(SCE_NET_ENFILE),                    // 23
    DIRTYSOCK_ErrorName(SCE_NET_EMFILE),                    // 24
    DIRTYSOCK_ErrorName(SCE_NET_ENOTTY),                    // 25
    DIRTYSOCK_ErrorName(SCE_NET_ETXTBSY),                   // 26
    DIRTYSOCK_ErrorName(SCE_NET_EFBIG),                     // 27
    DIRTYSOCK_ErrorName(SCE_NET_ENOSPC),                    // 28
    DIRTYSOCK_ErrorName(SCE_NET_ESPIPE),                    // 29
    DIRTYSOCK_ErrorName(SCE_NET_EROFS),                     // 30
    DIRTYSOCK_ErrorName(SCE_NET_EMLINK),                    // 31
    DIRTYSOCK_ErrorName(SCE_NET_EPIPE),                     // 32
    DIRTYSOCK_ErrorName(SCE_NET_EDOM),                      // 33
    DIRTYSOCK_ErrorName(SCE_NET_ERANGE),                    // 34
    DIRTYSOCK_ErrorName(SCE_NET_EAGAIN),                    // 35
    DIRTYSOCK_ErrorName(SCE_NET_EWOULDBLOCK),               // SCE_NET_EAGAIN
    DIRTYSOCK_ErrorName(SCE_NET_EINPROGRESS),               // 36
    DIRTYSOCK_ErrorName(SCE_NET_EALREADY),                  // 37
    DIRTYSOCK_ErrorName(SCE_NET_ENOTSOCK),                  // 38
    DIRTYSOCK_ErrorName(SCE_NET_EDESTADDRREQ),              // 39
    DIRTYSOCK_ErrorName(SCE_NET_EMSGSIZE),                  // 40
    DIRTYSOCK_ErrorName(SCE_NET_EPROTOTYPE),                // 41
    DIRTYSOCK_ErrorName(SCE_NET_ENOPROTOOPT),               // 42
    DIRTYSOCK_ErrorName(SCE_NET_EPROTONOSUPPORT),           // 43
    DIRTYSOCK_ErrorName(SCE_NET_ESOCKTNOSUPPORT),           // 44
    DIRTYSOCK_ErrorName(SCE_NET_EOPNOTSUPP),                // 45
    DIRTYSOCK_ErrorName(SCE_NET_EPFNOSUPPORT),              // 46
    DIRTYSOCK_ErrorName(SCE_NET_EAFNOSUPPORT),              // 47
    DIRTYSOCK_ErrorName(SCE_NET_EADDRINUSE),                // 48
    DIRTYSOCK_ErrorName(SCE_NET_EADDRNOTAVAIL),             // 49
    DIRTYSOCK_ErrorName(SCE_NET_ENETDOWN),                  // 50
    DIRTYSOCK_ErrorName(SCE_NET_ENETUNREACH),               // 51
    DIRTYSOCK_ErrorName(SCE_NET_ENETRESET),                 // 52
    DIRTYSOCK_ErrorName(SCE_NET_ECONNABORTED),              // 53
    DIRTYSOCK_ErrorName(SCE_NET_ECONNRESET),                // 54
    DIRTYSOCK_ErrorName(SCE_NET_ENOBUFS),                   // 55
    DIRTYSOCK_ErrorName(SCE_NET_EISCONN),                   // 56
    DIRTYSOCK_ErrorName(SCE_NET_ENOTCONN),                  // 57
    DIRTYSOCK_ErrorName(SCE_NET_ESHUTDOWN),                 // 58
    DIRTYSOCK_ErrorName(SCE_NET_ETOOMANYREFS),              // 59
    DIRTYSOCK_ErrorName(SCE_NET_ETIMEDOUT),                 // 60
    DIRTYSOCK_ErrorName(SCE_NET_ECONNREFUSED),              // 61
    DIRTYSOCK_ErrorName(SCE_NET_ELOOP),                     // 62
    DIRTYSOCK_ErrorName(SCE_NET_ENAMETOOLONG),              // 63
    DIRTYSOCK_ErrorName(SCE_NET_EHOSTDOWN),                 // 64
    DIRTYSOCK_ErrorName(SCE_NET_EHOSTUNREACH),              // 65
    DIRTYSOCK_ErrorName(SCE_NET_ENOTEMPTY),                 // 66
    DIRTYSOCK_ErrorName(SCE_NET_EPROCLIM),                  // 67
    DIRTYSOCK_ErrorName(SCE_NET_EUSERS),                    // 68
    DIRTYSOCK_ErrorName(SCE_NET_EDQUOT),                    // 69
    DIRTYSOCK_ErrorName(SCE_NET_ESTALE),                    // 70
    DIRTYSOCK_ErrorName(SCE_NET_EREMOTE),                   // 71
    DIRTYSOCK_ErrorName(SCE_NET_EBADRPC),                   // 72
    DIRTYSOCK_ErrorName(SCE_NET_ERPCMISMATCH),              // 73
    DIRTYSOCK_ErrorName(SCE_NET_EPROGUNAVAIL),              // 74
    DIRTYSOCK_ErrorName(SCE_NET_EPROGMISMATCH),             // 75
    DIRTYSOCK_ErrorName(SCE_NET_EPROCUNAVAIL),              // 76
    DIRTYSOCK_ErrorName(SCE_NET_ENOLCK),                    // 77
    DIRTYSOCK_ErrorName(SCE_NET_ENOSYS),                    // 78
    DIRTYSOCK_ErrorName(SCE_NET_EFTYPE),                    // 79
    DIRTYSOCK_ErrorName(SCE_NET_EAUTH),                     // 80
    DIRTYSOCK_ErrorName(SCE_NET_ENEEDAUTH),                 // 81
    DIRTYSOCK_ErrorName(SCE_NET_EIDRM),                     // 82
    DIRTYSOCK_ErrorName(SCE_NET_ENOMSG),                    // 83
    DIRTYSOCK_ErrorName(SCE_NET_EOVERFLOW),                 // 84
    DIRTYSOCK_ErrorName(SCE_NET_ECANCELED),                 // 85
    DIRTYSOCK_ErrorName(SCE_NET_EADHOC),                    // 160
    DIRTYSOCK_ErrorName(SCE_NET_ERESERVED161),              // 161
    DIRTYSOCK_ErrorName(SCE_NET_ERESERVED162),              // 162
    DIRTYSOCK_ErrorName(SCE_NET_ENODATA),                   // 164
    DIRTYSOCK_ErrorName(SCE_NET_EDESC),                     // 165
    DIRTYSOCK_ErrorName(SCE_NET_EDESCTIMEDOUT),             // 166
    // libnet
    DIRTYSOCK_ErrorName(SCE_NET_ENOTINIT),                  // 200
    DIRTYSOCK_ErrorName(SCE_NET_ENOLIBMEM),                 // 201
    DIRTYSOCK_ErrorName(SCE_NET_ETLS),                      // 202
    DIRTYSOCK_ErrorName(SCE_NET_ECALLBACK),                 // 203
    DIRTYSOCK_ErrorName(SCE_NET_EINTERNAL),                 // 204
    DIRTYSOCK_ErrorName(SCE_NET_ERETURN),                   // 205
    DIRTYSOCK_ErrorName(SCE_NET_ENOALLOCMEM),               // 206
    // resolver
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_EINTERNAL),        // 220
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_EBUSY),            // 221
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENOSPACE),         // 222
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_EPACKET),          // 223
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ERESERVED224),     // 224
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENODNS),           // 225
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ETIMEDOUT),        // 226
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENOSUPPORT),       // 227
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_EFORMAT),          // 228
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ESERVERFAILURE),   // 229
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENOHOST),          // 230
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENOTIMPLEMENTED),  // 231
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ESERVERREFUSED),   // 232
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_ENORECORD),        // 233
    DIRTYSOCK_ErrorName(SCE_NET_RESOLVER_EALIGNMENT),       // 234

    /*
        errors from sys/sce_errno.h
    */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPERM),                // -2147352575  /* 0x80020001 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOENT),               // -2147352574  /* 0x80020002 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESRCH),                // -2147352573  /* 0x80020003 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EINTR),                // -2147352572  /* 0x80020004 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EIO),                  // -2147352571  /* 0x80020005 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENXIO),                // -2147352570  /* 0x80020006 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_E2BIG),                // -2147352569  /* 0x80020007 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOEXEC),              // -2147352568  /* 0x80020008 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EBADF),                // -2147352567  /* 0x80020009 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECHILD),               // -2147352566  /* 0x8002000A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EDEADLK),              // -2147352565  /* 0x8002000B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOMEM),               // -2147352564  /* 0x8002000C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EACCES),               // -2147352563  /* 0x8002000D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EFAULT),               // -2147352562  /* 0x8002000E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTBLK),              // -2147352561  /* 0x8002000F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EBUSY),                // -2147352560  /* 0x80020010 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EEXIST),               // -2147352559  /* 0x80020011 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EXDEV),                // -2147352558  /* 0x80020012 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENODEV),               // -2147352557  /* 0x80020013 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTDIR),              // -2147352556  /* 0x80020014 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EISDIR),               // -2147352555  /* 0x80020015 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EINVAL),               // -2147352554  /* 0x80020016 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENFILE),               // -2147352553  /* 0x80020017 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EMFILE),               // -2147352552  /* 0x80020018 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTTY),               // -2147352551  /* 0x80020019 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ETXTBSY),              // -2147352550  /* 0x8002001A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EFBIG),                // -2147352549  /* 0x8002001B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOSPC),               // -2147352548  /* 0x8002001C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESPIPE),               // -2147352547  /* 0x8002001D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EROFS),                // -2147352546  /* 0x8002001E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EMLINK),               // -2147352545  /* 0x8002001F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPIPE),                // -2147352544  /* 0x80020020 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EDOM),                 // -2147352543  /* 0x80020021 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ERANGE),               // -2147352542  /* 0x80020022 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EAGAIN),               // -2147352541  /* 0x80020023 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EWOULDBLOCK),          // -2147352541  /* 0x80020023 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EINPROGRESS),          // -2147352540  /* 0x80020024 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EALREADY),             // -2147352539  /* 0x80020025 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTSOCK),             // -2147352538  /* 0x80020026 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EDESTADDRREQ),         // -2147352537  /* 0x80020027 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EMSGSIZE),             // -2147352536  /* 0x80020028 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROTOTYPE),           // -2147352535  /* 0x80020029 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOPROTOOPT),          // -2147352534  /* 0x8002002A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROTONOSUPPORT),      // -2147352533  /* 0x8002002B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESOCKTNOSUPPORT),      // -2147352532  /* 0x8002002C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EOPNOTSUPP),           // -2147352531  /* 0x8002002D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTSUP),              // -2147352531  /* 0x8002002D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPFNOSUPPORT),         // -2147352530  /* 0x8002002E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EAFNOSUPPORT),         // -2147352529  /* 0x8002002F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EADDRINUSE),           // -2147352528  /* 0x80020030 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EADDRNOTAVAIL),        // -2147352527  /* 0x80020031 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENETDOWN),             // -2147352526  /* 0x80020032 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENETUNREACH),          // -2147352525  /* 0x80020033 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENETRESET),            // -2147352524  /* 0x80020034 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECONNABORTED),         // -2147352523  /* 0x80020035 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECONNRESET),           // -2147352522  /* 0x80020036 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOBUFS),              // -2147352521  /* 0x80020037 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EISCONN),              // -2147352520  /* 0x80020038 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTCONN),             // -2147352519  /* 0x80020039 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESHUTDOWN),            // -2147352518  /* 0x8002003A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ETOOMANYREFS),         // -2147352517  /* 0x8002003B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ETIMEDOUT),            // -2147352516  /* 0x8002003C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECONNREFUSED),         // -2147352515  /* 0x8002003D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ELOOP),                // -2147352514  /* 0x8002003E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENAMETOOLONG),         // -2147352513  /* 0x8002003F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EHOSTDOWN),            // -2147352512  /* 0x80020040 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EHOSTUNREACH),         // -2147352511  /* 0x80020041 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTEMPTY),            // -2147352510  /* 0x80020042 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROCLIM),             // -2147352509  /* 0x80020043 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EUSERS),               // -2147352508  /* 0x80020044 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EDQUOT),               // -2147352507  /* 0x80020045 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESTALE),               // -2147352506  /* 0x80020046 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EREMOTE),              // -2147352505  /* 0x80020047 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EBADRPC),              // -2147352504  /* 0x80020048 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ERPCMISMATCH),         // -2147352503  /* 0x80020049 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROGUNAVAIL),         // -2147352502  /* 0x8002004A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROGMISMATCH),        // -2147352501  /* 0x8002004B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROCUNAVAIL),         // -2147352500  /* 0x8002004C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOLCK),               // -2147352499  /* 0x8002004D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOSYS),               // -2147352498  /* 0x8002004E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EFTYPE),               // -2147352497  /* 0x8002004F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EAUTH),                // -2147352496  /* 0x80020050 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENEEDAUTH),            // -2147352495  /* 0x80020051 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EIDRM),                // -2147352494  /* 0x80020052 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOMSG),               // -2147352493  /* 0x80020053 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EOVERFLOW),            // -2147352492  /* 0x80020054 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECANCELED),            // -2147352491  /* 0x80020055 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EILSEQ),               // -2147352490  /* 0x80020056 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOATTR),              // -2147352489  /* 0x80020057 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EDOOFUS),              // -2147352488  /* 0x80020058 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EBADMSG),              // -2147352487  /* 0x80020059 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EMULTIHOP),            // -2147352486  /* 0x8002005A */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOLINK),              // -2147352485  /* 0x8002005B */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EPROTO),               // -2147352484  /* 0x8002005C */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOTCAPABLE),          // -2147352483  /* 0x8002005D */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ECAPMODE),             // -2147352482  /* 0x8002005E */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOBLK),               // -2147352481  /* 0x8002005F */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EICV),                 // -2147352480  /* 0x80020060 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ENOPLAYGOENT),         // -2147352479  /* 0x80020061 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_EREVOKE),              // -2147352478  /* 0x80020062 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESDKVERSION),          // -2147352477  /* 0x80020063 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESTART),               // -2147352476  /* 0x80020064 */
    DIRTYSOCK_ErrorName(SCE_KERNEL_ERROR_ESTOP),                // -2147352475  /* 0x80020065 */

    /*
        errors from libnet/errno.h
    */
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPERM),                   // 0x80410101
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOENT),                  // 0x80410102
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ESRCH),                   // 0x80410103
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EINTR),                   // 0x80410104
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EIO),                     // 0x80410105
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENXIO),                   // 0x80410106
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_E2BIG),                   // 0x80410107
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOEXEC),                 // 0x80410108
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EBADF),                   // 0x80410109
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECHILD),                  // 0x8041010A
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EAGAIN),                  // 0x8041010B
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOMEM),                  // 0x8041010C
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EACCES),                  // 0x8041010D
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EFAULT),                  // 0x8041010E
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTBLK),                 // 0x8041010F
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EBUSY),                   // 0x80410110
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EEXIST),                  // 0x80410111
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EXDEV),                   // 0x80410112
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENODEV),                  // 0x80410113
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTDIR),                 // 0x80410114
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EISDIR),                  // 0x80410115
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EINVAL),                  // 0x80410116
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENFILE),                  // 0x80410117
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EMFILE),                  // 0x80410118
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTTY),                  // 0x80410119
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ETXTBSY),                 // 0x8041011A
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EFBIG),                   // 0x8041011B
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOSPC),                  // 0x8041011C
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ESPIPE),                  // 0x8041011D
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EROFS),                   // 0x8041011E
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EMLINK),                  // 0x8041011F
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPIPE),                   // 0x80410120
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EDOM),                    // 0x80410121
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERANGE),                  // 0x80410122
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EAGAIN),                  // 0x80410123
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EIDRM),                   // 0x80410124
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EALREADY),                // 0x80410125
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTSOCK),                // 0x80410126
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EDESTADDRREQ),            // 0x80410127
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EMSGSIZE),                // 0x80410128
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROTOTYPE),              // 0x80410129
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOPROTOOPT),             // 0x8041012A
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROTONOSUPPORT),         // 0x8041012B
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ESOCKTNOSUPPORT),         // 0x8041012C
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EOPNOTSUPP),              // 0x8041012D
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPFNOSUPPORT),            // 0x8041012E
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EAFNOSUPPORT),            // 0x8041012F
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EADDRINUSE),              // 0x80410130
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EADDRNOTAVAIL),           // 0x80410131
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENETDOWN),                // 0x80410132
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENETUNREACH),             // 0x80410133
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENETRESET),               // 0x80410134
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECONNABORTED),            // 0x80410135
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECONNRESET),              // 0x80410136
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOBUFS),                 // 0x80410137
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EISCONN),                 // 0x80410138
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTCONN),                // 0x80410139
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ESHUTDOWN),               // 0x8041013A
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ETOOMANYREFS),            // 0x8041013B
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ETIMEDOUT),               // 0x8041013C
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECONNREFUSED),            // 0x8041013D
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ELOOP),                   // 0x8041013E
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENAMETOOLONG),            // 0x8041013F
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EHOSTDOWN),               // 0x80410140
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EHOSTUNREACH),            // 0x80410141
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTEMPTY),               // 0x80410142
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROCLIM),                // 0x80410143
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EUSERS),                  // 0x80410144
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EDQUOT),                  // 0x80410145
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ESTALE),                  // 0x80410146
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EREMOTE),                 // 0x80410147
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EBADRPC),                 // 0x80410148
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERPCMISMATCH),            // 0x80410149
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROGUNAVAIL),            // 0x8041014A
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROGMISMATCH),           // 0x8041014B
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EPROCUNAVAIL),            // 0x8041014C
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOLCK),                  // 0x8041014D
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOSYS),                  // 0x8041014E
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EFTYPE),                  // 0x8041014F
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EAUTH),                   // 0x80410150
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENEEDAUTH),               // 0x80410151
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EIDRM),                   // 0x80410152
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOMS),                   // 0x80410153
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EOVERFLOW),               // 0x80410154
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECANCELED),               // 0x80410155
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EADHOC),                  // 0x804101A0
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERESERVED161),            // 0x804101A1
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERESERVED162),            // 0x804101A2
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENODATA),                 // 0x804101A4
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EDESC),                   // 0x804101A5
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EDESCTIMEDOUT),           // 0x804101A6
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOTINIT),                // 0x804101C8
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOLIBMEM),               // 0x804101C9
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERESERVED202),            // 0x804101CA
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ECALLBACK),               // 0x804101CB
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_EINTERNAL),               // 0x804101CC
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ERETURN),                 // 0x804101CD
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_ENOALLOCMEM),             // 0x804101CE
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_EINTERNAL),      // 0x804101DC
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_EBUSY),          // 0x804101DD
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENOSPACE),       // 0x804101DE
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_EPACKET),        // 0x804101DF
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ERESERVED224),   // 0x804101E0
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENODNS),         // 0x804101E1
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ETIMEDOUT),      // 0x804101E2
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENOSUPPORT),     // 0x804101E3
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_EFORMAT),        // 0x804101E4
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ESERVERFAILURE), // 0x804101E5
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENOHOST),        // 0x804101E6
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENOTIMPLEMENTED),// 0x804101E7
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ESERVERREFUSED), // 0x804101E8
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_ENORECORD),      // 0x804101E9
    DIRTYSOCK_ErrorName(SCE_NET_ERROR_RESOLVER_EALIGNMENT),     // 0x804101EA

    /*
        errors from libnetctl_error.h
    */
    DIRTYSOCK_ErrorName(SCE_NET_CTL_ERROR_NOT_INITIALIZED),     // 0x80412101
    DIRTYSOCK_ErrorName(SCE_NET_CTL_ERROR_INVALID_CODE),        // 0x80412106
    DIRTYSOCK_ErrorName(SCE_NET_CTL_ERROR_INVALID_ADDR),        // 0x80412107
    DIRTYSOCK_ErrorName(SCE_NET_CTL_ERROR_NOT_CONNECTED),       // 0x80412108
    DIRTYSOCK_ErrorName(SCE_NET_CTL_ERROR_NOT_AVAIL),           // 0x80412109

    /*
        errors from np_error.h
    */

    DIRTYSOCK_ErrorName(SCE_NP_ERROR_ALREADY_INITIALIZED),                                  // -2141913087/0x80550001
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_NOT_INITIALIZED),                                      // -2141913086/0x80550002
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_INVALID_ARGUMENT),                                     // -2141913085/0x80550003
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_UNKNOWN_PLATFORM_TYPE),                                // -2141913084/0x80550004
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_OUT_OF_MEMORY),                                        // -2141913083/0x80550005
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_SIGNED_OUT),                                           // -2141913082/0x80550006
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_USER_NOT_FOUND),                                       // -2141913081/0x80550007
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_CALLBACK_ALREADY_REGISTERED),                          // -2141913080/0x80550008
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_CALLBACK_NOT_REGISTERED),                              // -2141913079/0x80550009
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_NOT_SIGNED_UP),                                        // -2141913078/0x8055000A
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_AGE_RESTRICTION),                                      // -2141913077/0x8055000B
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_LOGOUT),                                               // -2141913076/0x8055000C
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST),                         // -2141913075/0x8055000D
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST_FOR_TITLE),               // -2141913074/0x8055000E
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST),                               // -2141913073/0x8055000F
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED),                          // -2141913072/0x80550010
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_INVALID_SIZE),                                         // -2141913071/0x80550011
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_ABORTED),                                              // -2141913070/0x80550012
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_REQUEST_MAX),                                          // -2141913069/0x80550013
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_REQUEST_NOT_FOUND),                                    // -2141913068/0x80550014
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_INVALID_ID),                                           // -2141913067/0x80550015
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_NP_TITLE_DAT_NOT_FOUND),                               // -2141913066/0x80550016
    DIRTYSOCK_ErrorName(SCE_NP_ERROR_INCONSISTENT_NP_TITLE_ID),                             // -2141913065/0x80550017

    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_INVALID_ARGUMENT),                                // -2141912319/0x80550301
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_INVALID_SIZE),                                    // -2141912318/0x80550302
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_OUT_OF_MEMORY),                                   // -2141912317/0x80550303
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ABORTED),                                         // -2141912316/0x80550304
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_REQUEST_MAX),                                     // -2141912315/0x80550305
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_REQUEST_NOT_FOUND),                               // -2141912314/0x80550306
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_INVALID_ID),                                      // -2141912313/0x80550307
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_SERVICE_END),                                     // -2141912064/0x80550400
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_SERVICE_DOWN),                                    // -2141912063/0x80550401
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_SERVICE_BUSY),                                    // -2141912062/0x80550402
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_SERVER_MAINTENANCE),                              // -2141912061/0x80550403
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_DATA_LENGTH),                           // -2141912048/0x80550410
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_USER_AGENT),                            // -2141912047/0x80550411
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_VERSION),                               // -2141912046/0x80550412
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_SERVICE_ID),                            // -2141912032/0x80550420
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_CREDENTIAL),                            // -2141912031/0x80550421
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_ENTITLEMENT_ID),                        // -2141912030/0x80550422
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_S_INVALID_CONSUMED_COUNT),                        // -2141912029/0x80550423
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_INVALID_CONSOLE_ID),                              // -2141912028/0x80550424
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_CONSOLE_ID_SUSPENDED),                            // -2141912025/0x80550427
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_CLOSED),                                  // -2141912016/0x80550430
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_SUSPENDED),                               // -2141912015/0x80550431
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_EULA),                              // -2141912014/0x80550432
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT1),                          // -2141912000/0x80550440
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT2),                          // -2141911999/0x80550441
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT3),                          // -2141911998/0x80550442
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT4),                          // -2141911997/0x80550443
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT5),                          // -2141911996/0x80550444
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT6),                          // -2141911995/0x80550445
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT7),                          // -2141911994/0x80550446
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT8),                          // -2141911993/0x80550447
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT9),                          // -2141911992/0x80550448
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT10),                         // -2141911991/0x80550449
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT11),                         // -2141911990/0x8055044A
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT12),                         // -2141911989/0x8055044B
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT13),                         // -2141911988/0x8055044C
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT14),                         // -2141911987/0x8055044D
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT15),                         // -2141911986/0x8055044E
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_ACCOUNT_RENEW_ACCOUNT16),                         // -2141911985/0x8055044F
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_SUB_ACCOUNT_RENEW_EULA),                          // -2141911985/0x8055044F
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_ERROR_UNKNOWN),                                         // -2141911936/0x80550480

    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_ARGUMENT),                                // -2141911551/0x80550601
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INSUFFICIENT),                                    // -2141911550/0x80550602
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_PARSER_FAILED),                                   // -2141911549/0x80550603
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_PROTOCOL_ID),                             // -2141911548/0x80550604
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_NP_ID),                                   // -2141911547/0x80550605
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_NP_ENV),                                  // -2141911546/0x80550606
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_CHARACTER),                               // -2141911544/0x80550608
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_NOT_MATCH),                                       // -2141911543/0x80550609
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_INVALID_TITLEID),                                 // -2141911542/0x8055060A
    DIRTYSOCK_ErrorName(SCE_NP_UTIL_ERROR_UNKNOWN),                                         // -2141911538/0x8055060E
#ifndef DIRTYCODE_PS5
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_ALREADY_INITIALIZED),                        // -2141911295/0x80550701
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_NOT_INITIALIZED),                            // -2141911294/0x80550702
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_OUT_OF_MEMORY),                              // -2141911293/0x80550703
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_ARGUMENT),                           // -2141911292/0x80550704
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_NO_LOGIN),                                   // -2141911291/0x80550705
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_TOO_MANY_OBJECTS),                           // -2141911290/0x80550706
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_ABORTED),                                    // -2141911289/0x80550707
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_BAD_RESPONSE),                               // -2141911288/0x80550708
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_BODY_TOO_LARGE),                             // -2141911287/0x80550709
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_HTTP_SERVER),                                // -2141911286/0x8055070A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_SIGNATURE),                          // -2141911285/0x8055070B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INSUFFICIENT_ARGUMENT),                      // -2141911284/0x8055070C
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_UNKNOWN_TYPE),                               // -2141911283/0x8055070D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_ID),                                 // -2141911282/0x8055070E
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_ONLINE_ID),                          // -2141911281/0x8055070F
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_TYPE),                               // -2141911279/0x80550711
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_TRANSACTION_ALREADY_END),                    // -2141911278/0x80550712
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_PARTITION),                          // -2141911277/0x80550713
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_INVALID_ALIGNMENT),                          // -2141911276/0x80550714
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_CLIENT_HANDLE_ALREADY_EXISTS),               // -2141911275/0x80550715
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_NO_RESOURCE),                                // -2141911274/0x80550716
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_REQUEST_BEFORE_END),                         // -2141911273/0x80550717
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_TOO_MANY_SLOTID),                            // -2141911272/0x80550718
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_TOO_MANY_NPID),                              // -2141911271/0x80550719
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_SCORE_INVALID_SAVEDATA_OWNER),               // -2141911270/0x8055071A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_ERROR_TUS_INVALID_SAVEDATA_OWNER),                 // -2141911269/0x8055071B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_BAD_REQUEST),                         // -2141911039/0x80550801
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_TICKET),                      // -2141911038/0x80550802
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_SIGNATURE),                   // -2141911037/0x80550803
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_NPID),                        // -2141911035/0x80550805
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_FORBIDDEN),                           // -2141911034/0x80550806
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INTERNAL_SERVER_ERROR),               // -2141911033/0x80550807
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_VERSION_NOT_SUPPORTED),               // -2141911032/0x80550808
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_SERVICE_UNAVAILABLE),                 // -2141911031/0x80550809
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_PLAYER_BANNED),                       // -2141911030/0x8055080A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_CENSORED),                            // -2141911029/0x8055080B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_RECORD_FORBIDDEN),            // -2141911028/0x8055080C
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_USER_PROFILE_NOT_FOUND),              // -2141911027/0x8055080D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UPLOADER_DATA_NOT_FOUND),             // -2141911026/0x8055080E
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_QUOTA_MASTER_NOT_FOUND),              // -2141911025/0x8055080F
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_TITLE_NOT_FOUND),             // -2141911024/0x80550810
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_BLACKLISTED_USER_ID),                 // -2141911023/0x80550811
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_GAME_RANKING_NOT_FOUND),              // -2141911022/0x80550812
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_STORE_NOT_FOUND),             // -2141911020/0x80550814
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NOT_BEST_SCORE),                      // -2141911019/0x80550815
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_LATEST_UPDATE_NOT_FOUND),             // -2141911018/0x80550816
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_BOARD_MASTER_NOT_FOUND),      // -2141911017/0x80550817
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_GAME_DATA_MASTER_NOT_FOUND),  // -2141911016/0x80550818
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ANTICHEAT_DATA),              // -2141911015/0x80550819
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_DATA),                      // -2141911014/0x8055081A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NO_SUCH_USER_NPID),                   // -2141911013/0x8055081B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ENVIRONMENT),                 // -2141911011/0x8055081D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ONLINE_NAME_CHARACTER),       // -2141911009/0x8055081F
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ONLINE_NAME_LENGTH),          // -2141911008/0x80550820
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ABOUT_ME_CHARACTER),          // -2141911007/0x80550821
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ABOUT_ME_LENGTH),             // -2141911006/0x80550822
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_SCORE),                       // -2141911005/0x80550823
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_OVER_THE_RANKING_LIMIT),              // -2141911004/0x80550824
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_FAIL_TO_CREATE_SIGNATURE),            // -2141911002/0x80550826
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_MASTER_INFO_NOT_FOUND),       // -2141911001/0x80550827
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_OVER_THE_GAME_DATA_LIMIT),            // -2141911000/0x80550828
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_SELF_DATA_NOT_FOUND),                 // -2141910998/0x8055082A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_USER_NOT_ASSIGNED),                   // -2141910997/0x8055082B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_GAME_DATA_ALREADY_EXISTS),            // -2141910996/0x8055082C
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MANY_RESULTS),                    // -2141910995/0x8055082D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NOT_RECORDABLE_VERSION),              // -2141910994/0x8055082E
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_TITLE_MASTER_NOT_FOUND), // -2141910968/0x80550848
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_VIRTUAL_USER),                // -2141910967/0x80550849
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND),         // -2141910966/0x8055084A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NON_PLUS_MEMBER),                     // -2141910947/0x8055085D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_SEQUENCE),                    // -2141910946/0x8055085E
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_SAVEDATA_NOT_FOUND),                  // -2141910945/0x8055085F
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MANY_SAVEDATA_FILES),             // -2141910944/0x80550860
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MUCH_TOTAL_SAVEDATA_SIZE),        // -2141910943/0x80550861
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NOT_YET_DOWNLOADABLE),                // -2141910942/0x80550862
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_BLACKLISTED_TITLE),                   // -2141910936/0x80550868
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_ICONDATA),                  // -2141910935/0x80550869
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RECORD_DATE_IS_NEWER_THAN_COMP_DATE), // -2141910930/0x8055086E
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_SAVEDATA),                  // -2141910934/0x8055086A
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_SIGNATURE),                   // -2141910933/0x8055086B
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_MD5SUM),                      // -2141910932/0x8055086C
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MUCH_SAVEDATA_SIZE),              // -2141910931/0x8055086D
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_CONDITIONS_NOT_SATISFIED),            // -2141910925/0x80550873
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UNSUPPORTED_PLATFORM),                // -2141910920/0x80550878
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_BEFORE_SERVICE),             // -2141910880/0x805508A0
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_END_OF_SERVICE),             // -2141910879/0x805508A1
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_MAINTENANCE),                // -2141910878/0x805508A2
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_BEFORE_SERVICE),              // -2141910877/0x805508A3
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_END_OF_SERVICE),              // -2141910876/0x805508A4
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_MAINTENANCE),                 // -2141910875/0x805508A5
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_NO_SUCH_TITLE),                       // -2141910874/0x805508A6
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_BEFORE_SERVICE),   // -2141910870/0x805508AA
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_END_OF_SERVICE),   // -2141910869/0x805508AB
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_MAINTENANCE),      // -2141910868/0x805508AC
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_FSR_BEFORE_SERVICE),                  // -2141910867/0x805508AD
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_FSR_END_OF_SERVICE),                  // -2141910866/0x805508AE
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_FSR_MAINTENANCE),                     // -2141910865/0x805508AF
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UBS_BEFORE_SERVICE),                  // -2141910864/0x805508B0
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UBS_END_OF_SERVICE),                  // -2141910863/0x805508B1
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UBS_MAINTENANCE),                     // -2141910862/0x805508B2
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_BASIC_BLACKLISTED_USER_ID),           // -2141910861/0x805508B3
    DIRTYSOCK_ErrorName(SCE_NP_COMMUNITY_SERVER_ERROR_UNSPECIFIED),                         // -2141910785/0x805508FF
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_OUT_OF_MEMORY),                              // -2141910015/0x80550C01
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_ALREADY_INITIALIZED),                        // -2141910014/0x80550C02
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_NOT_INITIALIZED),                            // -2141910013/0x80550C03
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_MAX),                                // -2141910012/0x80550C04
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_ALREADY_EXISTS),                     // -2141910011/0x80550C05
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_NOT_FOUND),                          // -2141910010/0x80550C06
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_ALREADY_STARTED),                    // -2141910009/0x80550C07
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_NOT_STARTED),                        // -2141910008/0x80550C08
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SERVER_NOT_FOUND),                           // -2141910007/0x80550C09
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_ARGUMENT),                           // -2141910006/0x80550C0A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_CONTEXT_ID),                         // -2141910005/0x80550C0B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_SERVER_ID),                          // -2141910004/0x80550C0C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_WORLD_ID),                           // -2141910003/0x80550C0D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_LOBBY_ID),                           // -2141910002/0x80550C0E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_ROOM_ID),                            // -2141910001/0x80550C0F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_MEMBER_ID),                          // -2141910000/0x80550C10
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_ATTRIBUTE_ID),                       // -2141909999/0x80550C11
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_CASTTYPE),                           // -2141909998/0x80550C12
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_SORT_METHOD),                        // -2141909997/0x80550C13
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_MAX_SLOT),                           // -2141909996/0x80550C14
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_MATCHING_SPACE),                     // -2141909994/0x80550C16
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_BLOCK_KICK_FLAG),                    // -2141909993/0x80550C17
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_MESSAGE_TARGET),                     // -2141909992/0x80550C18
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_RANGE_FILTER_MAX),                           // -2141909991/0x80550C19
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INSUFFICIENT_BUFFER),                        // -2141909990/0x80550C1A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_DESTINATION_DISAPPEARED),                    // -2141909989/0x80550C1B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_REQUEST_TIMEOUT),                            // -2141909988/0x80550C1C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_ALIGNMENT),                          // -2141909987/0x80550C1D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONNECTION_CLOSED_BY_SERVER),                // -2141909986/0x80550C1E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SSL_VERIFY_FAILED),                          // -2141909985/0x80550C1F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SSL_HANDSHAKE),                              // -2141909984/0x80550C20
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SSL_SEND),                                   // -2141909983/0x80550C21
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SSL_RECV),                                   // -2141909982/0x80550C22
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_JOINED_SESSION_MAX),                         // -2141909981/0x80550C23
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_ALREADY_JOINED),                             // -2141909980/0x80550C24
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_SESSION_TYPE),                       // -2141909979/0x80550C25
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_NP_SIGNED_OUT),                              // -2141909978/0x80550C26
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_BUSY),                                       // -2141909977/0x80550C27
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SERVER_NOT_AVAILABLE),                       // -2141909976/0x80550C28
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_NOT_ALLOWED),                                // -2141909975/0x80550C29
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_ABORTED),                                    // -2141909974/0x80550C2A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_REQUEST_NOT_FOUND),                          // -2141909973/0x80550C2B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SESSION_DESTROYED),                          // -2141909972/0x80550C2C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CONTEXT_STOPPED),                            // -2141909971/0x80550C2D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_REQUEST_PARAMETER),                  // -2141909970/0x80550C2E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_NOT_NP_SIGN_IN),                             // -2141909969/0x80550C2F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_ROOM_NOT_FOUND),                             // -2141909968/0x80550C30
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_ROOM_MEMBER_NOT_FOUND),                      // -2141909967/0x80550C31
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_LOBBY_NOT_FOUND),                            // -2141909966/0x80550C32
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_LOBBY_MEMBER_NOT_FOUND),                     // -2141909965/0x80550C33
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_KEEPALIVE_TIMEOUT),                          // -2141909964/0x80550C34
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_TIMEOUT_TOO_SHORT),                          // -2141909963/0x80550C35
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_TIMEDOUT),                                   // -2141909962/0x80550C36
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_SLOTGROUP),                          // -2141909961/0x80550C37
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_INVALID_ATTRIBUTE_SIZE),                     // -2141909960/0x80550C38
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_CANNOT_ABORT),                               // -2141909959/0x80550C39
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_ERROR_SESSION_NOT_FOUND),                          // -2141909958/0x80550C3A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_BAD_REQUEST),                         // -2141909759/0x80550D01
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_SERVICE_UNAVAILABLE),                 // -2141909758/0x80550D02
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_BUSY),                                // -2141909757/0x80550D03
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_END_OF_SERVICE),                      // -2141909756/0x80550D04
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_INTERNAL_SERVER_ERROR),               // -2141909755/0x80550D05
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_PLAYER_BANNED),                       // -2141909754/0x80550D06
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_FORBIDDEN),                           // -2141909753/0x80550D07
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_BLOCKED),                             // -2141909752/0x80550D08
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_UNSUPPORTED_NP_ENV),                  // -2141909751/0x80550D09
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_INVALID_TICKET),                      // -2141909750/0x80550D0A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_INVALID_SIGNATURE),                   // -2141909749/0x80550D0B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_EXPIRED_TICKET),                      // -2141909748/0x80550D0C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_ENTITLEMENT_REQUIRED),                // -2141909747/0x80550D0D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_CONTEXT),                     // -2141909746/0x80550D0E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_CLOSED),                              // -2141909745/0x80550D0F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_TITLE),                       // -2141909744/0x80550D10
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_WORLD),                       // -2141909743/0x80550D11
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_LOBBY),                       // -2141909742/0x80550D12
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_ROOM),                        // -2141909741/0x80550D13
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_LOBBY_INSTANCE),              // -2141909740/0x80550D14
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_ROOM_INSTANCE),               // -2141909739/0x80550D15
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_PASSWORD_MISMATCH),                   // -2141909737/0x80550D17
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_LOBBY_FULL),                          // -2141909736/0x80550D18
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_ROOM_FULL),                           // -2141909735/0x80550D19
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_GROUP_FULL),                          // -2141909733/0x80550D1B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_USER),                        // -2141909732/0x80550D1C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_GROUP_PASSWORD_MISMATCH),             // -2141909731/0x80550D1D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_TITLE_PASSPHRASE_MISMATCH),           // -2141909730/0x80550D1E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_LOBBY_ALREADY_EXIST),                 // -2141909723/0x80550D25
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_ROOM_ALREADY_EXIST),                  // -2141909722/0x80550D26
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_CONSOLE_BANNED),                      // -2141909720/0x80550D28
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_ROOMGROUP),                        // -2141909719/0x80550D29
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_GROUP),                       // -2141909718/0x80550D2A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NO_PASSWORD),                         // -2141909717/0x80550D2B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_INVALID_GROUP_SLOT_NUM),              // -2141909716/0x80550D2C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_INVALID_PASSWORD_SLOT_MASK),          // -2141909715/0x80550D2D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_DUPLICATE_GROUP_LABEL),               // -2141909714/0x80550D2E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_REQUEST_OVERFLOW),                    // -2141909713/0x80550D2F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_ALREADY_JOINED),                      // -2141909712/0x80550D30
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_NAT_TYPE_MISMATCH),                   // -2141909711/0x80550D31
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_ROOM_INCONSISTENCY),                  // -2141909710/0x80550D32
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SERVER_ERROR_BLOCKED_USER_IN_ROOM),                // -2141909709/0x80550D33
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_NOT_INITIALIZED),                  // -2141909503/0x80550E01
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_ALREADY_INITIALIZED),              // -2141909502/0x80550E02
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_OUT_OF_MEMORY),                    // -2141909501/0x80550E03
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CTXID_NOT_AVAILABLE),              // -2141909500/0x80550E04
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CTX_NOT_FOUND),                    // -2141909499/0x80550E05
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_REQID_NOT_AVAILABLE),              // -2141909498/0x80550E06
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_REQ_NOT_FOUND),                    // -2141909497/0x80550E07
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_PARSER_CREATE_FAILED),             // -2141909496/0x80550E08
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_PARSER_FAILED),                    // -2141909495/0x80550E09
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_INVALID_NAMESPACE),                // -2141909494/0x80550E0A
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_NETINFO_NOT_AVAILABLE),            // -2141909493/0x80550E0B
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_PEER_NOT_RESPONDING),              // -2141909492/0x80550E0C
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CONNID_NOT_AVAILABLE),             // -2141909491/0x80550E0D
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CONN_NOT_FOUND),                   // -2141909490/0x80550E0E
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_PEER_UNREACHABLE),                 // -2141909489/0x80550E0F
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_TERMINATED_BY_PEER),               // -2141909488/0x80550E10
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_TIMEOUT),                          // -2141909487/0x80550E11
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CTX_MAX),                          // -2141909486/0x80550E12
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_RESULT_NOT_FOUND),                 // -2141909485/0x80550E13
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_CONN_IN_PROGRESS),                 // -2141909484/0x80550E14
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_INVALID_ARGUMENT),                 // -2141909483/0x80550E15
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_OWN_NP_ID),                        // -2141909482/0x80550E16
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_TOO_MANY_CONN),                    // -2141909481/0x80550E17
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_TERMINATED_BY_MYSELF),             // -2141909480/0x80550E18
    DIRTYSOCK_ErrorName(SCE_NP_MATCHING2_SIGNALING_ERROR_MATCHING2_PEER_NOT_FOUND),         // -2141909479/0x80550E19
#endif
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_NOT_INITIALIZED),                       // -2141905150/0x80551F02
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_BAD_RESPONSE),                          // -2141905149/0x80551F03
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_OUT_OF_MEMORY),                         // -2141905148/0x80551F04
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_INVALID_ARGUMENT),                      // -2141905147/0x80551F05
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_INVALID_SIZE),                          // -2141905146/0x80551F06
    DIRTYSOCK_ErrorName(SCE_NP_BANDWIDTH_TEST_ERROR_CONTEXT_NOT_AVAILABLE),                 // -2141905145/0x80551F07

#ifndef DIRTYCODE_PS5
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_UNKNOWN),                                        // -2141903615/0x80552501
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_ALREADY_INITIALIZED),                            // -2141903614/0x80552502
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_NOT_INITIALIZED),                                // -2141903613/0x80552503
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_INVALID_ARGUMENT),                               // -2141903612/0x80552504
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_OUT_OF_MEMORY),                                  // -2141903611/0x80552505
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_NOT_IN_PARTY),                                   // -2141903610/0x80552506
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_VOICE_NOT_ENABLED),                              // -2141903609/0x80552507
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_MEMBER_NOT_FOUND),                               // -2141903608/0x80552508
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_SEND_BUSY),                                      // -2141903607/0x80552509
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_SEND_OUT_OF_CONTEXT),                            // -2141903600/0x80552510
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_INVALID_STATE),                                  // -2141903599/0x80552511
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_INVALID_LOCAL_PARTY_MEMBER),                     // -2141903598/0x80552512
    DIRTYSOCK_ErrorName(SCE_NP_PARTY_ERROR_INVALID_PROCESS_TYPE),                           // -2141903597/0x80552513

    #if SCE_ORBIS_SDK_VERSION < 0x07008001u
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_INVALID_ARGUMENT),                        // -2141903358/0x80552602
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_OUT_OF_MEMORY),                           // -2141903357/0x80552603
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_EXCEEDS_MAX),                             // -2141903356/0x80552604
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_UGM_RESTRICTION),                         // -2141903355/0x80552605
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_ABORTED),                                 // -2141903354/0x80552606
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_ACCOUNT_NOT_BOUND),                       // -2141903353/0x80552607
    DIRTYSOCK_ErrorName(SCE_NP_SNS_FACEBOOK_ERROR_CANCELED_BY_SYSTEM),                      // -2141903352/0x80552608
    #endif

    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_NOT_INITIALIZED),                            // -2141903103/0x80552701
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_ALREADY_INITIALIZED),                        // -2141903102/0x80552702
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_OUT_OF_MEMORY),                              // -2141903101/0x80552703
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CTXID_NOT_AVAILABLE),                        // -2141903100/0x80552704
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CTX_NOT_FOUND),                              // -2141903099/0x80552705
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_REQID_NOT_AVAILABLE),                        // -2141903098/0x80552706
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_REQ_NOT_FOUND),                              // -2141903097/0x80552707
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_PARSER_CREATE_FAILED),                       // -2141903096/0x80552708
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_PARSER_FAILED),                              // -2141903095/0x80552709
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_INVALID_NAMESPACE),                          // -2141903094/0x8055270A
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_NETINFO_NOT_AVAILABLE),                      // -2141903093/0x8055270B
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_PEER_NOT_RESPONDING),                        // -2141903092/0x8055270C
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CONNID_NOT_AVAILABLE),                       // -2141903091/0x8055270D
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CONN_NOT_FOUND),                             // -2141903090/0x8055270E
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_PEER_UNREACHABLE),                           // -2141903089/0x8055270F
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_TERMINATED_BY_PEER),                         // -2141903088/0x80552710
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_TIMEOUT),                                    // -2141903087/0x80552711
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CTX_MAX),                                    // -2141903086/0x80552712
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_RESULT_NOT_FOUND),                           // -2141903085/0x80552713
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_CONN_IN_PROGRESS),                           // -2141903084/0x80552714
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_INVALID_ARGUMENT),                           // -2141903083/0x80552715
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_OWN_NP_ID),                                  // -2141903082/0x80552716
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_TOO_MANY_CONN),                              // -2141903081/0x80552717
    DIRTYSOCK_ErrorName(SCE_NP_SIGNALING_ERROR_TERMINATED_BY_MYSELF),                       // -2141903080/0x80552718
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_OUT_OF_MEMORY),                                 // -2141902591/0x80552901
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT),                              // -2141902590/0x80552902
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_INVALID_LIB_CONTEXT_ID),                        // -2141902589/0x80552903
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_LIB_CONTEXT_NOT_FOUND),                         // -2141902588/0x80552904
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_USER_CONTEXT_NOT_FOUND),                        // -2141902587/0x80552905
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_REQUEST_NOT_FOUND),                             // -2141902586/0x80552906
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN),                                 // -2141902585/0x80552907
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_INVALID_CONTENT_PARAMETER),                     // -2141902584/0x80552908
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_ABORTED),                                       // -2141902583/0x80552909
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_USER_CONTEXT_ALREADY_EXIST),                    // -2141902582/0x8055290A
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_PUSH_EVENT_FILTER_NOT_FOUND),                   // -2141902581/0x8055290B
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_PUSH_EVENT_CALLBACK_NOT_FOUND),                 // -2141902580/0x8055290C
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_HANDLE_NOT_FOUND),                              // -2141902579/0x8055290D
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_SERVICE_PUSH_EVENT_FILTER_NOT_FOUND),           // -2141902578/0x8055290E
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_SERVICE_PUSH_EVENT_CALLBACK_NOT_FOUND),         // -2141902577/0x8055290F
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_SIGNED_IN_USER_NOT_FOUND),                      // -2141902576/0x80552910
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_LIB_CONTEXT_BUSY),                              // -2141902575/0x80552911
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_USER_CONTEXT_BUSY),                             // -2141902574/0x80552912
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_REQUEST_BUSY),                                  // -2141902573/0x80552913
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_INVALID_HTTP_STATUS_CODE),                      // -2141902572/0x80552914
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_PROHIBITED_HTTP_HEADER),                        // -2141902571/0x80552915
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_PROHIBITED_FUNCTION_CALL),                      // -2141902570/0x80552916
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_MULTIPART_PART_NOT_FOUND),                      // -2141902569/0x80552917
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_PARAMETER_TOO_LONG),                            // -2141902568/0x80552918
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_HANDLE_BUSY),                                   // -2141902567/0x80552919
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_LIB_CONTEXT_MAX),                               // -2141902566/0x8055291A
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_USER_CONTEXT_MAX),                              // -2141902565/0x8055291B
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_EXTD_PUSH_EVENT_FILTER_NOT_FOUND),              // -2141902564/0x8055291C
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_EXTD_PUSH_EVENT_CALLBACK_NOT_FOUND),            // -2141902563/0x8055291D
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_AFTER_SEND),                                    // -2141902562/0x8055291E
    DIRTYSOCK_ErrorName(SCE_NP_WEBAPI_ERROR_TIMEOUT),                                       // -2141902561/0x8055291F
#endif

#ifdef DIRTYCODE_PS5
    { (uint32_t)-2099249150, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_2_UNKNOWN_ERROR_HAS_OCCURRED" },          // -2099249150/0x82E00002
    { (uint32_t)-2099249134, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_18_NEEDS_SOFTWARE_UPDATE" },              // -2099249134/0x82E00012
    { (uint32_t)-2099249132, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_20_UNMATCH_SIGN_IN_ID_OR_PASSWORD" },     // -2099249132/0x82E00014
    { (uint32_t)-2099249125, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_27_ACCOUNT_HAS_BEEN_REMOVED_LOGICALLY" }, // -2099249125/0x82E0001B
    { (uint32_t)-2099249124, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_28_ACCOUNT_HAS_BEEN_BANNED" },            // -2099249124/0x82E0001C
    { (uint32_t)-2099249123, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_29_CONSOLE_HAS_BEEN_BANNED" },            // -2099249123/0x82E0001D
    { (uint32_t)-2099249101, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_51_NEEDS_EMAIL_VERIFICATION" },           // -2099249101/0x82E00033
    { (uint32_t)-2099249052, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_100_PASSWORD_WAS_RESET_BY_SERVER" },      // -2099249052/0x82E00064
    { (uint32_t)-2099249049, "SCE_NP_WEBAPI_SERVER_ERROR_OAUTH_103_NEEDS_EULA_RE_AGREEMENT" },           // -2099249049/0x82E00067

#endif

    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_UNKNOWN),                                  // -2141902336/0x80552A00
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_INVALID_REQUEST),                          // -2141902335/0x80552A01
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_UNAUTHORIZED_CLIENT),                      // -2141902334/0x80552A02
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_ACCESS_DENIED),                            // -2141902333/0x80552A03
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_UNSUPPORTED_RESPONSE_TYPE),                // -2141902332/0x80552A04
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_SERVER_ERROR),                             // -2141902330/0x80552A06
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_TEMPORARILY_UNAVAILABLE),                  // -2141902329/0x80552A07
    DIRTYSOCK_ErrorName(SCE_NP_AUTH_SERVER_ERROR_INVALID_GRANT),                            // -2141902327/0x80552A09

#ifndef DIRTYCODE_PS5
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_OUT_OF_MEMORY),                        // -2141902079/0x80552B01
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_INVALID_ARGUMENT),                     // -2141902078/0x80552B02
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_LIB_CONTEXT_NOT_FOUND),                // -2141902077/0x80552B03
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_NOT_SIGNED_IN),                        // -2141902076/0x80552B04
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_HANDLE_NOT_FOUND),                     // -2141902075/0x80552B05
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_ABORTED),                              // -2141902074/0x80552B06
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_SIGNED_IN_USER_NOT_FOUND),             // -2141902073/0x80552B07
    DIRTYSOCK_ErrorName(SCE_NP_IN_GAME_MESSAGE_ERROR_NOT_PREPARED),                         // -2141902072/0x80552B08
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_UNKNOWN),                                       // -2141907456/0x80551600
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_NOT_INITIALIZED),                               // -2141907455/0x80551601
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_ALREADY_INITIALIZED),                           // -2141907454/0x80551602
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_OUT_OF_MEMORY),                                 // -2141907453/0x80551603
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT),                              // -2141907452/0x80551604
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INSUFFICIENT_BUFFER),                           // -2141907451/0x80551605
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_EXCEEDS_MAX),                                   // -2141907450/0x80551606
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_ABORT),                                         // -2141907449/0x80551607
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_HANDLE),                                // -2141907448/0x80551608
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_CONTEXT),                               // -2141907447/0x80551609
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_TROPHY_ID),                             // -2141907446/0x8055160A
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_GROUP_ID),                              // -2141907445/0x8055160B
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_TROPHY_ALREADY_UNLOCKED),                       // -2141907444/0x8055160C
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_PLATINUM_CANNOT_UNLOCK),                        // -2141907443/0x8055160D
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_ACCOUNTID_NOT_MATCH),                           // -2141907442/0x8055160E
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_NOT_REGISTERED),                                // -2141907441/0x8055160F
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_ALREADY_REGISTERED),                            // -2141907440/0x80551610
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_BROKEN_DATA),                                   // -2141907439/0x80551611
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INSUFFICIENT_SPACE),                            // -2141907438/0x80551612
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_CONTEXT_ALREADY_EXISTS),                        // -2141907437/0x80551613
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_ICON_FILE_NOT_FOUND),                           // -2141907436/0x80551614
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_TRP_FILE_FORMAT),                       // -2141907434/0x80551616
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_UNSUPPORTED_TRP_FILE),                          // -2141907433/0x80551617
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_TROPHY_CONF_FORMAT),                    // -2141907432/0x80551618
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_UNSUPPORTED_TROPHY_CONF),                       // -2141907431/0x80551619
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_TROPHY_NOT_UNLOCKED),                           // -2141907430/0x8055161A
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_USER_NOT_FOUND),                                // -2141907428/0x8055161C
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_USER_NOT_LOGGED_IN),                            // -2141907427/0x8055161D
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_CONTEXT_USER_LOGOUT),                           // -2141907426/0x8055161E
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_USE_TRP_FOR_DEVELOPMENT),                       // -2141907425/0x8055161F
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_NP_TITLE_ID),                           // -2141907424/0x80551620
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_NP_SERVICE_LABEL),                      // -2141907423/0x80551621
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_NOT_SUPPORTED),                                 // -2141907422/0x80551622
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_CONTEXT_EXCEEDS_MAX),                           // -2141907421/0x80551623
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_HANDLE_EXCEEDS_MAX),                            // -2141907420/0x80551624
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INVALID_USER_ID),                               // -2141907419/0x80551625
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_TITLE_CONF_NOT_INSTALLED),                      // -2141907418/0x80551626
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_BROKEN_TITLE_CONF),                             // -2141907417/0x80551627
    DIRTYSOCK_ErrorName(SCE_NP_TROPHY_ERROR_INCONSISTENT_TITLE_CONF),                       // -2141907416/0x80551628
#endif

    /*
        errors from user_service_error.h
    */

    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_INTERNAL),                                   // -2137653247/0x80960001
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_NOT_INITIALIZED),                            // -2137653246/0x80960002
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED),                        // -2137653245/0x80960003
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_NO_MEMORY),                                  // -2137653244/0x80960004
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_INVALID_ARGUMENT),                           // -2137653243/0x80960005
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTED),                    // -2137653242/0x80960006
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_NO_EVENT),                                   // -2137653241/0x80960007
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_NOT_LOGGED_IN),                              // -2137653239/0x80960009
    DIRTYSOCK_ErrorName(SCE_USER_SERVICE_ERROR_BUFFER_TOO_SHORT),                           // -2137653238/0x8096000A

    /*
        errors from audioin.h
    */

    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_FATAL),                                          // -2144993024/0x80260100
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_HANDLE),                                 // -2144993023/0x80260101
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_SIZE),                                   // -2144993022/0x80260102
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_FREQ),                                   // -2144993021/0x80260103
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_TYPE),                                   // -2144993020/0x80260104
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_POINTER),                                // -2144993019/0x80260105
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_INVALID_PARAM),                                  // -2144993018/0x80260106
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_PORT_FULL),                                      // -2144993017/0x80260107
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_OUT_OF_MEMORY),                                  // -2144993016/0x80260108
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_NOT_OPENED),                                     // -2144993015/0x80260109
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_BUSY),                                           // -2144993014/0x8026010A
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_SYSTEM_MEMORY),                                  // -2144993013/0x8026010B
    DIRTYSOCK_ErrorName(SCE_AUDIO_IN_ERROR_SYSTEM_IPC),                                     // -2144993012/0x8026010C

    /*
        errors from audioout.h
    */

    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_NOT_OPENED),                                    // -2144993279/0x80260001
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_BUSY),                                          // -2144993278/0x80260002
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_PORT),                                  // -2144993277/0x80260003
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_POINTER),                               // -2144993276/0x80260004
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_PORT_FULL),                                     // -2144993275/0x80260005
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_SIZE),                                  // -2144993274/0x80260006
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_FORMAT),                                // -2144993273/0x80260007
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_SAMPLE_FREQ),                           // -2144993272/0x80260008
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_VOLUME),                                // -2144993271/0x80260009
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_PORT_TYPE),                             // -2144993270/0x8026000A
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_CONF_TYPE),                             // -2144993268/0x8026000C
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_OUT_OF_MEMORY),                                 // -2144993267/0x8026000D
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_ALREADY_INIT),                                  // -2144993266/0x8026000E
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_NOT_INIT),                                      // -2144993265/0x8026000F
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_MEMORY),                                        // -2144993264/0x80260010
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_SYSTEM_RESOURCE),                               // -2144993263/0x80260011
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_TRANS_EVENT),                                   // -2144993262/0x80260012
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_FLAG),                                  // -2144993261/0x80260013
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_MIXLEVEL),                              // -2144993260/0x80260014
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_ARG),                                   // -2144993259/0x80260015
    DIRTYSOCK_ErrorName(SCE_AUDIO_OUT_ERROR_INVALID_PARAM),                                 // -2144993258/0x80260016

    /*
        errors from voice_types.h
    */

    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_LIBVOICE_NOT_INIT),                                 // -2142369791/0x804E0801
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_LIBVOICE_INITIALIZED),                              // -2142369790/0x804E0802
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_GENERAL),                                           // -2142369789/0x804E0803
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_PORT_INVALID),                                      // -2142369788/0x804E0804
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_ARGUMENT_INVALID),                                  // -2142369787/0x804E0805
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_CONTAINER_INVALID),                                 // -2142369786/0x804E0806
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_TOPOLOGY),                                          // -2142369785/0x804E0807
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_RESOURCE_INSUFFICIENT),                             // -2142369784/0x804E0808
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_SERVICE_DETACHED),                                  // -2142369782/0x804E080A
    DIRTYSOCK_ErrorName(SCE_VOICE_ERROR_SERVICE_ATTACHED),                                  // -2142369781/0x804E080B

    /*
        errors from common_dialog\error.h
    */
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_NOT_SYSTEM_INITIALIZED),                    // -2135425023/0x80B80001
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED),                // -2135425022/0x80B80002
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_NOT_INITIALIZED),                           // -2135425021/0x80B80003
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED),                       // -2135425020/0x80B80004
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_NOT_FINISHED),                              // -2135425019/0x80B80005
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_INVALID_STATE),                             // -2135425018/0x80B80006
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_RESULT_NONE),                               // -2135425017/0x80B80007
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_BUSY),                                      // -2135425016/0x80B80008
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_OUT_OF_MEMORY),                             // -2135425015/0x80B80009
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_PARAM_INVALID),                             // -2135425014/0x80B8000A
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_NOT_RUNNING),                               // -2135425013/0x80B8000B
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_ALREADY_CLOSE),                             // -2135425012/0x80B8000C
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_ARG_NULL),                                  // -2135425011/0x80B8000D
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_UNEXPECTED_FATAL),                          // -2135425010/0x80B8000E
    DIRTYSOCK_ErrorName(SCE_COMMON_DIALOG_ERROR_NOT_SUPPORTED),                             // -2135425009/0x80B8000F

    /*
        errors from app_content.h
    */
    DIRTYSOCK_ErrorName(SCE_APP_CONTENT_ERROR_NOT_INITIALIZED),                             // -2133262335/0x80D90001
    DIRTYSOCK_ErrorName(SCE_APP_CONTENT_ERROR_PARAMETER),                                   // -2133262334/0x80D90002
    DIRTYSOCK_ErrorName(SCE_APP_CONTENT_ERROR_INTERNAL),                                    // -2133262326/0x80D9000A

    /*
        errors from libssh.h
    */

    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BEFORE_INIT),                                         // -2137657343/0x8095F001
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ALREADY_INITED),                                      // -2137657342/0x8095F002
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BROKEN),                                              // -2137657341/0x8095F003
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_NOT_FOUND),                                           // -2137657340/0x8095F004
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INVALID_FORMAT),                                      // -2137657339/0x8095F005
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INVALID_ID),                                          // -2137657338/0x8095F006
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INVALID_VALUE),                                       // -2137657337/0x8095F007
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_OUT_OF_SIZE),                                         // -2137657336/0x8095F008
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INTERNAL),                                            // -2137657335/0x8095F009
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INVALID_CERT),                                        // -2137657334/0x8095F00A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_CN_CHECK),                                            // -2137657333/0x8095F00B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_UNKNOWN_CA),                                          // -2137657332/0x8095F00C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_NOT_AFTER_CHECK),                                     // -2137657331/0x8095F00D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_NOT_BEFORE_CHECK),                                    // -2137657330/0x8095F00E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_EAGAIN),                                              // -2137657329/0x8095F00F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_FATAL_ALERT),                                         // -2137657328/0x8095F010
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BUSY),                                                // -2137657326/0x8095F012
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_OUT_OF_MEMORY),                                       // -2137712683/0x809517D5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_READ_TIMEOUT),                                        // -2137712880/0x80951710
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_SOCKET_CLOSED),                                       // -2137712883/0x8095170D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BAD_LENGTH),                                          // -2137712781/0x80951773
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INDEX_OOB),                                           // -2137712776/0x80951778
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INVALID_ARG),                                         // -2137712774/0x8095177A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_EOF),                                                 // -2137712772/0x8095177C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BAD_EXPONENT),                                        // -2137712771/0x8095177D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INCOMPLETE_SEARCH),                                   // -2137712770/0x8095177E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_INTERNAL_ERROR),                                      // -2137712769/0x8095177F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BAD_CERT_LENGTH),                                     // -2137710382/0x809520D2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_BAD_SIGN_ALGO),                                       // -2137710383/0x809520D1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_MISMATCH_PUBLIC_KEYS),                                // -2137710381/0x809520D3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_KEY_BLOB_CORRUPT),                                    // -2137710380/0x809520D4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_PROTOCOL),                                            // -2137711278/0x80951D52
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_PROTOCOL_RECEIVE_RECORD),                             // -2137711251/0x80951D6D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_PROTOCOL_VERSION),                                    // -2137711240/0x80951D78
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS),                                            // -2137712984/0x809516A8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_THREAD_CREATE),                              // -2137712983/0x809516A9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_MUTEX_CREATE),                               // -2137712982/0x809516AA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_MUTEX_WAIT),                                 // -2137712981/0x809516AB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_MUTEX_RELEASE),                              // -2137712980/0x809516AC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_MUTEX_FREE),                                 // -2137712979/0x809516AD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_GMT_TIME_NOT_AVAILABLE),                     // -2137712978/0x809516AE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_WRAP_MUTEX_WAIT),                            // -2137712977/0x809516AF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RTOS_WRAP_MUTEX_RELEASE),                         // -2137712976/0x809516B0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP),                                             // -2137712884/0x8095170C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_READ_ERROR),                                  // -2137712882/0x8095170E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_READ_BLOCK_FAIL),                             // -2137712881/0x8095170F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_WRITE_ERROR),                                 // -2137712879/0x80951711
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_WRITE_BLOCK_FAIL),                            // -2137712878/0x80951712
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_INIT_FAIL),                                   // -2137712877/0x80951713
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_LISTEN_SOCKET_ERROR),                         // -2137712876/0x80951714
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_LISTEN_BIND_ERROR),                           // -2137712875/0x80951715
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_LISTEN_ERROR),                                // -2137712874/0x80951716
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_ACCEPT_ERROR),                                // -2137712873/0x80951717
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_SOCKOPT_ERROR),                               // -2137712872/0x80951718
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_CONNECT_CREATE),                              // -2137712871/0x80951719
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_CONNECT_ERROR),                               // -2137712870/0x8095171A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_SOCKET_SHARE),                                // -2137712869/0x8095171B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_GETSOCKNAME),                                 // -2137712868/0x8095171C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_ACCEPT_CREATE),                               // -2137712867/0x8095171D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_LISTEN_ADDRINFO),                             // -2137712866/0x8095171E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_TOO_MANY_SOCKETS),                            // -2137712865/0x8095171F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_NO_SUCH_SOCKET),                              // -2137712864/0x80951720
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_END_OF_SOCKET_LIST),                          // -2137712863/0x80951721
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_SELECT_ERROR),                                // -2137712862/0x80951722
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_CONNECT_EALREADY),                            // -2137712861/0x80951723
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_CONNECT_INPROGRESS),                          // -2137712860/0x80951724
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TCP_WOULDBLOCK),                                  // -2137712859/0x80951725
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GENERAL),                                         // -2137712784/0x80951770
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NULL_POINTER),                                    // -2137712783/0x80951771
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DIVIDE_BY_ZERO),                                  // -2137712782/0x80951772
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_MODULO),                                      // -2137712780/0x80951774
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EVEN_NUMBER),                                     // -2137712779/0x80951775
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MISSING_STATE_CHANGE),                            // -2137712778/0x80951776
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BUFFER_OVERFLOW),                                 // -2137712777/0x80951777
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NOT_FOUND),                                       // -2137712775/0x80951779
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FALSE),                                           // -2137712773/0x8095177B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM),                                             // -2137712684/0x809517D4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_ALLOC_PTR),                                   // -2137712682/0x809517D6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_ALLOC_SIZE),                                  // -2137712681/0x809517D7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_FREE_PTR),                                    // -2137712680/0x809517D8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PAYLOAD),                                         // -2137712584/0x80951838
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PAYLOAD_EMPTY),                                   // -2137712583/0x80951839
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PAYLOAD_TOO_LARGE),                               // -2137712582/0x8095183A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH),                                            // -2137712384/0x80951900
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_MESG_FRAGMENTED),                            // -2137712383/0x80951901
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_UNEXPECTED_MESG),                            // -2137712382/0x80951902
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_FAILED),                                     // -2137712381/0x80951903
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_BAD_SIGNATURE),                              // -2137712380/0x80951904
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_MISCONFIGURED),                              // -2137712379/0x80951905
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_MISCONFIGURED_PROMPTS),                      // -2137712378/0x80951906
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AUTH_UNKNOWN_METHOD),                             // -2137712377/0x80951907
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BASE64),                                          // -2137712284/0x80951964
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BASE64_BAD_INPUT),                                // -2137712283/0x80951965
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BASE64_NOT_INITIALIZED),                          // -2137712282/0x80951966
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BLOWFISH),                                        // -2137712184/0x809519C8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BLOWFISH_BAD_LENGTH),                             // -2137712183/0x809519C9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DES),                                             // -2137712084/0x80951A2C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DES_BAD_LENGTH),                                  // -2137712083/0x80951A2D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DES_BAD_KEY_LENGTH),                              // -2137712082/0x80951A2E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DES_CIPHER_FAILED),                               // -2137712081/0x80951A2F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_3DES),                                            // -2137711984/0x80951A90
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_3DES_BAD_LENGTH),                                 // -2137711983/0x80951A91
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_3DES_BAD_KEY_LENGTH),                             // -2137711982/0x80951A92
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_3DES_CIPHER_FAILED),                              // -2137711981/0x80951A93
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_KEY_EXCHANGE),                                    // -2137711784/0x80951B58
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_CLIENT_E),                                    // -2137711783/0x80951B59
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RAND),                                            // -2137711684/0x80951BBC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RAND_SEED_TOO_LARGE),                             // -2137711683/0x80951BBD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RAND_CTX_NOT_INITIALIZED),                        // -2137711682/0x80951BBE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RAND_TERMINATE_THREADS),                          // -2137711681/0x80951BBF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SESSION),                                         // -2137711484/0x80951C84
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SESSION_BAD_PAYLOAD),                             // -2137711483/0x80951C85
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SESSION_NOT_OPEN),                                // -2137711482/0x80951C86
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_KEY),                                             // -2137711384/0x80951CE8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_KEY),                                         // -2137711383/0x80951CE9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_KEY_BLOB),                                    // -2137711382/0x80951CEA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_KEY_TYPE),                                    // -2137711381/0x80951CEB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BAD_KEY_BLOB_VERSION),                            // -2137711380/0x80951CEC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL),                                             // -2137711284/0x80951D4C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_TOO_MANY_CONNECTIONS),                        // -2137711283/0x80951D4D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INIT_CONNECTION),                             // -2137711282/0x80951D4E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_BAD_ID),                                      // -2137711281/0x80951D4F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_DISABLED),                                    // -2137711280/0x80951D50
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CONFIG),                                      // -2137711279/0x80951D51
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_PRESECRET),                           // -2137711277/0x80951D53
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_PADDING),                             // -2137711276/0x80951D54
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_MAC),                                 // -2137711275/0x80951D55
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_BAD_STATE),                                   // -2137711274/0x80951D56
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NOT_OPEN),                                    // -2137711273/0x80951D57
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NO_CIPHER_MATCH),                             // -2137711272/0x80951D58
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CERT_VALIDATION_FAILED),                      // -2137711271/0x80951D59
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNABLE_TO_RSVP),                              // -2137711270/0x80951D5A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNSUPPORTED_DIGEST),                          // -2137711269/0x80951D5B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_BAD_RECORD_SIZE),                             // -2137711268/0x80951D5C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNKNOWN_CERTIFICATE_AUTHORITY),               // -2137711267/0x80951D5D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NO_SELF_SIGNED_CERTIFICATES),                 // -2137711266/0x80951D5E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_MUTUAL_AUTHENTICATION_DISABLED),              // -2137711265/0x80951D5F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_MUTUAL_AUTHENTICATION_FAILED),                // -2137711264/0x80951D60
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_MUTUAL_AUTHENTICATION_NOT_REQUESTED),         // -2137711263/0x80951D61
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_MUTUAL_AUTHENTICATION_REQUEST_IGNORED),       // -2137711262/0x80951D62
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_MUTUAL_AUTHENTICATION_REQUIRED),              // -2137711261/0x80951D63
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_CERT_VERIFY_MSG_SIZE),                // -2137711260/0x80951D64
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_CERT_REQUEST_MSG_SIZE),               // -2137711259/0x80951D65
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_MSG_SIZE),                            // -2137711258/0x80951D66
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_SIGNATURE),                           // -2137711257/0x80951D67
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_KEY_SIZE_TOO_LONG),                    // -2137711256/0x80951D68
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_IOCTL_FAILED),                                // -2137711255/0x80951D69
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_VERSION_UNKNOWN),                      // -2137711254/0x80951D6A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_VERSION_SSL3),                         // -2137711253/0x80951D6B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_START),                                // -2137711252/0x80951D6C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_PROCESS_FINISHED),                   // -2137711250/0x80951D6E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_PROCESS_CERTIFICATE),                // -2137711249/0x80951D6F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_PROCESS_CLIENT_HELLO),               // -2137711248/0x80951D70
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_PROCESS_SERVER_HELLO),               // -2137711247/0x80951D71
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_BAD_LENGTH),                         // -2137711246/0x80951D72
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_BAD_STATE),                          // -2137711245/0x80951D73
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PROTOCOL_SERVER),                             // -2137711244/0x80951D74
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PSK_BAD_CONFIG),                              // -2137711243/0x80951D75
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PSK_BAD_LENGTH),                              // -2137711242/0x80951D76
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_PSK_CALLBACK_OVERFLOW_BUF),                   // -2137711241/0x80951D77
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CRYPT_BLOCK_SIZE),                            // -2137711239/0x80951D79
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_FATAL_ALERT),                                 // -2137711238/0x80951D7A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_CERT_LENGTH),                         // -2137711237/0x80951D7B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NO_DATA_TO_SEND),                             // -2137711236/0x80951D7C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NO_DATA_TO_RECEIVE),                          // -2137711235/0x80951D7D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EAP_DATA_SEND),                               // -2137711234/0x80951D7E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_DUPLICATE),                         // -2137711233/0x80951D7F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_LENGTH),                            // -2137711232/0x80951D80
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_SERVER_LIST_LENGTH),                // -2137711231/0x80951D81
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_SERVER_NAME_LENGTH),                // -2137711230/0x80951D82
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_UNSOLICITED_OFFER),                 // -2137711229/0x80951D83
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_UNRECOGNIZED_NAME),                 // -2137711228/0x80951D84
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_TICKET_INIT),                                 // -2137711227/0x80951D85
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNSUPPORTED_ALGORITHM),                       // -2137711226/0x80951D86
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_KEY_TYPE),                            // -2137711225/0x80951D87
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_UNKNOWN_FORMAT),                    // -2137711224/0x80951D88
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNSUPPORTED_CURVE),                           // -2137711223/0x80951D89
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_NO_CIPHERSUITE),                              // -2137711222/0x80951D8A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_WARNING_ALERT),                               // -2137711221/0x80951D8B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INNER_APP_VERIFY_DATA),                       // -2137711220/0x80951D8C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_INVALID_INNER_TYPE),                          // -2137711219/0x80951D8D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_TOO_MANY_REHANDSHAKES),                       // -2137711218/0x80951D8E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_BAD_HEADER_VERSION),                          // -2137711217/0x80951D8F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_BAD_HELLO_COOKIE),                           // -2137711216/0x80951D90
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_CONNECT_TIMED_WAIT),                         // -2137711215/0x80951D91
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_SRTP_CALLBACK_MISSING),                      // -2137711214/0x80951D92
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_SRTP_NO_PROFILE_MATCH),                      // -2137711213/0x80951D93
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_SRTP_EXTENSION_FAILURE),                     // -2137711212/0x80951D94
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_UNABLE_TO_SIGN_CERTIFICATE_VERIFY),           // -2137711211/0x80951D95
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_RETRANSMISSION_BUFFERS_FULL),                // -2137711210/0x80951D96
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_SEND_BUFFER),                                // -2137711209/0x80951D97
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_DROP_REPLAY_RECORD),                         // -2137711208/0x80951D98
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_MTU_TOO_SMALL),                              // -2137711207/0x80951D99
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_DEFRAG_INVALID_HOLE_DESCRIPTOR),             // -2137711206/0x80951D9A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DTLS_DEFRAG_HOLE_SIZE_TOO_SMALL),                 // -2137711205/0x80951D9B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_RENEGOTIATE_LENGTH),                   // -2137711204/0x80951D9C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_RENEGOTIATE_CLIENT_VERIFY),            // -2137711203/0x80951D9D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_RENEGOTIATE_NOT_ALLOWED),              // -2137711202/0x80951D9E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_RENEGOTIATE_ILLEGAL_SCSV),             // -2137711201/0x80951D9F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_SERVER_RENEGOTIATE_ILLEGAL_EXTENSION),        // -2137711200/0x80951DA0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_RENEGOTIATE_LENGTH),                   // -2137711199/0x80951DA1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_RENEGOTIATE_CLIENT_VERIFY),            // -2137711198/0x80951DA2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_RENEGOTIATE_SERVER_VERIFY),            // -2137711197/0x80951DA3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_RENEGOTIATE_NOT_ALLOWED),              // -2137711196/0x80951DA4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CLIENT_RENEGOTIATE_ILLEGAL_EXTENSION),        // -2137711195/0x80951DA5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_DUPLICATE_NAMETYPE_SNI),            // -2137711194/0x80951DA6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_CERTIFICATE_STATUS_UNSOLICITED),    // -2137711193/0x80951DA7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_CERTIFICATE_STATUS_LENGTH),         // -2137711192/0x80951DA8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_CERTIFICATE_STATUS_RESPONSE),       // -2137711191/0x80951DA9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_EXTENSION_ELLIPTIC_CURVE_NOT_RECEIVED),       // -2137711190/0x80951DAA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SSL_CONNECTION_BUSY),                             // -2137711189/0x80951DAB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT),                                            // -2137711184/0x80951DB0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_BUFFER_OVERFLOW),                            // -2137711183/0x80951DB1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STRING_TOO_LONG),                            // -2137711182/0x80951DB2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_STRUCT),                             // -2137711181/0x80951DB3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_NOT_EXPECTED_OID),                           // -2137711180/0x80951DB4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_RSA_EXPONENT_TOO_BIG),                       // -2137711179/0x80951DB5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_EXPIRED),                                    // -2137711178/0x80951DB6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_BAD_COMMON_NAME),                            // -2137711177/0x80951DB7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_PARENT_CERTIFICATE),                 // -2137711176/0x80951DB8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_SIGNATURE),                          // -2137711175/0x80951DB9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_INTERMEDIATE_CERTIFICATE),           // -2137711174/0x80951DBA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_CERT_POLICY),                        // -2137711173/0x80951DBB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_UNSUPPORTED_DIGEST),                         // -2137711172/0x80951DBC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_DNE_STRING_TOO_LONG),                        // -2137711171/0x80951DBD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_CHAIN_NOT_VERIFIED),                         // -2137711170/0x80951DBE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STACK_OVERFLOW),                             // -2137711169/0x80951DBF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_REVOKED),                                    // -2137711168/0x80951DC0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_HASH_TOO_LONG),                              // -2137711167/0x80951DC1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_KEYUSAGE_MISSING),                           // -2137711166/0x80951DC2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_UNKNOWN_CRITICAL_EXTENSION),                 // -2137711165/0x80951DC3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_UNSUPPORTED_SIGNATURE_ALGO),                 // -2137711164/0x80951DC4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_RSA_MODULUS_TOO_BIG),                        // -2137711163/0x80951DC5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_BAD_SUBJECT_NAME),                           // -2137711162/0x80951DC6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_KEY_SIGNATURE_OID_MISMATCH),                 // -2137711161/0x80951DC7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_KEYUSAGE),                           // -2137711160/0x80951DC8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_START_TIME_VALID_IN_FUTURE),                 // -2137711159/0x80951DC9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_INVALID_EXTENDED_KEYUSAGE),                  // -2137711158/0x80951DCA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_UNRECOGNIZED_OID),                           // -2137711157/0x80951DCB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_BASIC_CONSTRAINT_EXTENSION_NOT_FOUND),       // -2137711156/0x80951DCC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA),                                             // -2137711084/0x80951E14
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_KEY),                                 // -2137711083/0x80951E15
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_DECRYPTION),                                  // -2137711082/0x80951E16
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_PKCS1),                               // -2137711081/0x80951E17
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_PKCS1_VERSION),                       // -2137711080/0x80951E18
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_PKCS8),                               // -2137711079/0x80951E19
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_EXPONENT),                            // -2137711078/0x80951E1A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_INVALID_MODULUS),                             // -2137711077/0x80951E1B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_KEY_NOT_READY),                               // -2137711076/0x80951E1C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_DISABLED),                                    // -2137711075/0x80951E1D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_KEY_LENGTH_TOO_SMALL),                        // -2137711074/0x80951E1E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_BAD_SIGNATURE),                               // -2137711073/0x80951E1F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_UNKNOWN_PKCS5_ALGO),                          // -2137711072/0x80951E20
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_UNKNOWN_PKCS8_ALGOID),                        // -2137711071/0x80951E21
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_UNSUPPORTED_PKCS8_OPTION),                    // -2137711070/0x80951E22
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_BUILT_WITH_NO_PKCS8_DECRYPTION),              // -2137711069/0x80951E23
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RSA_UNSUPPORTED_PKCS8_ALGO),                      // -2137711068/0x80951E24
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES),                                             // -2137710884/0x80951EDC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_PAD_LENGTH),                              // -2137710883/0x80951EDD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_LENGTH),                                  // -2137710882/0x80951EDE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_OPERATION),                               // -2137710881/0x80951EDF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_KEY_LENGTH),                              // -2137710880/0x80951EE0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_KEY_DIR),                                 // -2137710879/0x80951EE1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_KEY_MATERIAL),                            // -2137710878/0x80951EE2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_CIPHER_MODE),                             // -2137710877/0x80951EE3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_CIPHER_FAILED),                               // -2137710876/0x80951EE4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_CCM_AUTH_FAIL),                               // -2137710875/0x80951EE5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_AES_BAD_ARG),                                     // -2137710874/0x80951EE6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TREE),                                            // -2137710784/0x80951F40
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TREE_LINKEDCHILD),                                // -2137710783/0x80951F41
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN),                                             // -2137710684/0x80951FA4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_ZERO_LENGTH),                                 // -2137710683/0x80951FA5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_UNRECOGNIZED_PRIMITIVE),                      // -2137710682/0x80951FA6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_INCONSISTENT_LENGTH),                         // -2137710681/0x80951FA7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_INVALID_DATA),                                // -2137710680/0x80951FA8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_BAD_LENGTH_FIELD),                            // -2137710679/0x80951FA9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_NULL_FUNC_PTR),                               // -2137710678/0x80951FAA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_BAD_INT_LENGTH_FIELD),                        // -2137710677/0x80951FAB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ASN_UNEXPECTED_END),                              // -2137710676/0x80951FAC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PRIME),                                           // -2137710584/0x80952008
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PRIME_EXPECTED),                                  // -2137710583/0x80952009
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_AUTH),                                       // -2137710384/0x809520D0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_URI),                                             // -2137710284/0x80952134
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_URI_INVALID_FORMAT),                              // -2137710283/0x80952135
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PEM),                                             // -2137710084/0x809521FC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PEM_STRUCTURE),                                   // -2137710083/0x809521FD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_BAD_ID),                                      // -2137709934/0x80952292
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_BAD_INDEX),                                   // -2137709933/0x80952293
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_INVALID_MODE),                                // -2137709932/0x80952294
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_INVALID_BUNDLE),                              // -2137709931/0x80952295
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_INVALID_ID_INFO),                             // -2137709930/0x80952296
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_UNACCEPTABLE_TS),                             // -2137709929/0x80952297
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SPD_UNMATCHED_ALGOS),                             // -2137709928/0x80952298
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO),                                          // -2137709784/0x80952328
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_DH_UNSUPPORTED_GROUP),                     // -2137709783/0x80952329
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_BAD_PAD),                                  // -2137709782/0x8095232A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_BAD_KEY_TYPE),                             // -2137709781/0x8095232B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_ECC_DISABLED),                             // -2137709780/0x8095232C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_RSA_DISABLED),                             // -2137709779/0x8095232D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_DSA_DISABLED),                             // -2137709778/0x8095232E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_DSA_SIGN_VERIFY_RS_TEST),                  // -2137709777/0x8095232F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_AEAD_FAIL),                                // -2137709776/0x80952330
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_FAILURE),                                  // -2137709775/0x80952331
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRYPTO_BAD_HASH),                                 // -2137709774/0x80952332
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP),                                             // -2137709584/0x809523F0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_INTERFACE_NOT_FOUND),                         // -2137709583/0x809523F1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_BIND),                                        // -2137709582/0x809523F2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_BAD_ADDRESS),                                 // -2137709581/0x809523F3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_SOCKET),                                      // -2137709580/0x809523F4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_CONNECT),                                     // -2137709579/0x809523F5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_IOCTL),                                       // -2137709578/0x809523F6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_WRITE),                                       // -2137709577/0x809523F7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_READ),                                        // -2137709576/0x809523F8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_HOSTNAME_NOT_FOUND),                          // -2137709575/0x809523F9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_WRITE_LEN),                                   // -2137709574/0x809523FA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_NO_FREE_PORTS),                               // -2137709573/0x809523FB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_PORT_IN_USE),                                 // -2137709572/0x809523FC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_BAD_CONTEXT),                                 // -2137709571/0x809523FD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_GETSOCKNAME),                                 // -2137709570/0x809523FE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_READ_TIMEOUT),                                // -2137709569/0x809523FF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_BIND_CTX),                                    // -2137709568/0x80952400
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_UDP_GETADDR),                                     // -2137709567/0x80952401
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ARC4),                                            // -2137709384/0x809524B8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ARC4_BAD_LENGTH),                                 // -2137709383/0x809524B9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL),                                        // -2137709284/0x8095251C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL_NULL_PTR),                               // -2137709283/0x8095251D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL_CREATE),                                 // -2137709282/0x8095251E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL_COUNT_ZERO),                             // -2137709281/0x8095251F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL_GET_POOL_EMPTY),                         // -2137709280/0x80952520
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_POOL_BAD_PUT_POOL_OBJ),                       // -2137709279/0x80952521
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FF),                                              // -2137709084/0x809525E4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FF_DIFFERENT_FIELDS),                             // -2137709083/0x809525E5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FF_INVALID_PT_STRING),                            // -2137709082/0x809525E6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FF_UNSUPPORTED_PT_REPRESENTATION),                // -2137709081/0x809525E7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC),                                              // -2137708984/0x80952648
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_UNUSED),                                       // -2137708983/0x80952649
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_UNSUPPORTED_CURVE),                            // -2137708982/0x8095264A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_INVALID_KEY_FILE_FORMAT),                      // -2137708981/0x8095264B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_UNKNOWN_KEY_FILE_VERSION),                     // -2137708980/0x8095264C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_INCOMPLETE_KEY_FILE),                          // -2137708979/0x8095264D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_INVALID_KEY_TYPE),                             // -2137708978/0x8095264E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_PUBLIC_KEY),                                   // -2137708977/0x8095264F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_INFINITE_RESULT),                              // -2137708976/0x80952650
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_EC_DIFFERENT_CURVE),                              // -2137708975/0x80952651
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RC2),                                             // -2137708684/0x80952774
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RC2_BAD_LENGTH),                                  // -2137708683/0x80952775
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7),                                           // -2137708584/0x809527D8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_STRUCT),                            // -2137708583/0x809527D9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_UNSUPPORTED_ENCRYPTALGO),                   // -2137708582/0x809527DA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_UNSUPPORTED_DIGESTALGO),                    // -2137708581/0x809527DB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_SIGNATURE),                         // -2137708580/0x809527DC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_HASH_TOO_LONG),                             // -2137708579/0x809527DD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NO_CERT_FOR_SIGNER),                        // -2137708578/0x809527DE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NO_RECIPIENT_KEY_MATCH),                    // -2137708577/0x809527DF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NO_CONTENT),                                // -2137708576/0x809527E0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_UNSUPPORTED_KDF),                           // -2137708575/0x809527E1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_UNSUPPORTED_KEY_WRAP),                      // -2137708574/0x809527E2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_UNSUPPORTED_CONTENT_TYPE),                  // -2137708573/0x809527E3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NO_ENCRYPTED_CONTENT),                      // -2137708572/0x809527E4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_ENCRYPTED_LENGTH),                  // -2137708571/0x809527E5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_WRONG_CALLBACK),                            // -2137708570/0x809527E6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_TYPE_FOR_OP),                       // -2137708569/0x809527E7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_DIGEST_DONT_MATCH),                         // -2137708568/0x809527E8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_DETACHED_DATA),                             // -2137708567/0x809527E9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NOT_DETACHED_SIGNATURE),                    // -2137708566/0x809527EA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_DETACHED_DATA_ALREADY_SET),                 // -2137708565/0x809527EB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_ALREADY_STREAMING),                         // -2137708564/0x809527EC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_CONTEXT_COMPLETED),                         // -2137708563/0x809527ED
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_RNG_FUN_REQUIRED_FOR_ECC),                  // -2137708562/0x809527EE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_TAG_VALUE),                         // -2137708561/0x809527EF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_INVALID_ITEM_VALUE),                        // -2137708560/0x809527F0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_CONTEXT_NOT_COMPLETED),                     // -2137708559/0x809527F1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_NO_RECEIPT_REQUEST),                        // -2137708558/0x809527F2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_MISSING_AUTH_ATTRIBUTE),                    // -2137708557/0x809527F3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS7_MISMATCH_SIG_HASH_ALGO),                    // -2137708556/0x809527F4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12),                                          // -2137708484/0x8095283C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_WRONG_VERSION),                            // -2137708483/0x8095283D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_INVALID_STRUCT),                           // -2137708482/0x8095283E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_NOT_EXPECTED_OID),                         // -2137708481/0x8095283F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_PASSWORD_NEEDED),                          // -2137708480/0x80952840
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_INTEGRITY_CHECK_FAILED),                   // -2137708479/0x80952841
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_NO_KNOWN_SIGNERS),                         // -2137708478/0x80952842
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_UNSUPPORTED_ALGO),                         // -2137708477/0x80952843
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_UNKNOWN_BAGTYPE),                          // -2137708476/0x80952844
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_INVALID_ENCRYPT_ARG),                      // -2137708475/0x80952845
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_INVALID_PRIVACY_MODE),                     // -2137708474/0x80952846
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_INVALID_INTEGRITY_MODE),                   // -2137708473/0x80952847
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_DECRYPT_CALLBACK_NOT_SET),                 // -2137708472/0x80952848
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS12_NOT_PRINTABLE_PASSWORD),                   // -2137708471/0x80952849
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DER_ENCODER),                                     // -2137708184/0x80952968
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DER_ENCODER_NOT_SERIALIZED),                      // -2137708183/0x80952969
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DER_ENCODER_OPAQUE),                              // -2137708182/0x8095296A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DER_PARENT_NOT_BER),                              // -2137708181/0x8095296B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DER_BER_NOT_TERMINATED),                          // -2137708180/0x8095296C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BITMAP),                                          // -2137707884/0x80952A94
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BITMAP_CREATE_FAIL),                              // -2137707883/0x80952A95
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BITMAP_TABLE_FULL),                               // -2137707882/0x80952A96
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BITMAP_BIT_IS_SET),                               // -2137707881/0x80952A97
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_BITMAP_BAD_RANGE),                                // -2137707880/0x80952A98
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART),                                        // -2137707784/0x80952AF8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_NULL_PTR),                               // -2137707783/0x80952AF9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_CREATE),                                 // -2137707782/0x80952AFA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_BAD_LENGTH),                             // -2137707781/0x80952AFB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_ALLOC_FAIL),                             // -2137707780/0x80952AFC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_FREE_LIST_DAMAGED),                      // -2137707779/0x80952AFD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MEM_PART_BAD_ADDRESS),                            // -2137707778/0x80952AFE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER),                                           // -2137707684/0x80952B5C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_SEMINIT_FAILED),                            // -2137707683/0x80952B5D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_NO_CALLBACKFN),                             // -2137707682/0x80952B5E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_INVALID_TIMEOUT),                           // -2137707681/0x80952B5F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_NO_CONTBL),                                 // -2137707680/0x80952B60
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_INVALID_TIMER),                             // -2137707679/0x80952B61
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_TIMER_TIMERS_IN_USE),                             // -2137707678/0x80952B62
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ID),                                              // -2137707584/0x80952BC0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ID_CREATE_MAP_FAILED),                            // -2137707583/0x80952BC1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ID_ADD_RANGE_FAILED),                             // -2137707582/0x80952BC2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ID_ALLOC_FAILED),                                 // -2137707581/0x80952BC3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ID_INIT_FAILED),                                  // -2137707580/0x80952BC4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RBTREE),                                          // -2137707484/0x80952C24
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RBTREE_INSERT_FAILED),                            // -2137707483/0x80952C25
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RBTREE_DUPLICATE_NODE),                           // -2137707482/0x80952C26
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_RBTREE_CREATE_FAILED),                            // -2137707481/0x80952C27
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS5),                                           // -2137705784/0x809532C8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS5_BAD_ITERATION_COUNT),                       // -2137705783/0x809532C9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS5_INVALID_HASH_FUNCTION),                     // -2137705782/0x809532CA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS5_DKLEN_TOO_LONG),                            // -2137705781/0x809532CB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKCS5_DKLEN_TOO_SHORT),                           // -2137705780/0x809532CC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH),                                         // -2137705584/0x80953390
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_BAD_TYPE),                                // -2137705583/0x80953391
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_BAD_OFFSET),                              // -2137705582/0x80953392
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_BAD_TABLE),                               // -2137705581/0x80953393
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_ELEM_NOT_FOUND),                          // -2137705580/0x80953394
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_MUTEX_INIT),                              // -2137705579/0x80953395
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INTHASH_BAD_TABLEIDX),                            // -2137705578/0x80953396
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH),                                         // -2137705484/0x809533F4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_NODE_NOT_FOUND),                          // -2137705483/0x809533F5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_NODE_EXISTS),                             // -2137705482/0x809533F6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_NULL_DATUM),                              // -2137705481/0x809533F7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_INVALID_FUNC),                            // -2137705480/0x809533F8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_NOTEMPTY),                                // -2137705479/0x809533F9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STRHASH_NOMATCH),                                 // -2137705478/0x809533FA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE),                                      // -2137705284/0x809534BC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE_UNKNOWN_KEY_TYPE),                     // -2137705283/0x809534BD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE_CERT_KEY_MISMATCH),                    // -2137705282/0x809534BE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE_LOCKED_STORE),                         // -2137705281/0x809534BF
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE_UNSUPPORTED_SIGNALGO),                 // -2137705280/0x809534C0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CERT_STORE_UNSUPPORTED_ECCURVE),                  // -2137705279/0x809534C1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_HASH_VALUE),                                      // -2137705184/0x80953520
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_HASH_TABLE),                                      // -2137705084/0x80953584
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_HASH_TABLE_BAD_SIZE),                             // -2137705083/0x80953585
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SOCKET),                                          // -2137704984/0x809535E8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SOCKET_NULL_REPLY),                               // -2137704983/0x809535E9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SOCKET_INVALID_LEN),                              // -2137704982/0x809535EA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SOCKET_INVALID_MSG),                              // -2137704981/0x809535EB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_SOCKET_NULL_PARAMS),                              // -2137704980/0x809535EC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DEBUG_CONSOLE),                                   // -2137704584/0x80953778
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_DEBUG_CONSOLE_CHANNEL),                           // -2137704583/0x80953779
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_REDBLACK),                                        // -2137704284/0x809538A4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_REDBLACK_NULL_DATUM),                             // -2137704283/0x809538A5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CIRCQ),                                           // -2137704184/0x80953908
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CIRCQ_FULL),                                      // -2137704183/0x80953909
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CIRCQ_EMPTY),                                     // -2137704182/0x8095390A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MOC_IPV4_ERROR),                                  // -2137703984/0x809539D0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CC_CRAWL),                                        // -2137703884/0x80953A34
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CC_CRAWL_PAST_END_BUFFER),                        // -2137703883/0x80953A35
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP),                                            // -2137703784/0x80953A98
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_MISSING_SIGNER_CERT),                        // -2137703783/0x80953A99
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_MISSING_SIGNER_KEY),                         // -2137703782/0x80953A9A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_MISSING_ISSUER_CERT),                        // -2137703781/0x80953A9B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_INVALID_STRUCT),                             // -2137703780/0x80953A9C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_MISSING_SIGNATURE),                          // -2137703779/0x80953A9D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_INVALID_SIGNATURE),                          // -2137703778/0x80953A9E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_RESPONDER_CHECK),                            // -2137703777/0x80953A9F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_UNKNOWN_RESPONSE_STATUS),                    // -2137703776/0x80953AA0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_UNSUPPORTED_RESPONSE_TYPE),                  // -2137703775/0x80953AA1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_MISSING_RSIGNER_CERTS),                      // -2137703774/0x80953AA2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_INIT_FAIL),                                  // -2137703773/0x80953AA3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_ILLEGAL_STATE),                              // -2137703772/0x80953AA4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_NO_MORE_RESPONSE),                           // -2137703771/0x80953AA5
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_BAD_ALGO),                                   // -2137703770/0x80953AA6
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_EXPIRED_RESPONSE),                           // -2137703769/0x80953AA7
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_NONCE_CHECK_FAIL),                           // -2137703768/0x80953AA8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_BAD_REQUEST),                                // -2137703767/0x80953AA9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_INVALID_INPUT),                              // -2137703766/0x80953AAA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_BAD_AIA),                                    // -2137703765/0x80953AAB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_REQUEST_RESPONSE_MISMATCH),                  // -2137703764/0x80953AAC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_FIND_PEER_ADDR_FAILED),                      // -2137703763/0x80953AAD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OCSP_RESPONDER_CONNECT_FAILED),                   // -2137703762/0x80953AAE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OTP),                                             // -2137703184/0x80953CF0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OTP_INVALID_CHALLENGE),                           // -2137703183/0x80953CF1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OTP_INVALID_ALGORITHM),                           // -2137703182/0x80953CF2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OTP_UNSUPPORTED_ALGORITHM),                       // -2137703181/0x80953CF3
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_OTP_INVALID_SEED),                                // -2137703180/0x80953CF4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MTLS),                                            // -2137702784/0x80953E80
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MTLS_BAD_PAYLOAD_LENGTH),                         // -2137702783/0x80953E81
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MTLS_UNKNOWN_MESSAGE_TYPE),                       // -2137702782/0x80953E82
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MTLS_FIND_CHANNEL_RECEIVER_FAILED),               // -2137702781/0x80953E83
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_MTLS_BAD_CALLBACK_CONFIG),                        // -2137702780/0x80953E84
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI),                                             // -2137702684/0x80953EE4
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP),                                             // -2137702584/0x80953F48
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_CONTEXT),                             // -2137702583/0x80953F49
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_PARAMETER),                           // -2137702582/0x80953F4A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_VERSION),                             // -2137702581/0x80953F4B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_IP_RESPONSE),                         // -2137702580/0x80953F4C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_ALGORITHM_NOT_SUPPORTED),                     // -2137702579/0x80953F4D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_MISSING_TRANSACTION_ID),                      // -2137702578/0x80953F4E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_MISSING_RECIPIENT_NONCE),                     // -2137702577/0x80953F4F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_STATE),                               // -2137702576/0x80953F50
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_SUBJECT_NAME_REQUIRED),                       // -2137702575/0x80953F51
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_KEY_REQUIRED),                                // -2137702574/0x80953F52
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_CERTIFICATE_NOT_FOUND),                       // -2137702573/0x80953F53
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_POPTYPE_MISMATCH),                            // -2137702572/0x80953F54
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_CALL_FOR_CERT),                       // -2137702571/0x80953F55
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_INVALID_CALL_FOR_STATUS),                     // -2137702570/0x80953F56
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_MISSING_CREDENTIALS),                         // -2137702569/0x80953F57
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_RESPONSE_TYPE_MISMATCH),                      // -2137702568/0x80953F58
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_SENDER_KID),                                  // -2137702567/0x80953F59
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_PROTECTION_ERROR),                            // -2137702566/0x80953F5A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_NAME_MISMATCH),                               // -2137702565/0x80953F5B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_TRANSACTION_ID_MISMATCH),                     // -2137702564/0x80953F5C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CMP_RECIPIENT_NONCE_MISMATCH),                    // -2137702563/0x80953F5D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRMF),                                            // -2137702484/0x80953FAC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRMF_MISSING_SIGNER_KEY),                         // -2137702483/0x80953FAD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_CRMF_INVALID_ARG),                                // -2137702482/0x80953FAE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS),                                            // -2137702284/0x80954074
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_DSA_FAIL),                                   // -2137702283/0x80954075
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_DSA_SIGN_VERIFY_FAIL),                       // -2137702282/0x80954076
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_ECDSA_FAIL),                                 // -2137702281/0x80954077
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_ECDSA_SIGN_VERIFY_FAIL),                     // -2137702280/0x80954078
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_RSA_FAIL),                                   // -2137702279/0x80954079
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_RSA_SIGN_VERIFY_FAIL),                       // -2137702278/0x8095407A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_RNG_FAIL),                                   // -2137702277/0x8095407B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_SELF_TEST_INCOMPLETE),                       // -2137702276/0x8095407C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_RNG_KAT),                                    // -2137702275/0x8095407D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HASH_KAT_NULL),                              // -2137702274/0x8095407E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HASH_KAT_FAILED),                            // -2137702273/0x8095407F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HASH_KAT_LEN_FAILED),                        // -2137702272/0x80954080
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HMAC_HASH_KAT_NULL),                         // -2137702271/0x80954081
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HMAC_HASH_KAT_FAILED),                       // -2137702270/0x80954082
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_HMAC_HASH_KAT_LEN_FAILED),                   // -2137702269/0x80954083
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_SYM_KAT_NULL),                               // -2137702268/0x80954084
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_SYM_KAT_FAILED),                             // -2137702267/0x80954085
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_SYM_KAT_LEN_FAILED),                         // -2137702266/0x80954086
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_RSA_PCT_FAILED),                             // -2137702265/0x80954087
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_ECDSA_PCT_FAILED),                           // -2137702264/0x80954088
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_ECDH_PCT_FAILED),                            // -2137702263/0x80954089
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_DH_PCT_FAILED),                              // -2137702262/0x8095408A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_INTEGRITY_FAILED),                           // -2137702261/0x8095408B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_NISTRNG_KAT_FAILED),                         // -2137702260/0x8095408C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_CTRDRBG_FAIL),                               // -2137702259/0x8095408D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_FIPS_ECDRBG_FAIL),                                // -2137702258/0x8095408E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT),                                      // -2137702084/0x8095413C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_REQUEST_TIMEOUT),                      // -2137702083/0x8095413D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CERT_REVOKED),                         // -2137702082/0x8095413E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CERT_EXPIRED),                         // -2137702081/0x8095413F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CDPS_MISSING),                         // -2137702080/0x80954140
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_ISSUER_INVALID),                       // -2137702079/0x80954141
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_ISSUER_EXPIRED),                       // -2137702078/0x80954142
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_ISSUER_MISSING),                       // -2137702077/0x80954143
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CERT_PATH),                            // -2137702076/0x80954144
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CRL_NOT_AVAILABLE),                    // -2137702075/0x80954145
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_TOO_MANY_REQUESTS),                    // -2137702074/0x80954146
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_PENDING_CERTPATH_CHECK),               // -2137702073/0x80954147
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_PENDING_CRL_CHECK),                    // -2137702072/0x80954148
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_INVALID_CALLBACK_TYPE),                // -2137702071/0x80954149
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_NOT_FOUND),                            // -2137702070/0x8095414A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_INVALID_ARGUMENT),                     // -2137702069/0x8095414B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_PROFILE_FAILURE),                      // -2137702068/0x8095414C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_NO_SUCH_SESSION),                      // -2137702067/0x8095414D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_TOO_MANY_SESSIONS),                    // -2137702066/0x8095414E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CRL_SIGNATURE_VERIFICATION),           // -2137702065/0x8095414F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_MISSING_CA_URL_INFO),                  // -2137702064/0x80954150
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_MISSING_CERT_URL_INFO),                // -2137702063/0x80954151
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_WRONG_CERTTYPE),                       // -2137702062/0x80954152
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_INIT),                              // -2137702061/0x80954153
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CREATE),                            // -2137702060/0x80954154
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_TXN_FAILED),                        // -2137702059/0x80954155
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_EXTND_CERT_NOT_FOUND),              // -2137702058/0x80954156
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_EXTND_CERT_ADD_FAILED),             // -2137702057/0x80954157
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_EXTND_CERT_DEL_FAILED),             // -2137702056/0x80954158
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_TMP_KEY_ADD_FAILED),                // -2137702055/0x80954159
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_ADD_FAILED),                   // -2137702054/0x8095415A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_NOT_FOUND),                    // -2137702053/0x8095415B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_DEL_FAILED),                   // -2137702052/0x8095415C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CDP_ENTRY_ADD_FAILED),              // -2137702051/0x8095415D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CDP_ENTRY_NOT_FOUND),               // -2137702050/0x8095415E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CRL_NOT_FOUND),                     // -2137702049/0x8095415F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_URL_ADD_FAILED),                    // -2137702048/0x80954160
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_URL_NOT_FOUND),                     // -2137702047/0x80954161
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CRL_TRUNCATE_FAILED),               // -2137702046/0x80954162
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_SNO_NOT_FOUND),                // -2137702045/0x80954163
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_FG_TO_BG_MSG_FAILED),               // -2137702044/0x80954164
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_BG_TO_FG_MSG_FAILED),               // -2137702043/0x80954165
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_EXTND_CERT_SESINST_NOT_FOUND),      // -2137702042/0x80954166
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_EXTND_CERT_SESINST_CUR_FAILED),     // -2137702024/0x80954178
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_ISSUER_CUR_FAILED),            // -2137702023/0x80954179
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CERT_SNO_CUR_FAILED),               // -2137702022/0x8095417A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CRL_ISSUER_CUR_FAILED),             // -2137702021/0x8095417B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CRL_SNO_CUR_FAILED),                // -2137702020/0x8095417C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_URL_CUR_FAILED),                    // -2137702019/0x8095417D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_DB_CDP_ENTRY_CUR_FAILED),              // -2137702018/0x8095417E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CRL_PARSE_ERROR),                      // -2137702017/0x8095417F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_NO_RESULT_OBTAINED),                   // -2137702016/0x80954180
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CERT_NOT_AVAILABLE),                   // -2137702015/0x80954181
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_TOO_MANY_FOREGROUND_PROCESS),          // -2137702014/0x80954182
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_FOREGROUND_PROCESS_NOT_FOUND),         // -2137702013/0x80954183
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_FOREGROUND_FAILED_TO_REGISTER),        // -2137702012/0x80954184
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_BACKGROUND_NOT_READY),                 // -2137702011/0x80954185
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_SET_CRL_CHECKING_MODE_FAILED),         // -2137702010/0x80954186
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_UNEXPECTED_ASN1_STRUCTURE),            // -2137702009/0x80954187
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_STALE_CRL_RECEIVED),                   // -2137702008/0x80954188
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_PENDING_OCSP_CHECK),                   // -2137702007/0x80954189
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_OCSP_INVALID),                         // -2137702006/0x8095418A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_OCSP_STATUS_UNKNOWN),                  // -2137702005/0x8095418B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_OCSP_RESPONSE),                        // -2137702004/0x8095418C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_OCSP_SIGNING_CERT_INFO_MISSING),       // -2137702003/0x8095418D
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_OCSP_RESPONDER_URL_MISSING),           // -2137702002/0x8095418E
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_PRIVATE_KEY_NOT_FOUND),                // -2137702001/0x8095418F
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_SCEP_RENEW_CERT_NOT_SUPPORTED),        // -2137702000/0x80954190
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_SCEP_MAX_POLL_RETRY_COUNT_RCHD),       // -2137701999/0x80954191
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_CRL_CHECKING_DISABLED),                // -2137701998/0x80954192
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_PKI_CLIENT_SCEP_NO_HTTP_RESP_CONTENT),            // -2137701997/0x80954193
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STACK),                                           // -2137701984/0x809541A0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STACK_UNDERFLOW),                                 // -2137701983/0x809541A1
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_STACK_OVERFLOW),                                  // -2137701982/0x809541A2
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE),                                        // -2137701884/0x80954204
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_TABLE_TOO_BIG),                          // -2137701883/0x80954205
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_BAD_ID),                                 // -2137701882/0x80954206
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_CLOSED),                                 // -2137701881/0x80954207
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_STALE_ID),                               // -2137701880/0x80954208
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_FREE_LIST_EMPTY),                        // -2137701879/0x80954209
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_INSTANCE_PASSED_END_LIST_REACHED),                // -2137701878/0x8095420A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_KDF),                                        // -2137701684/0x809542CC
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_KDF_INVALID_COUNTER_SIZE),                   // -2137701683/0x809542CD
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_KDF_COUNTER_KEY_SIZES),                      // -2137701682/0x809542CE
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG),                                        // -2137701584/0x80954330
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_HASH_DF_BAD_OUTPUT_LEN),                 // -2137701583/0x80954331
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_UNSUPPORTED_CURVE),                      // -2137701582/0x80954332
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_EC_DRBG_ARG_TOO_LONG),                   // -2137701581/0x80954333
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_EC_DBRG_INVALID_HASH_ID),                // -2137701580/0x80954334
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_DBRG_RESEED_NEEDED),                     // -2137701579/0x80954335
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_BLOCK_CIPHER_DF_BAD_OUTPUT_LEN),         // -2137701578/0x80954336
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_CTR_INVALID_KEY_LENGTH),                 // -2137701577/0x80954337
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_CTR_INVALID_OUTPUT_LENGTH),              // -2137701576/0x80954338
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_DRBG_TOO_MANY_BITS),                     // -2137701575/0x80954339
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_CTR_BAD_ENTROPY_INPUT_LEN),              // -2137701574/0x8095433A
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_EC_ENTROPY_OR_NONCE_TOO_SHORT),          // -2137701573/0x8095433B
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_NIST_RNG_CTR_BAD_PERSO_STR_LEN),                  // -2137701572/0x8095433C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_XML_PARSE_NO_BEGIN_TAG),                          // -2137701384/0x809543F8
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_XML_PARSE_NO_END_TAG),                            // -2137701383/0x809543F9
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_XML_PARSE_END_TAG_TOO_EARLY),                     // -2137701382/0x809543FA
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_XML_PARSE_VALUE_NOT_FOUND),                       // -2137701381/0x809543FB
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_LWIP_UNSUPPORTED_FUNCTION),                       // -2137701284/0x8095445C
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_ENTROPY_UNINITIALIZED),                           // -2137701184/0x809544C0
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR),                               // -2137701084/0x80954524
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_BADMODID),                      // -2137701083/0x80954525
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_BADID),                         // -2137701082/0x80954526
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_MODIDNOTREGISTERED),            // -2137701081/0x80954527
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_INVALID_ARGS),                  // -2137701080/0x80954528
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_RESOURCE_NOT_FOUND),            // -2137701079/0x80954529
    DIRTYSOCK_ErrorName(SCE_SSL_ERROR_ERR_GARBAGE_COLLECTOR_AGAINMODIDREG),                 // -2137701078/0x8095452A

    /*
        errors from libhttp/libhttp_error.h
    */

    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_BEFORE_INIT),                                        // -2143088639/0x80431001
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_ALREADY_INITED),                                     // -2143088608/0x80431020
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_BUSY),                                               // -2143088607/0x80431021
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_OUT_OF_MEMORY),                                      // -2143088606/0x80431022
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_NOT_FOUND),                                          // -2143088603/0x80431025
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_INVALID_VERSION),                                    // -2143088534/0x8043106a
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_INVALID_ID),                                         // -2143088384/0x80431100
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_OUT_OF_SIZE),                                        // -2143088380/0x80431104
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_INVALID_VALUE),                                      // -2143088130/0x804311fe
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_INVALID_URL),                                        // -2143080352/0x80433060
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_UNKNOWN_SCHEME),                                     // -2143088543/0x80431061
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_NETWORK),                                            // -2143088541/0x80431063
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_BAD_RESPONSE),                                       // -2143088540/0x80431064
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_BEFORE_SEND),                                        // -2143088539/0x80431065
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_AFTER_SEND),                                         // -2143088538/0x80431066
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_TIMEOUT),                                            // -2143088536/0x80431068
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_UNKNOWN_AUTH_TYPE),                                  // -2143088535/0x80431069
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_UNKNOWN_METHOD),                                     // -2143088533/0x8043106b
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_READ_BY_HEAD_METHOD),                                // -2143088529/0x8043106f
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_NOT_IN_COM),                                         // -2143088528/0x80431070
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_NO_CONTENT_LENGTH),                                  // -2143088527/0x80431071
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_CHUNK_ENC),                                          // -2143088526/0x80431072
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_TOO_LARGE_RESPONSE_HEADER),                          // -2143088525/0x80431073
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_SSL),                                                // -2143088523/0x80431075
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_INSUFFICIENT_STACKSIZE),                             // -2143088522/0x80431076
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_ABORTED),                                            // -2143088512/0x80431080
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_UNKNOWN),                                            // -2143088511/0x80431081
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_EAGAIN),                                             // -2143088510/0x80431082
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_PROXY),                                              // -2143088508/0x80431084
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_BROKEN),                                             // -2143088507/0x80431085
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_PARSE_HTTP_NOT_FOUND),                               // -2143084507/0x80432025
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_PARSE_HTTP_INVALID_RESPONSE),                        // -2143084448/0x80432060
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_PARSE_HTTP_INVALID_VALUE),                           // -2143084034/0x804321fe
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_EPACKET),                                   // -2143068159/0x80436001
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ENODNS),                                    // -2143068158/0x80436002
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ETIMEDOUT),                                 // -2143068157/0x80436003
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ENOSUPPORT),                                // -2143068156/0x80436004
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_EFORMAT),                                   // -2143068155/0x80436005
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ESERVERFAILURE),                            // -2143068154/0x80436006
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ENOHOST),                                   // -2143068153/0x80436007
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ENOTIMPLEMENTED),                           // -2143068152/0x80436008
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ESERVERREFUSED),                            // -2143068151/0x80436009
    DIRTYSOCK_ErrorName(SCE_HTTP_ERROR_RESOLVER_ENORECORD),                                 // -2143068150/0x8043600a
    DIRTYSOCK_ErrorName(SCE_HTTPS_ERROR_CERT),                                              // -2143072160/0x80435060
    DIRTYSOCK_ErrorName(SCE_HTTPS_ERROR_HANDSHAKE),                                         // -2143072159/0x80435061
    DIRTYSOCK_ErrorName(SCE_HTTPS_ERROR_IO),                                                // -2143072158/0x80435062
    DIRTYSOCK_ErrorName(SCE_HTTPS_ERROR_INTERNAL),                                          // -2143072157/0x80435063
    DIRTYSOCK_ErrorName(SCE_HTTPS_ERROR_PROXY),                                             // -2143072156/0x80435064

    // NULL terminate
    DIRTYSOCK_ListEnd()
};
#endif

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/
/*F********************************************************************************/
/*!
    \Function DirtyErrAppCallbackSet

    \Description
        This function set the callback used to report Sony error code back to
        Application layer to statisfy TRC4034

    \Input pCallback  - callback function pointer

    \Version 08/07/2013 (tcho) First Version
*/
/********************************************************************************F*/
void DirtyErrAppCallbackSet(const DirtySockAppErrorCallback pCallback)
{
    pAppErrorCallback = pCallback;
}

/*F********************************************************************************/
/*!
    \Function DirtyErrAppReport

    \Description
        This function invokes the callback used to report Sony error code back to
        Application layer to statisfy TRC4034

    \Input iError - Error code to be pass back to the application layer

    \Version 08/07/2013 (tcho) First Version
*/
/********************************************************************************F*/
void DirtyErrAppReport(int32_t iError)
{
    if (pAppErrorCallback != NULL)
    {
        (*pAppErrorCallback)(iError);
    }
}

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

    for (iErr = 0; pList[iErr].uError != DIRTYSOCK_LISTTERM; iErr++)
    {
        if (pList[iErr].uError == uError)
        {
            ds_snzprintf(pBuffer, iBufSize, "%s/%d", pList[iErr].pErrorName, (signed)uError);
            return;
        }
    }
    #endif

    ds_snzprintf(pBuffer, iBufSize, "%d", uError);
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
    int32_t iErr;

    for (iErr = 0; _DirtyErr_List[iErr].uError != DIRTYSOCK_LISTTERM; iErr++)
    {
        if (_DirtyErr_List[iErr].uError == uError)
        {
            ds_snzprintf(pBuffer, iBufSize, "%s/0x%08x", _DirtyErr_List[iErr].pErrorName, uError);
            return;
        }
    }
    #endif

    ds_snzprintf(pBuffer, iBufSize, "0x%08x", uError);
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
