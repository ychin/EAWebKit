/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_INTERAL_WIN32RUNNER_H
#define EACALLSTACK_INTERAL_WIN32RUNNER_H


#include <EABase/eabase.h>


#if defined(EA_PLATFORM_WINDOWS)


/**
 * conversion from 'eastl::deque<T,Allocator>::size_type' to 'eastl::DequeIterator<T,Pointer,Reference,kDequeSubarraySize>::difference_type',
 * signed/unsigned mismatch
 */
EA_DISABLE_VC_WARNING(4365);

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EAAddressRep.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EASTL/deque.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>


EA_DISABLE_ALL_VC_WARNINGS();

#include <Windows.h>

EA_RESTORE_ALL_VC_WARNINGS();


namespace EA
{
namespace Callstack
{
	// This class is essentially a trimmed down version of Win32Runner from the EARunner package.
	// If you are looking for significantly broader functionality (in terms of both cross platform
	// and the ability to launch remote processes), see EARunner.

	class Win32Runner
	{
	public:
		Win32Runner();
		virtual ~Win32Runner();

		bool   Init();
		bool   Shutdown();
		int    Run();

		void   SetTargetProcessPath(const char16_t* pPath);
		void   SetArgumentString(const char16_t* pArguments);
		void   SetResponseTimeout(int64_t timeSeconds);
		void   SetUnilateralTimeout(int64_t timeSeconds);
		size_t GetOutput(EA::Callstack::CAString8& sOutput) const;

		// High level functionality
		static int RunToCompletionWithOutput(const char16_t* pCommandString, int64_t responseTimeoutSeconds, int64_t unilateralTimeoutSeconds, EA::Callstack::CAString8& sOutput);
		static int RunToCompletionWithOutput(const char*  pCommandString, int64_t responseTimeoutSeconds, int64_t unilateralTimeoutSeconds, EA::Callstack::CAString8& sOutput);

	public:
		// Time measurement
		typedef int64_t RunnerTime;

		/// Result
		///
		/// Defines what is an OK result and what is an error. An application
		/// that has an exit code value of 0 is OK.
		///
		enum Result
		{
			kResultOK = 0,
			kResultError
		};


		/// ExitReason
		///
		/// Defines the reason why the run was ended.
		///
		enum ExitReason
		{
			kExitReasonNone = 0,    /// The run wasn't done, and so there is no exit and thus no reason for exit.
			kExitReasonDone,        /// The run completed by itself.
			kExitReasonException,   /// The run encountered an exception or similar abnormal termination.
			kExitReasonTimeout,     /// The run timed out, possibly due to a hang or deadlock.
			kExitReasonCancel       /// The run was cancelled by the user of EARunner.
		};

		static const RunnerTime kRunnerTimeMin = LLONG_MIN;
		static const RunnerTime kRunnerTimeMax = LLONG_MAX;

		/// CancelCheckFunctionType
		/// Used by EARunner::SetCancelCheckFunction.
		typedef bool (*CancelCheckFunctionType)(Win32Runner*, void* pContext);

	protected:
		// Container types
		#if (EASTL_VERSION_N > 11000)  // EASTL versions <= 1.10.00 have a deque bug which has been shown to sometimes cause the following code to crash.
		typedef eastl::deque<char, EA::Callstack::EASTLCoreAllocator>                    Deque8;
		#else
		typedef eastl::basic_string<char, EA::Callstack::EASTLCoreAllocator>             Deque8;
		#endif
		typedef eastl::basic_string<char, EA::Callstack::EASTLCoreAllocator>             String8;
		typedef eastl::fixed_string<char,  256, true, EA::Callstack::EASTLCoreAllocator> FixedString8;
		typedef eastl::fixed_string<char16_t, 256, true, EA::Callstack::EASTLCoreAllocator> FixedString16;

		// Pipes
		struct Pipe
		{
			Pipe();

			void Reset();

			String8 msOutput;       // Or Deque8. A user was experiencing heap corruption related to msOutput with deque, but we don't know if it was due to deque or even anything with this library.
			HANDLE  mhStdinNew;
			HANDLE  mhStdoutNew;
			HANDLE  mhStdinWrite;
			HANDLE  mhStdoutRead;
		};

	protected:
		// The following virtual functions can be subclassed.
		virtual void   ExceptionOccurred(const void* pExceptionInformation);
		virtual void   TimeoutOccurred();
		virtual void   CancelOccurred();
		virtual void   CompletionOccurred(int exitCode);
		virtual void   TraceOccurred(const char* pText);

		void   TraceLastError(unsigned error);
		int    LocalRun();
		bool   AllocatePipes();
		void   FreePipes();
		void   ReadPipes();
		void   FreeChildHandles();
		void   ProcessDebugEvent(DEBUG_EVENT& debugEvent);

		RunnerTime GetRunnerTime();
		void       GetPathDirectory(const FixedString16& sPath, FixedString16& sDirectory);
		void       GetPathFileName(const FixedString16& sPath, FixedString16& sFileName);

		// Hide the following functions, as this class is not copyable.
		Win32Runner(Win32Runner&);
		void operator=(Win32Runner&);

	protected:
		bool                    mbInitialized;                  /// True if we have been initialized.
		bool                    mbPrintOutput;                  /// True if we print output to stdout and/or OutputDebugString.
		bool                    mbEnableProcessConsole;         /// True if we a new console window is created for the created process. Otherwise our own console is used. Default is false.
		int                     mExitCode;                      /// Initialize mExitCode to kResultError so the only way this program returns kResultOK is if the target process completes without error.
		ExitReason              mExitReason;                    /// One of enum ExitReason.
		RunnerTime              mResponseTimeoutSeconds;        /// If no response comes from the target process within this timeout, the app is assumed AWOL.
		RunnerTime              mResponseTimeoutAbsolute;       /// The absolute time at which mResponseTimeoutSeconds will pass.
		RunnerTime              mUnilateralTimeoutSeconds;      /// If the app doesn't quit within this timeout, the app is assumed to be AWOL, regardless of its responsiveness.
		RunnerTime              mUnilateralTimeoutAbsolute;     /// The absolute time at which mUnilateralTimeoutSeconds will pass.
		CancelCheckFunctionType mpCancelCheckFunction;          /// User-supplied function which is polled to see if we should cancel the run.
		void*                   mpCancelCheckFunctionContext;   /// User-supplied function context. Usually stores a 'this' pointer for the user.
		volatile bool           mbProcessIsRunning;             /// Used to tell if the process is currently running or not.
		FixedString16           msTargetProcessPath;            /// The path to the target process.
		FixedString16           msArguments;                    /// Holds command line arguments to the target process. This is an alternative to mTargetArgc/mTargetArgv.
		RunnerTime              mProcessStartTime;              /// Cache the time the process was started to avoid wraparound problems with GetTickCount()

		DWORD                   mProcessPriority;               ///
		FixedString16           msWorkingDirectory;             /// Working directory for the target process.
		bool                    mbActAsDebugger;                ///
		#if defined(EA_PLATFORM_WINDOWS)
		PROCESS_INFORMATION     mChildProcessInfo;              ///
		#endif
		SECURITY_ATTRIBUTES     mSecurityAttributes;            ///
		Pipe                    mPipe;                          ///
		FixedString8            mDebugString8;                  /// Stores 8 bit debug output strings from app.
		FixedString16           mDebugString16;                 /// Stores 16 bit debug output strings from app.

	}; // class Win32Runner

} // namespace Callstack
} // namespace EA


#endif  // EA_PLATFORM_WINDOWS

#endif // EACALLSTACK_INTERAL_WIN32RUNNER_H
