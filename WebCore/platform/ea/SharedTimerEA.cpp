/*
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * All rights reserved.
 *
 * Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.
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


#include "config.h"

#include <wtf/CurrentTime.h>
#include <float.h>

// Note by Arpit Baldeva: I sent a mail to webkit community (http://lists.macosforge.org/pipermail/webkit-dev/2011-October/018332.html )
// and have not received any answer. Based on my understanding, the way our code is structured, we are fine simply calling
// the timer function from our View::Tick(). 
// In addition, I have implemented some new code and disabled it using ENABLE(FIRE_TIMER_IMPLEMENTATION). This code is implemented
// using clock() which usually has milliseconds resolution. If needed, we can later reimplement it using a Timer Interface.

// The email is pasted below verbatim.
//
/*
Hi,

I'd like to understand how a port is supposed to implement this file. My understanding is that


1.       setSharedTimerFiredFunction sets a function that the port should call.

2.       setSharedTimerFireInterval is called by WebCore with a time interval. The port is supposed to call the function pointer set through setSharedTimerFiredFunction after this time interval is elapsed. If any timer is in progress, it should reset itself.

3.       stopSharedTimer is called by WebCore. If this is called, any scheduled timer callback should not be called.

Does it sound correct?

The general understanding I have is that ThreadTimers.cpp does all the housekeeping of registered timers (like when each one 
should be called). We implement our View/WebView with a Tick() function that application is supposed to call (usually at 60 FPS). If we simply call the
timer callback set in setSharedTimerFiredFunction inside the Tick(), do we run into any risk (For example, firing a timer too 
soon than it was scheduled)? On surface, that does not seem to be the case since ThreadTimers::sharedTimerFiredInternal() 
checks the nextFireTime of the timer that is supposed to be fired earliest. The only side effect I can see with this approach 
is that any timer set to fire with a frequency higher than the View::Tick() frequency will not fire at higher frequency but 
would end up in sync with the View::Tick() frequency. This also means that all the startOneShot(0) timers would end up 
triggering next "frame". Is this correct understanding?

So if we have a View::Tick() running at 60 FPS that calls the shared timer function callback, does that seem alright 
(except the issues noted above)? In my local testing, I did not see anything wrong.

I appreciate any answers.

Thanks.
*/

#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
#include <time.h>
const double  kLargeFloat = 10 * 86400 * 1000.0f; //10 days in ms.
#endif


namespace WebCore {

void (*gSharedTimerFiredFunction)() = NULL;

// abaldeva: 06/13/2011 - Reimplemented fire timer functionality. Earlier code did lot of unnecessary stuff. Webcore does it on
// its end.

// At the start, we don't want to fire any timer until any timer is added.
// Not using DBL_MAX in order to avoid overflow when doing arithmetic operations.
#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
static double sEarliestFireIntervalMS = kLargeFloat;	
static double sStartTimeMS = 0;
#endif

//The function pointer here is indicating to the same location all the time. 
void setSharedTimerFiredFunction(void (*f)()) 
{
	gSharedTimerFiredFunction = f;
}
// This function is called with the earliest possible fire interval of current fire timer objects in two condition. 
// 1. Any time a timer object is added to the shared timer 
// 2. or if any timer object fired its timer and next one may be available. 

void setSharedTimerFireInterval(double interval)
{
#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
	double intervalInMS = (interval * 1000);
	sEarliestFireIntervalMS = intervalInMS;
	sStartTimeMS = clock() * 1000.0 /CLOCKS_PER_SEC;
#endif
}

// For some reason, WebCore calls this time to time. This means it is not interested in processing timer objects for 
// whatever reason.
void stopSharedTimer()
{
#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
	sEarliestFireIntervalMS = kLargeFloat;
#endif
}

// This is equivalent of an OS message loop which we proxy through our View loop. Any time we happen to have a timer object 
// with its fire time past the current time, we fire the timer.
void fireTimerIfNeeded()
{
#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
	double currentTime = clock() * 1000.0 /CLOCKS_PER_SEC;
	if(currentTime > (sStartTimeMS + sEarliestFireIntervalMS) )
	{
#endif	
	if(gSharedTimerFiredFunction)
		gSharedTimerFiredFunction();


#if ENABLE(FIRE_TIMER_IMPLEMENTATION)
	}
#endif
}



}

// vim: ts=4 sw=4 et
