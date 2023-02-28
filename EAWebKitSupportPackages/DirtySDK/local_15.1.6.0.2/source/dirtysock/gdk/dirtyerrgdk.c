/*H********************************************************************************/
/*!
    \File dirtyerrgdk.c

    \Description
        Dirtysock debug error routines.

    \Copyright
        Copyright (c) 2019 Electronic Arts Inc.

    \Version 08/07/2019 (jbrookes) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <XGameErr.h>
#include <winhttp.h>
#include <winerror.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

#if DIRTYSOCK_ERRORNAMES

static DirtyErrT _DirtyErr_List[] =
{
    /*
        WinError.h"
    */
    DIRTYSOCK_ErrorName(ERROR_INVALID_FUNCTION),    // 1
    DIRTYSOCK_ErrorName(ERROR_ACCESS_DENIED),       // 5
    DIRTYSOCK_ErrorName(ERROR_NO_MORE_FILES),       // 18
    DIRTYSOCK_ErrorName(ERROR_ALREADY_ASSIGNED),    // 85
    DIRTYSOCK_ErrorName(ERROR_INVALID_PARAMETER),   // 87
    DIRTYSOCK_ErrorName(ERROR_INSUFFICIENT_BUFFER), // 122
    DIRTYSOCK_ErrorName(ERROR_ALREADY_EXISTS),      // 183
    DIRTYSOCK_ErrorName(ERROR_NO_DATA),             // 232
    DIRTYSOCK_ErrorName(ERROR_INVALID_PORT_ATTRIBUTES), // 545
    DIRTYSOCK_ErrorName(ERROR_IO_INCOMPLETE),       // 996
    DIRTYSOCK_ErrorName(ERROR_IO_PENDING),          // 997
    DIRTYSOCK_ErrorName(ERROR_NOT_FOUND),           // 1168
    DIRTYSOCK_ErrorName(ERROR_NO_MATCH),            // 1169
    DIRTYSOCK_ErrorName(ERROR_CONNECTION_INVALID),  // 1229
    DIRTYSOCK_ErrorName(ERROR_SERVICE_NOT_FOUND),   // 1243
    DIRTYSOCK_ErrorName(ERROR_FUNCTION_FAILED),     // 1627

    DIRTYSOCK_ErrorName(RPC_S_CALL_FAILED),         // 1726L
    DIRTYSOCK_ErrorName(RPC_S_CALL_FAILED_DNE),     // 1727L

    DIRTYSOCK_ErrorName(OR_INVALID_OXID),           // 1910L -- the object exporter specified was not found

    // Misc other errors
    DIRTYSOCK_ErrorName(ERROR_INVALID_STATE),       // 5023L - The group or resource is not in the correct state to perform the requested operation

    /*
        WinSock2.h
    */
    // WinSock error codes from 10000-11999
    DIRTYSOCK_ErrorName(WSAEINTR),              // 10004L - A blocking operation was interrupted by a call to WSACancelBlockingCall.
    DIRTYSOCK_ErrorName(WSAEBADF),              // 10009L - The file handle supplied is not valid.
    DIRTYSOCK_ErrorName(WSAEACCES),             // 10013L - An attempt was made to access a socket in a way forbidden by its access permissions.
    DIRTYSOCK_ErrorName(WSAEFAULT),             // 10014L - The system detected an invalid pointer address in attempting to use a pointer argument in a call.
    DIRTYSOCK_ErrorName(WSAEINVAL),             // 10022L - An invalid argument was supplied.
    DIRTYSOCK_ErrorName(WSAEMFILE),             // 10024L - Too many open sockets.
    DIRTYSOCK_ErrorName(WSAEWOULDBLOCK),        // 10035L - A non-blocking socket operation could not be completed immediately.
    DIRTYSOCK_ErrorName(WSAEINPROGRESS),        // 10036L - A blocking operation is currently executing.
    DIRTYSOCK_ErrorName(WSAEALREADY),           // 10037L - An operation was attempted on a non-blocking socket that already had an operation in progress.
    DIRTYSOCK_ErrorName(WSAENOTSOCK),           // 10038L - An operation was attempted on something that is not a socket.
    DIRTYSOCK_ErrorName(WSAEDESTADDRREQ),       // 10039L - A required address was omitted from an operation on a socket.
    DIRTYSOCK_ErrorName(WSAEMSGSIZE),           // 10040L - A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself.
    DIRTYSOCK_ErrorName(WSAEPROTOTYPE),         // 10041L - A protocol was specified in the socket function call that does not support the semantics of the socket type requested.
    DIRTYSOCK_ErrorName(WSAENOPROTOOPT),        // 10042L - An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call.
    DIRTYSOCK_ErrorName(WSAEPROTONOSUPPORT),    // 10043L - The requested protocol has not been configured into the system, or no implementation for it exists.
    DIRTYSOCK_ErrorName(WSAESOCKTNOSUPPORT),    // 10044L - The support for the specified socket type does not exist in this address family.
    DIRTYSOCK_ErrorName(WSAEOPNOTSUPP),         // 10045L - The attempted operation is not supported for the type of object referenced.
    DIRTYSOCK_ErrorName(WSAEPFNOSUPPORT),       // 10046L - The protocol family has not been configured into the system or no implementation for it exists.
    DIRTYSOCK_ErrorName(WSAEAFNOSUPPORT),       // 10047L - An address incompatible with the requested protocol was used.
    DIRTYSOCK_ErrorName(WSAEADDRINUSE),         // 10048L - Only one usage of each socket address (protocol/network address/port) is normally permitted.
    DIRTYSOCK_ErrorName(WSAEADDRNOTAVAIL),      // 10049L - The requested address is not valid in its context.
    DIRTYSOCK_ErrorName(WSAENETDOWN),           // 10050L - A socket operation encountered a dead network.
    DIRTYSOCK_ErrorName(WSAENETUNREACH),        // 10051L - A socket operation was attempted to an unreachable network.
    DIRTYSOCK_ErrorName(WSAENETRESET),          // 10052L - The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.
    DIRTYSOCK_ErrorName(WSAECONNABORTED),       // 10053L - An established connection was aborted by the software in your host machine.
    DIRTYSOCK_ErrorName(WSAECONNRESET),         // 10054L - An existing connection was forcibly closed by the remote host.
    DIRTYSOCK_ErrorName(WSAENOBUFS),            // 10055L - An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.
    DIRTYSOCK_ErrorName(WSAEISCONN),            // 10056L - A connect request was made on an already connected socket.
    DIRTYSOCK_ErrorName(WSAENOTCONN),           // 10057L - A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.
    DIRTYSOCK_ErrorName(WSAESHUTDOWN),          // 10058L - A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.
    DIRTYSOCK_ErrorName(WSAETOOMANYREFS),       // 10059L - Too many references to some kernel object.
    DIRTYSOCK_ErrorName(WSAETIMEDOUT),          // 10060L - A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.
    DIRTYSOCK_ErrorName(WSAECONNREFUSED),       // 10061L - No connection could be made because the target machine actively refused it.
    DIRTYSOCK_ErrorName(WSAELOOP),              // 10062L - Cannot translate name.
    DIRTYSOCK_ErrorName(WSAENAMETOOLONG),       // 10063L - Name component or name was too long.
    DIRTYSOCK_ErrorName(WSAEHOSTDOWN),          // 10064L - A socket operation failed because the destination host was down.
    DIRTYSOCK_ErrorName(WSAEHOSTUNREACH),       // 10065L - A socket operation was attempted to an unreachable host.
    DIRTYSOCK_ErrorName(WSAENOTEMPTY),          // 10066L - Cannot remove a directory that is not empty.
    DIRTYSOCK_ErrorName(WSAEPROCLIM),           // 10067L - A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.
    DIRTYSOCK_ErrorName(WSAEUSERS),             // 10068L - Ran out of quota.
    DIRTYSOCK_ErrorName(WSAEDQUOT),             // 10069L - Ran out of disk quota.
    DIRTYSOCK_ErrorName(WSAESTALE),             // 10070L - File handle reference is no longer available.
    DIRTYSOCK_ErrorName(WSAEREMOTE),            // 10071L - Item is not available locally.
    DIRTYSOCK_ErrorName(WSASYSNOTREADY),        // 10091L - WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.
    DIRTYSOCK_ErrorName(WSAVERNOTSUPPORTED),    // 10092L - The Windows Sockets version requested is not supported.
    DIRTYSOCK_ErrorName(WSANOTINITIALISED),     // 10093L - Either the application has not called WSAStartup, or WSAStartup failed.
    DIRTYSOCK_ErrorName(WSAEDISCON),            // 10101L - Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.
    DIRTYSOCK_ErrorName(WSAENOMORE),            // 10102L - No more results can be returned by WSALookupServiceNext.
    DIRTYSOCK_ErrorName(WSAECANCELLED),         // 10103L - A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
    DIRTYSOCK_ErrorName(WSAEINVALIDPROCTABLE),  // 10104L - The procedure call table is invalid.
    DIRTYSOCK_ErrorName(WSAEINVALIDPROVIDER),   // 10105L - The requested service provider is invalid.
    DIRTYSOCK_ErrorName(WSAEPROVIDERFAILEDINIT),// 10106L - The requested service provider could not be loaded or initialized.
    DIRTYSOCK_ErrorName(WSASYSCALLFAILURE),     // 10107L - A system call that should never fail has failed.
    DIRTYSOCK_ErrorName(WSASERVICE_NOT_FOUND),  // 10108L - No such service is known. The service cannot be found in the specified name space.
    DIRTYSOCK_ErrorName(WSATYPE_NOT_FOUND),     // 10109L - The specified class was not found.
    DIRTYSOCK_ErrorName(WSA_E_NO_MORE),         // 10110L - No more results can be returned by WSALookupServiceNext.
    DIRTYSOCK_ErrorName(WSA_E_CANCELLED),       // 10111L - A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
    DIRTYSOCK_ErrorName(WSAEREFUSED),           // 10112L - A database query failed because it was actively refused.
    DIRTYSOCK_ErrorName(WSAHOST_NOT_FOUND),     // 11001L - No such host is known.

    /*
        winhttp.h
    */

    DIRTYSOCK_ErrorName(ERROR_WINHTTP_OUT_OF_HANDLES),                          // 12001L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_TIMEOUT),                                 // 12002L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INTERNAL_ERROR),                          // 12004L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INVALID_URL),                             // 12005L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_UNRECOGNIZED_SCHEME),                     // 12006L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_NAME_NOT_RESOLVED),                       // 12007L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INVALID_OPTION),                          // 12009L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_OPTION_NOT_SETTABLE),                     // 12011L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SHUTDOWN),                                // 12012L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_LOGIN_FAILURE),                           // 12015L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_OPERATION_CANCELLED),                     // 12017L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INCORRECT_HANDLE_TYPE),                   // 12018L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INCORRECT_HANDLE_STATE),                  // 12019L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CANNOT_CONNECT),                          // 12029L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CONNECTION_ERROR),                        // 12030L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_RESEND_REQUEST),                          // 12032L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CERT_DATE_INVALID),                // 12037L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CERT_CN_INVALID),                  // 12038L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED),                 // 12044L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_INVALID_CA),                       // 12045L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CERT_REV_FAILED),                  // 12057L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN),                 // 12100L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND),                 // 12101L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND),                  // 12102L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN),                  // 12103L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_HEADER_NOT_FOUND),                        // 12150L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INVALID_SERVER_RESPONSE),                 // 12152L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INVALID_HEADER),                          // 12153L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_INVALID_QUERY_REQUEST),                   // 12154L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_HEADER_ALREADY_EXISTS),                   // 12155L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_REDIRECT_FAILED),                         // 12156L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CHANNEL_ERROR),                    // 12157L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT),                   // 12166L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT),               // 12167L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_INVALID_CERT),                     // 12169L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CERT_REVOKED),                     // 12170L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_NOT_INITIALIZED),                         // 12172L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_FAILURE),                          // 12175L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_UNHANDLED_SCRIPT_TYPE),                   // 12176L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SCRIPT_EXECUTION_ERROR),                  // 12177L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR),                // 12178L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE),                 // 12179L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_AUTODETECTION_FAILED),                    // 12180L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_HEADER_COUNT_EXCEEDED),                   // 12181L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_HEADER_SIZE_OVERFLOW),                    // 12182L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW),   // 12183L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW),                 // 12184L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY),              // 12185L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY),       // 12186L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED_PROXY),           // 12187L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_SECURE_FAILURE_PROXY),                    // 12188L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_RESERVED_189),                            // 12189L
    DIRTYSOCK_ErrorName(ERROR_WINHTTP_HTTP_PROTOCOL_MISMATCH),                  // 12190L

    // undefined
    { 0x80072fa8, "ERROR_INTERNET_NSAL_NOT_AUTHORIZED" },  // 12202L

    // IPSec error codes 13000-13999
    DIRTYSOCK_ErrorName(ERROR_IPSEC_IKE_NO_PRIVATE_KEY),    // 13820L - IKE negotiation failed because the machine certificate used does not have a private key. IPsec certificates require a private key. Contact your Network Security administrator about replacing with a certificate that has a private key.

    /*
        WinError.h
    */

    DIRTYSOCK_ErrorName(E_PENDING),                     // 0x8000000AL - The data necessary to complete this operation is not yet available.

    // Generic errors = 0x80004xxx
    DIRTYSOCK_ErrorName(E_ABORT),                       // 0x80004004
    DIRTYSOCK_ErrorName(E_FAIL),                        // 0x80004005

    DIRTYSOCK_ErrorName(E_ACCESSDENIED),                // 0x80070005

    // Cert errors; 0x800B0100 through 0x800B0114

    DIRTYSOCK_ErrorName(TRUST_E_NOSIGNATURE),           // 0x800B0100L - No signature was present in the subject.
    DIRTYSOCK_ErrorName(CERT_E_EXPIRED),                // 0x800B0101L - A required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file.
    DIRTYSOCK_ErrorName(CERT_E_VALIDITYPERIODNESTING),  // 0x800B0102L - The validity periods of the certification chain do not nest correctly.
    DIRTYSOCK_ErrorName(CERT_E_ROLE),                   // 0x800B0103L - A certificate that can only be used as an end-entity is being used as a CA or visa versa.
    DIRTYSOCK_ErrorName(CERT_E_PATHLENCONST),           // 0x800B0104L - A path length constraint in the certification chain has been violated.
    DIRTYSOCK_ErrorName(CERT_E_CRITICAL),               // 0x800B0105L - A certificate contains an unknown extension that is marked 'critical'.
    DIRTYSOCK_ErrorName(CERT_E_PURPOSE),                // 0x800B0106L - A certificate being used for a purpose other than the ones specified by its CA.
    DIRTYSOCK_ErrorName(CERT_E_ISSUERCHAINING),         // 0x800B0107L - A parent of a given certificate in fact did not issue that child certificate.
    DIRTYSOCK_ErrorName(CERT_E_MALFORMED),              // 0x800B0108L - A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
    DIRTYSOCK_ErrorName(CERT_E_UNTRUSTEDROOT),          // 0x800B0109L - A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.
    DIRTYSOCK_ErrorName(CERT_E_CHAINING),               // 0x800B010AL - A certificate chain could not be built to a trusted root authority.
    DIRTYSOCK_ErrorName(TRUST_E_FAIL),                  // 0x800B010BL - Generic trust failure.
    DIRTYSOCK_ErrorName(CERT_E_REVOKED),                // 0x800B010CL - A certificate was explicitly revoked by its issuer.
    DIRTYSOCK_ErrorName(CERT_E_UNTRUSTEDTESTROOT),      // 0x800B010DL - The certification path terminates with the test root which is not trusted with the current policy settings.
    DIRTYSOCK_ErrorName(CERT_E_REVOCATION_FAILURE),     // 0x800B010EL - The revocation process could not continue - the certificate(s) could not be checked.
    DIRTYSOCK_ErrorName(CERT_E_CN_NO_MATCH),            // 0x800B010FL - The certificate's CN name does not match the passed value.
    DIRTYSOCK_ErrorName(CERT_E_WRONG_USAGE),            // 0x800B0110L - The certificate is not valid for the requested usage.
    DIRTYSOCK_ErrorName(TRUST_E_EXPLICIT_DISTRUST),     // 0x800B0111L - The certificate was explicitly marked as untrusted by the user.
    DIRTYSOCK_ErrorName(CERT_E_UNTRUSTEDCA),            // 0x800B0112L - A certification chain processed correctly, but one of the CA certificates is not trusted by the policy provider.
    DIRTYSOCK_ErrorName(CERT_E_INVALID_POLICY),         // 0x800B0113L - The certificate has invalid policy.
    DIRTYSOCK_ErrorName(CERT_E_INVALID_NAME),           // 0x800B0114L - The certificate has an invalid name. The name is not included in the permitted list or is explicitly excluded.


    /* winerror.h */
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_AMBIGUOUS),               // 0x8019012CL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_BAD_GATEWAY),             // 0x801901F6L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_BAD_METHOD),              // 0x80190195L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_BAD_REQUEST),             // 0x80190190L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_CONFLICT),                // 0x80190199L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_DENIED),                  // 0x80190191L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_EXPECTATION_FAILED),      // 0x801901A1L
    { 0x801901AD, "HTTP_E_STATUS_429_TOO_MANY_REQUESTS" },      // 0x801901ADL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_FORBIDDEN),               // 0x80190193L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_GATEWAY_TIMEOUT),         // 0x801901F8L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_GONE),                    // 0x8019019AL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_LENGTH_REQUIRED),         // 0x8019019BL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_MOVED),                   // 0x8019012DL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_NONE_ACCEPTABLE),         // 0x80190196L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_NOT_FOUND),               // 0x80190194L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_NOT_MODIFIED),            // 0x80190130L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_NOT_SUPPORTED),           // 0x801901F5L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_PAYMENT_REQ),             // 0x80190192L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_PRECOND_FAILED),          // 0x8019019CL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_PROXY_AUTH_REQ),          // 0x80190197L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_RANGE_NOT_SATISFIABLE),   // 0x801901A0L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_REDIRECT),                // 0x8019012EL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_REDIRECT_KEEP_VERB),      // 0x80190133L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_REDIRECT_METHOD),         // 0x8019012FL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_REQUEST_TIMEOUT),         // 0x80190198L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_REQUEST_TOO_LARGE),       // 0x8019019DL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_SERVER_ERROR),            // 0x801901F4L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_SERVICE_UNAVAIL),         // 0x801901F7L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_UNEXPECTED),              // 0x80190001L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR), // 0x80190005L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_UNSUPPORTED_MEDIA),       // 0x8019019FL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_URI_TOO_LONG),            // 0x8019019EL
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_USE_PROXY),               // 0x80190131L
    DIRTYSOCK_ErrorName(HTTP_E_STATUS_VERSION_NOT_SUP),         // 0x801901F9L
    DIRTYSOCK_ErrorName(ONL_E_ACTION_REQUIRED),                 // 0x8086000CL
    DIRTYSOCK_ErrorName(WEB_E_INVALID_JSON_STRING),             // 0x83750007L
    DIRTYSOCK_ErrorName(WEB_E_UNEXPECTED_CONTENT),              // 0x83750005L

    /*
        urlmon.h
    */
    DIRTYSOCK_ErrorName(INET_E_INVALID_URL),
    DIRTYSOCK_ErrorName(INET_E_NO_SESSION),
    DIRTYSOCK_ErrorName(INET_E_CANNOT_CONNECT),
    DIRTYSOCK_ErrorName(INET_E_RESOURCE_NOT_FOUND),
    DIRTYSOCK_ErrorName(INET_E_OBJECT_NOT_FOUND),
    DIRTYSOCK_ErrorName(INET_E_DATA_NOT_AVAILABLE),
    DIRTYSOCK_ErrorName(INET_E_DOWNLOAD_FAILURE),
    DIRTYSOCK_ErrorName(INET_E_AUTHENTICATION_REQUIRED),
    DIRTYSOCK_ErrorName(INET_E_NO_VALID_MEDIA),
    DIRTYSOCK_ErrorName(INET_E_CONNECTION_TIMEOUT),
    DIRTYSOCK_ErrorName(INET_E_INVALID_REQUEST),
    DIRTYSOCK_ErrorName(INET_E_UNKNOWN_PROTOCOL),
    DIRTYSOCK_ErrorName(INET_E_SECURITY_PROBLEM),
    DIRTYSOCK_ErrorName(INET_E_CANNOT_LOAD_DATA),
    DIRTYSOCK_ErrorName(INET_E_CANNOT_INSTANTIATE_OBJECT),
    DIRTYSOCK_ErrorName(INET_E_INVALID_CERTIFICATE),
    DIRTYSOCK_ErrorName(INET_E_REDIRECT_FAILED),
    DIRTYSOCK_ErrorName(INET_E_REDIRECT_TO_DIR),

    /*
        types_c.h (requires c++ thus we hardcode)
    */
    { 0x89235200, "E_XBL_RUNTIME_ERROR" },                   // 0x89235200L
    { 0x89235201, "E_XBL_RTA_GENERIC_ERROR" },               // 0x89235201L
    { 0x89235202, "E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED" },  // 0x89235202L
    { 0x89235203, "E_XBL_RTA_ACCESS_DENIED" },               // 0x89235203L
    { 0x89235204, "E_XBL_AUTH_UNKNOWN_ERROR" },              // 0x89235204L
    { 0x89235205, "E_XBL_AUTH_RUNTIME_ERROR" },              // 0x89235205L
    { 0x89235206, "E_XBL_AUTH_NO_TOKEN" },                   // 0x89235206L
    { 0x89235207, "E_XBL_ALREADY_INITIALIZED" },             // 0x89235207L
    { 0x89235208, "E_XBL_NOT_INITIALIZED" },                 // 0x89235208L
    { 0x89235209, "E_XBL_RTA_NOT_ACTIVATED" },               // 0x89235209L

    /*
        XGameErr.h
    */
    DIRTYSOCK_ErrorName(E_GAMERUNTIME_NOT_INITIALIZED),         // 0x89240100L - The game runtime has not yet been initialized.
    DIRTYSOCK_ErrorName(E_GAMERUNTIME_DLL_NOT_FOUND),           // 0x89240101L - The game runtime dll was not found.
    DIRTYSOCK_ErrorName(E_GAMERUNTIME_VERSION_MISMATCH),        // 0x89240102L - The game runtime dll does not support this version of XDK.
    DIRTYSOCK_ErrorName(E_GAMERUNTIME_WINDOW_NOT_FOREGROUND),   // 0x89240103L - The operation requires an application window to be in the foreground. 
    DIRTYSOCK_ErrorName(E_GAMERUNTIME_SUSPENDED),               // 0x89240104L - The game runtime is in a suspended state.
    DIRTYSOCK_ErrorName(E_GAMEUSER_MAX_USERS_ADDED),            // 0x89245100L - Can't add this user because the max number of users has been added.
    DIRTYSOCK_ErrorName(E_GAMEUSER_SIGNED_OUT),                 // 0x89245101L - Can't perform the operation because the user is signed out.
    DIRTYSOCK_ErrorName(E_GAMEUSER_RESOLVE_USER_ISSUE_REQUIRED),// 0x89245102L - Needs UI to resolve an issue with this user.
    DIRTYSOCK_ErrorName(E_GAMEUSER_DEFERRAL_NOT_AVAILABLE),     // 0x89245103L - Not an appropriate time to request deferral.
    DIRTYSOCK_ErrorName(E_GAMEUSER_USER_NOT_FOUND),             // 0x89245104L - User matching the id was not found
    DIRTYSOCK_ErrorName(E_GAMEUSER_NO_TOKEN_REQUIRED),          // 0x89245105L - No token is required for this call
    DIRTYSOCK_ErrorName(E_GAMEUSER_NO_DEFAULT_USER),            // 0x89245106L - There is no current default user
    DIRTYSOCK_ErrorName(E_GAMEUSER_FAILED_TO_RESOLVE),          // 0x89245107L - Failed to resolve the given privilege
    DIRTYSOCK_ErrorName(E_GAMEUSER_NO_TITLE_ID),                // 0x89245108L - An xbox live title id must be configured
    DIRTYSOCK_ErrorName(E_GAMEUSER_UNKNOWN_GAME_IDENTITY),      // 0x89245109L - The game identity is not recognized
    DIRTYSOCK_ErrorName(E_GAMEUSER_NO_PACKAGE_IDENTITY),        // 0x89245110L - A package identity must be configured
    DIRTYSOCK_ErrorName(E_GAMEUSER_FAILED_TO_GET_TOKEN),        // 0x89245111L - The token request failed

    DIRTYSOCK_ErrorName(E_GAMEPACKAGE_APP_NOT_PACKAGED),        // 0x89245200L - The game is not packaged in a container
    DIRTYSOCK_ErrorName(E_GAMEPACKAGE_NO_INSTALLED_LANGUAGES),  // 0x89245201L - The game uses Intelligent Delivery to selectively install languages, but none of the languages are installed.

    DIRTYSOCK_ErrorName(E_GAMESTORE_LICENSE_ACTION_NOT_APPLICABLE_TO_PRODUCT), // 0x89245300L - The game requested a license for a product that cannot be licensed
    DIRTYSOCK_ErrorName(E_GAMESTORE_NETWORK_ERROR),             // 0x89245301L - The game failed to communicate with the store network
    DIRTYSOCK_ErrorName(E_GAMESTORE_SERVER_ERROR),              // 0x89245302L - The game received a bad response from the store server
    DIRTYSOCK_ErrorName(E_GAMESTORE_INSUFFICIENT_QUANTITY),     // 0x89245303L - The user does not have enough of this consumable to use the requested amount
    DIRTYSOCK_ErrorName(E_GAMESTORE_ALREADY_PURCHASED),         // 0x89245304L - The user already owns this product

    /*
       not defined
    */

    { 0x87dd0021, "AM_E_NO_TOKEN_REQUIRED"},    // The endpoint does not require an authorization token, but the application is attempting to retrieve a token via GetTokenAndSignatureAsync
                                                // ref: https://github.com/microsoft/xbox-live-api/blob/master/Include/xsapi/errors.h
    { 0xC00CE56D, "E_XML_INVALID" },            // xml error: End tag '%2' does not match the start tag '%1'. 
                                                // ref: MsXml2.h

    // NULL terminate
    DIRTYSOCK_ListEnd()
};

#define DIRTYERR_NUMERRORS (sizeof(_DirtyErr_List) / sizeof(_DirtyErr_List[0]))

#endif // #if DIRTYSOCK_ERRORNAMES

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function DirtyErrNameList

    \Description
        This function takes as input a system-specific error code, and either
        resolves it to its define name if it is recognized, or formats it as a hex
        number if not.

    \Input *pBuffer - [out] pointer to output buffer to store result
    \Input iBufSize - size of output buffer
    \Input uError   - error code to format
    \Input *pList   - error list to use

    \Version 06/13/2005 (jbrookes)
*/
/********************************************************************************F*/
void DirtyErrNameList(char *pBuffer, int32_t iBufSize, uint32_t uError, const DirtyErrT *pList)
{
    static char strUnknown[16];

    #if DIRTYSOCK_ERRORNAMES
    int32_t iErr;

    // first try to match exactly
    for (iErr = 0; pList[iErr].uError != DIRTYSOCK_LISTTERM; iErr++)
    {
        if ((pList[iErr].uError & ~0x80000000) == (uError & ~0x80000000))
        {
            ds_snzprintf(pBuffer, iBufSize, "%s/0x%08x", pList[iErr].pErrorName, uError);
            return;
        }
    }

    // if not found, try to match lower 16 bits of error code
    for (iErr = 0; pList[iErr].uError != DIRTYSOCK_LISTTERM; iErr++)
    {
        if ((pList[iErr].uError & ~0xffff0000) == (uError & ~0xffff0000))
        {
            ds_snzprintf(pBuffer, iBufSize, "%s/0x%08x", pList[iErr].pErrorName, uError);
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

    \Version 06/13/2005 (jbrookes)
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

    \Version 06/13/2005 (jbrookes)
*/
/********************************************************************************F*/
const char *DirtyErrGetName(uint32_t uError)
{
    static char strName[128];
    DirtyErrName(strName, sizeof(strName), uError);
    return(strName);
}
