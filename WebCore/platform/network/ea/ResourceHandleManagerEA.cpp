/*
* Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
* Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
* Copyright (C) 2007 Alp Toker <alp@atoker.com>
* Copyright (C) 2007 Holger Hans Peter Freyther
* Copyright (C) 2008 Collabora Ltd.
* Copyright (C) 2008 Nuanti Ltd.
* Copyright (C) 2009 Appcelerator Inc.
* Copyright (C) 2009 Brent Fulgham <bfulgham@webkit.org>
* All rights reserved.
* Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/////////////////////////////////////////////////////////////////////////////
// Resource-related classes:
//     ResourceHandleManager
//     ResourceHandle
//     ResourceHandleInternal
//     ResourceHandleClient
//     ResourceRequest : public ResourceRequestBase
//     ResourceResponse : public ResourceResponseBase
//     
//
// In order to implement the loading of these resources in a way that can
// be plugged in by the user, we add the following classes:
//     TransportHandler
//     TransportInfo
//     TransportServer
//     ResourceHandleManager::JobInfo
//
//		
/////////////////////////////////////////////////////////////////////////////


#include "config.h"
#include "ResourceHandleManagerEA.h"

#include "DataURL.h"
#include <wtf/text/CString.h>
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include "ResourceError.h"
#include "ResourceHandle.h"
#include "ResourceHandleInternal.h"
#include "NetworkingContext.h"
#include "SharedBuffer.h"
#include "HTTPParsers.h"
#include "wtf/text/Base64.h"
#include "Timer.h"
#include <ctype.h>
#include <errno.h>
#include <wtf/Vector.h>
#include <wtf/MainThread.h>
#include <wtf/StringExtras.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>
#include <WebFrame.h>
#include <WebPage.h>
#include <internal/include/EAWebKitFPUPrecision.h>
#include <internal/include/EAWebKitString.h>   // For Stricmp and friends.
#include <internal/include/EAWebKitDomainFilter.h>
#include <internal/include/EAWebKitAssert.h>
#include <EAIO/FnEncode.h>          // For Strlcpy and friends.
#include <EASTL/fixed_substring.h>
#include <internal/include/EAWebkit_p.h>

namespace Local
{
static void GetSystemPathFromFileURL(const WebCore::URL& kurl, EA::WebKit::FixedString8_128& sPath)
{
	WTF::String           sURLPath = WebCore::decodeURLEscapeSequences(kurl.path());
	EA::WebKit::FixedString16_128 sPath16(sURLPath.characters(), sURLPath.length());

	// Look for Microsoft-style paths (e.g. "/C|/windows/test.html", to convert to "C:\windows\test.html")
    if((sPath16[0] == '/') &&
        sPath16[1] &&
        (sPath16[2] == ':' || sPath16[2] == '|'))
    {
        sPath16.erase(0, 1);  // Remove leading '/' char
        sPath16[1] = ':';
    }

	for(eastl_size_t i = 0, iEnd = sPath16.length(); i < iEnd; ++i)
	{
		if(sPath16[i] == '/')
			sPath16[i] = '\\';
	}

    EA::WebKit::ConvertToString8(sPath16, sPath);
}

EA::WebKit::View* GetViewFromResourceHandle(WebCore::ResourceHandle* pRH)
{
	const WebCore::NetworkingContext* context = pRH->getInternal()->m_context.get();
	if(context && context->isValid())
	{
		EA::WebKit::WebFrame* originatingFrame = (EA::WebKit::WebFrame*) (context->originatingObject());
		return originatingFrame->page()->view();
	}

	EAW_ASSERT_MSG(false,"No view found with this job. This is rare.");
	return NULL;
}

    
} // namespace Local


namespace WebCore 
{




/////////////////////////////////////////////////////////////////////////////
// ResourceHandleManager
/////////////////////////////////////////////////////////////////////////////
ResourceHandleManager* ResourceHandleManager::m_pInstance = NULL;

const double kPollTimeSeconds = 0.016; //60 Frames per Second
ResourceHandleManager::ResourceHandleManager()
    : m_downloadTimer(this, &ResourceHandleManager::downloadTimerCallback)
    , m_pendingResourceHandleList()
    , m_runningJobs(0)
    , m_pollTimeSeconds(kPollTimeSeconds)
    , m_maxConcurrentJobs(16)
    , m_maxParallelConnectionsPerHost(6)
    , m_cookieManager()
    , m_THInfoList()
    , m_JobInfoList()
    , m_JobIdNext(0)
    , m_CondemnedJobsExist(false)
#if _DEBUG
	, m_readVolume(0)
    , m_writeVolume(0)
#endif
{
	m_pollTimeSeconds   = kPollTimeSeconds;
}


ResourceHandleManager::~ResourceHandleManager()
{
    // Before we kill ResourceHandleManager, we
	// 1. Mark all jobs for removal,
	CondemnAllJobs();
	// 2. Run the Job loop which cleans up everything since we already marked all jobs for removal,  
    ProcessJobs();
	// 3. Remove the transport handlers.
    RemoveTransportHandlers();

	m_THDiskCache.Shutdown(NULL);
    m_cookieManager.Shutdown();
}


ResourceHandleManager* ResourceHandleManager::sharedInstance()
{
    if(!m_pInstance)
        m_pInstance = new ResourceHandleManager;

    return m_pInstance;
}


void ResourceHandleManager::finalize()
{
    if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

// This is called from ResourceHandle::start() which gets called from the Core code. We can think of this function
// as an entry point into our resource loading system.
// The following is applicable only for the asynchronous resources. Synchronous resources (slightly uncommon as it means literally
// freezing the Browser) are loaded via ResourceHandle::loadResourceSynchronously() which calls into ResourceHandleManager::loadResourceSynchronously().
void ResourceHandleManager::add(ResourceHandle* pRH)
{
	// This ref is done because we want the ResourceHandle to exist beyond the call to the didFinishLoading() 
	// or didFail(). This is how the state machine is set up.
	pRH->ref();

	// We don't add this ResouceHandle to transfer loop right away to avoid Re-Entrancy issues. This ResourceHandle
	// may be getting added from a Job already in the transfer loop.

	m_pendingResourceHandleList.push_back(pRH);

	if(!m_downloadTimer.isActive())
		m_downloadTimer.startOneShot(m_pollTimeSeconds);
}

// This is called from ResourceHandle::cancel() which gets called from the Core code. For example, this can happen
// when the user navigates to some other page while the current page is loading.
void ResourceHandleManager::cancel(ResourceHandle* pRH)
{
	// If the job is pending but has not started yet.
	if (removePendingJob(pRH))
		return;

	// The job has started. Mark it for removal here. We'll remove it in our ProcessJobs loop.
	ResourceHandleInternal* d = pRH->getInternal();
	d->m_cancelled = true;

}


void ResourceHandleManager::changeResourceHandleState(ResourceHandle* pRH, bool paused)
{
	for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); ++it)
	{
		JobInfo& jobInfo = *it;

		if(jobInfo.mpRH == pRH)
		{
			jobInfo.mbPaused = paused;
			break;
		}
	}
}


// This function needs to synchronously (i.e. blocking in place) do a resource load.
// No deref is required on ResourceHandle because we don't ref it. We only for add a ref to ResourceHandle
// for asynchronous job.
void ResourceHandleManager::loadResourceSynchronously(ResourceHandle* pRH)
{
	URL kurl = pRH->firstRequest().url();

	if (kurl.protocolIsData()) {
		handleDataURL(pRH);
		return;
	}

	ResourceHandleInternal* handle = pRH->getInternal();
	handle->m_defersLoading = false; //Similar to Curl version

	JobInfo jobInfo;
	bool jobInitialized = initializeJob(jobInfo, pRH, true);

	if(jobInitialized)
	{
		bool bStateComplete = false;
		bool bRemoveJob     = false;
		// Keep processing job unless we are asked to remove it. It can happen either normally or
		// if a state fails.
		while(!bRemoveJob)
		{
			ProcessJobState(jobInfo,bStateComplete,bRemoveJob);
			bStateComplete = false;
		}
	}
	else
	{
		jobInfo.mbSuccess = false;
	}

	jobInfo.mJobState = kJSRemove;
	// Success is handled in DataDone. Failure is handled here.
	if(!jobInfo.mbSuccess)
	{
		notifyJobFailed(jobInfo);
	}

	// We promise the TransportHandler that if we Init it then eventually we will Shutdown it. This allows it to clean up.
	if(jobInfo.mbTHInitialized && !jobInfo.mbTHShutdown)
	{
		bool bStateComplete;
		jobInfo.mpTH->ShutdownJob(&jobInfo.mTInfo, bStateComplete);
	}

	// No mpRH->deref() needed because we did not ref() it at the first place since it is synchronous job.
	jobInfo.mpRH = NULL;

}

void ResourceHandleManager::notifyJobFailed(const JobInfo& jobInfo)
{
	EAW_ASSERT_MSG(!jobInfo.mbSuccess, "notifyJobFailed called but jobInfo.mbSuccess is not false\n");

	ResourceHandleInternal* pRHI = jobInfo.mpRH->getInternal();
	//08/19/13 - Skip didFail notification if the job was cancelled as well as failed. Normally, when a job fails in any one of the state, it's jobInfo.mbSuccess is false which causes this 
	// call to be triggered. This function is not intended to be called when a job is cancelled and it is taken care of in the ProcessJobs function.
	// On some rare occasions, the job can be cancelled as well as fail in the same Tick. For example, headers received from the server, job cancelled by the application
	// based on the info and the job itself fail in network layer due to some sort of invalid request. The code below will cause this.
	/*
	if(jobInfo.mpTH->Transfer(&jobInfo.mTInfo, bStateComplete)) -> This can end up cancelling the job as well as report network failure
	{
		if(bStateComplete)
		{
			jobInfo.mJobState = kJSDisconnect;
			NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToDisconnect);
		}
	}
	else
	{
		bRemoveJob = true;
		jobInfo.mbSuccess = false; -> This will end up resulting in this notifyJob being called.
		NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
	}
	*/

	if(pRHI->m_cancelled)
		return;

	ResourceHandleClient* pRHC = pRHI->client();
	if(pRHC)
	{
		const WTF::String sURI(GetFixedString(jobInfo.mTInfo.mURI)->data(), GetFixedString(jobInfo.mTInfo.mURI)->length());
		const WTF::String sError("Transport job failed.");

		ResourceError error(sURI, EA::WebKit::kLETTransport, sURI, sError);
		pRHC->didFail(jobInfo.mpRH, error);
	}
}

void ResourceHandleManager::downloadTimerCallback(Timer<ResourceHandleManager>* /*timer*/)
{
    int runningHandles = 0;

    startPendingJobs();

    runningHandles += ProcessJobs();

    const bool started = startPendingJobs(); // new jobs might have been added in the meantime

    if(!m_downloadTimer.isActive() && (started || (runningHandles > 0) ))
        m_downloadTimer.startOneShot(m_pollTimeSeconds);

}


// Removes a job from our list of jobs that are queued for processing but haven't started yet.
bool ResourceHandleManager::removePendingJob(ResourceHandle* pRH)
{
	for(ResourceHandleList::iterator it = m_pendingResourceHandleList.begin(); it != m_pendingResourceHandleList.end(); ++it)
	{
		if(*it == pRH)
		{
			m_pendingResourceHandleList.remove(pRH);
			pRH->deref();
			return true;
		}
	}
	return false;
}


// Goes through our list of jobs that are queued and starts them. 
// Then removes them from the queue (m_pendingResourceHandleList).
bool ResourceHandleManager::startPendingJobs()
{
    // To consider: Come up with an algorithm to sort the various resources download priority order.
    bool started = false;
    while (!m_pendingResourceHandleList.empty() && (m_runningJobs < m_maxConcurrentJobs))
    {
        ResourceHandle* pRH = m_pendingResourceHandleList.front();
        m_pendingResourceHandleList.pop_front();  
        startPendingJob(pRH);
        started = true;
    }

    return started;
}

void ResourceHandleManager::startPendingJob(ResourceHandle* pRH)
{
    const URL& kurl = pRH->firstRequest().url();

	if (kurl.protocolIsData()) {
		handleDataURL(pRH);
		pRH->deref();
		return;
	}

	JobInfo& jobInfo = m_JobInfoList.push_back();
	bool jobInitialized = initializeJob(jobInfo, pRH, false);
	if(jobInitialized)
	{
		m_runningJobs++;
	}
	else
	{
		jobInfo.mpRH = pRH;
		jobInfo.mbSuccess = false;
		notifyJobFailed(jobInfo);
		pRH->deref();
		m_JobInfoList.pop_back();
	}
}

 
bool ResourceHandleManager::initializeJob(JobInfo& jobInfo, ResourceHandle* pRH, bool bSynchronous)
{
	URL kurl = pRH->firstRequest().url();
	kurl.removeFragmentIdentifier();// Remove any fragment part before sending the request.

	ResourceHandleInternal* pRHI = pRH->getInternal();
	String url		= kurl.string();
	String sScheme	= kurl.protocol();
	auto schemeChars = sScheme.charactersWithNullTermination();
	const char16_t* pScheme = schemeChars.data();

	EA::WebKit::TransportHandler* pTH = GetTransportHandler(pScheme);
	if(!pTH)
	{
#if _DEBUG
		// We cheat here a bit by assuming the url to be ascii encoded which is usually the case. This
		// allows for better debug info.
		const int kMaxSchemeLength= 32;
		char8_t scheme8[kMaxSchemeLength];
		size_t i=0;
		for(; i<kMaxSchemeLength-1 && i<EA::Internal::Strlen(pScheme); ++i)
		{
			scheme8[i] = pScheme[i];
		}
		
		scheme8[i] = '\0';

		EAW_ASSERT_FORMATTED(pTH, "No transport handler exists for \"%s\" transport scheme\n", &scheme8[0]);
#endif
		EA::WebKit::View* pView = Local::GetViewFromResourceHandle(pRH);
		if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(pView))
		{
			EA::WebKit::WatchDogNotificationInfo info;
			info.mpView = pView;
			info.mpUserData = pView ? pView->GetUserData() : NULL;
			info.mWatchDogNotificationType = EA::WebKit::WatchDogNotificationInfo::kUnknownTransportHandler;

			pClient->WatchDogNotification(info);
		}
		
		// We are not capable of handling this job. 
		return false;
	}

	if (kurl.isLocalFile() || pTH->IsLocalFile()) 
	{
		String query = kurl.query();
		// Remove any query part sent to a local file.
		if (!query.isEmpty()) {
			int queryIndex = url.find(query);
			if (queryIndex != -1)
				url = url.left(queryIndex - 1);
		}
		// Determine the MIME type based on the path.
		pRHI->m_response.setMimeType(MIMETypeRegistry::getMIMETypeForPath(url));
	}

	using namespace EA::WebKit;

	jobInfo.mId					= ++m_JobIdNext;
	jobInfo.mProcessInfo.mJobId = jobInfo.mId;
	jobInfo.mpRH				= pRH;
	jobInfo.mJobState			= kJSInit;
	jobInfo.mpTH				= pTH;
	jobInfo.mTInfo.mbAsync		= !bSynchronous;
	jobInfo.mbPaused			= pRHI->m_defersLoading; // Some jobs may start in pause state.

	GetFixedString(jobInfo.mTInfo.mURI)->assign(url.characters(), url.length());
	GetFixedString(jobInfo.mTInfo.mEffectiveURI)->assign(GetFixedString(jobInfo.mTInfo.mURI)->c_str());
	EA::IO::EAIOStrlcpy16(jobInfo.mTInfo.mScheme, pScheme, sizeof(jobInfo.mTInfo.mScheme) / sizeof(jobInfo.mTInfo.mScheme[0]));
	jobInfo.mTInfo.mPort = kurl.port();

	if(EA::Internal::Stricmp(jobInfo.mTInfo.mScheme, EA_CHAR16("file")) == 0)
		Local::GetSystemPathFromFileURL(kurl, *GetFixedString(jobInfo.mTInfo.mPath));

	const WebCore::HTTPHeaderMap& customHeaders = pRH->firstRequest().httpHeaderFields(); // typedef HashMap<String, String, CaseFoldingHash> HTTPHeaderMap;
	WebCore::HTTPHeaderMap::const_iterator end = customHeaders.end();

	// We translate WebCore::HTTPHeaderMap to EA::WebKit::HeaderMap.
	for(WebCore::HTTPHeaderMap::const_iterator it = customHeaders.begin(); it != end; ++it)
	{
		const WebCore::HTTPHeaderMap::KeyValuePairType& wcValue = *it;
		EA::WebKit::HeaderMap::value_type eaValue(HeaderMap::key_type(wcValue.key.characters(), wcValue.key.length()), HeaderMap::mapped_type(wcValue.value.characters(), wcValue.value.length()));

		GetHeaderMap(jobInfo.mTInfo.mHeaderMapOut)->insert(eaValue);
	}

	// Nothing to do. The transport handler fills this in.
	// jobInfo.mTInfo.mHeaderMapIn;

	jobInfo.mTInfo.mResultCode     = 0;  // We expect an HTTP-style code such as 200 or 404.

	const String& method = pRH->firstRequest().httpMethod();

	// Wondering what happens for a custom transport handler. 
	if(equalIgnoringCase(method,"GET"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypeGET;
	else if(equalIgnoringCase(method,"HEAD"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypeHEAD;
	else if(equalIgnoringCase(method,"POST"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypePOST;
	else if(equalIgnoringCase(method,"PUT"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypePUT;
	else if(equalIgnoringCase(method,"DELETE"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypeDELETE;
	else if(equalIgnoringCase(method,"OPTIONS"))
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypeOPTIONS;
	else // seems like this can be called for a custom transport handler 
		jobInfo.mTInfo.mHttpRequestType = EA::WebKit::kHttpRequestTypeUnknown;//EAW_ASSERT_MSG(false, "Unknown http method\n");*/


	// The mTimeout here means the total time available for a resource to be downloaded. This is different from the page time out or network time out
	// which means the time difference between the bytes received from the server by the client. The network time out is more useful. The resource time
	// out is set to the max value by default in WebCore and we keep it that way.
	jobInfo.mTInfo.mTimeoutInterval = pRH->firstRequest().timeoutInterval();
	jobInfo.mTInfo.mTimeout = EA::WebKit::GetTime() + jobInfo.mTInfo.mTimeoutInterval;

	// Set other values
	const EA::WebKit::Parameters& parameters = EA::WebKit::GetParameters();

	jobInfo.mTInfo.mbVerifySSLCert          = parameters.mVerifySSLCert;
	jobInfo.mTInfo.mpTransportServerJobInfo = &jobInfo;
	jobInfo.mTInfo.mpRH                     = pRH;
	jobInfo.mTInfo.mpView                   = Local::GetViewFromResourceHandle(pRH);
	jobInfo.mTInfo.mpTransportServer        = this;
	jobInfo.mTInfo.mpTransportHandler       = pTH;
	jobInfo.mTInfo.mTransportHandlerData    = 0;
	jobInfo.mTInfo.mpCookieManager          = &m_cookieManager;

	if(jobInfo.mTInfo.mHttpRequestType == EA::WebKit::kHttpRequestTypePOST)
		SetupTHPost(&jobInfo);
	else if(jobInfo.mTInfo.mHttpRequestType == EA::WebKit::kHttpRequestTypePUT)
		SetupTHPut(&jobInfo);

	//Nicki Vankoughnett:  This is where we decide if we want to load via the cache.
	//first, we decide if we need to invalidate the cache for this object, assuming said cache exists
	m_THDiskCache.InvalidateCachedDataIfRequired(&jobInfo.mTInfo);

	//Now we Determine if we can redirect this job to the local file cache.  
	if(m_THDiskCache.IsCachedDataValid(GetFixedString(jobInfo.mTInfo.mURI)->c_str()))
	{    
		jobInfo.mpTH = &m_THDiskCache;
		jobInfo.mProcessInfo.mProcessType = EA::WebKit::kVProcessTypeFileCacheJob;
	}    
	
	// Attach cookie headers
	m_cookieManager.OnHeadersSend(&jobInfo.mTInfo);

	jobInfo.mProcessInfo.mURI = &jobInfo.mTInfo.mURI;
	NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToInit);
	// Note by Arpit Baldeva: We want all the links to be filtered as per our domain filter. There does not seem a central/well known place in the 
	//code base where every link makes through. But this place does. So putting it here.
	//Ideally, I'd like to integrate it with an in-built security module for navigation but could not find that either.
	//Domain filter call is efficient in the sense that if the user has not set it up, it is just a "if" check (+ func call).
	if(!EA::WebKit::EAWebKitDomainFilter::GetInstance().CanNavigateToURL(kurl))
	{
		// Application wants to filter this link. Send a notification to the application that such
		// a link has been encountered. Application may want to do something with it.
		if(EAWebKitClient* const pClient = EA::WebKit::GetEAWebKitClient(jobInfo.mTInfo.mpView))
		{
			LinkNotificationInfo  lni;
			lni.mpView              = jobInfo.mTInfo.mpView;
			lni.mpUserData			= lni.mpView ? lni.mpView->GetUserData() : NULL;
			lni.mLinkNotificationType = EA::WebKit::LinkNotificationInfo::kLinkNavigationFiltered;

			const WTF::String& requestURI = kurl.string();
			GetFixedString(lni.mOriginalURI)->assign(requestURI.characters(), requestURI.length());
			pClient->LinkNotification(lni);
		}

		return false;
	}

	return true;

}

void ResourceHandleManager::CondemnJob(JobInfo* pJobInfo)
{
    // We don't actually remove it here, we put it into a condemned state, to be removed later.
    pJobInfo->mJobState = kJSRemove;
    m_CondemnedJobsExist = true;

	NOTIFY_PROCESS_STATUS(pJobInfo->mProcessInfo, EA::WebKit::kVProcessStatusEnded);
}


void ResourceHandleManager::CondemnAllJobs()
{
    for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); ++it)
    {
		JobInfo& jobInfo = *it;
		CondemnJob(&jobInfo);
    }
}


// Used for the HTTP "PUT" method, typically used to copy a file from client to server.
void ResourceHandleManager::SetupTHPut(JobInfo* /*pJobInfo*/)
{
    // For the "file" scheme, we just let the transport handler write the file to disk.
    // For the "http" scheme, we may want to set up some headers (such as content-disposition) that tell the server more about what to do.

    //if(Stricmp(pJobInfo->mScheme, "http") == 0)
    //{
    //    // To do: Do something to support HTTP put.
    //    Write TransportInfo::mPostSize with the file size.
    //}
}


// Used for the HTTP "POST" method, typically used to submit HTML form data to the server.
// We don't read the data here; it is instead read in the ReadData callback function.
void ResourceHandleManager::SetupTHPost(JobInfo* pJobInfo)
{
    FormData* const pFormData = pJobInfo->mpRH->firstRequest().httpBody();

    if(pFormData && !pFormData->elements().isEmpty())
    {
        const Vector<FormDataElement>& elements     = pFormData->elements();
        const size_t                   elementCount = elements.size();

        pJobInfo->mTInfo.mPostSize = 0;

        for(size_t i = 0; i < elementCount; i++)
        {
            const FormDataElement& element = elements[i];
			switch(element.m_type)
			{
			case FormDataElement::encodedFile:
				{
					long long fileSizeResult = -1;
					if(WebCore::getFileSize(element.m_filename, fileSizeResult))
					{
						EAW_ASSERT(fileSizeResult >= 0);
						pJobInfo->mTInfo.mPostSize += fileSizeResult;
					}
					break;
				}
			case FormDataElement::data:
				{
					pJobInfo->mTInfo.mPostSize += (int64_t)elements[i].m_data.size();
					break;
				}
			default:
				EAW_ASSERT_MSG(false, "Unhandled FormDataElement Type");
				break;
			}
        }
    }
    else
        pJobInfo->mTInfo.mPostSize = -1;
}


// This is a loop which runs each existing job as a simple state machine.
int ResourceHandleManager::ProcessJobs()
{
	NOTIFY_PROCESS_STATUS(EA::WebKit::kVProcessTypeTHJobs, EA::WebKit::kVProcessStatusStarted);

	for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); ++it)
	{
		JobInfo& jobInfo = *it;
       
        // We don't want to process a job if cancelled.  Also if cancelled, it is possible that the client pointer might no longer be valid (happens 
        // infrequently when switching between sites).  Skipping the ProcessJobState here should protect from calling the client. 
        ResourceHandleInternal* pRHI = jobInfo.mpRH->getInternal();
        if(pRHI->m_cancelled)
        {
            CondemnJob(&jobInfo);
            continue;
        }

		bool bStateComplete = false;
		bool bRemoveJob     = false;

		ProcessJobState(jobInfo, bStateComplete, bRemoveJob);

		if(bRemoveJob)
		{
			CondemnJob(&jobInfo);
		}
	}

	if(m_CondemnedJobsExist)
	{
		for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); )
		{
			JobInfo& jobInfo = *it;
			if(jobInfo.mJobState == kJSRemove)
			{
				if(!jobInfo.mbSuccess)
				{
					notifyJobFailed(jobInfo);
				}

				// We promise the TransportHandler that if we Init it then eventually we will Shutdown it. This allows it to clean up.
				if(jobInfo.mbTHInitialized && !jobInfo.mbTHShutdown)
				{
					bool bStateComplete;
					jobInfo.mpTH->ShutdownJob(&jobInfo.mTInfo, bStateComplete);
				}

				// This matches the ref() done in ResourceHandleManager::add().
				jobInfo.mpRH->deref();  
				jobInfo.mpRH = NULL;
				m_runningJobs--;

				it = m_JobInfoList.erase(it);
			}
			else
			{
				++it; //The job is not in removed state. Just increment the iterator.
			}
		}
		m_CondemnedJobsExist = false; 
	}

	EAW_ASSERT_MSG(m_runningJobs == m_JobInfoList.size(),"IMPOSSIBLE\n");
	NOTIFY_PROCESS_STATUS(EA::WebKit::kVProcessTypeTHJobs, EA::WebKit::kVProcessStatusEnded);
	return (int)m_JobInfoList.size();
}



void ResourceHandleManager::ProcessJobState(JobInfo& jobInfo, bool& bStateComplete, bool& bRemoveJob)
{
	if(!jobInfo.mbPaused)
	{
		switch (jobInfo.mJobState)
		{
		case kJSInit:
			if(jobInfo.mpTH->InitJob(&jobInfo.mTInfo, bStateComplete))
			{
				jobInfo.mbTHInitialized = true;

				if(bStateComplete)
				{
					jobInfo.mJobState = kJSConnect;
					NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToConnection);
				}

			}
			else
			{
				bRemoveJob = true;
				jobInfo.mbSuccess = false;
				NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
			}
			break;

		case kJSConnect:
			if(jobInfo.mpTH->Connect(&jobInfo.mTInfo, bStateComplete))
			{
				if(bStateComplete)
				{
					jobInfo.mJobState = kJSTransfer;
					NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToTransfer);
				}    
			}
			else
			{
				bRemoveJob = true;
				jobInfo.mbSuccess = false;
				NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
			}
			break;

		case kJSTransfer:
			if(jobInfo.mpTH->Transfer(&jobInfo.mTInfo, bStateComplete))
			{
				if(bStateComplete)
				{
					jobInfo.mJobState = kJSDisconnect;
					NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToDisconnect);
				}
			}
			else
			{
				bRemoveJob = true;
				jobInfo.mbSuccess = false;
				NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
			}
			break;

		case kJSDisconnect:
			if(jobInfo.mpTH->Disconnect(&jobInfo.mTInfo, bStateComplete))
			{
				if(bStateComplete)
				{
					jobInfo.mJobState = kJSShutdown;
					NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToShutdown);
				}
			}
			else
			{
				bRemoveJob = true;
				NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
			}

			break;

		case kJSShutdown:
			if(jobInfo.mpTH->ShutdownJob(&jobInfo.mTInfo, bStateComplete))
			{
				jobInfo.mbTHShutdown = true;

				if(bStateComplete)
				{
					bRemoveJob = true;
					NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
				}

			}
			else
			{
				bRemoveJob = true;
				NOTIFY_PROCESS_STATUS(jobInfo.mProcessInfo, EA::WebKit::kVProcessStatusQueuedToRemove);
			}
			break;

		case kJSRemove:
			bRemoveJob = true;
			break;
		}

	}

}

bool ResourceHandleManager::SetExpectedLength(EA::WebKit::TransportInfo* pTInfo, int64_t size)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI = pRH->getInternal();

    if(!pRHI->m_cancelled)
    {
        pRHI->m_response.setExpectedContentLength(size);

        return true;
    }

    return false;
}


bool ResourceHandleManager::SetEncoding(EA::WebKit::TransportInfo* pTInfo, const char* pEncoding)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	// This typically doesn't need to be called by the HTTP scheme, as the
    // HTTP scheme transport handler will instead call HeadersReceived and 
    // thus supply equivalent information.
	
    ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI = pRH->getInternal();

    if(!pRHI->m_cancelled)
    {
        const String sCharSet = pEncoding;
        pRHI->m_response.setTextEncodingName(sCharSet);

        return true;
    }

    return false;
}


bool ResourceHandleManager::SetMimeType(EA::WebKit::TransportInfo* pTInfo, const char* pMimeType)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);
	EAWEBKIT_THREAD_CHECK();
	// This typically doesn't need to be called by the HTTP scheme, as the
    // HTTP scheme transport handler will instead call HeadersReceived and 
    // thus supply equivalent information.

    ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI = pRH->getInternal();

    if(!pRHI->m_cancelled)
    {
        const String sMimeType = pMimeType;
        pRHI->m_response.setMimeType(sMimeType);

        return true;
    }

    return false;
}

// Test link for redirection which is an error - http://emp.sdes.ucf.edu/support_test.asp
// There is a disconnect between DirtySDK and us here if the "Location" header specified by the developer does not conform to
// standards and is not a fully qualified URL.
// DirtySDK handles this internally. So
// 1. Send Header callback, Send request to server.
// 2. Response received, Receive Header Callback. 
// 3. If redirect, format the request again and Send Header callback.
// 4. Send to Server.

inline static bool isHttpRedirect(int statusCode)
{
	return (statusCode>= 300 && statusCode < 400 ) && statusCode != 304;
}
// DirtySDK hands us all the headers when this call happens. This is different from the Curl version where
// the headers are received one by one.
bool ResourceHandleManager::HeadersReceived(EA::WebKit::TransportInfo* pTInfo)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);   
	EAWEBKIT_THREAD_CHECK();

	ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
	ResourceHandleInternal* pRHI = pRH->getInternal();
	
	JobInfo* pJobInfo = (JobInfo*)pTInfo->mpTransportServerJobInfo;       
	EAW_ASSERT_MSG(!pRHI->m_defersLoading && !pJobInfo->mbPaused, "This should not be called when deferred loading is activated (meaning job is paused)\n");

	m_cookieManager.OnHeadersRead(pTInfo); //We want to read cookies sent by the server regardless of the job state as the server might expect those cookies back.
	
	if(pRHI->m_cancelled || pJobInfo->mIgnoreJob)
		return false;

	

    // Copy mHeaderMapIn to m_response.
	for(EA::WebKit::HeaderMap::const_iterator it = GetHeaderMap(pTInfo->mHeaderMapIn)->begin(); it != GetHeaderMap(pTInfo->mHeaderMapIn)->end(); ++it)
    {
		const EA::WebKit::HeaderMap::key_type& sKey			= it->first;
		const EA::WebKit::HeaderMap::mapped_type& sValue	= it->second;

        // Translate EA::WebKit::FixedString16_256 to WTF::String (both are 16 bit string types).
        const WTF::String sWKey  (sKey.data(),   sKey.length());
        const WTF::String sWValue(sValue.data(), sValue.length());

        pRHI->m_response.setHTTPHeaderField(sWKey, sWValue);            
    }

	// Now all the information from DirtySDK headers is available - Use it to set some info.
	pRHI->m_response.setHTTPStatusCode(pTInfo->mResultCode);
	
	// It is safe to read from httpHeaderField using strings below as WebCore::HTTPHeaderMap is case-insensitive.
	pRHI->m_response.setExpectedContentLength(pRHI->m_response.httpHeaderField("Content-Length").toInt64());
	pRHI->m_response.setMimeType(extractMIMETypeFromMediaType(pRHI->m_response.httpHeaderField("Content-Type")));
	pRHI->m_response.setTextEncodingName(extractCharsetFromMediaType(pRHI->m_response.httpHeaderField("Content-Type")));
	pRHI->m_response.setSuggestedFilename(filenameFromHTTPContentDisposition(pRHI->m_response.httpHeaderField("Content-Disposition")));
	pRHI->m_response.setURL(pRH->firstRequest().url());

	// Deal with Http 1xx response
	// We have already parsed all the headers so nothing more left to do for this provisional response. We wait for the actual response to come in.
	if(pTInfo->mResultCode >= 100 && pTInfo->mResultCode < 200)
		return true;

	// Deal with the Http Redirection. In following m_response URL is set to be original URL in case
	// of no redirection whereas it is set to be redirected URL in case of redirection. This is similar
	// to curl version but different from 1.x version. 
	if (isHttpRedirect(pTInfo->mResultCode)) 
	{
		String location = pRHI->m_response.httpHeaderField("location");
		EAW_ASSERT_MSG(!location.isEmpty(), "Location can be empty, for example, in 304 case. This should have been filtered out.");
		
		if (!location.isEmpty()) 
		{
			URL newURL = URL(pRH->firstRequest().url(), location);

			ResourceRequest redirectedRequest = pRH->firstRequest();
			redirectedRequest.setURL(newURL);
			
			if ((pTInfo->mResultCode >= 301 && pTInfo->mResultCode <= 303) && pRH->firstRequest().httpMethod() == "POST")
				redirectedRequest.setHTTPMethod("GET");
			
			// Should not set Referer after a redirect from a secure resource to non-secure one.
			if (!redirectedRequest.url().protocolIs("https") && protocolIs(redirectedRequest.httpReferrer(), "https"))
				redirectedRequest.clearHTTPReferrer();
			
			if (pRHI->client())
				pRHI->client()->willSendRequest(pRH, redirectedRequest, pRHI->m_response);

			pRHI->m_firstRequest.setURL(newURL);

			const String& s(newURL.string());
			GetFixedString(pTInfo->mEffectiveURI)->assign(s.characters(), s.length());

			// Simply add the job again for the new transport handler to handle the redirected url. This will set up new job.
			if(!redirectedRequest.url().protocolIsInHTTPFamily())
			{
				EAW_ASSERT_MSG(pJobInfo->mTInfo.mbAsync,"Synchronous jobs not supported in this fashion");
				pJobInfo->mIgnoreJob = true;
				add(pRH);
			}
			return true;

		}
	}
	

    // Added checking the job state (kJSShutdown/kJSRemove) as a way not call a client that might have been be invalid (occured very infrequently when 
    // switching between sites).  We don't need to check for the cancelled job case since we already exit from it above.
    if (pRHI->client() && (pJobInfo->mJobState != kJSShutdown) && (pJobInfo->mJobState != kJSRemove))
		pRHI->client()->didReceiveResponse(pRH, pRHI->m_response);
	
	pRHI->m_response.setResponseFired(true);

    return true;

}


bool ResourceHandleManager::DataReceived(EA::WebKit::TransportInfo* pTInfo, const void* pData, int64_t size)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);   
	EAWEBKIT_THREAD_CHECK();
	JobInfo* pJobInfo = (JobInfo*)pTInfo->mpTransportServerJobInfo;       
	pJobInfo->mProcessInfo.mSize +=size;

	if(pJobInfo->mIgnoreJob)
		return false;

	ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI = pRH->getInternal();
#if _DEBUG
    m_readVolume += size;
#endif
    

	if(pRHI->m_cancelled)
		return false;

	EAW_ASSERT_MSG(!pRHI->m_defersLoading && !pJobInfo->mbPaused, "This should not be called when deferred loading is activated (meaning job is paused)\n");

    #if defined(AUTHOR_PPEDRIANA) && defined(EA_DEBUG)
        // Search for some incoming text. Problem: this won't catch text that straddles two receive  buffers.
        char* pString = (char*)(const char*)pData;
        eastl::fixed_substring<char> s(pString, pString + (eastl_size_t)size);
        if(s.find("some search phrase") < s.size())
            pString++; // Can put a breakpoint here. 
    #endif

	// Note by Paul Pedriana: 
	// Since HeadersReceived would not have been called for "file" or custom URIs,
    // the code to set the URI and fire didReceiveResponse hasn't been run,
    // which means the ResourceLoader's response does not contain the URI.
    // We run the code here for local files to resolve the issue.
    // This code here is similar to the Curl version of this code.
    if(!pRHI->m_response.responseFired())
    {
		const URL url(WebCore::URL(), GetFixedString(pTInfo->mEffectiveURI)->c_str());
        pRHI->m_response.setURL(url);
   
		if(pRHI->client())
			pRHI->client()->didReceiveResponse(pRH, pRHI->m_response);

        pRHI->m_response.setResponseFired(true);
		
		if(pRHI->m_cancelled)
			return false;
    }

    // We don't want to tell the client that we received any data if we are requiring 
    // authorization (user name/password). We want to wait until we have the final 
    // authorized page before we do that.
    if(!pJobInfo->mbAuthorizationRequired)
    {
		if(pRHI->client())
			pRHI->client()->didReceiveData(pRH, (char*)pData, (size_t)size, 0);
    }

    return true;

}


int64_t ResourceHandleManager::ReadData(EA::WebKit::TransportInfo* pTInfo, void* pData, int64_t size)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);   
	EAWEBKIT_THREAD_CHECK();

	ResourceHandle*         pRH      = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI     = pRH->getInternal();

	if (pRHI->m_cancelled)
		return 0;

	EAW_ASSERT_MSG(!pRHI->m_defersLoading,"We should never be called when deferred loading is activated." );
	
	if (!size )
		return 0;

	if (!pRHI->m_formDataStream.hasMoreElements())
		return 0;

	size_t readSize = 0;
	if(pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePOST || pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePUT)
    {
       readSize = pRHI->m_formDataStream.read(pData, (size_t)size, 1);

#if 0
	   //Update:02/25/2013 - Do not cancel the job for zero size data request. There is no standard that guides it and other browsers handle it fine.
	   if(!readSize )  
		  pRH->cancel();
#endif
#if _DEBUG
	   m_writeVolume += readSize;
#endif
    }
    return readSize;
}


bool ResourceHandleManager::DataDone(EA::WebKit::TransportInfo* pTInfo, bool result)
{
	SET_AUTOFPUPRECISION(EA::WebKit::kFPUPrecisionExtended);   
	EAWEBKIT_THREAD_CHECK();
	JobInfo* pJobInfo = (JobInfo*)pTInfo->mpTransportServerJobInfo;       

	if(pJobInfo->mIgnoreJob)
		return false;

	
	ResourceHandle*         pRH  = static_cast<ResourceHandle*>(pTInfo->mpRH);
    ResourceHandleInternal* pRHI = pRH->getInternal();
    
	if(pRHI->m_cancelled)
		return false;

    

    // We don't want to tell the client that we received any data if we are requiring 
    // authorization (user name/password). We want to wait until we have the final 
    // authorized page before we do that.
    if(!pJobInfo->mbAuthorizationRequired)
    {
		// Following block of code (before pRHI->client()) is not making much sense. For http(s) schemes, this code
		// should have already executed. For file/custom transport schemes, we should indicate that more clearly. 
		pRHI->m_response.setHTTPStatusCode(pTInfo->mResultCode);

        // The following is the same as in our SetEffectiveURI function, so we could possibly
        // instead call that function instead of use this code. The main point is to call the
        // m_response setURL function.
		EAW_ASSERT(!GetFixedString(pTInfo->mEffectiveURI)->empty());
		WTF::String sEffectiveURI(GetFixedString(pTInfo->mEffectiveURI)->data(), GetFixedString(pTInfo->mEffectiveURI)->length());
        URL url(WebCore::URL(), sEffectiveURI);
        pRHI->m_response.setURL(url);

		if(pRHI->client())
        {
            if(result) // If succeeded... 
            {
				if( pTInfo->mpTransportHandler->CanCacheToDisk() &&  m_THDiskCache.CacheEnabled() )  //No sense in caching either stuff loaded via cache, or file:///
                {
                    //Check with the response header directives if we can cache this file
                    EA::WebKit::CacheResponseHeaderInfo cacheHeaderInfo;
                    if(cacheHeaderInfo.SetDirectivesFromHeader(pTInfo))
                    {
						const WTF::String& mimeTypeWTFStr = pRHI->m_response.mimeType();
						EAW_ASSERT_MSG(!mimeTypeWTFStr.isEmpty(),"No mime type!\n");
						if(!mimeTypeWTFStr.isEmpty() && MIMETypeRegistry::canShowMIMEType(mimeTypeWTFStr.lower()))
						{
							EA::WebKit::FixedString8_128 mimeStr;
							EA::WebKit::ConvertToString8(pRHI->m_response.mimeType(), mimeStr);

							if(pRHI->client())
							{
								const SharedBuffer* resourceData = pRHI->client()->getResourceData();
								//Note by Arpit Baldeva: Check against null pointer. Crashed on http://kotaku.com
								if(resourceData)
								{
									m_THDiskCache.CacheToDisk(*GetFixedString(pTInfo->mURI), mimeStr, *resourceData, cacheHeaderInfo);
								}
							}
						}
                    }
                }
      
				if(pRHI->client())
					pRHI->client()->didFinishLoading(pRH, 0);

				// 08/12/2013 - Removed this superfluous call as the cookies are read in the HeadersReceived call. In some rare case (for example, very small html embedding javascript that changes a cookie set in this
				// http transaction), it can result in a bug whereby any cookie set through JavaScript will again be overridden by the cookies from transport layer below.
				// m_cookieManager.OnHeadersRead(pTInfo); 
            }
            else
            {
                pJobInfo->mbSuccess = false;
                CondemnJob(pJobInfo);
            }
        }
    }

    return true;

}

void ResourceHandleManager::AddTransportHandler(EA::WebKit::TransportHandler* pTH, const char16_t* pScheme)
{
	EA::WebKit::FixedString16_32 scheme(pScheme);
	
	// If the Handler is already present
	for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
	{
		THInfo& thInfo = *it;
		if(thInfo.mpTH == pTH)
		{
			thInfo.mSchemesList.push_back(scheme);
			pTH->Init(pScheme);
			return;
		}
	}
	
	// If the Handler does not exist
	THInfo thInfo;
	thInfo.mpTH = pTH;
	thInfo.mSchemesList.push_back(scheme);

	pTH->Init(pScheme);
	m_THInfoList.push_front(thInfo); // We want the list to be LIFO.
}

void ResourceHandleManager::RemoveTransportHandler(EA::WebKit::TransportHandler* pTH, const char16_t* pScheme)
{
	// Note by Arpit Baldeva: Now we have the capability of an external TransportHandler installed for EAWebKit. So we make sure
	// that we finish up all the jobs that depend on this transport handler. Otherwise, this could randomly crash.  
	RemoveDependentJobs(pTH, pScheme);
	
	for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
    {
        THInfo& thInfo = *it;

        if(thInfo.mpTH == pTH)
        {
            thInfo.mpTH->Shutdown(pScheme);

			SchemesList::iterator schemeIter = eastl::find(thInfo.mSchemesList.begin(), thInfo.mSchemesList.end(),EA::WebKit::FixedString16_32(pScheme));
			if(schemeIter != thInfo.mSchemesList.end())
				thInfo.mSchemesList.erase(schemeIter);
            
			if(thInfo.mSchemesList.size() == 0)
				m_THInfoList.erase(it);
            
			break;
        } 
    }
}

void ResourceHandleManager::TickTransportHandlers()
{
	for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
	{
		const THInfo& thInfo = *it;
		thInfo.mpTH->Tick();
	}
}

void ResourceHandleManager::TickDownload()
{
	downloadTimerCallback(0);
}

void ResourceHandleManager::RemoveDependentJobs(EA::WebKit::TransportHandler* pTH, const char16_t* pScheme) 
{
	for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); ++it)
	{
		JobInfo& jobInfo = *it;
		if(jobInfo.mpTH == pTH && (EA::Internal::Stricmp(jobInfo.mTInfo.mScheme, pScheme) == 0))
			CondemnJob(&jobInfo);
	}
	// This will take care of removing condemned jobs.
	ProcessJobs();
}

void ResourceHandleManager::RemoveDependentJobs(EA::WebKit::View* pView)
{
	for(JobInfoList::iterator it = m_JobInfoList.begin(); it != m_JobInfoList.end(); ++it)
	{
		JobInfo& jobInfo = *it;
		if(Local::GetViewFromResourceHandle(jobInfo.mpRH) == pView)
			CondemnJob(&jobInfo);
	}
	// This will take care of removing condemned jobs.
	ProcessJobs();
}

void ResourceHandleManager::RemoveTransportHandlers()
{
    for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
    {
        const THInfo& thInfo = *it;
		for(SchemesList::const_iterator schemeIter = thInfo.mSchemesList.begin(); schemeIter != thInfo.mSchemesList.end(); ++schemeIter)
		{
			thInfo.mpTH->Shutdown(schemeIter->c_str());
		}
    }

    m_THInfoList.clear();
}


EA::WebKit::TransportHandler* ResourceHandleManager::GetTransportHandler(const char16_t* pScheme)
{
	EAW_ASSERT(pScheme);

    EA::WebKit::TransportHandler* pTransportHandler = GetTransportHandlerInternal(pScheme);
	
	if(pTransportHandler)
		return pTransportHandler;

	//If the transport handler does not exist, install the default transport handler for the scheme and check again.
	AddDefaultTransportHandler(pScheme);
	pTransportHandler = GetTransportHandlerInternal(pScheme);

    return pTransportHandler;
}

EA::WebKit::TransportHandler* ResourceHandleManager::GetTransportHandlerInternal(const char16_t* pScheme)
{
	// Match the exact scheme names first
	for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
	{
		const THInfo& thInfo = *it;

		for(SchemesList::const_iterator itScheme = thInfo.mSchemesList.begin(); itScheme != thInfo.mSchemesList.end(); ++itScheme)
		{
			if(EA::Internal::Stricmp((*itScheme).c_str(), pScheme) == 0)
				return thInfo.mpTH;
		}
	}

	// If we did not succeed in finding exact match, try one more time to figure out if scheme belongs to a family. This is so that a user can install handler for a family of schemes.
	// For example, a user can install a handler for "world". A scheme such as "world-yours://" or "world-mine://" or "worldstrange://" will go to this handler if a unique handler is not found already. 
	for(THInfoList::iterator it = m_THInfoList.begin(); it != m_THInfoList.end(); ++it)
	{
		const THInfo& thInfo = *it;

		for(SchemesList::const_iterator itScheme = thInfo.mSchemesList.begin(); itScheme != thInfo.mSchemesList.end(); ++itScheme)
		{
			String handlerScheme((*itScheme).c_str());
			String currentScheme(pScheme);
			if(currentScheme.startsWith(handlerScheme,false))
				return thInfo.mpTH;
		}
	}

	return NULL;
}

void ResourceHandleManager::AddDefaultTransportHandler(const char16_t* pScheme)
{
	if((EA::Internal::Stricmp(pScheme, EA_CHAR16("file")) == 0))
	{
		AddTransportHandler(&m_THFileScheme, EA_CHAR16("file"));
	}

#if ENABLE(DIRTYSDK_IN_DLL)
	if((EA::Internal::Stricmp(pScheme, EA_CHAR16("http")) == 0) || (EA::Internal::Stricmp(pScheme, EA_CHAR16("https")) == 0))
	{
		EAW_ASSERT_MSG(false, "No application supplied transport handler is found. Using the transport handler in the EAWebKit package.");
		AddTransportHandler(&m_THDirtySDK, pScheme);
	}
#endif
}

EA::WebKit::CookieManager* ResourceHandleManager::GetCookieManager()
{
    return &m_cookieManager;
}

bool ResourceHandleManager::SetDiskCacheParams(const EA::WebKit::DiskCacheInfo& diskCacheInfo)
{
	// This behavior is similar to cookies and is advertised somewhere.
	if(diskCacheInfo.mDiskCacheSize == 0)
	{
		m_THDiskCache.ClearCache();
		return true;
	}
	return m_THDiskCache.SetDiskCacheParams(diskCacheInfo);
}

void ResourceHandleManager::GetDiskCacheUsage(EA::WebKit::DiskCacheUsageInfo& diskCacheUsageInfo)
{
	m_THDiskCache.GetDiskCacheUsage(diskCacheUsageInfo);
}
void ResourceHandleManager::SetParams(const EA::WebKit::Parameters& params)
{
	const EA::WebKit::Parameters& parameters = EA::WebKit::GetParameters();
	
	m_maxConcurrentJobs = parameters.mMaxTransportJobs;
    m_maxParallelConnectionsPerHost = parameters.mMaxParallelConnectionsPerHost;
}

int ResourceHandleManager::GetMaxParallelConnectionsPerHost()
{
    return m_maxParallelConnectionsPerHost;
}

} // namespace WebCore
