/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Electronic Arts, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "Watchdog.h"

#include "CallFrame.h"
#include <wtf/CurrentTime.h>
#include <wtf/MathExtras.h>

//+EAWebKitChange
//4/1/2014
// The watchdog relies on a system provided facility that can install a timer and call back on a separate thread when the time elapsed. The m_timerDidFire boolean is set due to that. Once that happens, didFire(ExecState*) is called
// to check if the time actually elapsed.

// Made some changes below to address 4 things. 

// 1. Bypass the system timer mechanism and handle everything on the main thread. 

// 2. This is also required because current code has issues dealing with web worker threads that operate in an infinite while(true)
// loop. The reason being that when the worker terminates, m_didFire is set to true but m_timerDidFire is never set to true (even for the platforms that have watchdog mechanism enabled). Thus, the JS engine never ever checks for 
// didFire(ExecState). Thus, we end up with a situation where web worker has terminated but the VM is still executing the code.

// 3. There is another bug if the script becomes unresponsive. Once you terminate it, the VM can not evaluate scripts anymore because m_didFire is set to true. It might work in the multiple-process architecture where each view is its
// own process but not for us. We would want to be able to continue to use the View. 

// 4. Changed the watch dog timer to be enabled by default. This is to address JIT vs C Loop Interpreter different. JIT code does not emit time out check call
// unless the watchdog is enabled (check out JIT::emit_op_loop_hint implementation in JITOpcodes.cpp). This means didFire(ExecState*) is never called. The C Loop interpreter
// does not suffer from this problem as it only checks for m_timerDidFire (Check out generated OFFLINE_ASM_OPCODE_LABEL(op_loop_hint)). As a consequence, the JIT code
// never enables the watchdog timer for the worker thread. To address this, we give near infinite timeout to worker script. If somebody wants to disable the watchdog for
// main VM, they can always use the the API to set the timeout to infinity() again.


//-EAWebKitChange

namespace JSC {

#define NO_LIMIT std::numeric_limits<double>::infinity()

//+EAWebKitChange
//4/1/2014
static const int TICKS_BETWEEN_CHECK = 1024*1000; // Higher this number, less the accuracy of actual time out time but that is not critical to the functionality. Borrowed from TimeoutChecker in previous webkit versions
//-EAWebKitChange

Watchdog::Watchdog()
	//+EAWebKitChange
	//4/1/2014
	: m_timerDidFire(true)//false) // Set it to true by default so that main thread continuous checks for the timeout and does not rely on a system operate timer. 
	//-EAWebKitChange
    , m_didFire(false)
	//+EAWebKitChange
	//4/1/2014
	, m_limit(std::numeric_limits<double>::max()) // So that workers have watchdog mechanism enabled and can terminate
	//-EAWebKitChange
    , m_startTime(0)
    , m_elapsedTime(0)
    , m_reentryCount(0)
    , m_isStopped(true)
    , m_callback(0)
    , m_callbackData1(0)
    , m_callbackData2(0) 
	//+EAWebKitChange
	//4/1/2014
	, m_ticksCountBetweenCheck(TICKS_BETWEEN_CHECK) // Optimize performance
	//-EAWebKitChange
{
    initTimer();
}

Watchdog::~Watchdog()
{
    ASSERT(!isArmed());
    stopCountdown();
    destroyTimer();
}

void Watchdog::setTimeLimit(VM& vm, double limit,
    ShouldTerminateCallback callback, void* data1, void* data2)
{
    bool wasEnabled = isEnabled();

    if (!m_isStopped)
        stopCountdown();

    m_didFire = false; // Reset the watchdog.

    m_limit = limit;
    m_callback = callback;
    m_callbackData1 = data1;
    m_callbackData2 = data2;

    // If this is the first time that timeout is being enabled, then any
    // previously JIT compiled code will not have the needed polling checks.
    // Hence, we need to flush all the pre-existing compiled code.
    //
    // However, if the timeout is already enabled, and we're just changing the
    // timeout value, then any existing JITted code will have the appropriate
    // polling checks. Hence, there is no need to re-do this flushing.
    if (!wasEnabled) {
        // And if we've previously compiled any functions, we need to revert
        // them because they don't have the needed polling checks yet.
        vm.releaseExecutableMemory();
    }

    startCountdownIfNeeded();
}

bool Watchdog::didFire(ExecState* exec)
{
	//+EAWebKitChange
	//4/1/2014
	// These two short checks need to happen before the tick count check
	if (!isArmed())
		return false;
	
	if (m_didFire)
		return true;

	if(--m_ticksCountBetweenCheck)
		return false;

	m_ticksCountBetweenCheck = TICKS_BETWEEN_CHECK;

	/*
	if (!m_timerDidFire)
		return false;
	m_timerDidFire = false;
	*/
	//-EAWebKitChange
    
	stopCountdown();

    double currentTime = currentCPUTime();
    double deltaTime = currentTime - m_startTime;
    double totalElapsedTime = m_elapsedTime + deltaTime;
    if (totalElapsedTime > m_limit) {
        // Case 1: the allowed CPU time has elapsed.

        // If m_callback is not set, then we terminate by default.
        // Else, we let m_callback decide if we should terminate or not.
        bool needsTermination = !m_callback
            || m_callback(exec, m_callbackData1, m_callbackData2);
        if (needsTermination) {
            m_didFire = true;
            return true;
        }

        // The m_callback may have set a new limit. So, we may need to restart
        // the countdown.
        startCountdownIfNeeded();

    } else {
        // Case 2: the allowed CPU time has NOT elapsed.

        // Tell the timer to alarm us again when it thinks we've reached the
        // end of the allowed time.
        double remainingTime = m_limit - totalElapsedTime;
        m_elapsedTime = totalElapsedTime;
        m_startTime = currentTime;
        startCountdown(remainingTime);
    }

    return false;
}

bool Watchdog::isEnabled()
{
    return (m_limit != NO_LIMIT);
}

void Watchdog::fire()
{
    m_didFire = true;
}

void Watchdog::arm()
{
	//+EAWebKitChange
	//4/1/2014 - Add this to reset the state
	m_didFire = false;
	//-EAWebKitChange

	m_reentryCount++;
    if (m_reentryCount == 1)
        startCountdownIfNeeded();
}

void Watchdog::disarm()
{
    ASSERT(m_reentryCount > 0);
    if (m_reentryCount == 1)
        stopCountdown();
    m_reentryCount--;
}

void Watchdog::startCountdownIfNeeded()
{
    if (!m_isStopped)
        return; // Already started.

    if (!isArmed())
        return; // Not executing JS script. No need to start.

    if (isEnabled()) {
        m_elapsedTime = 0;
        m_startTime = currentCPUTime();
        startCountdown(m_limit);
    }
}

void Watchdog::startCountdown(double limit)
{
    ASSERT(m_isStopped);
    m_isStopped = false;
    startTimer(limit);
}

void Watchdog::stopCountdown()
{
    if (m_isStopped)
        return;
    stopTimer();
    m_isStopped = true;
}

Watchdog::Scope::Scope(Watchdog& watchdog)
    : m_watchdog(watchdog)
{
    m_watchdog.arm();
}

Watchdog::Scope::~Scope()
{
    m_watchdog.disarm();
}

} // namespace JSC
