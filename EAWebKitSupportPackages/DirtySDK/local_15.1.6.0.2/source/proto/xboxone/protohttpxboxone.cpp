/*H********************************************************************************/
/*!
    \File protohttpxboxone.cpp

    \Description
        This module implements the ProtoHttp API using Microsoft's IXMLHTTPRequest2
        API.

    \Notes
        \verbatim
        There are some differences between the 'normal' version of ProtoHttp and this
        implementation:
            - Certificates are managed by the system, so this module does not 
              implement any of the certificate APIs, which are stubbed.
            - The receive buffer will grow in size when downloading so as not to
              limit the download speed.  The amount of memory utilized will depend
              on the internal update rate and the speed of the transaction, with
              a faster transaction utilizing more memory.
            - The send buffer will grow in size to be able to hold the entire amount
              of data being sent in the transaction.

        Memory Allocation
            Only some of the memory allocated by this module is tracked by the
            DirtyMem API.  The following long-term allocations are not currently
            tracked:
            - CoCreateInstance() used to create the IXMLHttpRequest2() object
            - MakeAndInitialize() used to instantiate the HttpCallback object
            - MakeAndInitialize() used to create the SequentialStream output object

            The following temporary allocations are not currently tracked:
            - Two std::wstring allocations in _ProtoHttpRequest()
            - CoTaskMemAlloc() from pXmlHttp->GetAllResponseHeaders() in
              _ProtoHttpGetResponseHeader()

        Multithreading
            There are three types of thread involved when this module executes:
            - main thread: game thread invoking the public api
            - dedicated thread(s): used for offloading long blocking MS calls
                                   (one such thread per xmlhttp object)
            - MS worker thread(s): system thread notifying events by invoking HttpCallback(s) 
                                  (OnRedirect, OnHeadersAvailable, OnDataAvailable,
                                   OnResponseReceived, OnError)

            The interaction pattern between the main thread and the dedicated
            thread (the one associated with the only active xmlhttp object) is an
            "offload and wait" approach. During that "wait" period, the main thread
            never uses shared data structures while they are being used/altered by
            the dedicated thread. With such an approach we can avoid usage of extra
            critical section(s) and limit the complexity of the multithreaded solution
            without inducing delays in the execution of the main thread. 
            
            Note: There may be other dedicated threads running at a given point in time, 
            but they are all guaranteed to be "idle" and associated with an xmlhttp object
            that is in the killlist and queued for destruction.

            The critical sections used in the code are all meant to guard against
            concurrency issues that could result from asynchronous activities between
            the MS worker threads and the "main thread + dedicated thread" pair.

        State Machines
            There are two types of state machines involved when this module executes.

            #1 ProtoHttpStateE state machine

              ProtoHttpStateE is used as the main state machine of the module and
              it involves the typical states common to all flavors of DirtySDK ProtoHttp
              (ST_IDLE, ST_BODY, ST_DONE, etc.)

            #2 ProtoHttpXmlReqStateE state machine

              ProtoHttpXmlReqStateE is used as the state machine associated with a 
              single xmlhttp object. It serves two main purposes: 
                1- enable an "offload and wait" pattern for execution of long MS blocking calls
                2- enable the possibility of "aborting" request activity at any point
                   by pushing an xmlhttp object into a killlist such that async operations
                   complete safely before the memory gets released for good.

                 State machine illustration (active phase):

                              REQ_ST_INIT
                                  |
                                  | httpref state = ST_IDLE
                                  |
                                 \/
                              REQ_ST_CREATING_THREAD
                                  |
                                  | thread state = THREAD_LIFE_STATUS_ALIVE
                                  |
                                 \/
                              REQ_ST_CREATING_XMLHTTPREQ2
                                  |
                                  | xmlhttpreq2 object created
                                  |
                                 \/
                              REQ_ST_XMLHTTPREQ2_CREATED
                                  |
                                  | httpref state = ST_CONN
                                  | 
                                 \/
                              REQ_ST_OPENING_REQ*
                                  |
                                  | xlmhttpreq2 object opened
                                  |
                                 \/
                              REQ_ST_CONFIGURING_REQ*
                                  |
                                  | xlmhttpreq2 object configured
                                  |
                                  |                     pre-send buffer not empty
                              REQ_ST_PRESEND* -----------------------------------------------
                                  |                                                         |
                                  |                              ProtoHttpSend()            \/
                              REQ_ST_ACCUMULATING_DATA* ---------------------------------> REQ_ST_WRITING_OUTSTREAM* ----
                                  |                     <---------------------------------                    /\        |
                                  |                       data chunk accumulated successfully                  |        |
                                  |                                                                            ----------
                                  | all req data successfully accumulated in output stream object              pInpBuf not empty
                                  |
                                 \/
                              REQ_ST_SENDING_REQ*
                                  |
                                  | xlmhttpreq2->Send() completed
                                  |
                                 \/
                              REQ_ST_WAITING_RESPONSE*                     (from any state)
                                  |                                               |
                                  | response received                             | failure
                                  |                                               |
                                 \/                                              \/
                              REQ_ST_COMPLETE*                               REQ_ST_FAILED*


                  State machine illustration (killed phase):

                      (once req is in kill list)
                                 |
                                 | current state is any of the states flagged with a * in the above illustration
                                 |
                                \/
                         REQ_ST_CLEANINGUP
                                 |
                                 | resources released
                                 |
                                \/
                         REQ_ST_STOPPING_THREAD
                                 |
                                 | thread state = THREAD_LIFE_STATUS_NOTALIVE
                                 |
                                \/
                         REQ_ST_BYE


            Notice that the illustration above shows that there are strong dependencies
            the state of the active xmlhttp object (ProtoHttpXmlReqStateE) and the
            the module state (ProtoHttpStateE). State transitions are gated by the
            other state machine reaching a specific state. The opposite is also true
            in other scenarios.
        \endverbatim

    \Copyright
        Copyright (c) Electronic Arts 2013-2018.

    \Version 06/27/2013 (jbrookes) (initial implementation by akirchner)
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

#include <ctype.h>
#include <exception>
#include <iostream> // std::wstring unicodeUrl()
#include <ixmlhttprequest2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wrl.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtythread.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/xboxone/dirtyauthxboxone.h"
#include "DirtySDK/dirtyvers.h"
#include "dirtynetpriv.h" // SocketRate*

#include "DirtySDK/proto/protohttp.h"

/*** Defines **********************************************************************/

//! default ProtoHttp timeout
#define PROTOHTTP_TIMEOUT_DEFAULT (30*1000)

//! default maximum allowed redirections
#define PROTOHTTP_MAXREDIRECT   (3)

//! size of "last-received" header cache
#define PROTOHTTP_HDRCACHESIZE  (1024)

//! size of buffer used to save data until first ProtoHttpSend() call is performed
#define PROTOHTTP_PRESENDBUF_SIZE   (4 * 1024)

//! protohttp revision number (maj.min)
#define PROTOHTTP_VERSION       (0x0103)          // update this for major bug fixes or protocol additions/changes

//! maximum time we will wait to destroy
#define PROTOHTTP_DESTROY_TIMEOUT (5*1000)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! forward declaration of xmlhttp object type
typedef struct ProtoHttpXmlHttpT ProtoHttpXmlHttp;

class HttpCallback : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IXMLHTTPRequest2Callback>
{
    public:
        HttpCallback();
        virtual ~HttpCallback();

        STDMETHODIMP OnRedirect        (IXMLHTTPRequest2 *pXHR, const WCHAR *pwszRedirectUrl);
        STDMETHODIMP OnHeadersAvailable(IXMLHTTPRequest2 *pXHR, DWORD dwStatus, const WCHAR *pwszStatus);
        STDMETHODIMP OnDataAvailable   (IXMLHTTPRequest2 *pXHR, ISequentialStream *pResponseStream);
        STDMETHODIMP OnResponseReceived(IXMLHTTPRequest2 *pXHR, ISequentialStream *pResponseStream);
        STDMETHODIMP OnError           (IXMLHTTPRequest2 *pXHR, HRESULT hrError);

        ProtoHttpXmlHttpT *GetXmlHttp();
        void SetXmlHttp(ProtoHttpXmlHttpT *pXmlHttp);

    private:
        ProtoHttpXmlHttpT *m_pXmlHttp;
};

class SequentialStream : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ISequentialStream>
{
    public:
        SequentialStream();
        virtual ~SequentialStream();

        HRESULT STDMETHODCALLTYPE Read (void *pBuffer, ULONG uMaxBytes, ULONG *pBytesRead);
        HRESULT STDMETHODCALLTYPE Write(const void *pBuffer, ULONG uMaxBytes, ULONG *pBytesWritten);

        ProtoHttpRefT * GetProtoHttpRef() { return(m_pHttpRef); }
        void SetProtoHttpRef(ProtoHttpRefT *pHttpRef) { m_pHttpRef = pHttpRef; }
        int32_t GetDataSize() { return(m_uBufferPositionWrite - m_uBufferPositionRead); }
        char *GetData() { return(m_pBuffer); }

    protected:
        NetCritT  m_StreamCrit;
        char     *m_pBuffer;
        ULONG     m_uBufferSize;
        ULONG     m_uBufferPositionWrite;
        ULONG     m_uBufferPositionRead;
        int32_t   m_iMemGroup;
        void     *m_pMemGroupUserData;

    private:
        ProtoHttpRefT *m_pHttpRef; 

};

//! state used to track asynchronous evolution of a specific xmlhttp object
typedef enum ProtoHttpXmlReqStateE
{
    REQ_ST_INIT = 0,             //!< initial state
    REQ_ST_CREATING_THREAD,      //!< async creation of the dedicated thread used for offloading long blocking MS calls (waiting for thread to reach the THREAD_LIFE_STATUS_ALIVE life status)
    REQ_ST_CREATING_XMLHTTPREQ2, //!< async creation of the MS XMLHTTPReq2 object
    REQ_ST_XMLHTTPREQ2_CREATED,  //!< XMLHTTPReq2 object fully initialized and in standby for request initialization
    REQ_ST_WRITING_OUTSTREAM,    //!< async write to output stream object
    REQ_ST_OPENING_REQ,          //!< async opening of xmlhttp request
    REQ_ST_CONFIGURING_REQ,      //!< async setup of of xmlhttp request
    REQ_ST_PRESEND,              //!< request fully initialized and presend data being sent
    REQ_ST_ACCUMULATING_DATA,    //!< request fully initialized, presend data fully sent, but still accumulating request data from ProtoHttpSend()
    REQ_ST_SENDING_REQ,          //!< async sending of fully accumulated http request
    REQ_ST_WAITING_RESPONSE,     //!< waiting for one of the HttpCallback event to be fired by the system

    REQ_ST_FAILED,               //!< failure state
    REQ_ST_COMPLETE,             //!< complete

    // the below states are only valid when object is in killlist
    REQ_ST_CLEANINGUP,           //!< async clean up of MS resources 
    REQ_ST_STOPPING_THREAD,      //!< async termination the dedicated thread used for offloading long blocking MS calls (waiting for thread to reach the THREAD_LIFE_STATUS_NOTALIVE life status)
    REQ_ST_BYE,                  //!< it is safe to release obj memory

    REQ_ST_NUMSTATES             //!< number of xmlhttp states
}ProtoHttpXmlReqStateE;

//! global module state used to track evolution of the unique "active" request
typedef enum ProtoHttpStateE
{
    ST_IDLE,        //!< idle
    ST_AUTH,        //!< acquiring auth token
    ST_CONN,        //!< connecting state
    ST_RESP,        //!< sending, waiting for initial response
    ST_HEAD,        //!< processing header
    ST_BODY,        //!< receiving body
    ST_DONE,        //!< transaction success
    ST_FAIL,        //!< transaction failed

    ST_NUMSTATES    //!< number of states
}ProtoHttpStateE;

typedef struct ProtoHttpHeaderT
{
    int32_t iHdrCode;                                   //!< result code
    int32_t iHdrDate;                                   //!< last modified date
    int32_t iHeadSize;                                  //!< size of head data
    int64_t iBodySize;                                  //!< size of body data
    char strHdr[PROTOHTTP_HDRCACHESIZE];                //!< storage for most recently received HTTP header
}ProtoHttpHeaderT;

typedef struct ProtoHttpBufferT
{
    char *pInpBuf;                                      //!< pointer to input data
    int32_t iInpReadLen;                                //!< total length of buffer read but reset each time iInpLen bytes are read to prevent overallocation
    int32_t iInpReadTotal;                              //!< total length of the buffer read through the whole response
    int32_t iInpLen;                                    //!< total length of the URL in buffer
    int32_t iInpOvr;                                    //!< input overflow amount
    int32_t iInpTotal;                                  //!< total amount of data streamed through response buffer
}ProtoHttpBufferT;

typedef struct ProtoHttpBufferParamsT
{
    int32_t iInpMax;                                    //!< maximum buffer size
    int32_t iInpMaxIdeal;                               //!< requested (ideal) max size; (iInpMax may increase during receive operations)
}ProtoHttpBufferParamsT;

//! set of commands used to offload long blocking MS calls to the dedicated thread
typedef enum ProtoHttpBlockingOpsCmdE
{
    THREAD_CMD_NONE = 0,                                //!< no blocking operation to be performed by the dedicated thread
    THREAD_CMD_XMLHTTP_CREATE,                          //!< main thread wants the dedicated thread to create a xmlhttp object
    THREAD_CMD_OUTSTREAM_WRITE,                         //!< main thread wants the dedicated thread to write request data to the output stream
    THREAD_CMD_XMLHTTP_OPEN,                            //!< main thread wants the dedicated thread to open the http request
    THREAD_CMD_XMLHTTP_SETUP,                           //!< main thread wants the dedicated thread to configure the http request
    THREAD_CMD_XMLHTTP_SEND,                            //!< main thread wants the dedicated thread to fire the http request
    THREAD_CMD_CLEANUP,                                 //!< main thread wants the dedicated thread to free the xmlhttp object and associated resources
    THREAD_CMD_EXIT,                                    //!< main thread wants the dedicated thread to exit and go back to THREAD_LIFE_STATUS_NOTALIVE state

    THREAD_NUMCMDS                                      //!< number of commands
} ProtoHttpBlockingOpsCmdE;

//! xmlhttp object  (forward-declared earlier in the file)
struct ProtoHttpXmlHttpT
{
    struct ProtoHttpXmlHttpT * pNext;                       //! for kill list linking

    Microsoft::WRL::ComPtr<IXMLHTTPRequest2> cpXmlHttp;     //!< pointer to IXMLHTTPRequest2 object
    Microsoft::WRL::ComPtr<HttpCallback> cpXmlHttpCallback; //!< callback functor
    Microsoft::WRL::ComPtr<SequentialStream> cpOutStream;   //!< output stream (optional; used to send data)

    ProtoHttpRefT *pHttpRef;                                //!< http ref this is assocated with

    ProtoHttpXmlReqStateE eState;                           //!< xmlhttp object state
    ProtoHttpXmlReqStateE eNextState;                       //!< next xmlhttp object state to jump to while in REQ_ST_WAITING_RESPONSE (read from main thread, written from MS worker thread)

    int32_t iByeSafetyDelayStart;                           //!< time at which REQ_ST_BYE state's safety delay started

    struct {
        ProtoHttpRequestTypeE eRequestType;                 //!< request type of current request
        void *pMemGroupUserData;                            //!< user data associated with mem group (required for delayed destroy)
        int32_t iMemGroup;                                  //!< module mem group id (required for delayed destroy)
        int32_t iPort;                                      //!< server port
        int32_t iSecure;                                    //!< secure connection
        int32_t iOnDataThreshold;                           //!< ON_DATA_THRESHOLD setting
        char strHost[256];                                  //!< server name
        uint8_t bAuthenticate;                              //!< if enable authentication if TRUE, and disable it if FALSE
        int8_t  iVerbose;                                   //!< copy of state's verbose setting
        int8_t _pad[2];
    } HttpRefDataCache;                                     //!< used to cache httpref state info (for usage when xmlhttp object is in killlist and a more recent one now owns thos fields)

    char *pPreSendBuffer;                                   //!< buffer used to save data until first ProtoHttpSend() call is performed
    int32_t iPreSendBufferValidBytes;                       //!< number of valid bytes in pre-send buffer
    int32_t iPreSendBufferConsumedBytes;                    //!< number of consumed bytes in pre-send buffer

    /* thread-safety accessed controlled with pHttpRef->critical.Locker */
    ProtoHttpBufferT RequestBuffer;                         //!< multi-purpose buffer, used for request header formatting, request data buffering and response buffering

    char *pHdrStrt;                                         //!< saved position of header in RequestBuffer.pInpBuf

    //! group of variables associated with the dedicated thread used for offloading long blocking MS calls
    volatile enum
    {
        THREAD_LIFE_STATUS_NOTALIVE = 0,                    //!< thread is not yet running, or is no longer running
        THREAD_LIFE_STATUS_ALIVE,                           //!< thread is running normally
    } eBlockingOpsThreadLife;                               //!< variable used to monitor/control life of thread used to offload long MS calls from main thread
    volatile ProtoHttpBlockingOpsCmdE eBlockingOpsThreadCmd;//!< active command that the main thread offloaded to the dedicated thread 
    volatile int32_t iBlockingOpsThreadResult;              //!< dedicated thread writes blocking op's result here, main thread reads blocking op's result here
    DirtyConditionRefT *pBlockingOpsThreadCond;             //!< synchronization resource used to wake up the dedicated thread
    NetCritT BlockingOpsThreadCrit;                         //!< critical section associated with the above condition (pBlockingOpsThreadCond)
    uint8_t bWaitingForAsyncCompletion;                     //!< used by the main thread to distinguish between 'start'/'wait' phases of blocking ops
    uint8_t bInKillList;                                    //!< TRUE if xmlhttp object is in queued kill list, FALSE otherwise
    uint8_t bGracePeriodEntered;                            //!< TRUE if xmlhttp object has entered the completion grace period following a suspend event, FALSE otherwise
    uint8_t _pad[1];
};

typedef struct ProtoHttpCriticalT
{
    NetCritT Locker;                                        //!< mutex used to protect the following members
    ProtoHttpXmlHttpT *pXmlHttp;                            //!< xmlhttp object used for transaction
    ProtoHttpStateE eRequestState;                          //!< request state
    ProtoHttpBufferParamsT RequestBufferParams;             //!< params for multi-purpose buffer, used for request header formatting, request data buffering and response buffering
    int32_t iRequestSize;                                   //!< total request size
    int32_t iNumRedirect;                                   //!< number of redirections received
    int32_t iNumRedirProc;                                  //!< number of redirections processed
    uint8_t bHeaderReceived;                                //!< TRUE if header has been received, FALSE otherwise
    uint8_t bBodyReceived;                                  //!< TRUE if the entire body was received, FALSE otherwise
    uint8_t bTimeout;                                       //!< TRUE if a timeout occurred, FALSE otherwise
    uint8_t bRedirBugWorkaround;                            //!< TRUE if redirection bug workaround is being enabled
}ProtoHttpCriticalT;

//! http module state
struct ProtoHttpRefT
{
    // ----- global module config -----

    int32_t iMemGroup;                                  //!< module mem group id
    void *pMemGroupUserData;                            //!< user data associated with mem group

    ProtoHttpXmlHttpT *pXmlHttpKillList;                //!< list of killed xmlhttp objects queued for destruction (only when async op is no longer in progress)

    int32_t iMaxRedirect;                               //!< maximum number of redirections allowed
    int32_t iKeepAliveDflt;                             //!< keep-alive default (keep-alive will be reset to this value; can be overridden by user)
    uint32_t uTimeout;                                  //!< protocol timeout

    SocketRateT SendRate;                               //!< send rate estimation data
    SocketRateT RecvRate;                               //!< recv rate estimation data

    int8_t iVerbose;                                    //!< debug output level
    uint8_t _pad[3];

    // ----- variables specific to the active request -----

    ProtoHttpRequestTypeE eRequestType;                 //!< request type of current request
    ProtoHttpHeaderT responseHeader;                    //!< response header received
    ProtoHttpCriticalT critical;                        //!< variables that should be thread safe

    ProtoHttpCustomHeaderCbT  *pCustomHeaderCb;         //!< global callback for modifying request header
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;        //!< global callback for viewing recv header on recepit
    void *pCallbackRef;                                 //!< user ref for global callback

    ProtoHttpWriteCbT *pWriteCb;                        //!< optional data write callback
    ProtoHttpCustomHeaderCbT *pReqCustomHeaderCb;       //!< optional custom header callback
    ProtoHttpReceiveHeaderCbT *pReqReceiveHeaderCb;     //!< optional receive header callback
    void *pUserData;                                    //!< user data for callback

    char *pAppendHdr;                                   //!< append header buffer pointer
    int32_t iAppendLen;                                 //!< size of append header buffer

    char strTokenUrl[256];                              //!< token uri, in case different from hostname
    char strHost[256];                                  //!< server name
    char strBaseHost[256];                              //!< base server name (used for partial urls)
    char strProxy[256];                                 //!< proxy server name/address (including port)

    char strRequestHdr[PROTOHTTP_HDRCACHESIZE];         //!< storage for most recent HTTP request header
    char strLocation[1024];                             //!< stores most recent redirection header; used to work around 302/303 POST redirection bug in IXHR2

    int32_t iPort;                                      //!< server port
    int32_t iBasePort;                                  //!< base port (used for partial urls)
    int32_t iSecure;                                    //!< secure connection
    int32_t iBaseSecure;                                //!< base security setting (used for partial urls)
    int32_t iRecvSize;                                  //!< protohttprecvall variable to track received size
    int32_t iPendingXmlHttpDestructions;                //!< number of xml http object destructions still queued with netconn external cleanup
    int32_t iOnDataThreshold;                           //!< ON_DATA_THRESHOLD setting
    int32_t iKeepAlive;                                 //!< indicate if we should try to use keep-alive
    uint32_t uTimer;                                    //!< timeout timer
    int32_t iOldInpLen;                                 //!< saved former value of ProtoHttpBufferParamsT::iInpLen (needed for retry scenarios)
    int32_t iDestroyStartTime;                          //!< time when we started the destruction of this object

    uint8_t bAuthenticate;                              //!< if enable authentication if TRUE, and disable it if FALSE
    uint8_t bGetAuthToken;                              //!< if TRUE get and embed auth token
    uint8_t bRetriedOpen;                               //!< work-around for OR_INVALID_OXID bug
    int8_t iAuthTokenIdx;                               //!< index of user acquiring the auth token
};

/*** Function Prototypes **********************************************************/
static void _ProtoHttpMoveToNextState(ProtoHttpRefT *pHttpRef, ProtoHttpStateE eNewState);
static void _ProtoHttpXmlHttpMoveToNextState(ProtoHttpRefT *pHttpRef, ProtoHttpXmlHttpT *pXmlHttp, ProtoHttpXmlReqStateE eNewState);
static void _ProtoHttpDestroyActiveRequest(ProtoHttpRefT *pHttpRef);
static int32_t _ProtoHttpGetResponseHeader(ProtoHttpRefT *pHttpRef, int32_t iHdrCode, const wchar_t *pwStrCode);
static int32_t _ProtoHttpRead(ProtoHttpRefT *pHttpRef, ISequentialStream *pStream, char *pBuffer, int32_t iMaxRead);

/*** Variables ********************************************************************/

#if DIRTYCODE_LOGGING
//! names of module state (active request)
static const char *_ProtoHttp_strRequestState[ST_NUMSTATES] =
{
    "ST_IDLE",
    "ST_AUTH",
    "ST_CONN",
    "ST_RESP",
    "ST_HEAD",
    "ST_BODY",
    "ST_DONE",
    "ST_FAIL",
};

//! names of commands used to offload long blocking MS calls to the dedicated thread
static const char *_ProtoHttp_strThreadCmds[THREAD_NUMCMDS] =
{
    "THREAD_CMD_NONE           ",
    "THREAD_CMD_XMLHTTP_CREATE ",
    "THREAD_CMD_OUTSTREAM_WRITE",
    "THREAD_CMD_XMLHTTP_OPEN   ",
    "THREAD_CMD_XMLHTTP_SETUP  ",
    "THREAD_CMD_XMLHTTP_SEND   ",
    "THREAD_CMD_CLEANUP        ",
    "THREAD_CMD_EXIT           "
};
#endif

//! names of HTTP request types
static const char *_ProtoHttp_strRequestType[PROTOHTTP_NUMREQUESTTYPES] =
{
    "HEAD",
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "OPTIONS",
    "PATCH"
};

//! names of xmlhttp object states
static const char *_ProtoHttp_strXmlHttpState[REQ_ST_NUMSTATES] =
{
    "REQ_ST_INIT                ",
    "REQ_ST_CREATING_THREAD     ",
    "REQ_ST_CREATING_XMLHTTPREQ2",
    "REQ_ST_XMLHTTPREQ2_CREATED ",
    "REQ_ST_WRITING_OUTSTREAM   ",
    "REQ_ST_OPENING_REQ         ",
    "REQ_ST_CONFIGURING_REQ     ",
    "REQ_ST_PRESEND             ",
    "REQ_ST_ACCUMULATING_DATA   ",
    "REQ_ST_SENDING_REQ         ",
    "REQ_ST_WAITING_RESPONSE    ",
    "REQ_ST_FAILED              ",
    "REQ_ST_COMPLETE            ",
    "REQ_ST_CLEANINGUP          ",
    "REQ_ST_STOPPING_THREAD     ",
    "REQ_ST_BYE                 "
};

/*** Private Functions ************************************************************/

//$$ tmp - we have to define this to allow building this module in WinRT
#if defined(DIRTYCODE_PC)
const char *DirtyErrGetName(uint32_t uError)
{
    static char _strError[32];
    ds_snzprintf(_strError, sizeof(_strError), "0x%08x");
    return(_strError);
}
#endif

/*
    HttpCallback: implementation of IXMLHTTPRequest2Callback interface required by
    IXMLHTTPGetRequest2().
*/

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function HttpCallback

    \Description
        HttpCallback constructor

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
HttpCallback::HttpCallback() : m_pXmlHttp(NULL)
{
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \fn ~HttpCallback

    \Description
        HttpCallback destructor

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
HttpCallback::~HttpCallback()
{
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function OnRedirect

    \Description
        The requested URI was redirected by the HTTP server to a new URI.

    \Input *pXHR         - The interface pointer of originating IXMLHTTPRequest2 object.
    \Input *pRedirectUrl - The new URL to for the request.

    \Output
        IFACEMETHODIMP   - S_OK = success

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
IFACEMETHODIMP HttpCallback::OnRedirect(IXMLHTTPRequest2* pXHR, const WCHAR *pRedirectUrl)
{
    ProtoHttpXmlHttpT *pXmlHttp = GetXmlHttp();
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;
    ProtoHttpBufferT *pBuffer;
    char *pTmp;

    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] OnRedirect() url='%S'\n", pHttpRef, pXmlHttp, pRedirectUrl));

    NetCritEnter(&pHttpRef->critical.Locker);

    // ignore system event if xmlhttp object is no longer active and queued in kill list
    if (!pXmlHttp->bInKillList)
    {
        pBuffer = &pXmlHttp->RequestBuffer;

        // format null-terminated redirection response into input buffer for processing in update
        pXmlHttp->RequestBuffer.iInpLen += ds_snzprintf(pBuffer->pInpBuf+pBuffer->iInpLen, pHttpRef->critical.RequestBufferParams.iInpMax-pBuffer->iInpLen,
            "HTTP/1.1 302 Found\r\n"
            "Location: %S\r\n"
            "\r\n",
            pRedirectUrl);
        pXmlHttp->RequestBuffer.pInpBuf[pXmlHttp->RequestBuffer.iInpLen++] = '\0';

        // copy to temp location; in case we get a subsequent OnError and we need to re-issue the redirect ourselves
        wcstombs(pHttpRef->strLocation, pRedirectUrl, sizeof(pHttpRef->strLocation));

        // if location URL includes a '#' character, trim it
        if ((pTmp = strchr(pHttpRef->strLocation, '#')) != NULL)
        {
            *pTmp = '\0';
        }

        // check max redirection limit
        if (pHttpRef->critical.iNumRedirect == pHttpRef->iMaxRedirect)
        {
            NetPrintf(("protohttpxboxone: [%p][%p] maximum number of redirections (%d) exceeded\n", pHttpRef, pXmlHttp, pHttpRef->iMaxRedirect));

            // higher-level code is not expecting this to be reported as a "failure"... it rather expects SUCCESS with code 302
            // we fake that by setting the following vars.
            pHttpRef->critical.bHeaderReceived = TRUE;
            pHttpRef->critical.bBodyReceived = TRUE;

            // move to the completed state as nothing left to do
            pXmlHttp->eNextState = ProtoHttpXmlReqStateE::REQ_ST_COMPLETE;
        }
        else // track redirection
        {
            pHttpRef->critical.iNumRedirect += 1;
        }
    }

    NetCritLeave(&pHttpRef->critical.Locker);
    return(S_OK);
};

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function OnHeadersAvailable

    \Description
        The HTTP Headers have been downloaded and are ready for parsing. The string that is
        returned is owned by this function and should be copied or deleted before exit.

    \Input *pXHR       - The interface pointer of originating IXMLHTTPRequest2 object.
    \Input dwStatus    - The value of HTTP status code, e.g. 200, 404
    \Input *pwszStatus - The description text of HTTP status code.

    \Output
        IFACEMETHODIMP - S_OK = success

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
IFACEMETHODIMP HttpCallback::OnHeadersAvailable(IXMLHTTPRequest2 *pXHR, DWORD dwStatus, const WCHAR *pwszStatus)
{
    ProtoHttpXmlHttpT *pXmlHttp = GetXmlHttp();
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] OnHeadersAvailable(%d, %S)\n", pHttpRef, pXmlHttp, dwStatus, pwszStatus));

    NetCritEnter(&pHttpRef->critical.Locker);

    // ignore system event if xmlhttp object is no longer active and queued in kill list
    if (!pXmlHttp->bInKillList)
    {
        // get response header
        if (_ProtoHttpGetResponseHeader(pHttpRef, dwStatus, pwszStatus) == 0)
        {
            // indicate we've received the header
            pHttpRef->critical.bHeaderReceived = TRUE;
        }
        else
        {
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
        }
    }

    NetCritLeave(&pHttpRef->critical.Locker);

    return(S_OK);
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function OnDataAvailable

    \Description
        Part of the HTTP Data payload is available, we can start processing it
        here or copy it off and wait for the whole request to finish loading.

    \Input *pXHR            - Pointer to the originating IXMLHTTPRequest2 object.
    \Input *pResponseStream - Pointer to the input stream, which may only be part of the whole stream.

    \Output
        IFACEMETHODIMP      - S_OK = success

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
IFACEMETHODIMP HttpCallback::OnDataAvailable(IXMLHTTPRequest2 *pXHR, ISequentialStream *pResponseStream)
{
    ProtoHttpXmlHttpT *pXmlHttp = GetXmlHttp();
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;
    ProtoHttpBufferT *pRespBuf;
    int32_t iOnDataThresholdMargin;
    int32_t iBytesRead, iMaxRead, iBytesAvail;
    uint32_t uSleepCount = 0;

    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] OnDataAvailable (response stream: %p)\n", pHttpRef, pXmlHttp, pResponseStream));

    NetCritEnter(&pHttpRef->critical.Locker);

    // ignore system event if xmlhttp object is no longer active and queued in kill list
    if (!pXmlHttp->bInKillList)
    {
        pRespBuf = &pXmlHttp->RequestBuffer;

        /* MS suggests limiting the maximum read size to slightly over the ondatathreshold size; this improves
           performance by limiting the amount of buffer space that needs to be marshalled across VM boundaries,
           while still allowing us to pull the maximum amount of data available in one read */
        iOnDataThresholdMargin = (pHttpRef->iOnDataThreshold > 1024) ? pHttpRef->iOnDataThreshold + pHttpRef->iOnDataThreshold/8 : 1024;
        
        do
        {
            // calc max read and realloc buffer if no room to receive
            // $$TODO -- change to sleep here instead, like with rate throttling below?
            if ((iMaxRead = pHttpRef->critical.RequestBufferParams.iInpMax - pRespBuf->iInpLen) == 0)
            {
                int32_t iNewMax = pHttpRef->critical.RequestBufferParams.iInpMax * 2;
                NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] reallocing input buffer (%d->%d)\n", pHttpRef, pXmlHttp, pHttpRef->critical.RequestBufferParams.iInpMax, iNewMax));
                char *pNewBuf = (char *)DirtyMemAlloc(iNewMax, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
                ds_memcpy(pNewBuf, pRespBuf->pInpBuf, pRespBuf->iInpLen);
                DirtyMemFree(pRespBuf->pInpBuf, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
                pRespBuf->pInpBuf = pNewBuf;
                pHttpRef->critical.RequestBufferParams.iInpMax = iNewMax;
                iMaxRead = pHttpRef->critical.RequestBufferParams.iInpMax - pRespBuf->iInpLen;
            }

            // limit max read size based on threshold margin
            if (iMaxRead > iOnDataThresholdMargin)
            {
                iMaxRead = iOnDataThresholdMargin;
            }

            // handle optional data rate throttling
            while ((iBytesAvail = SocketRateThrottle(&pHttpRef->RecvRate, SOCK_STREAM, iMaxRead, "recv")) == 0)
            {
                NetCritLeave(&pHttpRef->critical.Locker);
                Sleep(16);
                uSleepCount += 1;
                NetCritEnter(&pHttpRef->critical.Locker);
            }

            // do the read, update buffer info
            if ((iBytesRead = _ProtoHttpRead(pHttpRef, pResponseStream, pRespBuf->pInpBuf + pRespBuf->iInpLen, iBytesAvail)) > 0)
            {
                NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] recv %d bytes\n", pHttpRef, pXmlHttp, iBytesRead));
            }
            pRespBuf->iInpLen += iBytesRead;
            pRespBuf->iInpTotal += iBytesRead;

            // update data rate estimation
            SocketRateUpdate(&pHttpRef->RecvRate, iBytesRead, "recv");
        }
        /* as per MS, only continue to read if we received the maximum amount the
           last time around; otherwise we know there is no data to be read */
        while (iBytesRead == iBytesAvail);
    }

    NetCritLeave(&pHttpRef->critical.Locker);

    return(S_OK);
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function OnResponseReceived

    \Description
        Called when the entire body has been received.

    \Input *pXHR            - Pointer to the originating IXMLHTTPRequest2 object.
    \Input *pResponseStream - Pointer to the complete input stream.

    \Output
        IFACEMETHODIMP      - S_OK = success

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
IFACEMETHODIMP HttpCallback::OnResponseReceived(IXMLHTTPRequest2 *pXHR, ISequentialStream *pResponseStream)
{
    ProtoHttpXmlHttpT *pXmlHttp = GetXmlHttp();
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] OnResponseReceived(response stream: %p)\n", pHttpRef, pXmlHttp, pResponseStream));

    NetCritEnter(&pHttpRef->critical.Locker);

    // forward data on to data callback
    OnDataAvailable(pXHR, pResponseStream);

    // make sure we have a state reference
    if (pHttpRef != NULL)
    {
        pHttpRef->critical.bBodyReceived = TRUE;
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] finished receiving data\n", pHttpRef, pXmlHttp));
    }

    // mark as complete; so we can clean up the ref
    pXmlHttp->eNextState = ProtoHttpXmlReqStateE::REQ_ST_COMPLETE;
    NetCritLeave(&pHttpRef->critical.Locker);
    return(S_OK);
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function OnError

    \Description
        Handle errors that have occurred during the HTTP request

    \Input *pXHR       - The interface pointer of IXMLHTTPRequest2 object
    \Input hError       - The errorcode for the httprequest

    \Output
        IFACEMETHODIMP - S_OK = success

    \Notes
        pXmlHttp->pHttpRef should never be null because cpXmlHttpCallback would have 
        been released and OnError will never trigger

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
IFACEMETHODIMP HttpCallback::OnError(IXMLHTTPRequest2 *pXHR, HRESULT hError)
{
    ProtoHttpXmlHttpT *pXmlHttp = GetXmlHttp();
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    NetPrintf(("protohttpxboxone: [%p][%p] OnError (%s)\n", pHttpRef, pXmlHttp, DirtyErrGetName(hError)));

    NetCritEnter(&pHttpRef->critical.Locker);

    if (((pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_POST) || (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_PUT) || (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_PATCH)) &&
            (hError == INET_E_REDIRECT_FAILED) && (pHttpRef->strLocation[0] != '\0'))
    {
        NetPrintf(("protohttpxboxone: [%p][%p] setting up for re-issue of redirection after failure\n", pHttpRef, pXmlHttp));
        pHttpRef->critical.bRedirBugWorkaround = TRUE;

        pXmlHttp->eNextState = ProtoHttpXmlReqStateE::REQ_ST_COMPLETE;
    }
    else if (pXmlHttp->eNextState == ProtoHttpXmlReqStateE::REQ_ST_COMPLETE && pHttpRef->critical.bHeaderReceived && pHttpRef->critical.bBodyReceived)
    {
        // Workaround for bug when OnError is received immediately after OnRedirect.
        // OnRedirect fakes a valid response if we have reached the configured number of max redirections
        // Nucleus /connect/auth redirects to "http://127.0.0.1/success" which can't be followed. Don't treat this as an error.
        NetPrintf(("protohttpxboxone: [%p][%p] ignoring error (%s) when request is already completed\n", pHttpRef, pXmlHttp, DirtyErrGetName(hError)));
    }
    else
    {
        // mark as failed, so we can release the xmlhttp obj and eventually force the httpref to fail state too
        pXmlHttp->eNextState = ProtoHttpXmlReqStateE::REQ_ST_FAILED;
    }
    
    NetCritLeave(&pHttpRef->critical.Locker);

    return(S_OK);
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function GetXmlHttp

    \Description
        Get XmlHttp object reference

    \Output
        ProtoHttpXmlHttpT * - Pointer to reference

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
ProtoHttpXmlHttpT * HttpCallback::GetXmlHttp()
{
    return(m_pXmlHttp);
}

/*F********************************************************************************/
/*!
    \relates  HttpCallback
    \Function SetXmlHttp

    \Description
        Set XmlHttp object reference

    \Input
        *pXmlHttp       - xmlhttp object reference

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
void HttpCallback::SetXmlHttp(ProtoHttpXmlHttpT *pXmlHttp)
{
    m_pXmlHttp = pXmlHttp;
}

/*
    SequentialStream: implementation of ISequentialStream interface required by
    IXMLHTTPGetRequest2().
*/

/*F********************************************************************************/
/*!
    \relates  SequentialStream
    \Function SequentialStream

    \Description
        SequentialStream constructor

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
SequentialStream::SequentialStream():
    m_pBuffer(NULL),
    m_uBufferSize(0),
    m_uBufferPositionWrite(0),
    m_uBufferPositionRead(0)
{
    NetCritInit(&m_StreamCrit, "SequentialStream");
};

/*F********************************************************************************/
/*!
    \relates  SequentialStream
    \fn ~SequentialStream

    \Description
        SequentialStream destructor

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
SequentialStream::~SequentialStream()
{
    if (m_pBuffer)
    {
        DirtyMemFree(m_pBuffer, PROTOHTTP_MEMID, m_iMemGroup, m_pMemGroupUserData);
    }

    NetCritKill(&m_StreamCrit);
};

/*F********************************************************************************/
/*!
    \relates  SequentialStream
    \Function Read

    \Description
        Reads from the uBufferPositionRead position

    \Input *pData     - [OUT] A pointer to the buffer which the stream data is read into
    \Input uMaxBytes  - The number of bytes of data to read from the stream object
    \Input uBytesRead - [OUT] A pointer to a ULONG variable that receives the actual number of bytes read from the stream object

    \Output
        HRESULT       - S_OK if succeed

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
HRESULT SequentialStream::Read(void *pData, ULONG uMaxBytes, ULONG *uBytesRead)
{
    ProtoHttpRefT *pHttpRef = GetProtoHttpRef();
    ULONG uBytesAvail, uBytesAvailMax;
    uint32_t uSleepCount = 0;

    if ((pData == NULL) || (uBytesRead == NULL))
    {
        return(STG_E_INVALIDPOINTER);
    }

    NetCritEnter(&m_StreamCrit);

    // clamp to max
    if ((uBytesAvailMax = (m_uBufferPositionWrite - m_uBufferPositionRead)) > uMaxBytes)
    {
        uBytesAvailMax = uMaxBytes;
    }

    // check for data to read
    if ((m_pBuffer == NULL) || (uBytesAvailMax == 0))
    {
        NetCritLeave(&m_StreamCrit);
        return(S_FALSE);
    }

    // handle optional data rate throttling
    while ((uBytesAvail = SocketRateThrottle(&pHttpRef->SendRate, SOCK_STREAM, uBytesAvailMax, "send")) == 0)
    {
        NetCritLeave(&m_StreamCrit);
        Sleep(16);
        uSleepCount += 1;
        NetCritEnter(&m_StreamCrit);
    }

    Sleep(250);

    // copy the data
    if (uBytesAvail > 0)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] sequentialstream read %d bytes\n", pHttpRef, uBytesAvail));
        ds_memcpy(pData, m_pBuffer + m_uBufferPositionRead, uBytesAvail);
        m_uBufferPositionRead += uBytesAvail;
        *uBytesRead = uBytesAvail;

        // update data rate estimation
        SocketRateUpdate(&pHttpRef->SendRate, uBytesAvail, "send");

        // compact the buffer
        if ((uBytesAvail = m_uBufferPositionWrite - m_uBufferPositionRead) > 0)
        {
            memmove(m_pBuffer, m_pBuffer + m_uBufferPositionRead, m_uBufferPositionWrite - m_uBufferPositionRead);
        }
        m_uBufferPositionWrite -= m_uBufferPositionRead;
        m_uBufferPositionRead = 0;
    }

    NetCritLeave(&m_StreamCrit);

    // return if there is data available
    return((uBytesAvail > 0) ? S_OK : S_FALSE);
}

/*F********************************************************************************/
/*!
    \relates  SequentialStream
    \Function Write

    \Description
        Writes to the end of the buffer, and increases the buffer size if needed

    \Input *pData           - input data to be written into the stream
    \Input uMaxBytes        - size of data to be written into the stream
    \Input *pBytesWritten   - [OUT] storage for actual number of bytes that were written

    \Output
        HRESULT             - S_OK if succeed

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
HRESULT SequentialStream::Write(const void *pData, ULONG uMaxBytes, ULONG *pBytesWritten)
{
#if DIRTYCODE_LOGGING
    ProtoHttpRefT *pHttpRef = GetProtoHttpRef();
#endif

    NetCritEnter(&m_StreamCrit);

    if (pData == NULL)
    {
        NetCritLeave(&m_StreamCrit);
        return(STG_E_INVALIDPOINTER);
    }
    ULONG uBufferSizeNew = m_uBufferSize;

    // find out if the size of the buffer should be increased
    while (uBufferSizeNew < (m_uBufferPositionWrite + uMaxBytes))
    {
        if (uBufferSizeNew == 0)
        {
            uBufferSizeNew = 1024;
        }
        else
        {
            uBufferSizeNew *= 2;
        }
    }

    // increase the size of the buffer if needed
    if (m_uBufferSize < uBufferSizeNew)
    {
        int32_t iMemGroup;
        void *pMemGroupUserData;
        char *pBufferNew;

        // query current mem group data
        DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

        // allocate new buffer
        if ((pBufferNew = (char *)DirtyMemAlloc(uBufferSizeNew, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
        {
            NetPrintf(("protohttpxboxone: [%p] sequentialstream [%p] Failed to increase sequential stream buffer size\n", pHttpRef, this));
            NetCritLeave(&m_StreamCrit);
            return(STG_E_MEDIUMFULL);
        }

        // if we had an old buffer, copy information over and free it
        if (m_pBuffer != NULL)
        {
            ds_memclr(pBufferNew, uBufferSizeNew);
            ds_memcpy(pBufferNew, m_pBuffer, m_uBufferSize);
            DirtyMemFree(m_pBuffer, PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData);
        }

        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p] realloc sequentialstream [%p] %d->%d bytes\n", pHttpRef, this, m_uBufferSize, uBufferSizeNew));

        m_iMemGroup = iMemGroup;
        m_pMemGroupUserData = pMemGroupUserData;
        m_pBuffer = pBufferNew;
        m_uBufferSize = uBufferSizeNew;
    }

    // append new data
    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] sequentialstream [%p] write %d bytes\n", pHttpRef, this, uMaxBytes));
    ds_memcpy(m_pBuffer + m_uBufferPositionWrite, pData, uMaxBytes);
    m_uBufferPositionWrite += uMaxBytes;
    NetCritLeave(&m_StreamCrit);

    if (pBytesWritten !=  NULL)
    {
        *pBytesWritten = uMaxBytes;
    }
    return(S_OK);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpMoveToNextState

    \Description
        Force xmlhttp object state transition.

    \Input *pHttpRef    - pointer to module state
    \Input *pXmlHttp    - pointer to xmlhttp object
    \Input eNewState    - new state to transit to

    \Version 11/03/2016 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpMoveToNextState(ProtoHttpRefT *pHttpRef, ProtoHttpXmlHttpT *pXmlHttp, ProtoHttpXmlReqStateE eNewState)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] xmlhttp object (%s) state transition  %s --> %s  %s\n",
        pHttpRef, pXmlHttp, (pXmlHttp->bInKillList ? "killed" : "active"), _ProtoHttp_strXmlHttpState[pXmlHttp->eState], _ProtoHttp_strXmlHttpState[eNewState],
        ((pXmlHttp->bWaitingForAsyncCompletion == FALSE) ? "" : ("** ERROR async op still in progress during state transition **"))));

    // state-specific initialization
    if (eNewState != pXmlHttp->eState)
    {
        switch (eNewState)
        {
        case REQ_ST_BYE:
            pXmlHttp->iByeSafetyDelayStart = NetTick();
            break;
        default:
            break;
        }
    }

    pXmlHttp->eState = eNewState;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpIsXmlHttpActivityInProgress

    \Description
        Returns whether the xmlhttp object is in a state that implies async activity
        potentially being in progress and possibly notified later via an MS callback.

        \Input *pXmlHttp    - pointer to xmlhttp object

        \Output
            uint32_t        - TRUE if xmlhttp activity is in progress, FALSE otherwise

        \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static uint32_t _ProtoHttpIsXmlHttpActivityInProgress(ProtoHttpXmlHttpT *pXmlHttp)
{
    switch (pXmlHttp->eState)
    {
        case ProtoHttpXmlReqStateE::REQ_ST_SENDING_REQ:
        case ProtoHttpXmlReqStateE::REQ_ST_WAITING_RESPONSE:
            return(TRUE);
        default:
            return(FALSE);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpMoveToNextState

    \Description
        Force active request state transition.

    \Input *pHttpRef    - pointer to module state
    \Input eNewState    - new state to transit to

    \Version 11/03/2016 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpMoveToNextState(ProtoHttpRefT *pHttpRef, ProtoHttpStateE eNewState)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] active request state transition  %s --> %s\n",
        pHttpRef, pHttpRef->critical.pXmlHttp, _ProtoHttp_strRequestState[pHttpRef->critical.eRequestState], _ProtoHttp_strRequestState[eNewState]));

    pHttpRef->critical.eRequestState = eNewState;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAddToKillList

    \Description
        Add xmlhttp object to the list of entries that needs to
        be cleaned up when pending async op is completed.

    \Input *pHttpRef    - pointer to module state
    \Input *pXmlHttp    - xmlhttp data space pointer

    \Version 11/11/2016 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpAddToKillList(ProtoHttpRefT *pHttpRef, ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpXmlHttpT *pOldHead = pHttpRef->pXmlHttpKillList;

    NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p][%p] xmlhttp data space added to kill list\n", pHttpRef, pXmlHttp));

    // insert at head
    pHttpRef->pXmlHttpKillList = pXmlHttp;

    // link
    pHttpRef->pXmlHttpKillList->pNext = pOldHead;

    pXmlHttp->bInKillList = TRUE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRemoveFromKillList

    \Description
        Remove xmlhttp object from the list of entries that needs to
        be cleanup when pending async op is completed.

    \Input *pHttpRef    - pointer to module state
    \Input *pXmlHttp    - xmlhttp data space pointer

    \Version 11/11/2016 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpRemoveFromKillList(ProtoHttpRefT *pHttpRef, ProtoHttpXmlHttpT *pXmlHttp)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] xmlhttp data space removed from kill list\n", pHttpRef, pXmlHttp));

    // is entry to be deleted is at head of list
    if (pXmlHttp == pHttpRef->pXmlHttpKillList)
    {
        pHttpRef->pXmlHttpKillList = pXmlHttp->pNext;
    }
    else
    {
        ProtoHttpXmlHttpT *pCurrent = pHttpRef->pXmlHttpKillList->pNext;
        ProtoHttpXmlHttpT *pPrevious = pHttpRef->pXmlHttpKillList;

        while (pCurrent != NULL)
        {
            if (pCurrent == pXmlHttp)
            {
                pPrevious->pNext = pCurrent->pNext;
                break;
            }

            pPrevious = pCurrent;
            pCurrent = pCurrent->pNext;
        }
    }

    pXmlHttp->bInKillList = FALSE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpApplyBaseUrl

    \Description
        Apply base url elements (if set) to any url elements not specified (relative
        url support).

    \Input *pHttpRef    - module ref
    \Input *pKind       - parsed http kind ("http" or "https")
    \Input *pHost       - [in/out] parsed URL host
    \Input iHostSize    - size of pHost buffer
    \Input *pPort       - [in/out] parsed port
    \Input *pSecure     - [in/out] parsed security (0 or 1)
    \Input bPortSpecified - TRUE if a port is explicitly specified in the url, else FALSE

    \Output
        uint32_t        - non-zero if changed, else zero

    \Version 02/03/2010 (jbrookes)
*/
/********************************************************************************F*/
static uint32_t _ProtoHttpApplyBaseUrl(ProtoHttpRefT *pHttpRef, const char *pKind, char *pHost, int32_t iHostSize, int32_t *pPort, int32_t *pSecure, uint8_t bPortSpecified)
{
    uint8_t bChanged = FALSE;
    if ((*pHost == '\0') && (pHttpRef->strBaseHost[0] != '\0'))
    {
        NetPrintf(("protohttpxboxone: [%p] host not present; setting to %s\n", pHttpRef, pHttpRef->strBaseHost));
        ds_strnzcpy(pHost, pHttpRef->strBaseHost, iHostSize);
        bChanged = TRUE;
    }
    if ((bPortSpecified == FALSE) && (pHttpRef->iBasePort != 0))
    {
        NetPrintf(("protohttpxboxone: [%p] port not present; setting to %d\n", pHttpRef, pHttpRef->iBasePort));
        *pPort = pHttpRef->iBasePort;
        bChanged = TRUE;
    }
    if (*pKind == '\0')
    {
        NetPrintf(("protohttpxboxone: [%p] kind (protocol) not present; setting to %d\n", pHttpRef, pHttpRef->iBaseSecure));
        *pSecure = pHttpRef->iBaseSecure;
        // if our port setting is default and incompatible with our security setting, override it
        if (((*pPort == 80) && (*pSecure == 1)) || ((*pPort == 443) && (*pSecure == 0)))
        {
            *pPort = *pSecure ? 443 : 80;
        }
        bChanged = TRUE;
    }
    return(bChanged);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetAppendHeader

    \Description
        Set given string as append header, allocating memory as required.

    \Input *pHttpRef    - module state pointer
    \Input *pAppendHdr  - append header string

    \Output
        int32_t         - zero=success, else error

    \Version 11/11/2004 (jbrookes) Split/combined from ProtoHttpGet() and ProtoHttpPost()
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSetAppendHeader(ProtoHttpRefT *pHttpRef, const char *pAppendHdr)
{
    int32_t iAppendBufLen, iAppendStrLen;

    // check for empty append string, in which case we free the buffer
    if ((pAppendHdr == NULL) || (*pAppendHdr == '\0'))
    {
        if (pHttpRef->pAppendHdr != NULL)
        {
            DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
            pHttpRef->pAppendHdr = NULL;
        }
        pHttpRef->iAppendLen = 0;
        return(0);
    }

    // check to see if append header is already set
    if ((pHttpRef->pAppendHdr != NULL) && (!strcmp(pAppendHdr, pHttpRef->pAppendHdr)))
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] ignoring set of append header '%s' that is already set\n", pHttpRef, pAppendHdr));
        return(0);
    }

    // get append header length
    iAppendStrLen = (int32_t)strlen(pAppendHdr);
    // append buffer size includes null and space for \r\n if not included by submitter
    iAppendBufLen = iAppendStrLen + 3;

    // see if we need to allocate a new buffer
    if (iAppendBufLen > pHttpRef->iAppendLen)
    {
        if (pHttpRef->pAppendHdr != NULL)
        {
            DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
        }
        if ((pHttpRef->pAppendHdr = (char *)DirtyMemAlloc(iAppendBufLen, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) != NULL)
        {
            pHttpRef->iAppendLen = iAppendBufLen;
        }
        else
        {
            NetPrintf(("protohttpxboxone: [%p] could not allocate %d byte buffer for append header\n", pHttpRef, iAppendBufLen));
            pHttpRef->iAppendLen = 0;
            return(-1);
        }
    }

    // copy append header
    ds_strnzcpy(pHttpRef->pAppendHdr, pAppendHdr, iAppendStrLen+1);

    // if append header is not \r\n terminated, do it here
    if (pHttpRef->pAppendHdr[iAppendStrLen-2] != '\r' || pHttpRef->pAppendHdr[iAppendStrLen-1] != '\n')
    {
        ds_strnzcat(pHttpRef->pAppendHdr, "\r\n", pHttpRef->iAppendLen);
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpResizeBuffer

    \Description
        Resize the buffer

    \Input *pHttpRef    - module state pointer
    \Input iBufMax      - new buffer size

    \Output
        int32_t         - zero=success, else failure

    \Version 02/21/2011 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpResizeBuffer(ProtoHttpRefT *pHttpRef, int32_t iBufMax)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iCopySize;
    char *pInpBuf;

    NetCritEnter(&pHttpRef->critical.Locker);

    NetPrintf(("protohttpxboxone: [%p] resizing input buffer from %d to %d bytes\n", pHttpRef, pHttpRef->critical.RequestBufferParams.iInpMax, iBufMax));
    if ((pInpBuf = (char *)DirtyMemAlloc(iBufMax, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p] could not resize input buffer\n", pHttpRef));
        NetCritLeave(&pHttpRef->critical.Locker);
        return(-1);
    }

    // calculate size of data to copy from old buffer to new
    if ((iCopySize = pXmlHttp->RequestBuffer.iInpLen) > iBufMax)
    {
        NetPrintf(("protohttpxboxone: [%p] warning; resize of input buffer is losing %d bytes of data\n", pHttpRef, iCopySize - iBufMax));
        iCopySize = iBufMax;
    }
    // copy valid contents of input buffer, if any, to new buffer
    ds_memcpy(pInpBuf, pXmlHttp->RequestBuffer.pInpBuf, iCopySize);

    // dispose of old buffer
    DirtyMemFree(pXmlHttp->RequestBuffer.pInpBuf, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);

    // update buffer variables
    pXmlHttp->RequestBuffer.pInpBuf = pInpBuf;
    pHttpRef->critical.RequestBufferParams.iInpMax = iBufMax;

    // clear overflow count
    pXmlHttp->RequestBuffer.iInpOvr = 0;

    NetCritLeave(&pHttpRef->critical.Locker);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpResizeBufferAuto

    \Description
        Resize the buffer, sizing based on increments of original buffer max.
        This function serves a similar function to what httpmanager does with
        normal protohttp.

    \Input *pHttpRef    - module state pointer
    \Input iBufMax      - requested buffer max

    \Output
        int32_t         - zero=success, else failure

    \Version 07/31/2013 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpResizeBufferAuto(ProtoHttpRefT *pHttpRef, int32_t iBufMax)
{
    int32_t iNewSize; 

    // calc new buffer size in increments of original buffer size
    for (iNewSize = pHttpRef->critical.RequestBufferParams.iInpMax; iNewSize < iBufMax; iNewSize += pHttpRef->critical.RequestBufferParams.iInpMax)
        ;

    // realloc buffer to allow for larger size
    return(_ProtoHttpResizeBuffer(pHttpRef, iNewSize));
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatUrl

    \Description
        This function builds the URL based on on values aquired on values cached by
        previous function calls

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - pointer to user-supplied url

    \Output
        int32_t         - negative for error, and ZERO for success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatUrl(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    char strHost[sizeof(pHttpRef->strHost)], strKind[32];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // if no url specified use the minimum
    if (*pUrl == '\0')
    {
        pUrl = "/";
    }

    // parse the url for kind, host, and port
    if (pHttpRef->strProxy[0] == '\0')
    {
        pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
    }
    else
    {
        NetPrintf(("protohttpxboxone: [%p] using proxy server %s\n", pHttpRef, pHttpRef->strProxy));
        ProtoHttpUrlParse2(pHttpRef->strProxy, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);
    }

    // determine if host, port, or security settings have changed since the previous request
    if ((iSecure != pHttpRef->iSecure) || (ds_stricmp(strHost, pHttpRef->strHost) != 0) || (iPort != pHttpRef->iPort))
    {
        pHttpRef->iKeepAlive = pHttpRef->iKeepAliveDflt;
    }

    // fill in any missing info (relative url) if available
    if (_ProtoHttpApplyBaseUrl(pHttpRef, strKind, strHost, sizeof(strHost), &iPort, &iSecure, bPortSpecified) != 0)
    {
        NetPrintf(("protohttpxboxone: [%p] %s://%s:%d%s\n", pHttpRef, iSecure ? "https" : "http", strHost, iPort, pUrl));
    }

    // determine if host, port, or security settings have changed since the previous request
    if ((iSecure != pHttpRef->iSecure) || (ds_stricmp(strHost, pHttpRef->strHost) != 0) || (iPort != pHttpRef->iPort))
    {
        // save new server/port/security state
        ds_strnzcpy(pHttpRef->strHost, strHost, sizeof(pHttpRef->strHost));
        pHttpRef->iPort = iPort;
        pHttpRef->iSecure = iSecure;
    }

    // save request type and url in input buffer
    pXmlHttp->RequestBuffer.iInpLen = ds_snzprintf(pXmlHttp->RequestBuffer.pInpBuf, pHttpRef->critical.RequestBufferParams.iInpMax,
        "%s %s", _ProtoHttp_strRequestType[pHttpRef->eRequestType], pUrl);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetRequestHeader

    \Description
        Set a request header into IXMLHTTPRequest2()

    \Input  *pXmlHttp   - xmlhttp object
    \Input  *pHeader    - unicode header name
    \Input  *pValue     - header value (not unicode)

    \Version 06/28/2013 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpSetRequestHeader(ProtoHttpXmlHttpT *pXmlHttp, const wchar_t *pHeader, const char *pValue)
{
    wchar_t *pWideStrBuf;
    int32_t iWideStrBufSize;
    HRESULT hResult;

    // allocate memory for converting the header value to unicode
    iWideStrBufSize = (signed) ((strlen(pValue) + 1) *  sizeof(wchar_t));
    if ((pWideStrBuf = (wchar_t *)DirtyMemAlloc(iWideStrBufSize, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] request header failure - unable to allocate (%d bytes) for mbs to wcs conversion buffer\n",
            pXmlHttp->pHttpRef, pXmlHttp, iWideStrBufSize));
        return;
    }

    // convert to wc
    MultiByteToWideChar(CP_UTF8, 0, pValue, -1, pWideStrBuf, iWideStrBufSize);

    // set the request header
    if ((hResult = pXmlHttp->cpXmlHttp->SetRequestHeader(pHeader, pWideStrBuf)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] could not set request header '%s' (err=%s)\n",
            pXmlHttp->pHttpRef, pXmlHttp, pValue, DirtyErrGetName(hResult)));
    }

    DirtyMemFree(pWideStrBuf, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSetRequestHeaders

    \Description
        Set all request headers into IXMLHTTPRequest2

    \Input *pXmlHttp    - xmlhttp object
    \Input *pHdrBuf     - pointer to headers
    \Input *pStrTempBuf - temp work buffer
    \Input iBufLen      - size of temp work buffer

    \Version 07/31/2013 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpSetRequestHeaders(ProtoHttpXmlHttpT *pXmlHttp, const char *pHdrBuf, char *pStrTempBuf, int32_t iBufLen)
{
    wchar_t wstrHeaderName[256];
    char strHeaderName[256];

    for ( ; ProtoHttpGetNextHeader(pXmlHttp, pHdrBuf, strHeaderName, sizeof(strHeaderName), pStrTempBuf, iBufLen, &pHdrBuf) == 0; )
    {
        MultiByteToWideChar(CP_UTF8, 0, strHeaderName, -1, wstrHeaderName, sizeof(wstrHeaderName));
        _ProtoHttpSetRequestHeader(pXmlHttp, wstrHeaderName, pStrTempBuf);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpRead

    \Description
        Read out of SequentialStream into provided buffer

    \Input  *pHttpRef   - module state pointer
    \Input  *pStream    - stream to read from
    \Input  *pBuffer    - [out] buffer to read into
    \Input  iMaxRead    - max buffer size

    \Output
        int32_t         - negative=error, else number of bytes read

    \Version 06/27/2013 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpRead(ProtoHttpRefT *pHttpRef, ISequentialStream *pStream, char *pBuffer, int32_t iMaxRead)
{
    ULONG uBytesRead = 0; 

    NetCritEnter(&pHttpRef->critical.Locker);

    // check for space to read into
    if ((pBuffer != NULL) && (iMaxRead > 0))
    {
        HRESULT hResult = pStream->Read(pBuffer, iMaxRead, &uBytesRead);
        if (SUCCEEDED(hResult) && (uBytesRead > 0))
        {
            #if DIRTYCODE_LOGGING
            if (pHttpRef->iVerbose > 2)
            {
                NetPrintf(("protohttpxboxone: [%p] recv %d bytes\n", pHttpRef, uBytesRead));
            }
            if (pHttpRef->iVerbose > 3)
            {
                NetPrintMem(pBuffer, uBytesRead, "http-recv");
            }
            #endif

            // received data, so update timeout
            pHttpRef->uTimer = NetTick() + pHttpRef->uTimeout;
        }
        else if (FAILED(hResult))
        {
            NetPrintf(("protohttpxboxone: read failed (err=%s)\n", DirtyErrGetName(hResult)));
            uBytesRead = (unsigned)-1;
        }
    }

    NetCritLeave(&pHttpRef->critical.Locker);

    return(uBytesRead);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpSaveRequestData

    \Description
        Store request data internally until the output stream object
        is successfully allocated and we can push data into into it.

    \Input  *pHttpRef   - module state pointer
    \Input  *pData      - pointer to buffer to be saved (cannot be NULL)
    \Input  iDataSize   - amount of data to be saved (assumed to be positive)

    \Output
        int32_t         - amount of bytes saved, < 0 if error

    \Version 09/20/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpSaveRequestData(ProtoHttpRefT *pHttpRef, const char *pData, int32_t iDataSize)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iResult = 0;

    if (iDataSize > pHttpRef->critical.RequestBufferParams.iInpMaxIdeal)
    {
        iDataSize = pHttpRef->critical.RequestBufferParams.iInpMaxIdeal;
    }

    if (iDataSize > pHttpRef->critical.RequestBufferParams.iInpMax)
    {
        iResult = _ProtoHttpResizeBuffer(pHttpRef, iDataSize);
    }

    if (iResult == 0)
    {
        // save request data
        ds_memcpy(pXmlHttp->RequestBuffer.pInpBuf, pData, iDataSize);
        pXmlHttp->RequestBuffer.iInpLen = iDataSize;
    }

    return(iDataSize);
}


/*F********************************************************************************/
/*!
    \Function _ProtoHttpFormatRequestHeader

    \Description
        Format request header, execute custom header callback, and set headers
        into IXMLHTTPRequest2 object.

    \Input *pHttpRef    - module state pointer
    \Input *pStrTempBuf - temp work buffer
    \Input iBufLen      - size of temp work buffer

    \Output
        int32_t         - negative=error, else zero

    \Version 07/18/2013 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpFormatRequestHeader(ProtoHttpRefT *pHttpRef, char *pStrTempBuf, int32_t iBufLen)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iInpLen, iInpMax, iResult;
    char *pInpBuf;
    ProtoHttpCustomHeaderCbT *pCustomHeaderCb;
    void *pUserData;

    // set up for header formatting
    pInpBuf = pXmlHttp->RequestBuffer.pInpBuf;
    iInpLen = pXmlHttp->RequestBuffer.iInpLen;
    iInpMax = pHttpRef->critical.RequestBufferParams.iInpMax;

    // add HTTP/1.1 and crlf after url
    iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, " HTTP/1.1\r\n");

    // append host (note: MS will supply host, but always with :port, so we override that ourselves)
    if ((pHttpRef->iSecure && (pHttpRef->iPort == 443)) || (pHttpRef->iPort == 80))
    {
        iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, "Host: %s\r\n", pHttpRef->strHost);
    }
    else
    {
        iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, "Host: %s:%d\r\n", pHttpRef->strHost, pHttpRef->iPort);
    }

    // disable keep-alive?
    if (pHttpRef->iKeepAlive == 0)
    {
        iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, "Connection: Close\r\n");
    }

    // append user-agent, if not specified in append header
    if ((pHttpRef->pAppendHdr == NULL) || !ds_stristr(pHttpRef->pAppendHdr, "User-Agent:"))
    {
        iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen,
            "User-Agent: ProtoHttp %d.%d/DS %d.%d.%d.%d.%d (" DIRTYCODE_PLATNAME ")\r\n",
            (PROTOHTTP_VERSION>>8)&0xff, PROTOHTTP_VERSION&0xff, DIRTYSDK_VERSION_YEAR, DIRTYSDK_VERSION_SEASON,
            DIRTYSDK_VERSION_MAJOR, DIRTYSDK_VERSION_MINOR, DIRTYSDK_VERSION_PATCH);
    }

    // append auth token if enabled
    if (pHttpRef->bGetAuthToken)
    {
        int32_t iTokenLen;
        if ((iResult = DirtyAuthCheckToken(pHttpRef->iAuthTokenIdx, pHttpRef->strTokenUrl, &iTokenLen, pStrTempBuf)) == DIRTYAUTH_SUCCESS)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] appending xsts token header (len=%d)\n", pHttpRef, iTokenLen));
            iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, "Authorization: %s\r\n", pStrTempBuf);
        }
        else
        {
            NetPrintf(("protohttpxboxone: [%p] failed to set authorization header (err=%d)\n", pHttpRef, iResult));
        }
    }

    // add append headers, if specified
    if ((pHttpRef->pAppendHdr != NULL) && (pHttpRef->pAppendHdr[0] != '\0'))
    {
        iInpLen += ds_snzprintf(pInpBuf+iInpLen, iInpMax-iInpLen, "%s", pHttpRef->pAppendHdr);
    }

    // request level callback takes priority to global
    if ((pCustomHeaderCb = pHttpRef->pReqCustomHeaderCb) != NULL)
    {
        pUserData = pHttpRef->pUserData;
    }
    else
    {
        pCustomHeaderCb = pHttpRef->pCustomHeaderCb;
        pUserData = pHttpRef->pCallbackRef;
    }

    // call custom header format callback, if specified
    if (pCustomHeaderCb != NULL)
    {
        /*
        We have plans to eventually remove the pData and iDataLen parameters from the ProtoHttpCustomHeaderCbT synopsis.
        There is no obvious need for them and we are not aware of any customer implementation really using them.
        For protohttpxboxone specifically, it allows us to eliminate a fair amount of complexity. Because that callback
        is not invoked in the context of the request initiation (before ProtoHttpPost() returns for instance) like
        we don on other platforms, we would have to internally buffer the request data to be able to pass a valid buffer
        pointer and buffer length here.
        */
        if ((iInpLen = pCustomHeaderCb(pHttpRef, pInpBuf, iInpMax, NULL, 0, pUserData)) < 0)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpxboxone: [%p] custom header callback error %d\n", pHttpRef, iInpLen));
            return(iInpLen);
        }
        if (iInpLen == 0)
        {
            iInpLen = (int32_t)strlen(pInpBuf);
        }
    }

    // make sure we were able to complete the header
    if (iInpLen > iInpMax)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpxboxone: [%p] not enough buffer to format request header (have %d, need %d)\n", pHttpRef, iInpMax, iInpLen));
        pXmlHttp->RequestBuffer.iInpOvr = iInpLen;
        return(PROTOHTTP_MINBUFF);
    }

    // save a copy of the header
    ds_strnzcpy(pHttpRef->strRequestHdr, pInpBuf, sizeof(pHttpRef->strRequestHdr));

    // success
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpProcessRedirect

    \Description
        Process pending redirect notifications

    \Input *pHttpRef    - module state pointer

    \Output
        int32_t         - 1 for success, 0 for no-op

    \Version 07/22/2013 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpProcessRedirect(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iHdrLen;
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;
    void *pUserData;

    // check for a redirection header
    if (pHttpRef->critical.iNumRedirect == pHttpRef->critical.iNumRedirProc)
    {
        return(0);
    }

    // copy header to header cache
    ds_strnzcpy(pHttpRef->responseHeader.strHdr, pXmlHttp->RequestBuffer.pInpBuf, sizeof(pHttpRef->responseHeader.strHdr));

    // set response code to 302/FOUND
    pHttpRef->responseHeader.iHdrCode = PROTOHTTP_RESPONSE_FOUND;
    pHttpRef->responseHeader.iHeadSize = (int32_t)strlen(pXmlHttp->RequestBuffer.pInpBuf);

    #if DIRTYCODE_LOGGING
    NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpxboxone: [%p] received %d response (%d bytes)\n", pHttpRef, pHttpRef->responseHeader.iHdrCode, pHttpRef->responseHeader.iHeadSize));
    if (pHttpRef->iVerbose > 1)
    {
        NetPrintWrap(pXmlHttp->RequestBuffer.pInpBuf, 80);
    }
    #endif

    // request level callback takes priority to global
    if ((pReceiveHeaderCb = pHttpRef->pReqReceiveHeaderCb) != NULL)
    {
        pUserData = pHttpRef->pUserData;
    }
    else
    {
        pReceiveHeaderCb = pHttpRef->pReceiveHeaderCb;
        pUserData = pHttpRef->pCallbackRef;
    }

    // trigger recv header user callback, if specified
    if (pReceiveHeaderCb != NULL)
    {
        pReceiveHeaderCb(pHttpRef, pXmlHttp->RequestBuffer.pInpBuf, pHttpRef->responseHeader.iHeadSize, pUserData);
    }

    // remove header from buffer
    iHdrLen = pHttpRef->responseHeader.iHeadSize + 1;
    if (iHdrLen < pXmlHttp->RequestBuffer.iInpLen)
    {
        memmove(pXmlHttp->RequestBuffer.pInpBuf, pXmlHttp->RequestBuffer.pInpBuf + iHdrLen, pXmlHttp->RequestBuffer.iInpLen - iHdrLen);
    }
    pXmlHttp->RequestBuffer.iInpLen -= iHdrLen;

    // track processing of redirection
    pHttpRef->critical.iNumRedirProc += 1;

    return(1);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpGetResponseHeader

    \Description
        Get response header from IXMLHttpRequest2 object

    \Input *pHttpRef    - module state pointer
    \Input iHdrCode     - header code
    \Input *pwStrCode   - status code for the request

    \Output
        int32_t         - negative=error, else zero

    \Version 07/22/2013 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpGetResponseHeader(ProtoHttpRefT *pHttpRef, int32_t iHdrCode, const wchar_t *pwStrCode)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;
    ProtoHttpBufferT *pBuffer;
    WCHAR *pwstrHeaders;
    int32_t iHdrLen;
    HRESULT hResult;
    
    NetCritEnter(&pHttpRef->critical.Locker);

    // reference response buffer
    pBuffer = &pXmlHttp->RequestBuffer;

    // get all headers
    if ((hResult = pXmlHttp->cpXmlHttp->GetAllResponseHeaders(&pwstrHeaders)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p] GetAllResponseHeaders() failed (err=%s)\n", pHttpRef, DirtyErrGetName(hResult)));
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
        NetCritLeave(&pHttpRef->critical.Locker);
        return(-1);
    }

    // calc header length; add a pad for HTTP status line we will add
    if ((iHdrLen = (int32_t)wcslen(pwstrHeaders) + 64) > pHttpRef->critical.RequestBufferParams.iInpMax)
    {
        _ProtoHttpResizeBufferAuto(pHttpRef, iHdrLen);
    }

    // format response header protocol, status, and status string, add null for later easy parsing
    pBuffer->iInpLen += ds_snzprintf(pBuffer->pInpBuf+pBuffer->iInpLen, pHttpRef->critical.RequestBufferParams.iInpMax-pBuffer->iInpLen, "HTTP/1.1 %d %S\r\n%S", iHdrCode, pwStrCode, pwstrHeaders);
    pBuffer->iInpLen += 1;

    // release tmp header buffer
    CoTaskMemFree(pwstrHeaders);

    // success
    NetCritLeave(&pHttpRef->critical.Locker);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpParseHeader

    \Description
        Parse incoming HTTP header.  The HTTP header is presumed to be at the
        beginning of the input buffer.

    \Input  *pHttpRef   - module state pointer

    \Output
        int32_t         - negative=not ready or error, else success

    \Version 11/12/2004 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpParseHeader(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    char strTemp[128];
    int32_t iHdrLen;
    ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb;
    void *pUserData;

    // save the header size (include final terminator)
    pHttpRef->responseHeader.iHeadSize = (int32_t)strlen(pXmlHttp->RequestBuffer.pInpBuf);
    // parse header code
    pHttpRef->responseHeader.iHdrCode = ProtoHttpParseHeaderCode(pXmlHttp->RequestBuffer.pInpBuf);

    #if DIRTYCODE_LOGGING
    NetPrintfVerbose((pHttpRef->iVerbose, 0, "protohttpxboxone: [%p] received %d response (%d bytes)\n", pHttpRef, pHttpRef->responseHeader.iHdrCode, pHttpRef->responseHeader.iHeadSize));
    if (pHttpRef->iVerbose > 1)
    {
        NetPrintWrap(pXmlHttp->RequestBuffer.pInpBuf, 80);
    }
    #endif

    // parse content-length field
    if ((ProtoHttpExtractHeaderValue(ProtoHttpFindHeaderValue(pXmlHttp->RequestBuffer.pInpBuf, "content-length"), strTemp, sizeof(strTemp), NULL)) != -1)
    {
        pHttpRef->responseHeader.iBodySize = ds_strtoll(strTemp, NULL, 10);
    }
    else
    {
        pHttpRef->responseHeader.iBodySize = -1;
    }

    // parse last-modified header
    if ((ProtoHttpExtractHeaderValue(ProtoHttpFindHeaderValue(pXmlHttp->RequestBuffer.pInpBuf, "last-modified"), strTemp, sizeof(strTemp), NULL)) != -1)
    {
        pHttpRef->responseHeader.iHdrDate = ds_strtotime(strTemp);
    }
    else
    {
        pHttpRef->responseHeader.iHdrDate = 0;
    }

    // terminate the header for easy parsing
    pXmlHttp->RequestBuffer.pInpBuf[pHttpRef->responseHeader.iHeadSize - 1] = pXmlHttp->RequestBuffer.pInpBuf[pHttpRef->responseHeader.iHeadSize - 2] = 0;

    // copy header to header cache
    ds_strnzcpy(pHttpRef->responseHeader.strHdr, pXmlHttp->RequestBuffer.pInpBuf, sizeof(pHttpRef->responseHeader.strHdr));

    // request level callback takes priority to global
    if ((pReceiveHeaderCb = pHttpRef->pReqReceiveHeaderCb) != NULL)
    {
        pUserData = pHttpRef->pUserData;
    }
    else
    {
        pReceiveHeaderCb = pHttpRef->pReceiveHeaderCb;
        pUserData = pHttpRef->pCallbackRef;
    }

    // trigger recv header user callback, if specified
    if (pReceiveHeaderCb != NULL)
    {
        pReceiveHeaderCb(pHttpRef, pXmlHttp->RequestBuffer.pInpBuf, pHttpRef->responseHeader.iHeadSize, pUserData);
    }

    // remove header (and null terminator) from input buffer
    iHdrLen = pHttpRef->responseHeader.iHeadSize + 1;
    if (iHdrLen < pXmlHttp->RequestBuffer.iInpLen)
    {
        memmove(pXmlHttp->RequestBuffer.pInpBuf, pXmlHttp->RequestBuffer.pInpBuf + iHdrLen, pXmlHttp->RequestBuffer.iInpLen - iHdrLen);
    }
    pXmlHttp->RequestBuffer.iInpLen -= iHdrLen;
    
    // handle response code?
    if (PROTOHTTP_GetResponseClass(pHttpRef->responseHeader.iHdrCode) == PROTOHTTP_RESPONSE_INFORMATIONAL)
    {
        NetPrintf(("protohttpxboxone: [%p] ignoring %d response from server\n", pHttpRef, pHttpRef->responseHeader.iHdrCode));
    }
    else if (PROTOHTTP_GetResponseClass(pHttpRef->responseHeader.iHdrCode) == PROTOHTTP_RESPONSE_REDIRECTION)
    {
        NetPrintf(("protohttpxboxone: [%p] redirection %d response from server\n", pHttpRef, pHttpRef->responseHeader.iHdrCode));
    }

    // header successfully parsed
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpWriteCbProcess

    \Description
        User write callback processing, if write callback is set

    \Input *pHttpRef    - module state pointer

    \Version 05/03/2012 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpWriteCbProcess(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    ProtoHttpWriteCbInfoT WriteCbInfo;
    int32_t iResult;

    ds_memclr(&WriteCbInfo, sizeof(WriteCbInfo));
    WriteCbInfo.eRequestType = pHttpRef->eRequestType;
    WriteCbInfo.eRequestResponse = (ProtoHttpResponseE)pHttpRef->responseHeader.iHdrCode;

    if ((pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_BODY) || ((pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_DONE) && (pXmlHttp->RequestBuffer.iInpReadTotal < pHttpRef->responseHeader.iBodySize)))
    {
        char strTempRecv[4096];
        while ((iResult = ProtoHttpRecv(pHttpRef, strTempRecv, 1, sizeof(strTempRecv))) > 0)
        {
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, strTempRecv, iResult, pHttpRef->pUserData);
        }
    }
    else if (pHttpRef->critical.eRequestState > ST_BODY)
    {
        if (pHttpRef->critical.eRequestState == ST_DONE)
        {
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, "", (pHttpRef->eRequestType == PROTOHTTP_REQUESTTYPE_HEAD) ? PROTOHTTP_HEADONLY : PROTOHTTP_RECVDONE, pHttpRef->pUserData);
        }
        if (pHttpRef->critical.eRequestState == ST_FAIL)
        {
            pHttpRef->pWriteCb(pHttpRef, &WriteCbInfo, "", pHttpRef->critical.bTimeout ? PROTOHTTP_TIMEOUT : PROTOHTTP_RECVFAIL, pHttpRef->pUserData);
        }
        pHttpRef->pWriteCb = NULL;
        pHttpRef->pReqCustomHeaderCb = NULL;
        pHttpRef->pReqReceiveHeaderCb = NULL;
        pHttpRef->pUserData = NULL;
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpDestroy

    \Description
        Destroy ProtoHttp ref

    \Input *pHttpRef  - module ref

    \Version 10/03/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpDestroy(ProtoHttpRefT *pHttpRef)
{
    NetPrintf(("protohttpxboxone: [%p] destructing...\n", pHttpRef));

    NetCritKill(&pHttpRef->critical.Locker);

    if (pHttpRef->pAppendHdr != NULL)
    {
        DirtyMemFree(pHttpRef->pAppendHdr, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
    }

    DirtyMemFree(pHttpRef, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpCreate

    \Description
        Create XmlHttp object

    \Input *pHttpRef        - module state pointer

    \Output
        ProtoHttpXmlHttpT * - XmlHttp object, or NULL on failure

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static ProtoHttpXmlHttpT *_ProtoHttpXmlHttpCreate(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttpT *pXmlHttp;

    // allocate memory for object
    if ((pXmlHttp = (ProtoHttpXmlHttpT *)DirtyMemAlloc(sizeof(*pXmlHttp), PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p] unable to allocate xmlhttp memory\n", pHttpRef));
        return(NULL);
    }
    ds_memclr(pXmlHttp, sizeof(*pXmlHttp));

    // allocate input buffer
    if ((pXmlHttp->RequestBuffer.pInpBuf = (char *)DirtyMemAlloc(pHttpRef->critical.RequestBufferParams.iInpMax, PROTOHTTP_MEMID, pHttpRef->iMemGroup, pHttpRef->pMemGroupUserData)) == NULL)
    {
        DirtyMemFree(pXmlHttp, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
        NetPrintf(("protohttpxboxone: [%p] unable to allocate request buffer\n", pHttpRef));
        ProtoHttpDestroy(pHttpRef);
        return(NULL);
    }

    // allocate resources required to synchronize main thread and dedicated thread used for offloading long blocking MS calls
    if ((pXmlHttp->pBlockingOpsThreadCond = DirtyConditionCreate("ProtoHttpBlockingOpsThreadWakeUpCond")) == NULL)
    {
        DirtyMemFree(pXmlHttp->RequestBuffer.pInpBuf, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
        DirtyMemFree(pXmlHttp, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
        NetPrintf(("protohttpxboxone: [%p] error creating ProtoHttpBlockingOpsThreadWakeUpCond condition\n", pHttpRef));
        return(NULL);
    }
    NetCritInit(&pXmlHttp->BlockingOpsThreadCrit, "ProtoHttpBlockingOpsThreadWakeUpCrit");

    // remember what http ref this xmlhttp object belongs to
    pXmlHttp->pHttpRef = pHttpRef;

    // save some state info for use when we don't have access to the state
    pXmlHttp->HttpRefDataCache.eRequestType = pHttpRef->eRequestType;
    pXmlHttp->HttpRefDataCache.pMemGroupUserData = pHttpRef->pMemGroupUserData;
    pXmlHttp->HttpRefDataCache.iMemGroup = pHttpRef->iMemGroup;
    pXmlHttp->HttpRefDataCache.iPort = pHttpRef->iPort;
    pXmlHttp->HttpRefDataCache.iSecure = pHttpRef->iSecure; 
    pXmlHttp->HttpRefDataCache.iOnDataThreshold = pHttpRef->iOnDataThreshold;
    ds_strnzcpy(pXmlHttp->HttpRefDataCache.strHost, pHttpRef->strHost, sizeof(pXmlHttp->HttpRefDataCache.strHost));
    pXmlHttp->HttpRefDataCache.bAuthenticate = pHttpRef->bAuthenticate;
    pXmlHttp->HttpRefDataCache.iVerbose = pHttpRef->iVerbose;

    return(pXmlHttp);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpDestroy

    \Description
        Destroy XmlHttp object. This function assumes we are already in a valid
        state to destroy the object.

    \Input *pXmlHttp        - xmlhttp object to destroy

    \Notes
        The current implementation is such that this function is called only when 
        the xmlhttp object reaches the REQ_ST_BYE state. Consequently there is always
        an implicit delay between the com objects being released with calls to 
        ReleaseAndGetAddressOf() while in state REQ_ST_CLEANINGUP, and the xmlhttp
        ref being freed here while in state REQ_ST_BYE.

        Experimentation showed that this delay happens to be necessary to avoid
        using a freed xmlhttp ref while executing an xmlhttp event handler
        (onError, onRedirect, etc.) possibly firing immediately after the
        call to ReleaseAndGetAddressOf(), which is a scenario that has been
        reported to rarely repro under some specific timing conditions.

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpDestroy(ProtoHttpXmlHttpT *pXmlHttp)
{
    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 1, "protohttpxboxone: [%p][%p] destruction of xmlhttp object\n", pXmlHttp->pHttpRef, pXmlHttp));

    // release resources associated with dedicated thread
    if (pXmlHttp->pBlockingOpsThreadCond != NULL)
    {
        DirtyConditionDestroy(pXmlHttp->pBlockingOpsThreadCond);
        pXmlHttp->pBlockingOpsThreadCond = NULL;
    }
    NetCritKill(&pXmlHttp->BlockingOpsThreadCrit);

    if (pXmlHttp->pPreSendBuffer != NULL)
    {
        DirtyMemFree(pXmlHttp->pPreSendBuffer, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
    }

    DirtyMemFree(pXmlHttp->RequestBuffer.pInpBuf, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);

    DirtyMemFree(pXmlHttp, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData);
}


/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpFillPreSendBuffer

    \Description
        Copy request data into XmlHttp object's pre-send buffer

    \Input *pXmlHttp    - XmlHttp object
    \Input *pData       - pointer to request data
    \Input iDataSize    - size of reqeust data

    \Output
        int32_t         - amount of bytes saved, < 0 if error

    \Version 02/05/2019 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpXmlHttpFillPreSendBuffer(ProtoHttpXmlHttpT *pXmlHttp, const char *pData, int64_t iDataSize)
{
    int32_t iBytesCopied;

    if (pXmlHttp->pPreSendBuffer == NULL)
    {
        if ((pXmlHttp->pPreSendBuffer = (char *)DirtyMemAlloc(PROTOHTTP_PRESENDBUF_SIZE, PROTOHTTP_MEMID, pXmlHttp->HttpRefDataCache.iMemGroup, pXmlHttp->HttpRefDataCache.pMemGroupUserData)) == NULL)
        {
            NetPrintf(("protohttpxboxone: [%p][%p] unable to allocate the pre-send buffer\n", pXmlHttp->pHttpRef, pXmlHttp));
            return(-1);
        }
    }
    ds_memclr(pXmlHttp->pPreSendBuffer, PROTOHTTP_PRESENDBUF_SIZE);

    iBytesCopied = DS_MIN(iDataSize, PROTOHTTP_PRESENDBUF_SIZE);
    ds_memcpy(pXmlHttp->pPreSendBuffer, pData, iBytesCopied);
    pXmlHttp->iPreSendBufferValidBytes = iBytesCopied;
    pXmlHttp->iPreSendBufferConsumedBytes = 0;

    return(iBytesCopied);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedIXMLHTTPRequest2Creation

    \Description
        Creates and initializes MS IXMLHTTPRequest2 object.

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedIXMLHTTPRequest2Creation(ProtoHttpXmlHttpT *pXmlHttp)
{
    HRESULT hResult;

    // instantiate IXMLHTTPRequest2 object
    if ((hResult = CoCreateInstance(__uuidof(FreeThreadedXMLHTTP60), NULL, CLSCTX_SERVER, __uuidof(IXMLHTTPRequest2), &pXmlHttp->cpXmlHttp)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to instantiate IXMLHTTPRequest2 object (err=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
        return(-1);
    }
    // instantiate HttpCallback object and set state ref
    if ((hResult = Microsoft::WRL::Details::MakeAndInitialize<HttpCallback>(&pXmlHttp->cpXmlHttpCallback)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to instantiate HttpCallback object (err=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
        return(-2);
    }
    // instantiate output stream object and set state ref
    if ((hResult = Microsoft::WRL::Details::MakeAndInitialize<SequentialStream>(&pXmlHttp->cpOutStream)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to instantiate OutStream object (err=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
        return(-3);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedOutStreamWrite

    \Description
        Write data into the out stream object associated with the specified
        xmlhttp object.

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedOutStreamWrite(ProtoHttpXmlHttpT *pXmlHttp)
{
    ULONG uBytesWritten;

    if (pXmlHttp->cpOutStream.Get()->Write(pXmlHttp->RequestBuffer.pInpBuf, pXmlHttp->RequestBuffer.iInpLen, &uBytesWritten) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to write %d bytes to output stream\n", pXmlHttp->pHttpRef, pXmlHttp, pXmlHttp->RequestBuffer.iInpLen));
        return(-1);
    }

    return(uBytesWritten);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedXmlHttpOpen

    \Description
        Open the xmlhttp request.

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error, positive for retry detection

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedXmlHttpOpen(ProtoHttpXmlHttpT *pXmlHttp)
{
    char  strUrlPrefix[300];
    wchar_t wstrRequestType[16], wstrUrlPrefix[300];
    char *pUrlStart, *pUrlEnd;
    HRESULT hResult;

    // find end of request url (note that url may have been modified by custom header callback)
    if ((pUrlEnd = ds_stristr(pXmlHttp->RequestBuffer.pInpBuf, " HTTP/1.1\r\n")) == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] could not find url terminator in request; aborting\n", pXmlHttp->pHttpRef, pXmlHttp));
        return(-1);
    }

    #if DIRTYCODE_LOGGING
    if (pXmlHttp->HttpRefDataCache.iVerbose > 0)
    {
        // temporarily null-terminate url for debug output
        char cUrlChar = *(pUrlEnd);
        *(pUrlEnd) = '\0';
        NetPrintf(("protohttpxboxone: [%p][%p] %s (state=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, pXmlHttp->RequestBuffer.pInpBuf,
            _ProtoHttp_strRequestState[pXmlHttp->pHttpRef->critical.eRequestState]));
        *(pUrlEnd) = cUrlChar;
    }
    #endif

    // skip request type to find url start
    for (pUrlStart = pXmlHttp->RequestBuffer.pInpBuf; *(pUrlStart) != ' '; pUrlStart += 1)
        ;
    pUrlStart += 1;

    // convert request type to unicode
    MultiByteToWideChar(CP_UTF8, 0, _ProtoHttp_strRequestType[pXmlHttp->HttpRefDataCache.eRequestType], -1, wstrRequestType, sizeof(wstrRequestType));

    // construct url prefix (protocol, hostname, port)
    ds_snzprintf(strUrlPrefix, sizeof(strUrlPrefix), "%s://%s:%d", pXmlHttp->HttpRefDataCache.iSecure ? "https" : "http", pXmlHttp->HttpRefDataCache.strHost, pXmlHttp->HttpRefDataCache.iPort);
    MultiByteToWideChar(CP_UTF8, 0, strUrlPrefix, -1, wstrUrlPrefix, sizeof(wstrUrlPrefix));

    // construct url
    std::wstring unicodeUrl(wstrUrlPrefix);
    std::wstring unicodeUri(pUrlStart, pUrlEnd);
    unicodeUrl += unicodeUri;

    // skip past end of url line and save the position
    pXmlHttp->pHdrStrt = ds_stristr(pUrlEnd, "\r\n") + 2;

    if ((hResult = pXmlHttp->cpXmlHttp->Open(wstrRequestType, unicodeUrl.c_str(), pXmlHttp->cpXmlHttpCallback.Get(), NULL, NULL, NULL, NULL)) != S_OK)
    {
        if ((hResult & 0xffff) == OR_INVALID_OXID)
        {
            NetPrintf(("protohttpxboxone: [%p][%p] failed to send data (err=%s) - retry scenario detected\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
            return(1);
        }
        else
        {
            NetPrintf(("protohttpxboxone: [%p][%p] failed to send data (err=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
            return(-1);
        }
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedXmlHttpSetup

    \Description
        Finalize initialization of xmlhttp request

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedXmlHttpSetup(ProtoHttpXmlHttpT *pXmlHttp)
{
    char strTempBuf[8 * 1024];
    HRESULT hResult;

    _ProtoHttpSetRequestHeaders(pXmlHttp, pXmlHttp->pHdrStrt, strTempBuf, sizeof(strTempBuf));

    // set request properties
    if ((hResult = pXmlHttp->cpXmlHttp->SetProperty(XHR_PROP_NO_AUTH, !pXmlHttp->HttpRefDataCache.bAuthenticate)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to %s authentication\n", pXmlHttp->pHttpRef, pXmlHttp, pXmlHttp->HttpRefDataCache.bAuthenticate ? "enable" : "disable"));
        return(-1);
    }
    /* disable ms timeouts because it only implements a connection timeout so we implement our own
    From MS doc:
    "The XHR_PROP_TIMEOUT property defaults to 0, which means that requests will not automatically time out. The value is
    specified in milliseconds and the minimum supported value is 5000 ms (5 seconds). The timeout property, when set, is
    only used to time out the initial connection. It will invoke an IXMLHTTPRequest2Callback::OnError handler if there is
    no response from the host in the specified timeframe. It cannot be used to time box requests which are active and are
    steadily downloading. If you want to cancel requests which are active and steadily downloading, but are taking a long
    time, then you must implement functionality to call Abort yourself.
    */
    if ((hResult = pXmlHttp->cpXmlHttp->SetProperty(XHR_PROP_TIMEOUT, 0x7fffffff)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to set timeout (err=%s)\n", pXmlHttp->pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
        return(-2);
    }
    if ((hResult = pXmlHttp->cpXmlHttp->SetProperty(XHR_PROP_NO_CRED_PROMPT, TRUE)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to disable credential prompt\n", pXmlHttp->pHttpRef, pXmlHttp));
        return(-3);
    }
    if ((hResult = pXmlHttp->cpXmlHttp->SetProperty(XHR_PROP_ONDATA_THRESHOLD, pXmlHttp->HttpRefDataCache.iOnDataThreshold)) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to set ondata threshold\n", pXmlHttp->pHttpRef, pXmlHttp));
        return(-4);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedXmlHttpSend

    \Description
        Let the xmlhttp object know that it is time to fire the request.

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedXmlHttpSend(ProtoHttpXmlHttpT *pXmlHttp)
{
#if DIRTYCODE_LOGGING
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;
#endif
    HRESULT hResult;

    if ((hResult = pXmlHttp->cpXmlHttp->Send(pXmlHttp->cpOutStream.Get(), pXmlHttp->cpOutStream.Get()->GetDataSize())) != S_OK)
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to send data (err=%s)\n", pHttpRef, pXmlHttp, DirtyErrGetName(hResult)));
        return(-1);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpOffloadedXmlHttpCleanup

    \Description
        Release MS IXMLHTTPRequest2 object and associated resources. 

    \Input *pXmlHttp        - XmlHttp object

    \Output
        int32_t             - 0 for success, negative for error

    \Notes
        Operation reserved for the dedicated thread used to offload long blocking
        MS system calls from main thread.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpOffloadedXmlHttpCleanup(ProtoHttpXmlHttpT *pXmlHttp)
{
    /* we need to explicitly release the ComPtr objects we created with
    CoCreateInstance/MakeInitialize as we are calling from C code, not
    a class with a destructor where this would happen automatically */
    pXmlHttp->cpXmlHttp.ReleaseAndGetAddressOf();
    pXmlHttp->cpXmlHttp = nullptr;
    pXmlHttp->cpXmlHttpCallback.ReleaseAndGetAddressOf();
    pXmlHttp->cpXmlHttpCallback = nullptr;
    pXmlHttp->cpOutStream.ReleaseAndGetAddressOf();
    pXmlHttp->cpOutStream = nullptr;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   _ProtoHttpBlockingOpsThread

    \Description
        Dedicated thread used to offload long block MS system calls from main thread.

    \Input *pParam      - module state pointer

    \Notes
        In this function, the usage of the condition mutex and the associated crit
        takes two things into account:
            1- DirtyConditionWait() shall be called before the main thread attempts
               a call to DirtyConditionSignal() to wake this thread up. If the main
               thread calls DirtyConditionSignal() too early, then this thread
               will miss the signal and block when calling DirtyConditionWait()
               until DirtyConditionSignal() is called again (which will not happen
               because the main thread will be waiting for the previous signal to be
               honored.
            2- DirtyConditionWait() internally leaves the critical section before
               sleeping, and it re-enters the criticaa section when it wakes up.

    \Version 09/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpBlockingOpsThread(void *pParam)
{
    ProtoHttpXmlHttpT *pXmlHttp = (ProtoHttpXmlHttpT *)pParam;
    uint8_t bExit = FALSE;

    NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 3, "protohttpxboxone: [%p][%p] thread used for offloading blocking ops is starting\n",
        pXmlHttp->pHttpRef, pXmlHttp));

    /* To avoid a possible race condition where the main thread would call DirtyConditionSignal()
       before we have a chance to call DirtyConditionWait(), we enter the crit before signaling the thread
       is alive. Because DirtyConditionWait() internally releases the crit before sleeping, we now have 
       (1) life=alive and (2) initiate wait on signal both performed atomically. */
    NetCritEnter(&pXmlHttp->BlockingOpsThreadCrit);

    pXmlHttp->eBlockingOpsThreadLife = ProtoHttpXmlHttpT::THREAD_LIFE_STATUS_ALIVE;

    // execute until we're killed
    while (bExit == FALSE)
    {
        int32_t iResult = 0;

        DirtyConditionWait(pXmlHttp->pBlockingOpsThreadCond, &pXmlHttp->BlockingOpsThreadCrit);

        /* DirtyConditionWait() internally re-enters the crit when it unblocks.
           Leave that crit explicitly here since the following activity is already
           thread-safe as the main thread is in "wait" state until this thread
           marks the operation complete */ 
        NetCritLeave(&pXmlHttp->BlockingOpsThreadCrit);

        switch (pXmlHttp->eBlockingOpsThreadCmd)
        {
            case THREAD_CMD_XMLHTTP_CREATE:
                iResult = _ProtoHttpOffloadedIXMLHTTPRequest2Creation(pXmlHttp);
                break;

            case THREAD_CMD_OUTSTREAM_WRITE:
                iResult = _ProtoHttpOffloadedOutStreamWrite(pXmlHttp);
                break;

            case THREAD_CMD_XMLHTTP_OPEN:
                iResult = _ProtoHttpOffloadedXmlHttpOpen(pXmlHttp);
                break;

            case THREAD_CMD_XMLHTTP_SETUP:
                iResult = _ProtoHttpOffloadedXmlHttpSetup(pXmlHttp);
                break;

            case THREAD_CMD_XMLHTTP_SEND:
                iResult = _ProtoHttpOffloadedXmlHttpSend(pXmlHttp);
                break;

            case THREAD_CMD_CLEANUP:
                iResult = _ProtoHttpOffloadedXmlHttpCleanup(pXmlHttp);
                break;

            case THREAD_CMD_EXIT:
                iResult = 0;
                NetPrintfVerbose((pXmlHttp->HttpRefDataCache.iVerbose, 3, "protohttpxboxone: [%p][%p] thread used for offloading long blocking MS calls is exiting\n", 
                    pXmlHttp->pHttpRef, pXmlHttp));
                bExit = TRUE;
                break;

            default:
                NetPrintf(("protohttpxboxone: [%p][%p] unknown cmd (%d) passed to thread used for offloading long blocking MS calls\n",
                    pXmlHttp->pHttpRef, pXmlHttp, pXmlHttp->eBlockingOpsThreadCmd));
                iResult = -1;
                break;
        }

        /* To avoid the possible race condition where the main thread would call DirtyConditionSignal()
           before we have a chance to call DirtyConditionWait(), we enter the crit before signaling the
           current operation is complete. Because DirtyConditionWait() internally releases the crit 
           before sleeping, we now have (1) ThreadCmd=THREAD_CMD_NONE and (2) initiate wait on signal
           both performed atomically. */
        NetCritEnter(&pXmlHttp->BlockingOpsThreadCrit);

        // let the main thread know that blocking operation is complete
        pXmlHttp->iBlockingOpsThreadResult = iResult;
        pXmlHttp->eBlockingOpsThreadCmd = THREAD_CMD_NONE;
    }

    // leave the crit entered before the while or at the end of the last loop
    NetCritLeave(&pXmlHttp->BlockingOpsThreadCrit);

    // to avoid crit being released while used, only signal that thread is not alive when the crit is no longer in use
    pXmlHttp->eBlockingOpsThreadLife = ProtoHttpXmlHttpT::THREAD_LIFE_STATUS_NOTALIVE;
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpCreatingThread

    \Description
        Async creation of the dedicated thread used for offloading long blocking
        MS calls

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpCreatingThread(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            int32_t iResult;
            DirtyThreadConfigT BlockingOpsThreadConfig;

            pXmlHttp->bWaitingForAsyncCompletion = TRUE;

            // initiate the creation of the dedicated thread used for offloading long blocking MS calls
            ds_memclr(&BlockingOpsThreadConfig, sizeof(BlockingOpsThreadConfig));
            BlockingOpsThreadConfig.pName = "ProtoHttpXBoxOneBlockingOpsThread";
            BlockingOpsThreadConfig.iAffinity = NetConnStatus('affn', 0, NULL, 0);
            BlockingOpsThreadConfig.iPriority = THREAD_PRIORITY_NORMAL;
            pXmlHttp->eBlockingOpsThreadLife = ProtoHttpXmlHttpT::THREAD_LIFE_STATUS_NOTALIVE;
            if ((iResult = DirtyThreadCreate(_ProtoHttpBlockingOpsThread, pXmlHttp, &BlockingOpsThreadConfig)) != 0)
            {
                NetPrintf(("protohttpxboxone: [%p][%p] error creating thread used for offloading long blocking MS calls (err=%d)\n", pHttpRef, pXmlHttp, iResult));
                pXmlHttp->bWaitingForAsyncCompletion = FALSE;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_STOPPING_THREAD);
        }
    }
    else
    {
        // wait until the thread confirms it is alive
        if (pXmlHttp->eBlockingOpsThreadLife == ProtoHttpXmlHttpT::THREAD_LIFE_STATUS_ALIVE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CREATING_XMLHTTPREQ2);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpCmdOffload()

    \Description
        Let the dedicated thread used for offloading long blocking
        MS calls that it is time to wake up and executed the specified
        operation.

    \Input *pXmlHttp    - XmlHttp object
    \Input eCmd         - command to be executed

    \Output
        int32_t         - 0 if success, negative otherwise

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpXmlHttpCmdOffload(ProtoHttpXmlHttpT *pXmlHttp, ProtoHttpBlockingOpsCmdE eCmd)
{
#if DIRTYCODE_LOGGING
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;
#endif
    int32_t iResult = 0;    // default to success

    NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p][%p] asking dedicated thread to execute cmd %s\n", pHttpRef, pXmlHttp, _ProtoHttp_strThreadCmds[eCmd]));

    // initiate the creation of the IXMLHTTPRequest2 object
    pXmlHttp->eBlockingOpsThreadCmd = eCmd;
    pXmlHttp->iBlockingOpsThreadResult = 0;
    NetCritEnter(&pXmlHttp->BlockingOpsThreadCrit);
    if (!DirtyConditionSignal(pXmlHttp->pBlockingOpsThreadCond))
    {
        NetPrintf(("protohttpxboxone: [%p][%p] failed to wake up thread for execution of cmd %s\n", pHttpRef, pXmlHttp, _ProtoHttp_strThreadCmds[eCmd]));
        iResult = -1;
    }
    NetCritLeave(&pXmlHttp->BlockingOpsThreadCrit);

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpCreatingXMLHTTPReq2

    \Description
        Async creation of the IXMLHTTPRequest2 object.

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpCreatingXMLHTTPReq2(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = TRUE;

            // initiate the creation of the IXMLHTTPRequest2 object
            if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_XMLHTTP_CREATE) < 0)
            {
                pXmlHttp->bWaitingForAsyncCompletion = FALSE;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;

            // check if offloaded operation succeeded
            if (pXmlHttp->iBlockingOpsThreadResult == 0)
            {
                // complete initialization of resources created by the dedicated thread
                pXmlHttp->cpOutStream->SetProtoHttpRef(pHttpRef);
                pXmlHttp->cpXmlHttpCallback->SetXmlHttp(pXmlHttp);

                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_XMLHTTPREQ2_CREATED);
            }
            else
            {
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpWritingToOutStream

    \Description
        Async write to out stream object

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpWritingToOutStream(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = TRUE;

            // initiate write operation on out stream
            if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_OUTSTREAM_WRITE) < 0)
            {
                pXmlHttp->bWaitingForAsyncCompletion = FALSE;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;

            // check if offloaded operation succeeded
            if (pXmlHttp->iBlockingOpsThreadResult >= 0)
            {
                int32_t iBytesWritten = pXmlHttp->iBlockingOpsThreadResult;

                #if DIRTYCODE_LOGGING
                if (pHttpRef->iVerbose > 2)
                {
                    NetPrintf(("protohttpxboxone: [%p] sent %d bytes\n", pHttpRef, iBytesWritten));
                }
                if (pHttpRef->iVerbose > 3)
                {
                    NetPrintMem(pXmlHttp->RequestBuffer.pInpBuf, iBytesWritten, "http-send");
                }
                #endif

                // sent data, so update timeout
                pHttpRef->uTimer = NetTick() + pHttpRef->uTimeout;

                // eliminate consumed bytes from pInpBuf
                if (iBytesWritten > 0)
                {
                    pXmlHttp->RequestBuffer.iInpLen -= iBytesWritten;  // update bytes left
                    memmove(pXmlHttp->RequestBuffer.pInpBuf, pXmlHttp->RequestBuffer.pInpBuf + iBytesWritten, pXmlHttp->RequestBuffer.iInpLen);
                }

                // move out of REQ_ST_WRITING_OUTSTREAM only when there is no longer any data to be written
                if (pXmlHttp->RequestBuffer.iInpLen <= 0)
                {
                    _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_ACCUMULATING_DATA);
                }
            }
            else
            {
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpOpeningRequest

    \Description
        Async opening of the request

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpOpeningRequest(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = TRUE;

            // initiate open
            if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_XMLHTTP_OPEN) < 0)
            {
                pXmlHttp->bWaitingForAsyncCompletion = FALSE;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;

            // check if offloaded operation succeeded
            if (pXmlHttp->iBlockingOpsThreadResult == 0)
            {
                // after following open call, request must complete before deletion of the xmlhttp object can take place
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CONFIGURING_REQ);
            }
            else if ((pXmlHttp->iBlockingOpsThreadResult > 0) && (pHttpRef->bRetriedOpen == FALSE))
            {
                NetPrintf(("protohttpxboxone: [%p][%p] failed to open request; executing re-try workaround\n", pHttpRef, pXmlHttp));
                pXmlHttp->RequestBuffer.iInpLen = pHttpRef->iOldInpLen;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_XMLHTTPREQ2_CREATED);
                pHttpRef->bRetriedOpen = TRUE;
            }
            else
            {
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpConfiguringRequest

    \Description
        Async setting up of the request

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpConfiguringRequest(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = TRUE;

            // initiate configuring the http request
            if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_XMLHTTP_SETUP) < 0)
            {
                pXmlHttp->bWaitingForAsyncCompletion = FALSE;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;

            // check if offloaded operation succeeded
            if (pXmlHttp->iBlockingOpsThreadResult == 0)
            {
                // count the attempt
                pHttpRef->iKeepAlive += 1;

                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_PRESEND);
            }
            else
            {
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpProcessingPreSendBuf

    \Description
        While in this state, we write the request data that was implicitly saved
        in the pre-send buffer when ProtoHttpPost(), ProtoHttpPut() or ProtoHttpPatch()
        was called.

    \Input *pXmlHttp        - XmlHttp object

    \Version 02/06/2019 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpProcessingPreSendBuf(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;
    int32_t iPreSendBufferBytesLeft = pXmlHttp->iPreSendBufferValidBytes - pXmlHttp->iPreSendBufferConsumedBytes;

    if (pXmlHttp->bInKillList == FALSE)
    {
        // if there is data in the pre-send buffer, make sure it gets sent before any additional data the user submits with ProtoHttpSend()
        if ((pXmlHttp->pPreSendBuffer != NULL) && (iPreSendBufferBytesLeft != 0))
        {
            int32_t iDataSent;

            // internally buffer request data (if there is such data)
            if ((iDataSent = _ProtoHttpSaveRequestData(pHttpRef, pXmlHttp->pPreSendBuffer + pXmlHttp->iPreSendBufferConsumedBytes, iPreSendBufferBytesLeft)) > 0)
            {
                pXmlHttp->iPreSendBufferConsumedBytes += iDataSent;
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_WRITING_OUTSTREAM);
            }
            else if (iDataSent < 0)
            {
                NetPrintf(("protohttpxboxone: [%p][%p] failed to save request data from pre-send buffer\n", pHttpRef, pXmlHttp));
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
            else
            {
                // _ProtoHttpSaveRequestData() is never supposed to return 0 when the iDataSize input param is not 0
                NetPrintf(("protohttpxboxone: [%p][%p] unexpected behavior when saving request data from pre-send buffer\n", pHttpRef, pXmlHttp));
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
        else
        {   // pre-send buffer is empty, move on to accumulating data from calls to ProtoHttpSend()
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_ACCUMULATING_DATA);
        }
    }
    else
    {
        _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpSendingRequest

    \Description
        Async sending of the request

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpSendingRequest(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        pXmlHttp->bWaitingForAsyncCompletion = TRUE;

        /* Before firing the http request, we have to initialize eNextState because later, 
           while in REQ_ST_WAITING_RESPONSE state, we will be waiting for eNextState to be 
           overriden by one of the xmlhttp event handlers to detect that we can stop waiting. */
        pXmlHttp->eNextState = ProtoHttpXmlReqStateE::REQ_ST_INIT;

        // initiate firing the http request
        if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_XMLHTTP_SEND) < 0)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;

            // check if offloaded operation succeeded
            if (pXmlHttp->iBlockingOpsThreadResult == 0)
            {
                // it's time to wait for the send result
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_WAITING_RESPONSE);
            }
            else
            {
                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpWaitingResponse

    \Description
        Waiting for a xmlhttp object state change to be triggered
        from the event handlers executed in MS worker threads

    \Input *pXmlHttp        - XmlHttp object

    \Version 11/09/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpWaitingResponse(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    // check if a new state has been selected as a result of one of the xmlhttp object event handlers being exercised
    if (pXmlHttp->eNextState != ProtoHttpXmlReqStateE::REQ_ST_INIT)
    {
        _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, pXmlHttp->eNextState);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpCleaningUp

    \Description
        Async cleanup of the xmlttp object and the related resources. ** Only when
        xmlhttp object in is killlist. **

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpCleaningUp(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        pXmlHttp->bWaitingForAsyncCompletion = TRUE;

        // initiate cleanup
        if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_CLEANUP) < 0)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;
        }
    }
    else
    {
        // check if thread is done with offloaded operation
        if (pXmlHttp->eBlockingOpsThreadCmd == THREAD_CMD_NONE)
        {
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_STOPPING_THREAD);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpXmlHttpStoppingThread

    \Description
        Async cleanup of the xmlttp object and the related resources. ** Only when
        xmlhttp object in is killlist. **

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpXmlHttpStoppingThread(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    // let the thread used for offloading long blocking MS calls know that is time to exit
    if (pXmlHttp->bWaitingForAsyncCompletion == FALSE)
    {
        if (_ProtoHttpXmlHttpCmdOffload(pXmlHttp, THREAD_CMD_EXIT) < 0)
        {
            NetPrintf(("protohttpxboxone: [%p][%p] signaling the dedicated thread to exit failed\n", pHttpRef, pXmlHttp));
        }
        else
        {
            pXmlHttp->bWaitingForAsyncCompletion = TRUE;
        }
    }
    else
    {
        if (pXmlHttp->eBlockingOpsThreadLife == ProtoHttpXmlHttpT::THREAD_LIFE_STATUS_NOTALIVE)
        {
            // thread is gone! go away for good... 
            pXmlHttp->bWaitingForAsyncCompletion = FALSE;
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_BYE);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateXmlHttp

    \Description
        Pumps state machine transition for specified xmlhttp object.

    \Input *pXmlHttp        - XmlHttp object

    \Version 09/19/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateXmlHttp(ProtoHttpXmlHttpT *pXmlHttp)
{
    ProtoHttpRefT *pHttpRef = pXmlHttp->pHttpRef;

    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_INIT)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CREATING_THREAD);
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_BYE);
        }
    }

    // async creation of the dedicated thread used for offloading long blocking MS calls to become alive
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_CREATING_THREAD)
    {
        _ProtoHttpXmlHttpCreatingThread(pXmlHttp);
    }

    // async creation of the MS XMLHTTPReq2 object
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_CREATING_XMLHTTPREQ2)
    {
        _ProtoHttpXmlHttpCreatingXMLHTTPReq2(pXmlHttp);
    }

    // MS XMLHTTPReq2 object is created
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_XMLHTTPREQ2_CREATED)
    {
        if (pXmlHttp->bInKillList != FALSE)
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }

    // async buffering of data into the output stream
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_WRITING_OUTSTREAM)
    {
        _ProtoHttpXmlHttpWritingToOutStream(pXmlHttp);
    }

    // async opening of req
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_OPENING_REQ)
    {
        _ProtoHttpXmlHttpOpeningRequest(pXmlHttp);
    }

    // async initialization of req
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_CONFIGURING_REQ)
    {
        _ProtoHttpXmlHttpConfiguringRequest(pXmlHttp);
    }

    // processing pre-send buffer
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_PRESEND)
    {
        _ProtoHttpXmlHttpProcessingPreSendBuf(pXmlHttp);
    }

    // waiting for more data to send
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_ACCUMULATING_DATA)
    {
        if (pXmlHttp->bInKillList != FALSE)
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }

    // async sending of req
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_SENDING_REQ)
    {
        _ProtoHttpXmlHttpSendingRequest(pXmlHttp);
    }

    // waiting for response
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_WAITING_RESPONSE)
    {
        _ProtoHttpXmlHttpWaitingResponse(pXmlHttp);
    }

    // failure detection
    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_FAILED)
    {
        if (pXmlHttp->bInKillList == FALSE)
        {
            if (pHttpRef->critical.eRequestState != ProtoHttpStateE::ST_FAIL)
            {
                _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
            }
        }
        else
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }

    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_COMPLETE)
    {
        if (pXmlHttp->bInKillList != FALSE)
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_CLEANINGUP);
        }
    }

    // from here, next states are sunset states (while in kill list)

    if (pXmlHttp->eState == REQ_ST_CLEANINGUP)
    {
        _ProtoHttpXmlHttpCleaningUp(pXmlHttp);
    }

    if (pXmlHttp->eState == REQ_ST_STOPPING_THREAD)
    {
        _ProtoHttpXmlHttpStoppingThread(pXmlHttp);
    }

    if (pXmlHttp->eState == REQ_ST_BYE)
    {
        /* A timer is used to block the state machine in state REQ_ST_BYE for a minimal duration before
           proceeding with final destruction. This delay is required to eliminate intermittent prod crashes
           reported by game teams and caused by OnError() firing after the XmlHttpCallback COM object
           has been released. */
        if (NetTickDiff(NetTick(), pXmlHttp->iByeSafetyDelayStart) > PROTOHTTP_DESTROY_TIMEOUT)
        {
            _ProtoHttpRemoveFromKillList(pHttpRef, pXmlHttp);
            _ProtoHttpXmlHttpDestroy(pXmlHttp);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpProcessKillList

    \Description
        Process kill list.

    \Input *pHttpRef    - module state pointer
    \Input bModuleDestroyed - are we destroying the module?

    \Version 11/11/2016 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpProcessKillList(ProtoHttpRefT *pHttpRef, uint8_t bModuleDestroyed)
{
    ProtoHttpXmlHttpT *pCurrent, *pNext;

    for (pCurrent = pHttpRef->pXmlHttpKillList; pCurrent != NULL; pCurrent = pNext)
    {
        pNext = pCurrent->pNext;  // save pNext right away as pCurrent may be freed and removed from the list in the next call

        // special case to deal with cases where the MS completions notifications are never fired
        if (_ProtoHttpIsXmlHttpActivityInProgress(pCurrent))
        {
            if (bModuleDestroyed && (NetTickDiff(NetTick(), pHttpRef->iDestroyStartTime) > PROTOHTTP_DESTROY_TIMEOUT))
            {
                if (pCurrent->bGracePeriodEntered || !NetConnStatus('ncon', 0, NULL, 0))
                {
                    if (pCurrent->bWaitingForAsyncCompletion == FALSE)
                    {
                        NetPrintf(("protohttpxboxone: [%p][%p] a suspend invalidated operation has exceeded its timeout, forcing it to failure state to avoid resources leaking\n", pHttpRef, pHttpRef->pXmlHttpKillList));
                        _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pHttpRef->pXmlHttpKillList, ProtoHttpXmlReqStateE::REQ_ST_FAILED);
                    }
                }
            }
        }

        /* update the xml http object to allow it to process its state machine. NOTE: this must come last,
           as this function call will invalidate pCurrent after REQ_ST_BYE timeout expires. */
        _ProtoHttpUpdateXmlHttp(pCurrent);
    }
}

/*F*************************************************************************************************/
/*!
    \Function _ProtoHttpDestroyCallback

    \Description
        Callback registered with NetConn for deferred destruction occurring only when 
        no requests are left in flight.

    \Input *pRef        - module state pointer

    \Output
        int32_t         - zero = success; -1 = try again; other negative = error

    \Version 10/03/2013 (mclouatre)
*/
/*************************************************************************************************F*/
static int32_t _ProtoHttpDestroyCallback(void *pRef)
{
    ProtoHttpRefT *pHttpRef = (ProtoHttpRefT *)pRef;

    // keep returning "try again" until kill list is empty
    _ProtoHttpProcessKillList(pHttpRef, TRUE);
    if (pHttpRef->pXmlHttpKillList != NULL)
    {
        return(-1);
    }

    _ProtoHttpDestroy(pHttpRef);

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpPrepareHeader

    \Description
        Formats the request header and handles the result

    \Input *pHttpRef        - module state pointer

    \Output
        int32_t             - negative=error, else zero

    \Version 09/05/2017 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _ProtoHttpPrepareHeader(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    char strTempBuf[8 * 1024];
    int32_t iInpLen, iResult;

    // save input length (url); we use this if we need to retry
    iInpLen = pXmlHttp->RequestBuffer.iInpLen;

    // format request header
    if ((iResult = _ProtoHttpFormatRequestHeader(pHttpRef, strTempBuf, sizeof(strTempBuf))) < 0)
    {
        if (iResult == PROTOHTTP_MINBUFF)
        {
            // resize buffer
            _ProtoHttpResizeBufferAuto(pHttpRef, pXmlHttp->RequestBuffer.iInpOvr);

            // reset and try again
            pXmlHttp->RequestBuffer.iInpLen = iInpLen;
            iResult = _ProtoHttpFormatRequestHeader(pHttpRef, strTempBuf, sizeof(strTempBuf));
        }
        if (iResult < 0)
        {
            return(iResult);
        }
    }

    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] sending request:\n", pHttpRef, pHttpRef->critical.pXmlHttp));
    if (pHttpRef->iVerbose > 1)
    {
        NetPrintWrap(pXmlHttp->RequestBuffer.pInpBuf, 132);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpDestroyActiveRequest

    \Description
        Destroy XmlHttp object; if the object is still in an active state,
        it gets added to the killlist instead.

    \Input *pHttpRef        - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpDestroyActiveRequest(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;

    if (pXmlHttp != NULL)
    {
        _ProtoHttpAddToKillList(pHttpRef, pXmlHttp);

        pHttpRef->critical.pXmlHttp = pXmlHttp = NULL;
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpReset

    \Description
        Reset module state for a new transaction.

    \Input *pHttpRef      - module state

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoHttpReset(ProtoHttpRefT *pHttpRef)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p] proceeding with httpref reset...\n", pHttpRef));

    // reset module state data
    pHttpRef->iRecvSize = 0;

    // reset request and response header
    ds_memclr(&pHttpRef->strRequestHdr, sizeof(pHttpRef->strRequestHdr));
    ds_memclr(&pHttpRef->responseHeader, sizeof(pHttpRef->responseHeader));
    pHttpRef->responseHeader.iBodySize = -1;

    // reset critical data
    _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_IDLE);
    pHttpRef->critical.iNumRedirect = 0;
    pHttpRef->critical.iNumRedirProc = 0;
    pHttpRef->critical.iRequestSize = 0;
    pHttpRef->critical.bBodyReceived = FALSE;
    pHttpRef->critical.bTimeout = FALSE;
    pHttpRef->critical.bHeaderReceived = FALSE;

    // reset 302/303 redirection workaround bug info
    pHttpRef->critical.bRedirBugWorkaround = FALSE;
    pHttpRef->strLocation[0] = '\0';

    // OR_INVALID_OXID bug workaround
    pHttpRef->bRetriedOpen = FALSE;

    // invalidate request-specific callbacks
    pHttpRef->pWriteCb = NULL;
    pHttpRef->pReqCustomHeaderCb = NULL;
    pHttpRef->pReqReceiveHeaderCb = NULL;
    pHttpRef->pUserData = NULL;
};

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_AUTH

    \Description
        Performs ST_AUTH state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_AUTH(ProtoHttpRefT *pHttpRef)
{
    int32_t iResult;

    if ((iResult = DirtyAuthCheckToken(pHttpRef->iAuthTokenIdx, pHttpRef->strTokenUrl, NULL, NULL)) == DIRTYAUTH_SUCCESS)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: token acquired; transitioning to connect state\n"));
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_CONN);
    }
    else if (iResult == DIRTYAUTH_PENDING)
    {
        return;
    }
    else
    {
        NetPrintf(("protohttpxboxone: token acquisition failed\n"));
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_CONN

    \Description
        Performs ST_CONN state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_CONN(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;

    // wait until we have a valid xmlhttp object
    if (pXmlHttp != NULL)
    {
        // here httpref state transition is driven by xmlhttp object state
        if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_XMLHTTPREQ2_CREATED)
        {
            if (_ProtoHttpPrepareHeader(pHttpRef) >= 0)
            {
                // save input length (url); we use this if we need to retry
                pHttpRef->iOldInpLen = pXmlHttp->RequestBuffer.iInpLen;

                _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_OPENING_REQ);
            }
            else
            {
                _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
            }
        }

        if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_ACCUMULATING_DATA)
        {
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_RESP);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_RESP

    \Description
        Performs ST_RESP state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_RESP(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;

    if (pXmlHttp->eState == ProtoHttpXmlReqStateE::REQ_ST_ACCUMULATING_DATA)
    {
        // has all the request data been fully accumulated into the output stream object?
        if (pHttpRef->critical.pXmlHttp->cpOutStream.Get()->GetDataSize() == pHttpRef->critical.iRequestSize)
        {
            // the xmlhttp object is ready to send, let's do it!

            // reset RequestBuffer now because it is re-used for response handling in HttpCallbacks
            pXmlHttp->RequestBuffer.iInpLen = 0;
            pXmlHttp->RequestBuffer.iInpReadLen = 0;

            // set connection timeout
            pHttpRef->uTimer = NetTick() + pHttpRef->uTimeout;

            // beyond this point, the xmlhttp request is "in progress", i.e. it must complete (or fail) before deletion of the xmlhttp object can take place
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_SENDING_REQ);
        }
    }

    // process redirect headers, if any
    for (; _ProtoHttpProcessRedirect(pHttpRef) > 0; )
        ;

    // implement 302/303 redirection bug workaround
    if (pHttpRef->critical.bRedirBugWorkaround)
    {
        NetPrintf(("protohttpxboxone: [%p] executing redirection bug workaround\n", pHttpRef));

        // clean up xmlhttp object
        _ProtoHttpDestroyActiveRequest(pHttpRef);

        // change request to a GET and data size to zero
        pHttpRef->eRequestType = PROTOHTTP_REQUESTTYPE_GET;
        pHttpRef->critical.iRequestSize = 0;

        // create new XmlHttp object for this request
        if ((pHttpRef->critical.pXmlHttp = _ProtoHttpXmlHttpCreate(pHttpRef)) == NULL)
        {
            NetPrintf(("protohttpxboxone: [%p] failed to create new xmlhttp object\n", pHttpRef));
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
            return;
        }

        // format request url
        _ProtoHttpFormatUrl(pHttpRef, pHttpRef->strLocation);

        // clear workaround flag and location url to prevent re-issue
        pHttpRef->critical.bRedirBugWorkaround = FALSE;
        pHttpRef->strLocation[0] = '\0';

        // re-save some state info initialized in _ProtoHttpXmlHttpCreate() but overrriden in _ProtoHttpFormatUrl()
        pHttpRef->critical.pXmlHttp->HttpRefDataCache.iPort = pHttpRef->iPort;
        pHttpRef->critical.pXmlHttp->HttpRefDataCache.iSecure = pHttpRef->iSecure;
        ds_strnzcpy(pHttpRef->critical.pXmlHttp->HttpRefDataCache.strHost, pHttpRef->strHost, sizeof(pHttpRef->critical.pXmlHttp->HttpRefDataCache.strHost));


        // move to conn state to issue redirect request
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_CONN);
        return;
    }

    // wait until we've received headers
    if (pHttpRef->critical.bHeaderReceived == FALSE)
    {
        return;
    }

    // move to process header
    _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_HEAD);

}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_HEAD

    \Description
        Performs ST_HEAD state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_HEAD(ProtoHttpRefT *pHttpRef)
{
    // parse the header
    if (_ProtoHttpParseHeader(pHttpRef) < 0)
    {
        NetPrintf(("protohttpxboxone: [%p] failed to parse header\n", pHttpRef));
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
        return;
    }

    // move to process body
    _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_BODY);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_BODY

    \Description
        Performs ST_BODY state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_BODY(ProtoHttpRefT *pHttpRef)
{
    if (pHttpRef->critical.bBodyReceived)
    {
        // finished receiving response
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_DONE);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_DONE

    \Description
        Performs ST_DONE state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_DONE(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;

    // if we didn't get a body length with the header, set it now
    if (pHttpRef->responseHeader.iBodySize == -1)
    {
        pHttpRef->responseHeader.iBodySize = pXmlHttp->RequestBuffer.iInpTotal;
    }

    #if 0
    /* We can no longer release the active request here because data
       can still be extracted from it with ProtoHttpRecv() beyond that point. 
       It will be released later upon ProtoHttpAbort() being called explicitly,
       or upon an implicit abort from a new request being initiated, or upon the
       module being destroyed. */
    _ProtoHttpDestroyActiveRequest(pHttpRef);
    #endif
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest_FAIL

    \Description
        Performs ST_FAIL state activity for httpref's active request.

    \Input *pHttpRef    - module state pointer

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest_FAIL(ProtoHttpRefT *pHttpRef)
{
    _ProtoHttpDestroyActiveRequest(pHttpRef);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpUpdateActiveRequest

    \Description
        Pumps state machine transition for active http request. The ProtoHttp
        module supports only one active request at a time.

    \Input *pHttpRef    - module state pointer

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
static void _ProtoHttpUpdateActiveRequest(ProtoHttpRefT *pHttpRef)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;

    if (!NetCritTry(&pHttpRef->critical.Locker))
    {
        return;
    }

    // check for timeout
    if ((pHttpRef->critical.eRequestState != ProtoHttpStateE::ST_IDLE) && (pHttpRef->critical.eRequestState != ProtoHttpStateE::ST_DONE) && (pHttpRef->critical.eRequestState != ProtoHttpStateE::ST_FAIL))
    {
        if (NetTickDiff(NetTick(), pHttpRef->uTimer) >= 0)
        {
            NetPrintf(("protohttpxboxone: [%p] server timeout (state=%s)\n", pHttpRef, _ProtoHttp_strRequestState[pHttpRef->critical.eRequestState]));
            // set to failure state and mark that a timeout occurred
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_FAIL);
            pHttpRef->critical.bTimeout = TRUE;
        }
    }

    /* pump state machine transition for xmlhttp object associated with the
       active request and monitor failure state */
    if (pXmlHttp != NULL)
    {
        _ProtoHttpUpdateXmlHttp(pXmlHttp);
    }

    // waiting for auth token
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_AUTH)
    {
        _ProtoHttpUpdateActiveRequest_AUTH(pHttpRef);
    }

    // issue request
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_CONN)
    {
        _ProtoHttpUpdateActiveRequest_CONN(pHttpRef);
    }

    // wait for response header
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_RESP)
    {
        _ProtoHttpUpdateActiveRequest_RESP(pHttpRef);
    }

    // process received header
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_HEAD)
    {
        _ProtoHttpUpdateActiveRequest_HEAD(pHttpRef);
    }

    // process received body
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_BODY)
    {
        _ProtoHttpUpdateActiveRequest_BODY(pHttpRef);
    }

    // cleanup active request if required
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_DONE)
    {
        _ProtoHttpUpdateActiveRequest_DONE(pHttpRef);
    }

    // cleanup active request if required
    if (pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_FAIL)
    {
        _ProtoHttpUpdateActiveRequest_FAIL(pHttpRef);
    }

    // write callback processing
    if (pHttpRef->pWriteCb != NULL)
    {
        _ProtoHttpWriteCbProcess(pHttpRef);
    }

    NetCritLeave(&pHttpRef->critical.Locker);
}

/*F********************************************************************************/
/*!
    \Function _ProtoHttpAbort

    \Description
        Abort active request, if any.

    \Input *pHttpRef    - module state pointer

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
static void _ProtoHttpAbort(ProtoHttpRefT *pHttpRef)
{
    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] abort\n", pHttpRef, pHttpRef->critical.pXmlHttp));

    _ProtoHttpDestroyActiveRequest(pHttpRef);

    _ProtoHttpReset(pHttpRef);
}

/*** Public Functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function ProtoHttpCreate

    \Description
        Allocate module state and prepare for use

    \Input iBufSize     - length of receive buffer

    \Output
        ProtoHttpRefT * - pointer to module state, or NULL

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
ProtoHttpRefT *ProtoHttpCreate(int32_t iBufSize)
{
    ProtoHttpRefT *pHttpRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // clamp the buffer size
    if (iBufSize < 4*1024)
    {
        iBufSize = 4*1024;
    }

    // allocate the resources
    if ((pHttpRef = (ProtoHttpRefT *)DirtyMemAlloc(sizeof(*pHttpRef), PROTOHTTP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p] unable to allocate module state\n", pHttpRef));
        return(NULL);
    }
    ds_memclr(pHttpRef, sizeof(*pHttpRef));

    // initialize mutex
    NetCritInit(&pHttpRef->critical.Locker, "protohttpxboxone");

    // save memgroup (will be used in ProtoHttpDestroy)
    pHttpRef->iMemGroup = iMemGroup;
    pHttpRef->pMemGroupUserData = pMemGroupUserData;

    // initialize other settings
    pHttpRef->bAuthenticate = TRUE;
    pHttpRef->responseHeader.iHdrCode = -1;
    pHttpRef->iVerbose = 1;
    pHttpRef->iOnDataThreshold = 64*1024;

    // save parms & set defaults
    pHttpRef->critical.RequestBufferParams.iInpMax = iBufSize;
    pHttpRef->critical.RequestBufferParams.iInpMaxIdeal = iBufSize;
    pHttpRef->uTimeout = PROTOHTTP_TIMEOUT_DEFAULT;
    pHttpRef->iMaxRedirect = PROTOHTTP_MAXREDIRECT;
    pHttpRef->iAuthTokenIdx = -1;

    NetPrintf(("protohttpxboxone: [%p] creation successful\n", pHttpRef));

    // return the state
    return(pHttpRef);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCallback

    \Description
        Set header callbacks.

    \Input *pHttpRef          - module state
    \Input *pCustomHeaderCb - pointer to custom send header callback (may be NULL)
    \Input *pReceiveHeaderCb- pointer to recv header callback (may be NULL)
    \Input *pCallbackRef    - user-supplied callback ref (may be NULL)

    \Notes
        The ProtHttpCustomHeaderCbt callback is used to allow customization of
        the HTTP header before sending.  It is more powerful than the append
        header functionality, allowing to make changes to any part of the header
        before it is sent.  The callback should return a negative code if an error
        occurred, zero can be returned if the application wants ProtoHttp to
        calculate the header size, or the size of the header can be returned if
        the application has already calculated it.  The header should *not* be
        terminated with the double \\r\\n that indicates the end of the entire
        header, as protohttp appends itself.

        The ProtoHttpReceiveHeaderCbT callback is used to view the header
        immediately on reception.  It can be used when the built-in header
        cache (retrieved with ProtoHttpStatus('htxt') is too small to hold
        the entire header received.  It is also possible with this method
        to view redirection response headers that cannot be retrieved
        normally.  This can be important if, for example, the application
        wishes to attach new cookies to a redirection response.  The
        custom response header and custom header callback can be used in
        conjunction to implement this type of functionality.

    \Version 1.0 02/16/2007 (jbrookes)
*/
/********************************************************************************F*/
void ProtoHttpCallback(ProtoHttpRefT *pHttpRef, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pCallbackRef)
{
    pHttpRef->pCustomHeaderCb  = pCustomHeaderCb;
    pHttpRef->pReceiveHeaderCb = pReceiveHeaderCb;
    pHttpRef->pCallbackRef     = pCallbackRef;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDestroy

    \Description
        Destroy the module and release its state

    \Input *pHttpRef    - module state pointer

    \Version 07/12/2013 (jbrookes)
*/
/********************************************************************************F*/
void ProtoHttpDestroy(ProtoHttpRefT *pHttpRef)
{
    NetCritEnter(&pHttpRef->critical.Locker); 
    _ProtoHttpDestroyActiveRequest(pHttpRef);
    NetCritLeave(&pHttpRef->critical.Locker);

    // save the start time in the case we need to timeout any requests
    pHttpRef->iDestroyStartTime = NetTick();

    // attempt to destroy the module
    if (_ProtoHttpDestroyCallback(pHttpRef) == -1)
    {
        // schedule for destruction with netconn
        if (!NetConnControl('recu', 0, 0, (void *)_ProtoHttpDestroyCallback, pHttpRef))
        {
            NetPrintf(("protohttpxboxone: [%p] destruction deferred to netconn because not all xmlhttp objects have been destroyed yet\n", pHttpRef));
        }
        else
        {
            NetPrintf(("protohttpxboxone: [%p] unable to schedule _ProtoHttpDestroyCallback with netconn\n", pHttpRef));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGet

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a getting
        data from the appropriate server.

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - the url to retrieve
    \Input bHeadOnly    - if TRUE only get header

    \Output
        int32_t         - negative=failure, else success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpGet(ProtoHttpRefT *pHttpRef, const char *pUrl, uint32_t bHeadOnly)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, bHeadOnly ? PROTOHTTP_REQUESTTYPE_HEAD : PROTOHTTP_REQUESTTYPE_GET));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecv

    \Description
        Return the actual url data.

    \Input *pHttpRef    - module state pointer
    \Input *pBuffer     - buffer to store data in
    \Input iBufMin      - minimum number of bytes to return (returns zero if this number is not available)
    \Input iBufMax      - maximum number of bytes to return (buffer size)

    \Output
        int32_t         - negative=error, zero=no data available or bufmax <= 0, positive=number of bytes read

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpRecv(ProtoHttpRefT *pHttpRef, char *pBuffer, int32_t iBufMin, int32_t iBufMax)
{
    ProtoHttpXmlHttp *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iResult = 0;

    if (!NetCritTry(&pHttpRef->critical.Locker))
    {
        return(0);
    }

    if ((pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_FAIL) || (pXmlHttp == NULL) || (pXmlHttp->bInKillList == TRUE))
    {
        iResult = pHttpRef->critical.bTimeout ? PROTOHTTP_TIMEOUT : PROTOHTTP_RECVFAIL;
    }
    else if (pHttpRef->critical.eRequestState < ProtoHttpStateE::ST_BODY)
    {
        iResult = PROTOHTTP_RECVWAIT;
    }
    else
    {
        int32_t iBufferLen = pXmlHttp->RequestBuffer.iInpLen - pXmlHttp->RequestBuffer.iInpReadLen;

        if ((pHttpRef->critical.eRequestState == ProtoHttpStateE::ST_DONE) && (iBufferLen == 0))
        {
            iResult = PROTOHTTP_RECVDONE;
        }
        else
        {
            //$$TODO -- give a chance to get more data?

            if ((iBufMax > 0) && (iBufferLen >= iBufMin))
            {
                // clamp the range
                if (iBufMin < 1)
                    iBufMin = 1;
                if (iBufMax < iBufMin)
                    iBufMax = iBufMin;
                if (iBufMin > pHttpRef->critical.RequestBufferParams.iInpMax)
                    iBufMin = pHttpRef->critical.RequestBufferParams.iInpMax;
                if (iBufMax > pHttpRef->critical.RequestBufferParams.iInpMax)
                    iBufMax = pHttpRef->critical.RequestBufferParams.iInpMax;

                // copy data to caller
                iResult = iBufferLen <= iBufMax ? iBufferLen : iBufMax;
                ds_memcpy(pBuffer, pXmlHttp->RequestBuffer.pInpBuf + pXmlHttp->RequestBuffer.iInpReadLen, iResult);

                // track where to read the next time
                pXmlHttp->RequestBuffer.iInpReadLen += iResult;
                pXmlHttp->RequestBuffer.iInpReadTotal += iResult;

                // if we have read the entire buffer, then reset the input lengths to zero
                if (pXmlHttp->RequestBuffer.iInpReadLen == pXmlHttp->RequestBuffer.iInpLen)
                {
                    pXmlHttp->RequestBuffer.iInpLen = 0;
                    pXmlHttp->RequestBuffer.iInpReadLen = 0;
                }

                #if DIRTYCODE_LOGGING
                if (pHttpRef->iVerbose > 3)
                {
                    NetPrintf(("protohttpxboxone: [%p] read %d bytes\n", pHttpRef, iResult));
                }
                if (pHttpRef->iVerbose > 4)
                {
                    NetPrintMem(pBuffer, iResult, "http-read");
                }
                #endif
            }
        }
    }

    NetCritLeave(&pHttpRef->critical.Locker);
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRecvAll

    \Description
        Return all of the url data.

    \Input *pHttpRef    - module state pointer
    \Input *pBuffer     - buffer to store data in
    \Input iBufSize     - size of buffer

    \Output
        int32_t         - PROTOHTTP_RECV*, or positive=bytes in response

    \Version 12/14/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpRecvAll(ProtoHttpRefT *pHttpRef, char *pBuffer, int32_t iBufSize)
{
    int32_t iRecvMax, iRecvResult;

    // try to read as much data as possible (leave one byte for null termination)
    for (iRecvMax = iBufSize-1; (iRecvResult = ProtoHttpRecv(pHttpRef, pBuffer + pHttpRef->iRecvSize, 1, iRecvMax - pHttpRef->iRecvSize)) > 0; )
    {
        // add to amount received
        pHttpRef->iRecvSize += iRecvResult;
    }

    // check response code
    if (iRecvResult == PROTOHTTP_RECVDONE)
    {
        // null-terminate response & return completion success
        if ((pBuffer != NULL) && (iBufSize > 0))
        {
            pBuffer[pHttpRef->iRecvSize] = '\0';
        }
        iRecvResult = pHttpRef->iRecvSize;
    }
    else if ((iRecvResult < 0) && (iRecvResult != PROTOHTTP_RECVWAIT))
    {
        // an error occurred
        NetPrintf(("protohttpxboxone: [%p] error %d receiving response\n", pHttpRef, iRecvResult));
    }
    else if (iRecvResult == 0)
    {
        iRecvResult = (pHttpRef->iRecvSize < iRecvMax) ? PROTOHTTP_RECVWAIT : PROTOHTTP_RECVBUFF;
    }

    // return result to caller
    return(iRecvResult);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPost

    \Description
        Initiate transfer of data to to the server via a HTTP POST command.

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - the URL that identifies the POST action.
    \Input *pData       - pointer to URL data (optional, may be NULL)
    \Input iDataSize    - size of data being uploaded (see Notes below)
    \Input bDoPut       - meanless on xboxone

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Notes
        Any data that is not sent as part of the Post transaction should be sent
        with ProtoHttpSend().  ProtoHttpSend() should be called at poll rate (i.e.
        similar to how often ProtoHttpUpdate() is called) until all of the data has
        been sent.  The amount of data bytes actually sent is returned by the
        function.

        If pData is not NULL and iDataSize is less than or equal to zero, iDataSize
        will be recalculated as the string length of pData, to allow for easy string
        sending.

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpPost(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pData, int64_t iDataSize, uint32_t bDoPut)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, pData, iDataSize, bDoPut ? PROTOHTTP_REQUESTTYPE_PUT : PROTOHTTP_REQUESTTYPE_POST));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSend

    \Description
        Send data during an ongoing Post transaction.

    \Input *pHttpRef    - module state pointer
    \Input *pData       - pointer to data to send
    \Input iDataSize    - size of data being sent

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Version 07/15/2013 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpSend(ProtoHttpRefT *pHttpRef, const char *pData, int32_t iDataSize)
{
    ProtoHttpXmlHttpT *pXmlHttp = pHttpRef->critical.pXmlHttp;
    int32_t iDataSent = 0;
    uint32_t bEarlyExit = FALSE;

    /* Implicit call to ProtoHttpUpdate() required to support scenarios for which
       higher level code calls ProtoHttpSend() in a loop without calling ProtoHttpUpdate().
       (introduced for parity with other platforms)

       $$todo - After diligently announcing to the community, remove this implicit call to
       ProtoHttpUpdate() and force the client code to call ProtoHttpUpdate() explicitly - shall be
       enforced consistently on all platforms, and shall only be introduced in a major release
       where public api impacts are permitted. */
    ProtoHttpUpdate(pHttpRef);

    if (!NetCritTry(&pHttpRef->critical.Locker))
    {
        return(0);
    }

    if (pHttpRef->critical.eRequestState < ProtoHttpStateE::ST_RESP)
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p] calling send when not yet in send state (state=%s)\n", pHttpRef, _ProtoHttp_strRequestState[pHttpRef->critical.eRequestState]));
        bEarlyExit = TRUE;
    }
    else if (pHttpRef->critical.eRequestState != ProtoHttpStateE::ST_RESP)
    {
        NetPrintf(("protohttpxboxone: [%p] calling send when in invalid module state (state=%s)\n", pHttpRef, _ProtoHttp_strRequestState[pHttpRef->critical.eRequestState]));
        iDataSent = -1;
        bEarlyExit = TRUE;
    }

    if (bEarlyExit == FALSE)
    {
        // pXmlHttp may be NULL if bEarlyExit is TRUE
        switch (pXmlHttp->eState)
        {
            case REQ_ST_INIT:
            case REQ_ST_CREATING_THREAD:
            case REQ_ST_CREATING_XMLHTTPREQ2:
            case REQ_ST_XMLHTTPREQ2_CREATED:
            case REQ_ST_WRITING_OUTSTREAM:
            case REQ_ST_OPENING_REQ:
            case REQ_ST_CONFIGURING_REQ:
            case REQ_ST_PRESEND:
                // not yet in a state where request data can be buffered internally, try again
                bEarlyExit = TRUE;
                break;

            case REQ_ST_ACCUMULATING_DATA:
                break;

            default:
                NetPrintf(("protohttpxboxone: [%p] calling send when in invalid request state (state=%s)\n", pHttpRef, _ProtoHttp_strXmlHttpState[pXmlHttp->eState]));
                iDataSent = -2;
                bEarlyExit = TRUE;
                break;
        }
    }

    if (bEarlyExit == FALSE)
    {
        // buffer request data internally (if there is such data)
        if ((iDataSent = _ProtoHttpSaveRequestData(pHttpRef, pData, iDataSize)) > 0)
        {
            _ProtoHttpXmlHttpMoveToNextState(pHttpRef, pXmlHttp, ProtoHttpXmlReqStateE::REQ_ST_WRITING_OUTSTREAM);
        }
        else if (iDataSent < 0)
        {
            NetPrintf(("protohttpxboxone: [%p] ProtoHttpSend() - failed to save request data (%d bytes)\n", pHttpRef, iDataSize));
        }
    }

    NetCritLeave(&pHttpRef->critical.Locker);

    return(iDataSent);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpDelete

    \Description
        Request deletion of a server-based resource.

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - Url describing reference to delete

    \Output
        int32_t         - negative=failure, zero=success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpDelete(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_DELETE));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpOptions

    \Description
        Request options from a server.

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - Url describing reference to get options on

    \Output
        int32_t         - negative=failure, zero=success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpOptions(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    return(ProtoHttpRequest(pHttpRef, pUrl, NULL, 0, PROTOHTTP_REQUESTTYPE_OPTIONS));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpPatch

    \Description
        Initiate transfer of data to to the server via a HTTP PATCH command.

    \Input *pState      - reference pointer
    \Input *pUrl        - the URL that identifies the POST action.
    \Input *pData       - pointer to URL data (optional, may be NULL)
    \Input iDataSize    - size of data being uploaded (see Notes below)

    \Output
        int32_t         - negative=failure, else number of data bytes sent

    \Notes
        Any data that is not sent as part of the Patch transaction should be sent
        with ProtoHttpSend().  ProtoHttpSend() should be called at poll rate (i.e.
        similar to how often ProtoHttpUpdate() is called) until all of the data has
        been sent.  The amount of data bytes actually sent is returned by the
        function.

        If pData is not NULL and iDataSize is less than or equal to zero, iDataSize
        will be recalculated as the string length of pData, to allow for easy string
        sending.

        If pData is NULL and iDataSize is negative, the transaction is assumed to
        be a streaming transaction and a chunked transfer will be performed.  A
        subsequent call to ProtoHttpSend() with iDataSize equal to zero will end
        the transaction.

    \Version 01/01/2017 (amakoukji) First Version
*/
/********************************************************************************F*/
int32_t ProtoHttpPatch(ProtoHttpRefT *pState, const char *pUrl, const char *pData, int64_t iDataSize)
{
    return(ProtoHttpRequest(pState, pUrl, pData, iDataSize, PROTOHTTP_REQUESTTYPE_PATCH));
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpRequestCb2

    \Description
        Initiate an HTTP transfer. Pass in a URL and the module starts a transfer
        from the appropriate server.  Use ProtoHttpRequest() macro wrapper if
        callback is not required.

    \Input *pHttpRef            - module state pointer
    \Input *pUrl                - the url to retrieve
    \Input *pData               - user data to send to server (PUT, POST and PATCH only)
    \Input iDataSize            - size of user data to send to server (PUT, POST and PATCH only)
    \Input eRequestType         - request type to make
    \Input *pWriteCb            - write callback (optional)
    \Input *pCustomHeaderCb     - custom header callback (optional)
    \Input *pReceiveHeaderCb    - receive header callback (optional)
    \Input *pUserData           - callback user data (optional)

    \Output
        int32_t                 - >=0: amount of bytes processed, <0:=failure

    \Version 09/11/2017 (eesponda)
*/
/********************************************************************************F*/
int32_t ProtoHttpRequestCb2(ProtoHttpRefT *pHttpRef, const char *pUrl, const char *pData, int64_t iDataSize, ProtoHttpRequestTypeE eRequestType, ProtoHttpWriteCbT *pWriteCb, ProtoHttpCustomHeaderCbT *pCustomHeaderCb, ProtoHttpReceiveHeaderCbT *pReceiveHeaderCb, void *pUserData)
{
    int32_t iResult;

    // validate request type
    if (eRequestType >= PROTOHTTP_NUMREQUESTTYPES)
    {
        NetPrintf(("protohttpxboxone: [%p] invalid request type %d\n", pHttpRef, eRequestType));
        return(-1);
    }

    // validate URL
    if (pUrl == NULL)
    {
        NetPrintf(("protohttpxboxone: [%p] invalid url\n", pHttpRef));
        return(-2);
    }

    // ignore pData and iDataSize if not POST, PUT, PATCH
    if ((eRequestType != PROTOHTTP_REQUESTTYPE_POST) &&
        (eRequestType != PROTOHTTP_REQUESTTYPE_PUT) &&
        (eRequestType != PROTOHTTP_REQUESTTYPE_PATCH))
    {
        pData = NULL;
        iDataSize = 0;
    }

    // currently IXHR2 does not support chunked transfers
    if ((pData == NULL) && (iDataSize < 0))
    {
        NetPrintf(("protohttpxboxone: [%p] IXmlHttpRequest2 does not support chunked transfer(streaming), please provide a payload\n", pHttpRef));
        return(-3);
    }

    // make the request
    NetCritEnter(&pHttpRef->critical.Locker);

    // clean up previous XmlHttp object, if allocated
    _ProtoHttpAbort(pHttpRef);

    // save request info
    pHttpRef->eRequestType = eRequestType;

    // save callbacks
    pHttpRef->pWriteCb = pWriteCb;
    pHttpRef->pReqCustomHeaderCb = pCustomHeaderCb;
    pHttpRef->pReqReceiveHeaderCb = pReceiveHeaderCb;
    pHttpRef->pUserData = pUserData;

    // create new XmlHttp object for this request
    if ((pHttpRef->critical.pXmlHttp = _ProtoHttpXmlHttpCreate(pHttpRef)) == NULL)
    {
        NetCritLeave(&pHttpRef->critical.Locker);
        return(-4);
    }

    // format request url
    if (_ProtoHttpFormatUrl(pHttpRef, pUrl) < 0)
    {
        _ProtoHttpXmlHttpDestroy(pHttpRef->critical.pXmlHttp);
        pHttpRef->critical.pXmlHttp = nullptr;
        NetCritLeave(&pHttpRef->critical.Locker);
        return(-5);
    }

    // re-save some state info initialized in _ProtoHttpXmlHttpCreate() but overrriden in _ProtoHttpFormatUrl()
    pHttpRef->critical.pXmlHttp->HttpRefDataCache.iPort = pHttpRef->iPort;
    pHttpRef->critical.pXmlHttp->HttpRefDataCache.iSecure = pHttpRef->iSecure;
    ds_strnzcpy(pHttpRef->critical.pXmlHttp->HttpRefDataCache.strHost, pHttpRef->strHost, sizeof(pHttpRef->critical.pXmlHttp->HttpRefDataCache.strHost));

    // special support for string sends
    if ((pData != NULL) && (iDataSize < 0))
    {
        iDataSize = strlen(pData);
    }
    // save total transaction size
    pHttpRef->critical.iRequestSize = iDataSize;

    if ((pData != NULL) && (iDataSize != 0))
    {
        /* Save request data (up to PROTOHTTP_PRESENDBUF_SIZE bytes) until we reach the internal state
           where we can use it. This is specifically for backward compatibility with client code
           assuming that ProtoHttpPost(), ProtoHttpPut() and ProtoHttpPatch() will always succeed
           sending small request without the need for using a subsequent call to ProtoHttpSend() 
           $$todo - after diligently announcing to the community in a future release, remove this
           complexity in a major release where public api impacts are permitted. */
        if ((iDataSize = _ProtoHttpXmlHttpFillPreSendBuffer(pHttpRef->critical.pXmlHttp, pData, iDataSize)) < 0)
        {
            _ProtoHttpXmlHttpDestroy(pHttpRef->critical.pXmlHttp);
            pHttpRef->critical.pXmlHttp = nullptr;
            NetCritLeave(&pHttpRef->critical.Locker);
            return(-6);
        }
    }

    // get auth token if requested
    if (pHttpRef->bGetAuthToken)
    {
        // get token url
        if (pHttpRef->strTokenUrl[0] == '\0')
        {
            ds_strnzcpy(pHttpRef->strTokenUrl, pHttpRef->strHost, sizeof(pHttpRef->strTokenUrl));
        }
        // make token request
        if ((iResult = DirtyAuthGetToken(pHttpRef->iAuthTokenIdx, pHttpRef->strTokenUrl, FALSE)) == DIRTYAUTH_PENDING)
        {
            // transition to authenticating state, return success
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_AUTH);
        }
        else if (iResult == DIRTYAUTH_SUCCESS)
        {
            // token is cached and already available, so we proceed directly to connection stage
            _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_CONN);
        }
        else
        {
            NetPrintf(("protohttpxboxone: [%p] DirtyAuthGetToken() returned unexpected result %d\n", pHttpRef, iResult));
            _ProtoHttpXmlHttpDestroy(pHttpRef->critical.pXmlHttp);
            pHttpRef->critical.pXmlHttp = nullptr;
            NetCritLeave(&pHttpRef->critical.Locker);
            return(-7);
        }
    }
    else
    {
        _ProtoHttpMoveToNextState(pHttpRef, ProtoHttpStateE::ST_CONN);
    }

    // set request timer
    pHttpRef->uTimer = NetTick() + pHttpRef->uTimeout;

    NetCritLeave(&pHttpRef->critical.Locker);
    return(iDataSize);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpAbort

    \Description
        Abort active request, if any.

    \Input *pHttpRef    - module state pointer

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
void ProtoHttpAbort(ProtoHttpRefT *pHttpRef)
{
    NetCritEnter(&pHttpRef->critical.Locker);

    NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p][%p] user-initiated abort\n", pHttpRef, pHttpRef->critical.pXmlHttp));

    _ProtoHttpAbort(pHttpRef);

    NetCritLeave(&pHttpRef->critical.Locker);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetBaseUrl

    \Description
        Set base url that will be used for any relative url references.

    \Input *pHttpRef      - module state
    \Input *pUrl        - base url

    \Version 02/03/2010 (jbrookes)
*/
/********************************************************************************F*/
void ProtoHttpSetBaseUrl(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    char strHost[sizeof(pHttpRef->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url for kind, host, and port
    ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // set base info
    NetPrintf(("protohttpxboxone: [%p] setting base url to %s://%s:%d\n", pHttpRef, iSecure ? "https" : "http", strHost, iPort));
    ds_strnzcpy(pHttpRef->strBaseHost, strHost, sizeof(pHttpRef->strBaseHost));
    pHttpRef->iBasePort = iPort;
    pHttpRef->iBaseSecure = iSecure;
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpGetLocationHeader

    \Description
        Get location header from the input buffer.  The Location header requires
        some special processing.

    \Input *pHttpRef    - module state pointer
    \Input *pInpBuf     - buffer holding header text
    \Input *pBuffer     - [out] output buffer for parsed location header, null for size request
    \Input iBufSize     - size of output buffer, zero for size request
    \Input **pHdrEnd    - [out] pointer past end of parsed header (optional)

    \Output
        int32_t         - negative=header not received, not found or not enough space, zero=success, positive=size query result

    \Version 03/24/2009 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpGetLocationHeader(ProtoHttpRefT *pHttpRef, const char *pInpBuf, char *pBuffer, int32_t iBufSize, const char **pHdrEnd)
{
    const char *pLocHdr;
    int32_t iLocLen, iLocPreLen=0;

    // get a pointer to header
    if ((pLocHdr = ProtoHttpFindHeaderValue(pHttpRef->responseHeader.strHdr, "location")) == NULL)
    {
        return(-1);
    }

    /* according to RFC location headers should be absolute, but some webservers respond with relative
       URL's.  we assume any url that does not start with "http://" or "https://" is a relative url,
       and if we find one, we assume the request keeps the same security, port, and host as the previous
       request */
    if (ds_strnicmp(pLocHdr, "http://", 7) && ds_strnicmp(pLocHdr, "https://", 8))
    {
        char strTemp[288]; // space for max DNS name (253 chars) plus max http url prefix

        // format http url prefix
        ds_snzprintf(strTemp, sizeof(strTemp), "%s://%s:%d", pHttpRef->iSecure ? "https" : "http", pHttpRef->strHost, pHttpRef->iPort);

        // make sure relative URL includes '/' as the first character, required when we format the redirection url
        if (*pLocHdr != '/')
        {
            ds_strnzcat(strTemp, "/", sizeof(strTemp));
        }

        // calculate url prefix length
        iLocPreLen = (int32_t)strlen(strTemp);

        // copy url prefix text if a buffer is specified
        if (pBuffer != NULL)
        {
            ds_strnzcpy(pBuffer, strTemp, iBufSize);
            pBuffer = (char *)((uint8_t *)pBuffer + iLocPreLen);
            iBufSize -= iLocPreLen;
        }
    }

    // extract location header and return size
    iLocLen = ProtoHttpExtractHeaderValue(pLocHdr, pBuffer, iBufSize, pHdrEnd);
    // if it's a size request add in (possible) url header length
    if ((pBuffer == NULL) && (iBufSize == 0))
    {
        iLocLen += iLocPreLen;
    }

    // return to caller
    return(iLocLen);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpControl

    \Description
        ProtoHttp control function.  Different selectors control different behaviors.

    \Input *pHttpRef    - module state pointer
    \Input iSelect      - control selector ('time')
    \Input iValue       - selector specific
    \Input iValue2      - selector specific
    \Input *pValue      - selector specific

    \Output
        int32_t         - selector specific

    \Notes
        Selectors are:

        \verbatim
            SELECTOR    DESCRIPTION'
            'apnd'      The given buffer will be appended to future headers sent
                        by ProtoHttp.  Note that the User-Agent and Accept lines
                        in the default header will be replaced, so if these lines
                        are desired, they should be supplied in the append header.
            'auth'      Enables/disables authentication (default=enabled)
            'ires'      Resize input buffer
            'keep'      Sets keep-alive; zero=disabled, else enabled
            'maxr'      Sets max recv rate (bytes/sec; zero=uncapped)
            'maxs'      Sets max send rate (bytes/sec; zero=uncapped)
            'odth'      Sets ONDATA_THRESHOLD property (default=64k)
            'prxy'      Sets proxy server
            'ratr'      return current recv rate estimation (bytes/sec)
            'rats'      return current send rate estimation (bytes/sec)
            'rmax'      Sets maximum number of redirections (default=3)
            'spam'      Sets debug output verbosity (0...n)
            'time'      Sets ProtoHttp client timeout in milliseconds (default=30s)
            'xtok'      Sets XSTS token enable/disable (default=disabled; iValue=
                        enabled true/false, iValue2=user index, pValue=
                        token URN, only required if different from URL)
        \endverbatim

    \Version 07/02/2013 (jbrookes)
*/
/*******************************************************************************F*/
int32_t ProtoHttpControl(ProtoHttpRefT *pHttpRef, int32_t iSelect, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iSelect == 'apnd')
    {
        return(_ProtoHttpSetAppendHeader(pHttpRef, (const char *)pValue));
    }
    if (iSelect == 'auth')
    {
        NetPrintf(("protohttpxboxone: [%p] %s authentication\n", pHttpRef, iValue ? "enabling" : "disabling"));
        pHttpRef->bAuthenticate = iValue ? TRUE : FALSE;
        return(0);
    }
    if (iSelect == 'ires')
    {
        // save 'ideal' max buffer size
        pHttpRef->critical.RequestBufferParams.iInpMaxIdeal = iValue;
        // resize the buffer
        return(_ProtoHttpResizeBuffer(pHttpRef, iValue));
    }
    if (iSelect == 'keep')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] setting keep-alive to %d\n", pHttpRef, iValue));
        pHttpRef->iKeepAlive = pHttpRef->iKeepAliveDflt = iValue;
        return(0);
    }
    if (iSelect == 'maxr')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] setting max recv rate to %d bytes/sec\n", pHttpRef, iValue));
        pHttpRef->RecvRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'maxs')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] setting max send rate to %d bytes/sec\n", pHttpRef, iValue));
        pHttpRef->SendRate.uMaxRate = iValue;
        return(0);
    }
    if (iSelect == 'odth')
    {
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: [%p] setting ondata_threshold property to %d\n", iValue));
        pHttpRef->iOnDataThreshold = iValue;
        return(0);
    }
    if (iSelect == 'prxy')
    {
        NetPrintf(("protohttpxboxone: [%p] setting %s as proxy server\n", pHttpRef, pValue));
        ds_strnzcpy(pHttpRef->strProxy, (const char *)pValue, sizeof(pHttpRef->strProxy));
        return(0);
    }
    // return current recv rate estimation
    if (iSelect == 'ratr')
    {
        return(pHttpRef->RecvRate.uCurRate);
    }
    // return current send rate estimation
    if (iSelect == 'rats')
    {
        return(pHttpRef->SendRate.uCurRate);
    }
    if (iSelect == 'rmax')
    {
        pHttpRef->iMaxRedirect = iValue;
        return(0);
    }
    if (iSelect == 'spam')
    {
        pHttpRef->iVerbose = iValue;
        return(0);
    }
    if (iSelect == 'time')
    {
        NetPrintf(("protohttpxboxone: [%p] setting timeout to %d ms\n", pHttpRef, iValue));
        pHttpRef->uTimeout = (unsigned)iValue;
        return(0);
    }
    if (iSelect == 'xtok')
    {
        pHttpRef->bGetAuthToken = iValue ? TRUE : FALSE;
        pHttpRef->iAuthTokenIdx = (int8_t)iValue2;
        NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: auth token acquisition %s (user index=%d)\n", (pHttpRef->bGetAuthToken == TRUE) ? "enabled" : "disabled", iValue2));
        if (pValue != NULL)
        {
            NetPrintfVerbose((pHttpRef->iVerbose, 1, "protohttpxboxone: setting token urn=%s\n", pValue));
            ds_strnzcpy(pHttpRef->strTokenUrl, (const char *)pValue, sizeof(pHttpRef->strTokenUrl));
        }
        else
        {
            pHttpRef->strTokenUrl[0] = '\0';
        }
        return(0);
    }
    NetPrintf(("protohttpxboxone: [%p] '%C' control is invalid\n", pHttpRef, iSelect));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpStatus

    \Description
        Return status of current HTTP transfer.  Status type depends on selector.

    \Input *pHttpRef    - module state pointer
    \Input iSelect      - info selector (see Notes)
    \Input *pBuffer     - [out] storage for selector-specific output
    \Input iBufSize     - size of buffer

    \Output
        int32_t         - selector specific

    \Notes
        Selectors are:

    \verbatim
        SELECTOR    RETURN RESULT
        'body'      negative=failed or pending, else size of body (for 64bit size, get via pBuffer)
        'code'      negative=no response, else server response code (ProtoHttpResponseE)
        'data'      negative=failed, zero=pending, positive=amnt of data ready
        'date'      last-modified date, if available
        'done'      negative=failed, zero=pending, positive=done
        'head'      returns size of header
        'host'      current host copied to output buffer
        'hres'      return hResult containing either the socket error or ssl error or the http status code 
        'htxt'      current received http header text copied to output buffer
        'info'      copies most recent info header received, if any, to output buffer (one time only)
        'imax'      size of input buffer
        'iovr'      returns input buffer overflow size (valid on PROTOHTTP_MINBUFF result code)
        'maxr'      returns configured max recv rate (bytes/sec; zero=uncapped)
        'maxs'      returns configured max send rate (bytes/sec; zero=uncapped)
        'port'      current port
        'rmax'      returns currently configured max redirection count
        'rtxt'      most recent http request header text copied to output buffer
        'time'      TRUE if the client timed out the connection, else FALSE
    \endverbatim

    \Version 07/02/2013 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpStatus(ProtoHttpRefT *pHttpRef, int32_t iSelect, void *pBuffer, int32_t iBufSize)
{
    int32_t iResult;
    ProtoHttpStateE eRequestState;

    // return response code
    if (iSelect == 'code')
    {
        return(pHttpRef->responseHeader.iHdrCode);
    }
    // most recent ssl error (not supported)
    if (iSelect == 'essl')
    {
        return(0);
    }
    // return current host
    if (iSelect == 'host')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->strHost, iBufSize);
        return(0);
    }
    //return hResult containing the http status code 
    if (iSelect == 'hres')
    {
        // return the http status code if there is one if not return the ssl error in hresult format
        if (pHttpRef->responseHeader.iHdrCode > 0)
        {
            return(DirtyErrGetHResult(DIRTYAPI_PROTO_HTTP, pHttpRef->responseHeader.iHdrCode, (pHttpRef->responseHeader.iHdrCode >= PROTOHTTP_RESPONSE_CLIENTERROR) ? TRUE : FALSE));
        }
        return(0);
    }
    // return size of input buffer
    if (iSelect == 'imax')
    {
        NetCritEnter(&pHttpRef->critical.Locker);
        iResult = pHttpRef->critical.RequestBufferParams.iInpMax;
        NetCritLeave(&pHttpRef->critical.Locker);
        return(iResult);
    }
    // return input overflow amount (valid after PROTOHTTP_MINBUFF result code)
    if (iSelect == 'iovr')
    {
        NetCritEnter(&pHttpRef->critical.Locker);
        iResult = pHttpRef->critical.pXmlHttp->RequestBuffer.iInpOvr;
        NetCritLeave(&pHttpRef->critical.Locker);
        return(iResult);
    }
    // return configured max recv rate
    if (iSelect == 'maxr')
    {
        return(pHttpRef->RecvRate.uMaxRate);
    }
    // return configured max send rate
    if (iSelect == 'maxs')
    {
        return(pHttpRef->SendRate.uMaxRate);
    }
    // return current port
    if (iSelect == 'port')
    {
        return(pHttpRef->iPort);
    }
    // return current redirection max
    if (iSelect == 'rmax')
    {
        return(pHttpRef->iMaxRedirect);
    }
    // return most recent http request header text
    if (iSelect == 'rtxt')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->strRequestHdr, iBufSize);
        return(0);
    }
    // return timeout indicator
    if (iSelect == 'time')
    {
        return(pHttpRef->critical.bTimeout);
    }

    // retrieve the state
    NetCritEnter(&pHttpRef->critical.Locker);
    eRequestState = pHttpRef->critical.eRequestState;
    NetCritLeave(&pHttpRef->critical.Locker);

    // done check: negative=failed, zero=pending, positive=done
    if (iSelect == 'done')
    {
        iResult = 0; // default to in progress

        if ((eRequestState == ST_IDLE) || (eRequestState == ST_DONE))
        {
            iResult = 1;
        }
        else if (eRequestState == ST_FAIL)
        {
            iResult = -1;
        }

        return(iResult);
    }
    // data check: negative=failed, zero=pending, positive=data ready
    if (iSelect == 'data')
    {
        if (eRequestState == ST_FAIL)
        {
            return(-1);
        }
        if ((eRequestState == ST_BODY) || (eRequestState == ST_DONE))
        {
            iResult = pHttpRef->responseHeader.iHeadSize + pHttpRef->responseHeader.iBodySize;
            return(iResult);
        }
        return(0);
    }

    // check the state
    if (eRequestState == ST_FAIL)
    {
        return(-1);
    }
    if ((eRequestState != ST_BODY) && (eRequestState != ST_DONE))
    {
        return(-2);
    }

    // parse the tokens
    if (iSelect == 'head')
    {
        return(pHttpRef->responseHeader.iHeadSize);
    }
    if (iSelect == 'body')
    {
        if ((pBuffer != NULL) && (iBufSize == sizeof(pHttpRef->responseHeader.iBodySize)))
        {
            ds_memcpy(pBuffer, &pHttpRef->responseHeader.iBodySize, iBufSize);
        }
        return(pHttpRef->responseHeader.iBodySize);
    }
    if (iSelect == 'date')
    {
        return(pHttpRef->responseHeader.iHdrDate);
    }
    if (iSelect == 'htxt')
    {
        ds_strnzcpy((char *)pBuffer, pHttpRef->responseHeader.strHdr, iBufSize);
        return(0);
    }

    // invalid token
    NetPrintfVerbose((pHttpRef->iVerbose, 2, "protohttpxboxone: [%p] '%C' status is invalid\n", pHttpRef, iSelect));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpCheckKeepAlive

    \Description
        Checks whether a request for the given Url would be a keep-alive transaction.
        Unlike the 'generic' version, this version is not aware of the current
        connection status, only whether the previous request was made to the same
        host/port/security setting.

    \Input *pHttpRef    - module state pointer
    \Input *pUrl        - Url to check

    \Output
        int32_t         - TRUE if a request to this Url would use keep-alive, else FALSE

    \Version 07/18/2013 (jbrookes)
*/
/********************************************************************************F*/
int32_t ProtoHttpCheckKeepAlive(ProtoHttpRefT *pHttpRef, const char *pUrl)
{
    char strHost[sizeof(pHttpRef->strHost)], strKind[8];
    int32_t iPort, iSecure;
    uint8_t bPortSpecified;

    // parse the url
    pUrl = ProtoHttpUrlParse2(pUrl, strKind, sizeof(strKind), strHost, sizeof(strHost), &iPort, &iSecure, &bPortSpecified);

    // see if a request for this url would use keep-alive
    if ((pHttpRef->iKeepAlive > 0) && (pHttpRef->iPort == iPort) && (pHttpRef->iSecure == iSecure) && !ds_stricmp(pHttpRef->strHost, strHost))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpUpdate

    \Description
        Give time to module to do its thing (should be called periodically to
        allow module to perform work)

    \Input *pHttpRef    - module state pointer

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
void ProtoHttpUpdate(ProtoHttpRefT *pHttpRef)
{
    /* if we detected a suspend from netconn, existing XmlHttp entries should be forced into
       "completion grace period" in case of completion notification never happening
       after a suspend event */
    if (NetConnStatus('susp', 0, NULL, 0))
    {
        ProtoHttpXmlHttpT *pXmlHttp;

        if (pHttpRef->critical.pXmlHttp != NULL)
        {
            pHttpRef->critical.pXmlHttp->bGracePeriodEntered = TRUE;
        }
        for (pXmlHttp = pHttpRef->pXmlHttpKillList; pXmlHttp != NULL; pXmlHttp = pXmlHttp->pNext)
        {
            pXmlHttp->bGracePeriodEntered = TRUE;
        }
    }

    // pump state machine of active request
    _ProtoHttpUpdateActiveRequest(pHttpRef);

    // deal with entries in kill list
    _ProtoHttpProcessKillList(pHttpRef, FALSE);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetCACert

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all HTTP instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

    \Input *pCACert - pointer to certificate data
    \Input iCertSize- certificate size in bytes

    \Output
        int32_t     - negative=failure, zero=success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert(const uint8_t *pCACert, int32_t iCertSize)
{
    return(0);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpSetCACert2

    \Description
        Add one or more X.509 CA certificates to the trusted list. A
        certificate added will be available to all HTTP instances for
        the lifetime of the application. This function can add one or more
        PEM certificates or a single DER certificate.

        This version of the function does not validate the CA at load time.
        The X509 certificate data will be copied and retained until the CA
        is validated, either by use of ProtoHttpValidateAllCA() or by the CA
        being used to validate a certificate.

    \Input *pCACert - pointer to certificate data
    \Input iCertSize- certificate size in bytes

    \Output
        int32_t     - negative=failure, zero=success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpSetCACert2(const uint8_t *pCACert, int32_t iCertSize)
{
    return(0);
}

/*F********************************************************************************/
/*!
    \Function ProtoHttpValidateAllCA

    \Description
        Validate all CA that have been added but not yet been validated.  Validation
        is a one-time process and disposes of the X509 certificate that is retained
        until validation.

    \Output
        int32_t     - negative=failure, zero=success

    \Version 11/01/2012 (akirchner)
*/
/********************************************************************************F*/
int32_t ProtoHttpValidateAllCA(void)
{
    return(0);
}

/*F*************************************************************************/
/*!
    \Function ProtoHttpClrCACerts

    \Description
        Clears all dynamic CA certs from the list.

    \Version 11/01/2012 (akirchner)
*/
/**************************************************************************F*/
void ProtoHttpClrCACerts(void)
{
}
