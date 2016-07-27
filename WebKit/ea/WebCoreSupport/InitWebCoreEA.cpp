/*
 * Copyright 2011 Hewlett-Packard Development Company, L.P. All rights reserved.
 * Copyright (C) 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Hewlett-Packard nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InitWebCoreEA.h"

#include "Chrome.h"
#include "ChromeClientEA.h"
#include "PlatformStrategiesEA.h"
#include "Font.h"
#include "Image.h"
#include "Logging.h"
#include "MemoryCache.h"
#include "NotImplemented.h"
#include "Page.h"
#include "ScriptController.h"
#include "SecurityPolicy.h"
#include "CallFrame.h"

#include <JavaScriptCore/runtime/InitializeThreading.h>
#include <wtf/MainThread.h>
#include <runtime/Options.h>
#include <EAWebKit/EAWebKitClient.h>

namespace WebCore {

static bool shouldTerminateCallback (JSC::ExecState*, void* data1, void* data2)
{
	bool shouldInterruptScript = true;

	if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient())
	{
		EA::WebKit::WatchDogNotificationInfo info;
		info.mpView = 0;
		info.mpUserData = 0;
		info.mWatchDogNotificationType = EA::WebKit::WatchDogNotificationInfo::kScriptRunningTooLong;

		pClient->WatchDogNotification(info);

		shouldInterruptScript = info.mInterruptScript;
	}


	return shouldInterruptScript;
}

void initializeWebCoreEA()
{
    static bool initialized = false;
    if (initialized)
        return;

#if !LOG_DISABLED
    WebCore::initializeLoggingChannelsIfNecessary();
#endif // !LOG_DISABLED
    ScriptController::initializeThreading();
    WTF::initializeMainThread();
    WebCore::SecurityPolicy::setLocalLoadPolicy(WebCore::SecurityPolicy::AllowLocalLoadsForLocalAndSubstituteData);

    PlatformStrategiesEA::initialize();

    if (!WebCore::memoryCache()->disabled())
        WebCore::memoryCache()->setDeadDecodedDataDeletionInterval(60);

#if !ENABLE(JIT)
	JSC::Options::useJIT() = false;
	JSC::Options::useRegExpJIT() = false;
#endif
//	WebCore::Image::setPlatformResource(name, 0);
	
    initialized = true;
}

void initializeVMTimeout()
{
	JSDOMWindow::commonVM()->watchdog.setTimeLimit(*JSDOMWindow::commonVM(),10.0,shouldTerminateCallback);
	//Use the line below to get rid of the watchdog timer and see better benchmark performances
	//JSDOMWindow::commonVM()->watchdog.setTimeLimit(*JSDOMWindow::commonVM(),std::numeric_limits<double>::infinity(),shouldTerminateCallback);
}

}
