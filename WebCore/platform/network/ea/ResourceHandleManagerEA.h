/*
* Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
* Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
* All rights reserved.
* Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.
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

#ifndef ResourceHandleManagerEA_h
#define ResourceHandleManagerEA_h

#include "Frame.h"
#include "Timer.h"
#include "ResourceHandleClient.h"
#include <wtf/Vector.h>

#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTransport.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <EASTL/fixed_list.h>
#include <EASTL/fixed_vector.h>
#include "CookieManagerEA.h"
#include "TransportHandlerDiskCacheEA.h"

#if ENABLE(DIRTYSDK_IN_DLL)
    #include "TransportHandlerDirtySDKEA.h" 
#endif

namespace WebCore {

	// TransportHandler info. This is a mapping of TransportHandler to supported schemes.
	typedef eastl::fixed_vector<EA::WebKit::FixedString16_32,  2, true, EA::WebKit::EASTLAllocator> SchemesList;
	struct THInfo 
	{
		EA::WebKit::TransportHandler*	mpTH;				// Pluggable TransportHandler
		SchemesList						mSchemesList;		// The list of schemes it supports(http/https etc).
	};

	// Various states a transport job goes through
	enum JobState   
	{
		kJSInit,        // Call TransportHandler::Init
		kJSConnect,     // Call TransportHandler::Connect
		kJSTransfer,    // Call TransportHandler::Transfer
		kJSDisconnect,  // Call TransportHandler::Disconnect
		kJSShutdown,    // Call TransportHandler::Shutdown
		kJSRemove       // Remove the Job
	};

	struct JobInfo
	{
		int								mId;						// This is simply an ever-increasing number.
		WebCore::ResourceHandle*		mpRH;						// WebKit ResourceHandle. Ref-counted while stored in this struct.
		JobState						mJobState;					// 
		EA::WebKit::TransportHandler*	mpTH;						// The TransportHandler for this job.
		EA::WebKit::TransportInfo		mTInfo;						// TransportInfo
		bool							mbTHInitialized;			// True if the TransportHandler initialized this job successfully.
		bool							mbTHShutdown;				// True if the TransportHandler shut down this job successfully during normal state flow.
		bool							mbSuccess;					// True if the job is completed successfully, false if the job failed.
		bool							mbAuthorizationRequired;	// True if the job was completed with a 401 or 407 result (authorization required).
		bool							mbPaused;					// True if the job needs to be paused for any reason.For example, if a breakpoint is put on a JavaScript in WebInspector tool.
		EA::WebKit::ViewProcessInfo		mProcessInfo;				// Used for process user callback notifications 
		bool							mIgnoreJob;					// If the job is redirected to another transport handler after initial set up, we set the job in the ignore mode. A new job is set up for new transport handler.  
		JobInfo()
			: mId(0)
			, mpRH(NULL)
			, mJobState(kJSInit)
			, mpTH(NULL)
			, mTInfo()
			, mbTHInitialized(false)
			, mbTHShutdown(false)
			, mbSuccess(true)
			, mbAuthorizationRequired(false)
			, mbPaused(false)
			, mProcessInfo(EA::WebKit::kVProcessTypeTransportJob, EA::WebKit::kVProcessStatusStarted)
			, mIgnoreJob(false)
		{
			NOTIFY_PROCESS_STATUS(mProcessInfo,EA::WebKit::kVProcessStatusStarted);
		}
	};

}

namespace WebCore {

class ResourceHandleManager : public EA::WebKit::TransportServer
{

public:
    static ResourceHandleManager* sharedInstance();
	static void                   finalize();

	//
	// Interface to WebCore::ResourceHandle(EA)
    //
	void add(ResourceHandle* pRH);
    void cancel(ResourceHandle* pRH);
    void changeResourceHandleState(ResourceHandle* pRH, bool paused);
	void loadResourceSynchronously(ResourceHandle* pRH);
	


	//
	// TransportServer Interface
	//
	bool    SetExpectedLength       (EA::WebKit::TransportInfo* pTInfo, int64_t size);
	bool    SetEncoding             (EA::WebKit::TransportInfo* pTInfo, const char* pEncoding);
	bool    SetMimeType             (EA::WebKit::TransportInfo* pTInfo, const char* pMimeType);
	bool    HeadersReceived         (EA::WebKit::TransportInfo* pTInfo);
	bool    DataReceived            (EA::WebKit::TransportInfo* pTInfo, const void* pData, int64_t size);
	int64_t ReadData                (EA::WebKit::TransportInfo* pTInfo, void* pData, int64_t size);
	bool    DataDone                (EA::WebKit::TransportInfo* pTInfo, bool result);



	//
	// TransportHandler helpers for the EAWebKit API
	//
	void							AddTransportHandler    (EA::WebKit::TransportHandler* pTH, const char16_t* pScheme);
	void							RemoveTransportHandler (EA::WebKit::TransportHandler* pTH, const char16_t* pScheme);
		//We first try to get the transport handler that the app may have provided to us. 
		//If none exists, we create the default one and return it.
	EA::WebKit::TransportHandler*	GetTransportHandler    (const char16_t* pScheme);
	


	//
	// Download Helpers
	//
	void							TickTransportHandlers();
	void							TickDownload();



	//
	// Misc
	//
	// Called when a View is being removed 
	void RemoveDependentJobs(EA::WebKit::View* pView);
	// Params
	void SetParams(const EA::WebKit::Parameters& params);
	//Disk cache 
	bool SetDiskCacheParams(const EA::WebKit::DiskCacheInfo& diskCacheInfo);
	void GetDiskCacheUsage(EA::WebKit::DiskCacheUsageInfo& diskCacheUsageInfo);
	EA::WebKit::CookieManager*         GetCookieManager();
	//EA::WebKit::AuthenticationManager* GetAuthenticationManager();

    int GetMaxParallelConnectionsPerHost();

private:
	ResourceHandleManager();
	virtual ~ResourceHandleManager();

	void		downloadTimerCallback(Timer<ResourceHandleManager>*);

	bool		startPendingJobs();
	void		startPendingJob(ResourceHandle* pRH);
	bool		removePendingJob(ResourceHandle* pRH);
	
	bool		initializeJob(JobInfo& jobInfo, ResourceHandle* pRH, bool bSynchronous);
	int			ProcessJobs(); // To be called repeatedly while there are active jobs.
	void		ProcessJobState(JobInfo& jobInfo, bool& bStateComplete, bool& bRemoveJob);
	void		notifyJobFailed(const JobInfo& jobInfo);
	void		CondemnJob(JobInfo* pJobInfo);
	void		CondemnAllJobs();

	
	void SetupTHPut(JobInfo* pJobInfo);
	void SetupTHPost(JobInfo* pJobInfo);
	
	EA::WebKit::TransportHandler*	GetTransportHandlerInternal(const char16_t* pScheme);
	void							AddDefaultTransportHandler(const char16_t* pScheme);	
	void							RemoveTransportHandlers();
	void							RemoveDependentJobs(EA::WebKit::TransportHandler* pTH, const char16_t* pScheme);

private:
	typedef eastl::fixed_list<ResourceHandle*, 16, true, EA::WebKit::EASTLAllocator> ResourceHandleList;   
	typedef eastl::fixed_list<THInfo,  2, true, EA::WebKit::EASTLAllocator> THInfoList;    // List of available TransportHandlers. We reserve 2 (One for the network and one for the application. Of course, the list is capable of auto-expansion).
	typedef eastl::list<JobInfo, EA::WebKit::EASTLAllocator> JobInfoList;   // Currently running jobs. We don't reserve any since JobInfo instances are a bit heavy. Trading memory usage(Saves about 6K from ResourceHandleManager size) for a small runtime penalty.
	
	static ResourceHandleManager* m_pInstance;
	EA::WebKit::TransportHandlerFileScheme  m_THFileScheme;               
	EA::WebKit::TransportHandlerDiskCache   m_THDiskCache;

#if ENABLE(DIRTYSDK_IN_DLL)
	EA::WebKit::TransportHandlerDirtySDK	m_THDirtySDK;
#endif

	Timer<ResourceHandleManager>        m_downloadTimer;                    // 
	
	ResourceHandleList					m_pendingResourceHandleList;        // This is a list of jobs that haven't been started yet.
    int                                 m_runningJobs;                      // This is a count of jobs that have been started and are running.
    double                              m_pollTimeSeconds;                  // Defaults to something small like 0.016 seconds (~60 frames/sec).
    int                                 m_maxConcurrentJobs;                // Max number of jobs occurring at at time.
    int                                 m_maxParallelConnectionsPerHost;    // Max number of parallel connections to host.
    EA::WebKit::CookieManager           m_cookieManager;                    // 

    THInfoList                              m_THInfoList;
    JobInfoList                             m_JobInfoList;
    int                                     m_JobIdNext;
    bool                                    m_CondemnedJobsExist;
	
#if _DEBUG
	uint64_t m_readVolume;      // Number of bytes ever read.
	uint64_t m_writeVolume;     // Number of bytes ever written.
#endif
};



} // namespace



#endif
