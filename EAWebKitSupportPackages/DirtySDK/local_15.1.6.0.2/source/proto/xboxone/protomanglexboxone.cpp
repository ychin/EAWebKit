/*H********************************************************************************/
/*!
    \File protomanglexboxone.cpp

    \Description
        Wrapper around Xbox One Secure Networking functionality.

    \Copyright
        Copyright (c) Electronic Arts 2013. ALL RIGHTS RESERVED.

    \Version 04/05/2013 (mclouatre)

    \Notes

    \verbatim
    ProtoMangleXboxone supports parallel resolution of secure device addresses
    for different remote consoles.

    Each instance is tracked with this state machine:

        PROTOMANGLE_STATE_INIT      <----| still conflicts with entry in kill list
                |             \__________|
   -------------|
   |            |
   |            | no conflict with an entry in the kill list
   |            |
   |            |
   |            |------------------------------------------------------------
   |            |                                                            |
   |            | we are the                                                 | we are the
   |            | SA creation initiator                                      | SA creation listener
   |            |                                                            |
   |           \/                                                           \/
   |    PROTOMANGLE_STATE_CREATING     <-| waiting for outbound       PROTOMANGLE_STATE_LISTENING   <-| waiting for inbound
   |            |             \__________| SA creation to complete           |             \__________| SA creation to complete
   |            |                                                            |
   |            |                                                            |
   |            |------------------------------------------------------------
   |            |
   |            |
   |            |------------------------------------------------------------
   |            |                             |                              |
   |            | success                     | fail                         | timed out
   |            |                             |                              | (here the async security association creation is still on going)
   |           \/                            \/                             \/
   |    PROTOMANGLE_STATE_DONE     PROTOMANGLE_STATE_FAILED    PROTOMANGLE_STATE_TIMEDOUT
   |            |                             |                              |
   |            |                             |                              |
   ---------------------------------------------------------------------------
                |
                | entry was moved to kill list and no async op in progress
                |
               \/
        PROTOMANGLE_STATE_CLEANING     <-| if SA initiator: waiting for security association destruction to complete
                |             \__________|
                |
               \/
              free

    At any time during the life time of such an instance, the instance can be moved
    to a kill list where it sits as an invalid instance until it reaches the CLEANING
    state and eventually gets freed. This allows us to not perform any cancel of 
    async operations which proved to not work properly.
    \endverbatim
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <xdk.h>        // for _XDK_VER
#include <wrl.h>        // required for robuffer.h
#include <robuffer.h>   // required for byte access to IBuffers

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtynet.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protomangle.h"

#include <collection.h> // required for Platform::Collections::Vector (must be included after dirtysock.h)

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Xbox::Networking;
using namespace Windows::Xbox::Multiplayer;

/*** Defines **********************************************************************/

#define PROTOMANGLE_SECURITYASSOC_CREATION_TIMEOUT   (20 * 1000) // default internal timeout for internally triggered async security association creation
#define PROTOMANGLE_SECURITYASSOC_CHECK_FREQUENCY    (2 * 1000)  // check for successful remote-initiated security association every 2 seconds

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

#if DIRTYCODE_LOGGING
const char *_strStates[] =
{
    "PROTOMANGLE_STATE_INIT        ",
    "PROTOMANGLE_STATE_CREATING    ",
    "PROTOMANGLE_STATE_LISTENING   ",
    "PROTOMANGLE_STATE_DONE        ",
    "PROTOMANGLE_STATE_FAILED      ",
    "PROTOMANGLE_STATE_TIMEDOUT    ",
    "PROTOMANGLE_STATE_CLEANING    "
};
#endif

//! state transitions for a single security association data space
typedef enum ProtoMangleAssocStateE
{
    PROTOMANGLE_STATE_INIT,              //!< created
    PROTOMANGLE_STATE_CREATING,          //!< establishment of the security association is in progress (locally-initiated)
    PROTOMANGLE_STATE_LISTENING,         //!< establishment of the security association is in progress (remotely-initiated)
    PROTOMANGLE_STATE_DONE,              //!< successful secure address resolution completion
    PROTOMANGLE_STATE_FAILED,            //!< secure address resolution failed
    PROTOMANGLE_STATE_TIMEDOUT,          //!< secure address resolution timedout
    PROTOMANGLE_STATE_CLEANING           //!< releasing allocated resources (includes async destroy if needed) - this state can only be reached when entry is in kill list
} ProtoMangleAssocStateE;

//! security association data space
struct ProtoMangleSecurityAssocT
{
    struct ProtoMangleSecurityAssocT * pNext;       //! for killlist linking

    //! dataspace state variables
    ProtoMangleAssocStateE eMangleState;            //!< dataspace state
    uint8_t bInKillList;                            //!< dataspace is no longer active; it is queued in kill list pending for destruction when async op completes
    uint8_t _pad0[3];

    //! remote client identifiers
    SecureDeviceAddress ^refSecureDeviceAddr;       //!< secure device address of the client (console) we want to establish a security association with
    int32_t iClientIndex;                           //!< client index

    //! resources required for managing SA creation initiating mode
    IAsyncAction ^refAsyncAction;
    IAsyncOperation<SecureDeviceAssociation ^> ^refCreateAssociationOp;
    SecureDeviceAssociation ^refSecurityAssociation;//!< locally-initiated SA
    Platform::String ^refRemoteCanonicalName;       //!< canonical name of the remote host (obtained from established locally-initiated SA)
    Platform::String ^refRemotePort;                //!< port of the remote host (obtained from locally-initiated SA)

    //! resources required for managing SA creation listening mode
    uint8_t bInitialScanDone;
    uint8_t _pad1[3];

    int32_t iSAWaitStartTick;                       //!< used to save current tick when moving to CREATING/LISTENING state

    //! outcome of successful security association establishment
    SOCKADDR_STORAGE remoteSocketAddress;           //!< ipv6 address for this client
    int32_t iIpv4Addr;                              //!< mapped ipv4 address for this client
};

struct ProtoMangleClientT
{
    ProtoMangleSecurityAssocT * pAssoc;     //!< security association data space used for this client
    SecureDeviceAddress ^refPendingSecureDeviceAddr;  //!< secure device address to process when conflicting entry in kill list is freed for good
};

//! internal module state
struct ProtoMangleRefT      //!< module state
{
    //! module memory group
    int32_t iMemGroup;
    void *pMemGroupUserData;

    NetCritT MangleCrit;                        //!< critical section 

    SecureDeviceAssociationTemplate ^ refSecureDeviceAssociationTemplate;

    Windows::Foundation::EventRegistrationToken IncomingAssociationEventToken;

    String ^refScid;                            //!< the service configuration id for the MultiplayerSessionReference
    String ^refSessionTemplateName;             //!< the session template name for the MultiplayerSessionReference
    String ^refSessionName;                     //!< the session name for the MultiplayerSessionReference

    int32_t iSelf;                              //!< client index of local client
    int32_t iMaxClients;                        //!< max number of clients
    int32_t iAssocCreateTimeout;                //!< timeout for local-initiated security association creation

    int8_t iVerbose;                            //!< debug output verbosity
    uint8_t _pad[2];

    //! collection of recently notified incoming remotely-initiated security associations
    Platform::Collections::Vector<SecureDeviceAssociation ^>^ refIncomingSecurityAssocVect;

    //! list of inactive security association data spaces queued for destruction when no async op in progress
    ProtoMangleSecurityAssocT *pSecurityAssocKillList;

    ProtoMangleRefT *pNext;                     //!< next ref used for linking active refs together

    //! array of game clients - must come last in ref as it is variable length
    ProtoMangleClientT aClients[1];    // variable length
};

//! global protomangle state
typedef struct ProtoMangleStateT
{
    Platform::Collections::Map<String ^, int32_t> ^ refRemoteHostRefCountMap;   //!< one ref count per remote host (security association canonical name/ref count)
    int32_t iRefCount;                                                       //!< number of protomangle instances using _ProtoMangle_State.refRemoteHostRefCountMap
    ProtoMangleRefT *pActiveList;                                               //!< list of active module refs to allow us to check for validity at points in time
    NetCritT StateCrit;                                                         //!< critical to protect the state access
} ProtoMangleStateT;

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

static ProtoMangleStateT _ProtoMangle_State;
static uint8_t _ProtoMangle_bStateInitialized = FALSE;

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _ProtoMangleExceptionReport

    \Description
        Report an exception to debug output

    \Input *pRef        - pointer to module state
    \Input e            - exception
    \Input *pFormat     - format string
    \Input ...          - variable argument list

    \Version 05/29/2013 (jbrookes)
*/
/********************************************************************************F*/
static void _ProtoMangleExceptionReport(ProtoMangleRefT *pRef, Exception ^e, const char *pFormat, ...)
{
#if DIRTYCODE_LOGGING
    va_list Args;
    char strText[1024];
    const wchar_t *pMessage = e->Message->Data();

    va_start(Args, pFormat);
    ds_vsnprintf(strText, sizeof(strText), pFormat, Args);
    va_end(Args);

    // exception messages include a line terminator; if no message we need to include one ourselves
    if (*pMessage == 0)
    {
        pMessage = L"\n";
    }
    // print exception to debug output
    NetPrintf(("protomanglexboxone: [%p] %s err=%s: %S", pRef, strText, DirtyErrGetName(e->HResult), pMessage));
#endif
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleMoveToNextState

    \Description
        Force ProtoMangle state machine transition

    \Input *pRef        - pointer to module state
    \Input *pAssoc      - security association data space pointer
    \Input eNewState    - new state to transit to

    \Version 02/04/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleMoveToNextState(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc, ProtoMangleAssocStateE eNewState)
{
    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p][%p] state transition for %s %d    %s --> %s\n",
        pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex, _strStates[pAssoc->eMangleState], _strStates[eNewState]));

    pAssoc->eMangleState = eNewState;
}

/*F********************************************************************************/
/*!
    \Function   _ProtoMangleIncomingAssociationEventHandler

    \Description
        Event handler (delegate) registered for incoming association events.

    \Input *pRef                        - pointer to module state
    \Input ^refSecurityAssociation      - winrt reference to security association being notified

    \Version 10/29/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleIncomingAssociationEventHandler(ProtoMangleRefT *pRef, SecureDeviceAssociation ^refSecurityAssociation)
{
    // make sure the global state is valid
    if (_ProtoMangle_bStateInitialized == FALSE)
    {
        NetPrintf(("protomanglexboxone: [%p] incoming security association notified but global state is not valid (ignoring)\n", pRef));
        return;
    }

    /* we check to make sure that the ref that was captured by the lambda is still valid. customers have reported crashes
       after the shutdown has completed, this implicitly means that we have already unregistered the event handler.
       to address this issue we have added the tracking of the refs to make sure that our ref is indeed valid at the time of the
       event. 
       we use the global crit to ensure that the manipulation of the list has completed */
    NetCritEnter(&_ProtoMangle_State.StateCrit);
    ProtoMangleRefT *pCurrent;
    for (pCurrent = _ProtoMangle_State.pActiveList; (pCurrent != NULL) && (pCurrent != pRef); pCurrent = pCurrent->pNext)
        ;
    if (pCurrent == NULL)
    {
        NetPrintf(("protomanglexboxone: [%p] incoming security association notified but ref is not valid (ignoring)\n", pRef));
        return;
    }

    NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] incoming security association event notified for remote host %S:%S - association state is %S\n",
        pRef, refSecurityAssociation->RemoteHostName->CanonicalName->Data(), refSecurityAssociation->RemotePort->Data(), refSecurityAssociation->State.ToString()->Data()));

    NetCritEnter(&pRef->MangleCrit);
    pRef->refIncomingSecurityAssocVect->Append(refSecurityAssociation);
    NetCritLeave(&pRef->MangleCrit);

    NetCritLeave(&_ProtoMangle_State.StateCrit);
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleRegisterIncomingAssociationEventHandler

    \Description
        Register the incoming association event handler.

    \Input *pRef        - pointer to module state

    \Output
        int32_t         - negative=error, zero=success

    \Version 10/29/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleRegisterIncomingAssociationEventHandler(ProtoMangleRefT *pRef)
{
    int32_t iRetCode = 0; // default to success

    try
    {
        pRef->IncomingAssociationEventToken = pRef->refSecureDeviceAssociationTemplate->AssociationIncoming += ref new TypedEventHandler<SecureDeviceAssociationTemplate ^, SecureDeviceAssociationIncomingEventArgs ^>(
            [ pRef ] (SecureDeviceAssociationTemplate ^refSecureDeviceAssociationTemplate, SecureDeviceAssociationIncomingEventArgs ^refArgs)
        {
            _ProtoMangleIncomingAssociationEventHandler(pRef, refArgs->Association);
        });

        NetPrintfVerbose((pRef->iVerbose, 2, "protomanglexboxone: [%p] successfully registered the incoming association event handler\n", pRef));
    }
    catch (Exception ^ e)
    {
        NetPrintf(("protomanglexboxone: [%p] exception caught while trying to register for incoming association events (err=%s: %S)\n", pRef, DirtyErrGetName(e->HResult), e->Message->Data()));
        iRetCode = -1;
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleUnregisterIncomingAssociationEventHandler

    \Description
        Unregister the incoming association event handler.

    \Input *pRef        - pointer to module state

    \Output
        int32_t         - negative=error, zero=success

    \Version 10/29/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleUnregisterIncomingAssociationEventHandler(ProtoMangleRefT *pRef)
{
    int32_t iRetCode = 0; // default to success

    try
    {
        pRef->refSecureDeviceAssociationTemplate->AssociationIncoming -= pRef->IncomingAssociationEventToken;
        pRef->IncomingAssociationEventToken.Value = 0;

        NetPrintfVerbose((pRef->iVerbose, 2, "protomanglexboxone: [%p] successfully unregistered the incoming association event handler\n", pRef));
    }
    catch (Exception ^ e)
    {
        NetPrintf(("protomanglexboxone: [%p] exception caught while trying to unregister for incoming association events (err=%s: %S)\n", pRef, DirtyErrGetName(e->HResult), e->Message->Data()));
        iRetCode = -1;
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleRemoteHostAddRef

    \Description
        Increment ref count for specified remote host.

    \Input *pRef                - pointer to module state
    \Input ^refRemoteIdentifier - remote host identifier string (canonical name + port)

    \Output
        int32_t                 - new ref count value

    \Version 08/28/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleRemoteHostAddRef(ProtoMangleRefT *pRef, String ^refRemoteIdentifier)
{
    int32_t iRefCount = 1;

    NetCritEnter(&pRef->MangleCrit);

    if (_ProtoMangle_State.refRemoteHostRefCountMap->HasKey(refRemoteIdentifier))
    {
        iRefCount = _ProtoMangle_State.refRemoteHostRefCountMap->Lookup(refRemoteIdentifier);
        iRefCount++;
        _ProtoMangle_State.refRemoteHostRefCountMap->Remove(refRemoteIdentifier);
    }
    _ProtoMangle_State.refRemoteHostRefCountMap->Insert(refRemoteIdentifier, iRefCount);

    NetCritLeave(&pRef->MangleCrit);

    NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] incremented remote host %S ref count to %d\n", pRef, refRemoteIdentifier->Data(), iRefCount));

    return(iRefCount);
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleRemoteHostDelRef

    \Description
        Decrement ref count for specified remote host.

    \Input *pRef                - pointer to module state
    \Input ^refRemoteIdentifier - remote host identifier string (canonical name + port)

    \Output
        int32_t                 - new ref count value

    \Version 08/28/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleRemoteHostDelRef(ProtoMangleRefT *pRef, String ^refRemoteIdentifier)
{
    int32_t iRefCount = 0;

    NetCritEnter(&pRef->MangleCrit);

    if (_ProtoMangle_State.refRemoteHostRefCountMap->HasKey(refRemoteIdentifier))
    {
        iRefCount = _ProtoMangle_State.refRemoteHostRefCountMap->Lookup(refRemoteIdentifier);
        iRefCount--;
        _ProtoMangle_State.refRemoteHostRefCountMap->Remove(refRemoteIdentifier);
        if (iRefCount != 0)
        {
            _ProtoMangle_State.refRemoteHostRefCountMap->Insert(refRemoteIdentifier, iRefCount);
        }
        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] decremented remote host %S ref count to %d\n", pRef, refRemoteIdentifier->Data(), iRefCount));
    }
    else
    {
        NetPrintf(("protomanglexboxone: [%p] can't decrement ref count for remote host %S because not in map\n", pRef, refRemoteIdentifier->Data()));
    }

    NetCritLeave(&pRef->MangleCrit);

    return(iRefCount);
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleAddIPv6ToIPv4Mapping

    \Description
        Tell dirtynet to add the ipv6-to-ipv4 mapping entry

    \Input *pRef        - pointer to module state
    \Input *pAssoc      - security association data space pointer

    \Output
        int32_t         - 0 for success, negative for failure

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleAddIPv6ToIPv4Mapping(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    int32_t iIpv4Addr = 0;

    try
    {
        // obtain ipv6 secure address for security association
        ArrayReference<BYTE> remoteSocketAddressBytes((BYTE*)&pAssoc->remoteSocketAddress, sizeof(pAssoc->remoteSocketAddress));
        pAssoc->refSecurityAssociation->GetRemoteSocketAddressBytes(remoteSocketAddressBytes);
    }
    catch (Exception ^ e)
    {
        _ProtoMangleExceptionReport(pRef, e, "SecureDeviceAssociation::GetRemoteSocketAddressBytes() threw exception");
        return(-1);
    }


    /*
    Tell dirtynet about the new ipv6 address.
    The return value is expected to be the fake ipv4 address that dirtynet internally mapped that ipv6 address to.
    We can then surface that address to the user
    */
    if ((iIpv4Addr = SocketControl(NULL, '+ip6', sizeof(pAssoc->remoteSocketAddress), &pAssoc->remoteSocketAddress, NULL)) != SOCKMAP_ERROR)
    {
        // save ipv4 addr
        pAssoc->iIpv4Addr = iIpv4Addr;
        return(0);
    }
    else
    {
        NetPrintf(("protomanglexboxone: [%p][%p] %s %d failed to add dirynet ipv6-to-ipv4 mapping entry\n",
            pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));
        return(-2);
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleRemoveIPv6ToIPv4Mapping

    \Description
        Tell dirtynet to release the ipv6-to-ipv4 mapping entry

    \Input *pRef    - pointer to module state
    \Input *pAssoc  - security association data space pointer

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleRemoveIPv6ToIPv4Mapping(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    int32_t iResult;

    // tell dirtynet to release the ipv6-to-ipv4 mapping entry
    if ((iResult = SocketControl(NULL, '-ip6', sizeof(pAssoc->remoteSocketAddress), &pAssoc->remoteSocketAddress, NULL)) < 0)
    {
        NetPrintf(("protomanglexboxone: [%p][%p] %s %d failed to release map dirtynet ipv6-to-ipv4 mapping entry\n",
            pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleUpdateIPv6ToIPv4Mapping

    \Description
        Tell dirtynet to update the ipv6-to-ipv4 mapping entry

    \Input *pRef    - pointer to module state
    \Input *pAssoc  - security association data space pointer
    \Input *pNewSocketAddress - new socket storage address
    
    \Version 12/03/2015 (amakoukji)
*/
/********************************************************************************F*/
static void _ProtoMangleUpdateIPv6ToIPv4Mapping(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc, SOCKADDR_STORAGE *pNewSocketAddress)
{
    int32_t iIpv4Addr = 0;

    /*
    Tell dirtynet about the ipv6 mapping update.
    The return value is expected to be the fake ipv4 address that dirtynet internally remapped that ipv6 address to.
    */
    if ((iIpv4Addr = SocketControl(NULL, '~ip6', sizeof(*pNewSocketAddress), &pAssoc->remoteSocketAddress, pNewSocketAddress)) != SOCKMAP_ERROR)
    {
        // sanity check
        if (pAssoc->iIpv4Addr != iIpv4Addr)
        {
            NetPrintf(("protomanglexboxone: unexpected ipv4 address remap\n"));
        }
    }
    else
    {
        NetPrintf(("protomanglexboxone: [%p][%p] %s %d failed to update dirynet ipv6-to-ipv4 mapping entry\n",
            pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleAddToKillList

    \Description
        Add security association data space to the list of entries that needs to
        be cleanup when pending async op is completed.

    \Input *pRef        - pointer to module state
    \Input *pAssoc      - security association data space pointer

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleAddToKillList(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    ProtoMangleSecurityAssocT *pOldHead = pRef->pSecurityAssocKillList;

    NetPrintfVerbose((pRef->iVerbose, 2, "protomanglexboxone: [%p][%p] security association data space for clnt idx %d added to kill list\n",
        pRef, pAssoc, pAssoc->iClientIndex));

    // insert at head
    pRef->pSecurityAssocKillList = pAssoc;
    pAssoc->bInKillList = TRUE;
    pRef->aClients[pAssoc->iClientIndex].pAssoc = NULL;

    // link
    pRef->pSecurityAssocKillList->pNext = pOldHead;
}

/*F********************************************************************************/
/*!
    \Function _ProtoMangleRemoveFromKillList

    \Description
        Remove security association data space from the list of entries that needs to
        be cleanup when pending async op is completed.

    \Input *pRef        - pointer to module state
    \Input *pAssoc      - security association data space pointer

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleRemoveFromKillList(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    NetPrintfVerbose((pRef->iVerbose, 2, "protomanglexboxone: [%p][%p] security association data space for killed clnt formerly at idx %d removed from kill list\n",
        pRef, pAssoc, pAssoc->iClientIndex));

    // is entry to be deleted is at head of list
    if (pAssoc == pRef->pSecurityAssocKillList)
    {
        pRef->pSecurityAssocKillList = pAssoc->pNext;
    }
    else
    {
        ProtoMangleSecurityAssocT *pCurrent = pRef->pSecurityAssocKillList->pNext;
        ProtoMangleSecurityAssocT *pPrevious = pRef->pSecurityAssocKillList;

        while (pCurrent != NULL)
        {
            if (pCurrent == pAssoc)
            {
                pPrevious->pNext = pCurrent->pNext;
                break;
            }

            pPrevious = pCurrent;
            pCurrent = pCurrent->pNext;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _ProtoMangleProcessIncomingSAs

    \Description
        Process the queue of incoming secure device associations.

    \Input *pRef    - pointer to module state

    \Version 09/06/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleProcessIncomingSAs(ProtoMangleRefT *pRef)
{
    int32_t iClientIndex;

    // attempt to acquire the crit
    if (!NetCritTry(&pRef->MangleCrit))
    {
        return;
    }

    while (pRef->refIncomingSecurityAssocVect->Size)
    {
        SecureDeviceAssociation^ refSecurityAssociation = pRef->refIncomingSecurityAssocVect->GetAt(0);
        bool bFound = false;

        // loop through all "listening" clients and look for a matching SecureDeviceAddress
        for (iClientIndex = 0; iClientIndex < pRef->iMaxClients; iClientIndex++)
        {
            ProtoMangleSecurityAssocT *pAssoc = pRef->aClients[iClientIndex].pAssoc;

            if ((pAssoc != NULL) && (pAssoc->eMangleState == PROTOMANGLE_STATE_LISTENING))
            {
                try
                {
                    if (refSecurityAssociation->RemoteSecureDeviceAddress->Compare(pAssoc->refSecureDeviceAddr) == 0)
                    {
                        pAssoc->refSecurityAssociation = refSecurityAssociation;
                        bFound = true;
                        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] matched incoming security association\n", pRef, pAssoc));
                    }
                }
                catch(Exception^ e)
                {
                    _ProtoMangleExceptionReport(pRef, e, "failed to compare secure device addresses (%p)", pAssoc);
                }
            }
        }

        // if we did not find a "listening" client, loop through all "done" clients and look for a matching SecureDeviceAddress
        // this will update the IPv6 address if necessary
        // GOSOPS-158463 demonstrated that this address can update in a cable pull scenario
        if (!bFound)
        {
            for (iClientIndex = 0; iClientIndex < pRef->iMaxClients; iClientIndex++)
            {
                ProtoMangleSecurityAssocT *pAssoc = pRef->aClients[iClientIndex].pAssoc;

                if ((pAssoc != NULL) && (pAssoc->eMangleState == PROTOMANGLE_STATE_DONE) && pAssoc->iIpv4Addr != 0)
                {
                    try
                    {
                        if (refSecurityAssociation->RemoteSecureDeviceAddress->Compare(pAssoc->refSecureDeviceAddr) == 0)
                        {
                            SOCKADDR_STORAGE newRemoteSocketAddress;
                            bool bAddressObtained = false;
                            NetPrintf(("protomanglexboxone: [%p][%p] matched incoming security association in PROTOMANGLE_STATE_DONE state\n", pRef, pAssoc));

                            try
                            {
                                // obtain ipv6 secure address for security association
                                ArrayReference<BYTE> remoteSocketAddressBytes((BYTE*)&newRemoteSocketAddress, sizeof(newRemoteSocketAddress));
                                refSecurityAssociation->GetRemoteSocketAddressBytes(remoteSocketAddressBytes);
                                bAddressObtained = true;
                            }
                            catch (Exception ^ e)
                            {
                                _ProtoMangleExceptionReport(pRef, e, "SecureDeviceAssociation::GetRemoteSocketAddressBytes() threw exception");
                                bAddressObtained = false;
                            }
                            
                            if (bAddressObtained && memcmp(&pAssoc->remoteSocketAddress, &newRemoteSocketAddress, sizeof(newRemoteSocketAddress)) != 0)
                            {
                                pAssoc->refSecurityAssociation = refSecurityAssociation;
                                _ProtoMangleUpdateIPv6ToIPv4Mapping(pRef, pAssoc, &newRemoteSocketAddress);
                                ds_memcpy(&pAssoc->remoteSocketAddress, &newRemoteSocketAddress, sizeof(pAssoc->remoteSocketAddress));
                            }
                            
                        }
                    }
                    catch(Exception^ e)
                    {
                        _ProtoMangleExceptionReport(pRef, e, "failed to compare secure device addresses (%p)", pAssoc);
                    }
                }
            }
        }

        pRef->refIncomingSecurityAssocVect->RemoveAt(0);
    }

    NetCritLeave(&pRef->MangleCrit);
}


/*F********************************************************************************/
/*!
    \Function    _ProtoMangleCheckForExistingSA

    \Description
        Check if the security association got created as a result of a remote-initiated creation
        attempt.

    \Input *pRef                    - reference pointer
    \Input *pAssoc                  - security association data space pointer

    \Output
        SecureDeviceAssociation ^   - winrt reference to found security association; or nullptr if not found

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static SecureDeviceAssociation ^ _ProtoMangleCheckForExistingSA(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    try
    {
        Windows::Foundation::Collections::IVectorView<SecureDeviceAssociation ^> ^refAssociationsView = pRef->refSecureDeviceAssociationTemplate->Associations;

        if (refAssociationsView->Size != 0)
        {
            Windows::Foundation::Collections::IIterator<SecureDeviceAssociation ^> ^it;
            for (it = refAssociationsView->First(); it->HasCurrent; it->MoveNext())
            {
                if (it->Current->State == Windows::Xbox::Networking::SecureDeviceAssociationState::Ready)
                {
                    if (it->Current->RemoteSecureDeviceAddress->Compare(pAssoc->refSecureDeviceAddr) == 0)
                    {
                        return(it->Current);
                    }
                }
            }
        }
    }
    catch(Exception^ e)
    {
        _ProtoMangleExceptionReport(pRef, e, "failed to query collection of exisiting security associations");
    }

    return(nullptr);
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleCreateSA

    \Description
        Initiate asynchronous creation of security association.

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 09/09/2014 (mclouatre)
*/
/*******************************************************************************F*/
static void _ProtoMangleCreateSA(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    try
    {
        pAssoc->refCreateAssociationOp = pRef->refSecureDeviceAssociationTemplate->CreateAssociationAsync(pAssoc->refSecureDeviceAddr, CreateSecureDeviceAssociationBehavior::Default);

        pAssoc->refCreateAssociationOp->Completed = ref new AsyncOperationCompletedHandler<SecureDeviceAssociation ^>(
            [pRef, pAssoc](IAsyncOperation<SecureDeviceAssociation ^>^ refCreateAssociationOp, Windows::Foundation::AsyncStatus asyncStatus)
        {
            switch (asyncStatus)
            {
                case AsyncStatus::Completed:
                    try
                    {
                        SecureDeviceAssociation ^refSecurityAssociation = refCreateAssociationOp->GetResults();
                        pAssoc->refSecurityAssociation = refSecurityAssociation;
                        pAssoc->refRemoteCanonicalName = refSecurityAssociation->RemoteHostName->CanonicalName;
                        pAssoc->refRemotePort = refSecurityAssociation->RemotePort;
                        _ProtoMangleRemoteHostAddRef(pRef, pAssoc->refRemoteCanonicalName + L":" + pAssoc->refRemotePort);

                        // only move to DONE state if current state is still CREATING
                        if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
                        {
                             NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] successfully created security association for %s %d (%S:%S)\n",
                                pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex,
                                pAssoc->refRemoteCanonicalName->Data(), pAssoc->refRemotePort->Data()));

                            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_DONE);
                        }
                        else
                        {
                            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] security association creation for %s %d (%S:%S) successfully completed beyond wait period\n",
                                pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex,
                                pAssoc->refRemoteCanonicalName->Data(), pAssoc->refRemotePort->Data()));
                        }
                    }
                    catch(Exception^ e)
                    {
                        _ProtoMangleExceptionReport(pRef, e, "failed to use security association properties (%p)", pAssoc);

                        // only move to FAILED state if current state is still CREATING
                        if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
                        {
                            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
                        }
                    }
                    break;

                case AsyncStatus::Error:
                    try
                    {
                        refCreateAssociationOp->GetResults();
                    }
                    catch (Exception ^ e)
                    {
                        _ProtoMangleExceptionReport(pRef, e, "failed to create security association (%p)", pAssoc);
                    }

                    // only move to FAILED state if current state is still CREATING
                    if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
                    {
                        _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
                    }
                    break;

                case AsyncStatus::Canceled:
                    NetPrintf(("protomanglexboxone: [%p][%p] creation of security association for %s %d was canceled\n",
                        pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));
                    // only move to FAILED state if current state is still CREATING
                    if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
                    {
                        _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
                    }
                    break;

                default:
                    NetPrintf(("protomanglexboxone: [%p][%p] unknown AsyncStatus code (%d) when creating security association for %s %d\n",
                        pRef, pAssoc, asyncStatus, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));
                    // only move to FAILED state if current state is still CREATING
                    if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
                    {
                        _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
                    }
                    break;
            }

            pAssoc->refCreateAssociationOp->Close();
            delete pAssoc->refCreateAssociationOp;
            pAssoc->refCreateAssociationOp = nullptr;
        });
    }
    catch (Exception ^ e)
    {
         pAssoc->refCreateAssociationOp = nullptr;
        _ProtoMangleExceptionReport(pRef, e, "SecureDeviceAssociationTemplate::CreateAssociationAsync() threw exception");
        _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
        return;
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleDestroySA

    \Description
        Initiate asynchronous destruction of locally-originated security association.

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 09/09/2014 (mclouatre)
*/
/*******************************************************************************F*/
static void _ProtoMangleDestroySA(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    SecureDeviceAssociation ^refSecurityAssociation = pAssoc->refSecurityAssociation;

    try
    {
        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] destroying security association (state:%S) for %s %d\n",
            pRef, pAssoc, refSecurityAssociation->State.ToString()->Data(), (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

        pAssoc->refAsyncAction = refSecurityAssociation->DestroyAsync();
        pAssoc->refAsyncAction->Completed = ref new AsyncActionCompletedHandler(
            [pRef, pAssoc](IAsyncAction^ asyncAction, Windows::Foundation::AsyncStatus asyncStatus)
        {
            switch (asyncStatus)
            {
            case AsyncStatus::Completed:
                NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p][%p] successfully destroyed security association for %s %d\n",
                    pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

                break;
            case AsyncStatus::Error:
                try { asyncAction->GetResults(); } catch (Exception ^ e) { _ProtoMangleExceptionReport(pRef, e, "failed to destroy security association (%p)", pAssoc); }

                // experimentation shows that DestroyAsync() can fail when the other end of the connection has already destroyed the association
                // in such a case we assume success and proceed instead of entering failure state
                break;
            case AsyncStatus::Canceled:
                NetPrintf(("protomanglexboxone: [%p][%p] destruction of security association for %s %d was canceled\n",
                    pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

                break;
            default:
                NetPrintf(("protomanglexboxone: [%p][%p] unknown AsyncStatus code (%d) when destroying security association for %s %d\n",
                    pRef, pAssoc, asyncStatus, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

                break;
            }

            delete pAssoc->refSecurityAssociation;
            pAssoc->refSecurityAssociation = nullptr;

            pAssoc->refAsyncAction->Close();
            delete pAssoc->refAsyncAction;
            pAssoc->refAsyncAction = nullptr;
        });
    }
    catch (Exception ^ e)
    {
        _ProtoMangleExceptionReport(pRef, e, "SecureDeviceAssociation::DestroyAsync() threw exception");

        pAssoc->refSecurityAssociation = nullptr;
        pAssoc->refAsyncAction = nullptr;
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleUpdateCleaning

    \Description
        Releasing previously allocated resources associated with the specified client index

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 04/05/2013 (mclouatre)
*/
/*******************************************************************************F*/
static void _ProtoMangleUpdateCleaning(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    // make sure destroy async op is no longer progress
    if (pAssoc->refAsyncAction == nullptr)
    {
        SecureDeviceAssociation ^refSecurityAssociation = pAssoc->refSecurityAssociation;

        if (refSecurityAssociation != nullptr)
        {
            // decrement refcount on this host and proceed with security association destroy if refount reaches 0
            if (_ProtoMangleRemoteHostDelRef(pRef, pAssoc->refRemoteCanonicalName + L":" + pAssoc->refRemotePort) == 0)
            {
                if (pAssoc->iIpv4Addr != 0)
                {
                    _ProtoMangleRemoveIPv6ToIPv4Mapping(pRef, pAssoc);
                }

                // the client with the biggest index is always the destuctor of the secure device association
                if (pRef->iSelf > pAssoc->iClientIndex)
                {
                    _ProtoMangleDestroySA(pRef, pAssoc);
                }
                else
                {
                    NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] destruction of security association for %s %d is left to remote peer\n",
                        pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

                    delete pAssoc->refSecurityAssociation;
                    pAssoc->refSecurityAssociation = nullptr;
                }
            }
            else
            {
                delete pAssoc->refSecurityAssociation;
                pAssoc->refSecurityAssociation = nullptr;
            }
        }
        else
        {
            if (pAssoc->refSecureDeviceAddr != nullptr)
            {
                delete pAssoc->refSecureDeviceAddr;
                pAssoc->refSecureDeviceAddr = nullptr;
            }

            if (pAssoc->refRemoteCanonicalName != nullptr)
            {
                delete pAssoc->refRemoteCanonicalName;
                pAssoc->refRemoteCanonicalName = nullptr;
            }

            if (pAssoc->refRemotePort!= nullptr)
            {
                delete pAssoc->refRemotePort;
                pAssoc->refRemotePort = nullptr;
            }

            _ProtoMangleRemoveFromKillList(pRef, pAssoc);

            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] releasing security association data space for %s %d\n",
                pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

            DirtyMemFree(pAssoc, PROTOMANGLE_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleUpdateInit

    \Description
        Attempt to kick start the secure address resolution cycle

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 12/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleUpdateInit(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    ProtoMangleSecurityAssocT *pKilledAssoc;

    // check if the kill list contains an entry for the exact same client
    for (pKilledAssoc = pRef->pSecurityAssocKillList; pKilledAssoc != NULL; pKilledAssoc = pKilledAssoc->pNext)
    {
        if (pRef->aClients[pAssoc->iClientIndex].refPendingSecureDeviceAddr->Compare(pKilledAssoc->refSecureDeviceAddr) == 0)
        {
            break;
        }
    }

    if (pKilledAssoc == NULL)
    {
        // kick start secure address resolution
        pAssoc->refSecureDeviceAddr = pRef->aClients[pAssoc->iClientIndex].refPendingSecureDeviceAddr;
        delete pRef->aClients[pAssoc->iClientIndex].refPendingSecureDeviceAddr;
        pRef->aClients[pAssoc->iClientIndex].refPendingSecureDeviceAddr = nullptr;

        // the client with the biggest index is always the initiator of the secure device association
        if (pRef->iSelf > pAssoc->iClientIndex)
        {
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_CREATING);
        }
        else
        {
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_LISTENING);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleUpdateCreating

    \Description
        Asynchronous establishment of security association. (locally-initiated)

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleUpdateCreating(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    if (pAssoc->refCreateAssociationOp == nullptr)
    {
        // initiate async op
        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] creating security association for %s %d\n",
            pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

        // save SA wait start tick
        pAssoc->iSAWaitStartTick = NetTick();

        if (pRef->refSecureDeviceAssociationTemplate == nullptr)
        {
            NetPrintf(("protomanglexboxone: [%p][%p] can't create security association because template loading failed\n", pRef, pAssoc));
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
            return;
        }

        _ProtoMangleCreateSA(pRef, pAssoc);
    }
    else
    {
        NetCritEnter(&pRef->MangleCrit);

        // ignore if the completion handler has just executed
        if ((pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING) && (pAssoc->refSecurityAssociation == nullptr))
        {
            // should we continue waiting?
            if (NetTickDiff(NetTick(), pAssoc->iSAWaitStartTick) > pRef->iAssocCreateTimeout)
            {
                // time out!
                NetPrintf(("protomanglexboxone: [%p][%p] creation of security association for %s %d timed out\n",
                    pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

                // move on
                _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_TIMEDOUT);
            }
        }

        NetCritLeave(&pRef->MangleCrit);
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleUpdateListening

    \Description
        Asynchronous establishment of security association.  (remotely-initiated)

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 09/08/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleUpdateListening(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    if (pAssoc->bInitialScanDone == FALSE)
    {
        // save SA wait start tick
        pAssoc->iSAWaitStartTick = NetTick();

        // check for an already existing security association
        pAssoc->refSecurityAssociation = _ProtoMangleCheckForExistingSA(pRef, pAssoc);

        pAssoc->bInitialScanDone = TRUE;

        #if DIRTYCODE_LOGGING
        if (pAssoc->refSecurityAssociation)
        {
            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] found existing security association during initial scan\n", pRef, pAssoc));
        }
        #endif
    }

    // deal with found security association (either found during the initial scan or while processing incoming remote-initiated SA)
    if (pAssoc->refSecurityAssociation != nullptr)
    {
        try
        {
            pAssoc->refRemoteCanonicalName =pAssoc->refSecurityAssociation->RemoteHostName->CanonicalName;
            pAssoc->refRemotePort = pAssoc->refSecurityAssociation->RemotePort;
            _ProtoMangleRemoteHostAddRef(pRef, pAssoc->refRemoteCanonicalName + L":" + pAssoc->refRemotePort);

            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p][%p] successfully found security association for %s %d (%S:%S)\n",
                pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex,
                pAssoc->refRemoteCanonicalName->Data(), pAssoc->refRemotePort->Data()));

            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_DONE);
        }
        catch(Exception^ e)
        {
            _ProtoMangleExceptionReport(pRef, e, "failed to use security association properties (%p)", pAssoc);
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_FAILED);
        }
    }
    else
    {
        // should we continue waiting?
        if (NetTickDiff(NetTick(), pAssoc->iSAWaitStartTick) > pRef->iAssocCreateTimeout)
        {
            // time out!
            NetPrintf(("protomanglexboxone: [%p][%p] listening for security association for %s %d timed out\n",
                pRef, pAssoc, (pAssoc->bInKillList?"killed clnt formerly at idx":"actv clnt at idx"), pAssoc->iClientIndex));

            // move on
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_TIMEDOUT);
        }
    }
}


/*F********************************************************************************/
/*!
    \Function    _ProtoMangleUpdateAssoc

    \Description
        Asynchronous establishment of security association.

    \Input *pRef        - reference pointer
    \Input *pAssoc      - security association data space pointer

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _ProtoMangleUpdateAssoc(ProtoMangleRefT *pRef, ProtoMangleSecurityAssocT *pAssoc)
{
    /*
    force cleanup of security association data space when:
        1 - it is init state (init) or it has been moved to a termination state (done, failed, timedout) or it has no async op in progress (listening), and
        2 - it has been moved to the kill list, and
        3 - the async op for creating the security association is no longer in progress (timedout state can be reached with that operation is still in progress)
    */
    if ((pAssoc->eMangleState == PROTOMANGLE_STATE_INIT) ||
        (pAssoc->eMangleState == PROTOMANGLE_STATE_LISTENING) ||
        (pAssoc->eMangleState == PROTOMANGLE_STATE_DONE) ||
        (pAssoc->eMangleState == PROTOMANGLE_STATE_FAILED) ||
        (pAssoc->eMangleState == PROTOMANGLE_STATE_TIMEDOUT))
    {
        if (pAssoc->bInKillList && (pAssoc->refCreateAssociationOp == nullptr))
        {
            _ProtoMangleMoveToNextState(pRef, pAssoc, PROTOMANGLE_STATE_CLEANING);
        }
    }

    // attempt to kick start the secure device address resolution
    if (pAssoc->eMangleState == PROTOMANGLE_STATE_INIT)
    {
        _ProtoMangleUpdateInit(pRef, pAssoc);
    }

    // waiting for completion of SecureDeviceAssociation creation (locally-initiated)
    if (pAssoc->eMangleState == PROTOMANGLE_STATE_CREATING)
    {
        _ProtoMangleUpdateCreating(pRef, pAssoc);
    }

    // waiting for completion of SecureDeviceAssociation creation (remotely-initiated)
    if (pAssoc->eMangleState == PROTOMANGLE_STATE_LISTENING)
    {
        _ProtoMangleUpdateListening(pRef, pAssoc);
    }

    // cleaning previously allocated resources for the current client index
    // note: must be at end of function because pAssoc may no longer be valid when _ProtoMangleUpdateCleaning() returns
    if (pAssoc->eMangleState == PROTOMANGLE_STATE_CLEANING)
    {
        _ProtoMangleUpdateCleaning(pRef, pAssoc);
    }
}

/*F********************************************************************************/
/*!
    \Function    _ProtoMangleDestroyInternal

    \Description
        Destroy the module and release its state

    \Input *pDestroyData    - pointer to ProtoMangle ref

    \Output
       int32_t              - 0 = success; -1 = try again; other negative=error

    \Version 04/29/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _ProtoMangleDestroyInternal(void *pDestroyData)
{
    ProtoMangleRefT *pRef = (ProtoMangleRefT *)pDestroyData;
    int32_t iClientIndex;
    int32_t iResult = -1;   // default to try again

    // at this point, ProtoMangleDestroy was called and ProtoMangleUpdate() is no longer pumped externally
    // so we pump it implicitly
    ProtoMangleUpdate(pRef);

    // loop through all security association data spaces and add all valid entries to the kill list
    for (iClientIndex = 0; iClientIndex < pRef->iMaxClients; iClientIndex++)
    {
        if (pRef->aClients[iClientIndex].pAssoc != NULL)
        {
            _ProtoMangleAddToKillList(pRef, pRef->aClients[iClientIndex].pAssoc);
        }
    }

    // only proceed with module destruction when kill list is empty
    if (pRef->pSecurityAssocKillList == NULL)
    {
        // remove from active list, acquire the global crit to ensue that the manipulation succeeds before we kill the mangle crit
        NetCritEnter(&_ProtoMangle_State.StateCrit);
        if (_ProtoMangle_State.pActiveList == pRef)
        {
            _ProtoMangle_State.pActiveList = _ProtoMangle_State.pActiveList->pNext;
        }
        else
        {
            ProtoMangleRefT *pCurrent, *pPrevious;
            for (pCurrent = _ProtoMangle_State.pActiveList->pNext, pPrevious = _ProtoMangle_State.pActiveList; pCurrent != NULL; pPrevious = pCurrent, pCurrent = pCurrent->pNext)
            {
                if (pCurrent != pRef)
                {
                    continue;
                }

                pPrevious->pNext = pCurrent->pNext;
                break;
            }
        }
        NetCritLeave(&_ProtoMangle_State.StateCrit);

        // make sure all pending secure device addr are being cleaned up
        for (iClientIndex = 0; iClientIndex < pRef->iMaxClients; iClientIndex++)
        {
            if (pRef->aClients[iClientIndex].refPendingSecureDeviceAddr != nullptr)
            {
                delete pRef->aClients[iClientIndex].refPendingSecureDeviceAddr;
                pRef->aClients[iClientIndex].refPendingSecureDeviceAddr = nullptr;
            }
        }

        if (pRef->refSecureDeviceAssociationTemplate != nullptr)
        {
            _ProtoMangleUnregisterIncomingAssociationEventHandler(pRef);

            if (pRef->refIncomingSecurityAssocVect != nullptr)
            {
                pRef->refIncomingSecurityAssocVect->Clear();
                pRef->refIncomingSecurityAssocVect = nullptr;
            }

            NetCritKill(&pRef->MangleCrit);

            delete pRef->refSecureDeviceAssociationTemplate;
            pRef->refSecureDeviceAssociationTemplate = nullptr;
        }

        _ProtoMangle_State.iRefCount -= 1;
        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] ref count on global protomangle state decremented to %d\n", pRef, _ProtoMangle_State.iRefCount));
        if (_ProtoMangle_State.iRefCount == 0)
        {
            _ProtoMangle_bStateInitialized = FALSE;
            NetCritKill(&_ProtoMangle_State.StateCrit);
            _ProtoMangle_State.refRemoteHostRefCountMap->Clear();
            _ProtoMangle_State.refRemoteHostRefCountMap = nullptr;
            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] global protomangle state destroyed\n", pRef));
        }

        pRef->refScid = nullptr;
        pRef->refSessionTemplateName = nullptr;
        pRef->refSessionName = nullptr;

        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] destruction complete\n", pRef));

        // release ref
        DirtyMemFree(pRef, PROTOMANGLE_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

        iResult = 0;   // success
    }

    return(iResult);  // try again
}

/*** Public Functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function    ProtoMangleCreate

    \Description
        Allocate module state and prepare for use

    \Input *pServer         - ignored
    \Input iPort            - max number of clients
    \Input *pGameID         - ignored
    \Input *pLKey           - multiplayer session id

    \Output
        ProtoMangleRefT *   - reference pointer (must be passed to all other functions)

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
ProtoMangleRefT *ProtoMangleCreate(const char *pServer, int32_t iPort, const char *pGameID, const char *pLKey)
{
    ProtoMangleRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    String ^refTemplateName = "GameUdpTemplate";
    SecureDeviceAssociationTemplate ^refTemplate;
    int32_t iSize;
    int32_t iMaxClients = iPort;

    // query netconn's mem group data (because we use the netconn's externcal cleanup feature)
    iMemGroup = NetConnStatus('mgrp', 0, &pMemGroupUserData, sizeof(pMemGroupUserData));

    // allocate and init module state
    iSize = sizeof(*pRef) + (sizeof(ProtoMangleClientT) * (iMaxClients - 1));
    if ((pRef = (ProtoMangleRefT *) DirtyMemAlloc(iSize, PROTOMANGLE_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protomanglexboxone: [%p] could not allocate module state\n", pRef));
        return(NULL);
    }
    ds_memclr(pRef, iSize);
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;
    pRef->iVerbose = 1;
    pRef->iMaxClients = iMaxClients;
    pRef->iAssocCreateTimeout = PROTOMANGLE_SECURITYASSOC_CREATION_TIMEOUT;
    // allocate the vector used to store incoming remotely-initiated security associations
    pRef->refIncomingSecurityAssocVect = ref new Platform::Collections::Vector<SecureDeviceAssociation ^>();

    // create singleton map (shared across all instances of protomanglexboxone) used to track ref counts on security associations
    if (_ProtoMangle_bStateInitialized == FALSE)
    {
        ds_memclr(&_ProtoMangle_State, sizeof(_ProtoMangle_State));
        NetCritInit(&_ProtoMangle_State.StateCrit, "_ProtoMangle_State");
        _ProtoMangle_bStateInitialized = TRUE;
        NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] global protomangle state initialized\n", pRef));
    }
    if (_ProtoMangle_State.iRefCount == 0)
    {
        _ProtoMangle_State.refRemoteHostRefCountMap = ref new Platform::Collections::Map<String ^, int>();
    }
    _ProtoMangle_State.iRefCount += 1;
    NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] ref count on global protomangle state incremented to %d\n", pRef, _ProtoMangle_State.iRefCount));

    try
    {
        // retrieve the specified template from the game network manifest
        refTemplate = pRef->refSecureDeviceAssociationTemplate = SecureDeviceAssociationTemplate::GetTemplateByName(refTemplateName);
    }
    catch (Exception ^ e)
    {
        _ProtoMangleExceptionReport(pRef, e, "SecureDeviceAssociationTemplate::GetTemplateByName('%S') failed; secure communications will be disabled\n", refTemplateName->ToString()->Data());
        ProtoMangleDestroy(pRef);
        return(NULL);
    }

    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] loaded template '%S'\n", pRef, refTemplateName->ToString()->Data()));

    if (_ProtoMangleRegisterIncomingAssociationEventHandler(pRef) != 0)
    {
        NetPrintf(("protomanglexboxone: [%p] failed to register incoming association event handler; secure communications will be disabled\n", pRef));
        ProtoMangleDestroy(pRef);
        return(NULL);
    }

    NetCritInit(&pRef->MangleCrit, "protomangle");

    // add ref to active list at head
    pRef->pNext = _ProtoMangle_State.pActiveList;
    _ProtoMangle_State.pActiveList = pRef;

    // return ref to caller
    return(pRef);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleDestroy

    \Description
        Destroy the module and release its state

    \Input *pRef    - reference pointer

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
void ProtoMangleDestroy(ProtoMangleRefT *pRef)
{
    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] shutting down\n", pRef));

    // transfer protomanglexboxone destruction ownership to NetConn
    NetConnControl('recu', 0, 0, (void *)_ProtoMangleDestroyInternal, pRef);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleConnect2

    \Description
        Initiate asynchronous resolution of secure address

    \Input *pRef        - module state
    \Input iClientIndex - client index
    \Input *pAddr       - pointer to SecureDeviceAddress of other xboxone device
    \Input iAddrSize    - size of SecureDeviceAddress, or size of client certificate for dedicated server

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
void ProtoMangleConnect2(ProtoMangleRefT *pRef, int32_t iClientIndex, const char *pAddr, int32_t iAddrSize)
{
    // sanity checks
    if (pRef->aClients[iClientIndex].refPendingSecureDeviceAddr != nullptr)
    {
        NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] attempt to initiate a new address resolution while another one is already pending for clnt idx %d\n",
            pRef, iClientIndex));
        return;
    }
    if (pRef->aClients[iClientIndex].pAssoc != NULL)
    {
        NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] attempt to initiate a new address resolution while another one is already ongoing for clnt idx %d\n",
            pRef, iClientIndex));
        return;
    }

    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] initiating resolution of secure address for clnt idx %d\n", pRef, iClientIndex));

    if (pRef->iVerbose > 1)
    {
        NetPrintMem(pAddr, iAddrSize, "remote-secure-addr-blob");
    }

    // get secure device address
    try
    {
        Array<uint8_t> ^refHostAddrBuf = ref new Array<uint8_t>((uint8_t*)pAddr, (uint32_t)iAddrSize);
        pRef->aClients[iClientIndex].refPendingSecureDeviceAddr = SecureDeviceAddress::FromBytes(refHostAddrBuf);
    }
    catch (Exception ^ e)
    {
        NetPrintf(("protomanglexboxone: [%p] exception caught while trying instantiate secure device address (err=%s: %S)\n", pRef, DirtyErrGetName(e->HResult), e->Message->Data()));
        return;
    }

    // allocate security association data space
    if ((pRef->aClients[iClientIndex].pAssoc = (ProtoMangleSecurityAssocT *) DirtyMemAlloc(sizeof(ProtoMangleSecurityAssocT), PROTOMANGLE_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("protomanglexboxone: [%p] could not allocate security association data space for clnt idx %d\n", pRef, iClientIndex));
        return;
    }
    ds_memclr(pRef->aClients[iClientIndex].pAssoc, sizeof(ProtoMangleSecurityAssocT));

    pRef->aClients[iClientIndex].pAssoc->iClientIndex = iClientIndex;
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleConnect

    \Description
        no-op on XboxOne

    \Input *pRef    - module state
    \Input iMyPort  - (ignored on XboxOne)
    \Input *pSessID - (ignored on XboxOne)

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
void ProtoMangleConnect(ProtoMangleRefT *pRef, int32_t iMyPort, const char *pSessID)
{
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleConnectSocket

    \Description
        no-op on XboxOne

    \Input *pRef    - module state
    \Input uSockRef - (ignored on XboxOne)
    \Input *pSessID - (ignored on XboxOne)

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
void ProtoMangleConnectSocket(ProtoMangleRefT *pRef, intptr_t uSockRef, const char *pSessID)
{
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleUpdate

    \Description
        Update module state

    \Input *pRef    - module reference

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
void ProtoMangleUpdate(ProtoMangleRefT *pRef)
{
    int32_t iClientIndex;
    ProtoMangleSecurityAssocT *pCurrent, *pNext;

    // deal with killed clients
    for (pCurrent = pRef->pSecurityAssocKillList; pCurrent != NULL; pCurrent = pNext)
    {
        pNext = pCurrent->pNext;  // save pNext right away as pCurrent may be freed and removed from the list in the next call 
        _ProtoMangleUpdateAssoc(pRef, pCurrent);
    }

    // deal with active clients
    for (iClientIndex = 0; iClientIndex < pRef->iMaxClients; iClientIndex++)
    {
        if (pRef->aClients[iClientIndex].pAssoc != NULL)
        {
            _ProtoMangleUpdateAssoc(pRef, pRef->aClients[iClientIndex].pAssoc);
        }
    }

    // deal with incoming remotely-initiated associations
    _ProtoMangleProcessIncomingSAs(pRef);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleComplete

    \Description
        Returns TRUE and fills in address and port if secure address resolution finished
        successfully.

    \Input *pRef    - module state
    \Input *pAddr   - storage for address (result==TRUE)
    \Input *pPort   - (ignored on XboxOne)

    \Output
        int32_t     - >0=complete; 0=tryagain; <0=error

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t ProtoMangleComplete(ProtoMangleRefT *pRef, int32_t *pAddr, int32_t *pPort)
{
    int32_t iResult = 1; // default to success
    int32_t iClientIndex = *pPort;

    switch (pRef->aClients[iClientIndex].pAssoc->eMangleState)
    {
        case PROTOMANGLE_STATE_INIT:
            iResult = 0; // wait for the kill list to no longer contain an entry for the exact same client, try again
            break;

        case PROTOMANGLE_STATE_CREATING:
        case PROTOMANGLE_STATE_LISTENING:
            iResult = 0; // in progress, try again
            break;

        case PROTOMANGLE_STATE_DONE:
            {
                ProtoMangleSecurityAssocT *pAssoc = pRef->aClients[iClientIndex].pAssoc;

                if (_ProtoMangleAddIPv6ToIPv4Mapping(pRef, pAssoc) == 0)
                {
                    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p][%p] successfully obtained secure address %a for actv clnt at idx %d (%S:%S)\n",
                        pRef, pRef->aClients[iClientIndex].pAssoc, pAssoc->iIpv4Addr, iClientIndex, pAssoc->refRemoteCanonicalName->Data(), pAssoc->refRemotePort->Data()));

                    // return address to caller
                    if (pAddr != NULL)
                    {
                        *pAddr = pAssoc->iIpv4Addr;
                    }
                }
                else
                {
                    NetPrintf(("protomanglexboxone: [%p][%p] critical error in ProtoMangleComplete() - failed to add ipv6 to ipv4 mapping\n", pRef, pRef->aClients[iClientIndex].pAssoc));
                    iResult = -1;
                }
            }

            break;

        case PROTOMANGLE_STATE_CLEANING:
            NetPrintf(("protomanglexboxone: [%p][%p] critical error in ProtoMangleComplete() - invalid state (%s)\n", 
                pRef, pRef->aClients[iClientIndex].pAssoc, _strStates[pRef->aClients[iClientIndex].pAssoc->eMangleState]));
            iResult = -2;
            break;

        case PROTOMANGLE_STATE_FAILED:
            iResult = -3;
            break;

        case PROTOMANGLE_STATE_TIMEDOUT:
            iResult = -4;
            break;

        default:
            NetPrintf(("protomanglexboxone: [%p][%p] critcal error in ProtoMangleComplete() - unknown internal state\n", pRef, pRef->aClients[iClientIndex].pAssoc));
            iResult = -5;
            break;
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleReport

    \Description
        no-op on XboxOne

    \Input *pRef    - mangle ref
    \Input eStatus  - (ignored on XboxOne)
    \Input iLatency - (ignored on XboxOne)

    \Output
        int32_t     - zero=success, negative=failure

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t ProtoMangleReport(ProtoMangleRefT *pRef, ProtoMangleStatusE eStatus, int32_t iLatency)
{
    return(0);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleControl

    \Description
        ProtoMangle control function.

    \Input *pRef    - mangle ref
    \Input iControl - control selector
    \Input iValue   - selector specific
    \Input iValue2  - selector specific
    \Input *pValue  - selector specific

    \Notes
        iControl can be one of the following:

        \verbatim
            abrt: abort currently ongoing secure address resolution
            cert: starts client certificate request async operation
            exsn: set the external session name for the MultiplayerSessionReference
            exst: set the external session template name for the MultiplayerSessionReference
            remv: remove specified client (release matching security association)
            scid: set the service configuration id for the MultiplayerSessionReference
            self: set local client index
            spam: sets debug output verbosity (0...n)
            time: set the maximum time waiting for locally-initiated async security association creation
        \endverbatim

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t ProtoMangleControl(ProtoMangleRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, const void *pValue)
{
    if (iControl == 'abrt')
    {
        int32_t iClientIndex = iValue;

        if (pRef->aClients[iClientIndex].pAssoc)
        {
            NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p][%p] aborting actv clnt at index %d while in state %s\n",
                pRef, pRef->aClients[iClientIndex].pAssoc, iClientIndex, _strStates[pRef->aClients[iClientIndex].pAssoc->eMangleState]));

            _ProtoMangleAddToKillList(pRef, pRef->aClients[iClientIndex].pAssoc);
        }
        else
        {
            NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] aborting actv clnt at index %d ignored\n", pRef, iClientIndex));
        }

        return(0);
    }

    if (iControl == 'spam')
    {
        NetPrintf(("protomanglexboxone: [%p] debug verbosity level changed from %d to %d\n", pRef, pRef->iVerbose, iValue));
        pRef->iVerbose = iValue;
        return(0);
    }

    if (iControl == 'exsn')
    {
        if (pValue != NULL)
        {
            wchar_t wstrValue[256+1];
            mbstowcs(wstrValue, (const char8_t*)pValue, sizeof(wstrValue) / 2);
            pRef->refSessionName = ref new String(wstrValue);
            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] setting the external session name to (%S)\n", pRef, pRef->refSessionName->Data()));
            return(0);
        }
        NetPrintf(("protomanglexboxone: [%p] invalid value (NULL) passed in for the external session name\n", pRef));
        return(-1);
    }

    if (iControl == 'exst')
    {
        if (pValue != NULL)
        {
            wchar_t wstrValue[256+1];
            mbstowcs(wstrValue, (const char8_t*)pValue, sizeof(wstrValue) / 2);
            pRef->refSessionTemplateName = ref new String(wstrValue);
            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] setting the external session template name to (%S)\n", pRef, pRef->refSessionTemplateName->Data()));
            return(0);
        }
        NetPrintf(("protomanglexboxone: [%p] invalid value (NULL) passed in for the external session template name\n", pRef));
        return(-1);
    }

    if (iControl == 'remv')
    {
        int32_t iResult = 0;   // default to success
        int32_t iClientIndex = iValue;

        // ignore remove request for client index -1 which corresponds to the DirtyCast gameserver
        if ((iClientIndex >= 0) && (iClientIndex < pRef->iMaxClients))
        {
            if (pRef->aClients[iClientIndex].pAssoc)
            {
                NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p][%p] explicitly removing actv clnt at index %d while in state %s\n",
                    pRef, pRef->aClients[iClientIndex].pAssoc, iClientIndex, _strStates[pRef->aClients[iClientIndex].pAssoc->eMangleState]));

                _ProtoMangleAddToKillList(pRef, pRef->aClients[iClientIndex].pAssoc);
            }
            else
            {
                NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] explicit removal of actv clnt at index %d ignored\n", pRef, iClientIndex));
            }
        }
        else if (iValue != -1)
        {
            NetPrintf(("protomanglexboxone: [%p] ProtoMangleControl('remv') used with an invalid clnt idx (%d)\n", pRef, iClientIndex));
            iResult = -1;  // signal error
        }
        return(iResult);
    }

    if (iControl == 'scid')
    {
        if (pValue != NULL)
        {
            wchar_t wstrValue[256+1];
            mbstowcs(wstrValue, (const char8_t*)pValue, sizeof(wstrValue) / 2);
            pRef->refScid = ref new String(wstrValue);
            NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] setting service config id to (%S)\n", pRef, pRef->refScid->Data()));
            return(0);
        }
        NetPrintf(("protomanglexboxone: [%p] invalid value (NULL) passed in for the service config id\n", pRef));
        return(-1);
    }

    if (iControl == 'self')
    {
        NetPrintf(("protomanglexboxone: [%p] self client index = %d\n", pRef, iValue));
        pRef->iSelf= iValue;
        return(0);
    }

    if (iControl == 'time')
    {
       NetPrintfVerbose((pRef->iVerbose, 1, "protomanglexboxone: [%p] security association creation timeout changed from %d to %d\n", pRef, pRef->iAssocCreateTimeout, iValue));
       pRef->iAssocCreateTimeout = iValue;
        return(0);
    }

    // unhandled
    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] ProtoMangleControl('%C') unhandled\n", pRef, iControl));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    ProtoMangleStatus

    \Description
        Return module status based on input selector.

    \Input *pRef    - mangle ref
    \Input iSelect  - status selector
    \Input *pBuf    - [out] storage for output data
    \Input iBufSize - size of output buffer

    \Notes
        iSelect can be one of the following:

        \verbatim
            idle: true/false indication of whether module is idle or not
            time: true/false indication of whether security assoc timed out
        \endverbatim

    \Version 04/05/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t ProtoMangleStatus(ProtoMangleRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    if (iSelect == 'idle')
    {
        int32_t iClientIndex = iBufSize;

        if ((pRef->aClients[iClientIndex].pAssoc == NULL) && (pRef->aClients[iClientIndex].refPendingSecureDeviceAddr == nullptr))
        {
            return(TRUE);
        }
        else
        {
            return(FALSE);
        }
    }
    if (iSelect == 'time')
    {
        int32_t iClientIndex = iBufSize;

        if ((pRef->aClients[iClientIndex].pAssoc != NULL) && (pRef->aClients[iClientIndex].pAssoc->eMangleState == PROTOMANGLE_STATE_TIMEDOUT))
        {
            return(TRUE);
        }
        else
        {
            return(FALSE);
        }
    }

    // unhandled
    NetPrintfVerbose((pRef->iVerbose, 0, "protomanglexboxone: [%p] ProtoMangleStatus('%C') unhandled\n", pRef, iSelect));
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function ProtoMangleFindTemplate

    \Description
        Find template suitable for given query, based on protocol, local port, and
        remote port.

    \Input *pStrTemplateName- [out] buffer to store template name in
    \Input iTemplateNameSize- size of template name buffer
    \Input iLocalPort       - local (bound) port to check template for
    \Input iRemotePort      - remote (connect) port to check template for
    \Input bTcp             - TRUE if TCP, else UDP
    \Input iVerbose         - debug level
    
    \Output
        const char *        - template name, or NULL if not found

    \Version 09/12/2013 (jbrookes)
*/
/********************************************************************************F*/
const char *ProtoMangleFindTemplate(char *pStrTemplateName, int32_t iTemplateNameSize, int32_t iLocalPort, int32_t iRemotePort, uint8_t bTcp, int32_t iVerbose)
{
    for (int32_t iTemplate = 0; iTemplate < (signed)Windows::Xbox::Networking::SecureDeviceAssociationTemplate::Templates->Size; iTemplate += 1)
    {
        Windows::Xbox::Networking::SecureDeviceAssociationTemplate ^refTemplate;
        try
        {
            refTemplate = Windows::Xbox::Networking::SecureDeviceAssociationTemplate::Templates->GetAt(iTemplate);
        }
        catch (Platform::Exception ^ e)
        {
            NetPrintf(("protomanglexboxone: failed loading SecureDeviceAssociationTemplate %d (err=%s)\n", iTemplate, DirtyErrGetName(e->HResult)));
            break;
        }
        NetPrintfVerbose((iVerbose, 2, "protomanglexboxone: [%02d] %S [%S[%s:%d:%d],%S[%s:%d:%d]]\n", iTemplate,
            refTemplate->Name->ToString()->Data(),
            refTemplate->InitiatorSocketDescription->Name->ToString()->Data(),
            refTemplate->InitiatorSocketDescription->IpProtocol == SecureIpProtocol::Tcp ? "tcp" : "udp",
            refTemplate->InitiatorSocketDescription->BoundPortRangeLower,
            refTemplate->InitiatorSocketDescription->BoundPortRangeUpper,
            refTemplate->AcceptorSocketDescription->Name->ToString()->Data(),
            refTemplate->AcceptorSocketDescription->IpProtocol == SecureIpProtocol::Tcp ? "tcp" : "udp",
            refTemplate->AcceptorSocketDescription->BoundPortRangeLower,
            refTemplate->AcceptorSocketDescription->BoundPortRangeUpper));

        /* 
        weed out templates that don't match what we want
        */

        // check against initiator protocols
        if (bTcp && (refTemplate->InitiatorSocketDescription->IpProtocol != SecureIpProtocol::Tcp))
        {
            NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; mismatched protocol\n", refTemplate->Name->ToString()->Data()));
            continue;
        }

        if (!bTcp && (refTemplate->InitiatorSocketDescription->IpProtocol != SecureIpProtocol::Udp))
        {
            NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; mismatched protocol\n", refTemplate->Name->ToString()->Data()));
            continue;
        }

        // check against initiator ports
        if ((refTemplate->InitiatorSocketDescription->BoundPortRangeLower == 0) && (refTemplate->InitiatorSocketDescription->BoundPortRangeUpper == 0))
        {
            if(iLocalPort < 57344)
            {
                NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; Initiator specifies 'any' but port is not in ephemeral range\n", refTemplate->Name->ToString()->Data()));
                continue;
            }
        }
        else if ((refTemplate->InitiatorSocketDescription->BoundPortRangeLower != 0) && (refTemplate->InitiatorSocketDescription->BoundPortRangeUpper != 0))
        {
            if (iLocalPort < refTemplate->InitiatorSocketDescription->BoundPortRangeLower)
            {
                NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; non-ephemeral port violated Initiator BoundPortRangeLower\n", refTemplate->Name->ToString()->Data()));
                continue;
            }
            if (iLocalPort > refTemplate->InitiatorSocketDescription->BoundPortRangeUpper)
            {
                NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; non-ephemeral port violated Initiator BoundPortRangeUpper\n", refTemplate->Name->ToString()->Data()));
                continue;
            }
        }
        else
        {
            NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; template can’t mix ephemeral port and non-ephemeral port in same port range\n", refTemplate->Name->ToString()->Data()));
            continue;
        }

        // check against acceptor ports
        if ((refTemplate->AcceptorSocketDescription->BoundPortRangeLower != 0) && (refTemplate->AcceptorSocketDescription->BoundPortRangeUpper != 0))
        {
            if (iRemotePort < refTemplate->AcceptorSocketDescription->BoundPortRangeLower)
            {
                NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; violated Acceptor BoundPortRangeLower\n", refTemplate->Name->ToString()->Data()));
                continue;
            }

            if (iRemotePort > refTemplate->AcceptorSocketDescription->BoundPortRangeUpper)
            {
                NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; violated Acceptor BoundPortRangeUpper\n", refTemplate->Name->ToString()->Data()));
                continue;
            }
        }
        else
        {
            NetPrintfVerbose((iVerbose, 1, "protomanglexboxone: rejecting template %S; Acceptor cannot specify 'any'\n", refTemplate->Name->ToString()->Data()));
            continue;
        }

        NetPrintfVerbose((iVerbose, 0, "protomanglexboxone: matched query for [%s:%d:%d] to template '%S'\n",
            bTcp ? "tcp" : "udp", iLocalPort, iRemotePort, refTemplate->Name->ToString()->Data()));
        ds_snzprintf(pStrTemplateName, iTemplateNameSize, "%S", refTemplate->Name->ToString()->Data());
        return(pStrTemplateName);
    }
    return(NULL);
}
