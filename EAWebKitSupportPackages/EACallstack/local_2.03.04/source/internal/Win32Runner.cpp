///////////////////////////////////////////////////////////////////////////////
// Win32Runner.cpp
//
// Copyright (c) 2008 Electronic Arts Inc.
// Created and maintained by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/internal/Win32Runner.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EATextUtil.h>


#if defined(EA_PLATFORM_WINDOWS)


#ifndef _WIN32_WINNT
	#undef  _WIN32_WINNT
	#define _WIN32_WINNT 0x0500 // Will work only on Win2000 and later 
#endif

#pragma warning(push, 0)

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <process.h>
#include <iostream>
#include <string>

#pragma warning(pop)
#pragma warning(disable: 4996) // 'stricmp': The POSIX name for this item is deprecated. 


namespace EA
{
namespace Callstack
{


///////////////////////////////////////////////////////////////////////////////
// ConsoleHandlerFunction
//
// This function intercepts console keyboard events from Windows.
//
BOOL WINAPI ConsoleHandlerFunction(DWORD dwCtrlType)
{
	switch(dwCtrlType)
	{
		// Possibly handler user control-c or control-break keyboard keys.
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
			break;
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Pipe::Pipe()
//
Win32Runner::Pipe::Pipe()
  : msOutput(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator())),
	mhStdinNew(NULL),
	mhStdoutNew(NULL),
	mhStdinWrite(NULL),
	mhStdoutRead(NULL)
{
}


///////////////////////////////////////////////////////////////////////////////
// Pipe::Reset()
//
void Win32Runner::Pipe::Reset()
{
	msOutput.clear();
	mhStdinNew   = NULL;
	mhStdoutNew  = NULL;
	mhStdinWrite = NULL;
	mhStdoutRead = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Win32Runner
//
Win32Runner::Win32Runner()
  : mbInitialized(false),
	mbPrintOutput(false),
	mbEnableProcessConsole(false),
	mExitCode(kResultError),
	mExitReason(kExitReasonNone),
	mResponseTimeoutSeconds(60),
	mResponseTimeoutAbsolute(kRunnerTimeMax),
	mUnilateralTimeoutSeconds(60 * 60),
	mUnilateralTimeoutAbsolute(kRunnerTimeMax),
	mpCancelCheckFunction(NULL),
	mpCancelCheckFunctionContext(NULL),
	mbProcessIsRunning(false),
	msTargetProcessPath(),
	msArguments(),
	mProcessStartTime(0),

	mProcessPriority(NORMAL_PRIORITY_CLASS),
	msWorkingDirectory(),
	mbActAsDebugger(true),
	mChildProcessInfo(),
	mSecurityAttributes(),
	mPipe(),
	mDebugString8(),
	mDebugString16()
{
	#if (EASTL_VERSION_N >= 11000)  // If using EASTL 1.10.00 or later...
		msTargetProcessPath.set_overflow_allocator(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator()));
		msArguments        .set_overflow_allocator(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator()));
		msWorkingDirectory .set_overflow_allocator(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator()));
		mDebugString8      .set_overflow_allocator(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator()));
		mDebugString16     .set_overflow_allocator(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "Win32Runner", EA::Callstack::GetAllocator()));
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// Win32Runner
//
Win32Runner::~Win32Runner()
{
	Win32Runner::Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool Win32Runner::Init()
{
	if(!mbInitialized)
	{
		mbInitialized = true;

		::SetConsoleCtrlHandler(ConsoleHandlerFunction, TRUE);

		memset(&mChildProcessInfo,   0, sizeof(mChildProcessInfo));

		memset(&mSecurityAttributes, 0, sizeof(mSecurityAttributes));
		mSecurityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES); 
		mSecurityAttributes.bInheritHandle       = TRUE; 
		mSecurityAttributes.lpSecurityDescriptor = NULL; 

		mPipe.Reset();
	}

	return mbInitialized;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool Win32Runner::Shutdown()
{
	// We leave some of the member variables as-is. In particular, the variables
	// that specify options on how to run this class are unmodified. 
	FreePipes();

	memset(&mChildProcessInfo, 0, sizeof(mChildProcessInfo));

	mExitCode          = kResultError;
	mExitReason        = kExitReasonNone;
	mbProcessIsRunning = false;

	mbInitialized = false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// SetTargetProcessPath
//
void Win32Runner::SetTargetProcessPath(const char16_t* pPath)
{
	msTargetProcessPath = pPath;
}


///////////////////////////////////////////////////////////////////////////////
// SetArgumentString
//
void Win32Runner::SetArgumentString(const char16_t* pArguments)
{
	msArguments = pArguments;
}


///////////////////////////////////////////////////////////////////////////////
// SetResponseTimeout
//
void Win32Runner::SetResponseTimeout(RunnerTime timeSeconds)
{
	mResponseTimeoutSeconds = timeSeconds;

	if(mResponseTimeoutSeconds <= 0)
		mResponseTimeoutSeconds = kRunnerTimeMax;
}


///////////////////////////////////////////////////////////////////////////////
// SetUnilateralTimeout
//
void Win32Runner::SetUnilateralTimeout(RunnerTime timeSeconds)
{
	mUnilateralTimeoutSeconds = timeSeconds;

	if(mUnilateralTimeoutSeconds <= 0)
		mUnilateralTimeoutSeconds = kRunnerTimeMax;
}


///////////////////////////////////////////////////////////////////////////////
// GetOutput
//
size_t Win32Runner::GetOutput(EA::Callstack::CAString8& sOutput) const
{
	// The string class, unlike other STL classes, doesn't have an iterator-based
	// insertion function, so we need to do it manually.
	eastl_size_t s    = 0;
	eastl_size_t sEnd = mPipe.msOutput.size();
	eastl_size_t d    = sOutput.size();
	eastl_size_t dEnd = d + mPipe.msOutput.size();

	sOutput.resize(dEnd);

	while(s < sEnd)
		sOutput[d++] = mPipe.msOutput[s++];

	return (size_t)mPipe.msOutput.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetRunnerTime
//
// Returns a 64 bit value indicating an absolute system millisecond time.
//
Win32Runner::RunnerTime Win32Runner::GetRunnerTime()
{
	FILETIME       lCurrentSystemTime;
	ULARGE_INTEGER lConvertedCurrentSystemTime;

	GetSystemTimeAsFileTime(&lCurrentSystemTime); // Returns a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	lConvertedCurrentSystemTime.HighPart = lCurrentSystemTime.dwHighDateTime;
	lConvertedCurrentSystemTime.LowPart  = lCurrentSystemTime.dwLowDateTime;

	return (RunnerTime)lConvertedCurrentSystemTime.QuadPart / 10000; // Convert 100-nanosecond (0.1 microsecond) to 1-millisecond.
}


///////////////////////////////////////////////////////////////////////////////
// GetPathDirectory
//
void Win32Runner::GetPathDirectory(const FixedString16& sPath, FixedString16& sDirectory)
{
	eastl_size_t pos(sPath.find_last_of(EA_CHAR16("\\/")));

	if(pos == FixedString8::npos)
		pos = sDirectory.size() - 1;

	sDirectory.assign(sPath.data(), pos + 1);
}


///////////////////////////////////////////////////////////////////////////////
// GetPathFileName
//
void Win32Runner::GetPathFileName(const FixedString16& sPath, FixedString16& sFileName)
{
	const eastl_size_t pos(sPath.find_last_of(EA_CHAR16("\\/")));

	if(pos == FixedString16::npos)
		sFileName = sPath;
	else
		sFileName.assign(sPath, pos + 1, sPath.size() - (pos + 1));
}


///////////////////////////////////////////////////////////////////////////////
// AllocatePipes
//
bool Win32Runner::AllocatePipes()
{
	bool bResult = true;

	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

EA_DISABLE_VC_WARNING(6248)
	SetSecurityDescriptorDacl(&sd, true, NULL, false);
EA_RESTORE_VC_WARNING()

	sa.lpSecurityDescriptor = &sd;
	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle       = true;

	if(!CreatePipe(&mPipe.mhStdinNew, &mPipe.mhStdinWrite, &sa, 0))
		bResult = false;

	if(!CreatePipe(&mPipe.mhStdoutRead, &mPipe.mhStdoutNew, &sa, 0))
		bResult = false;

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// FreePipes
//
void Win32Runner::FreePipes()
{
	// Don't clear the output, because the user will probably want it.
	// mPipe.msOutput.clear();

	if(mPipe.mhStdinNew)
	{
		CloseHandle(mPipe.mhStdinNew);
		mPipe.mhStdinNew = NULL;
	}

	if(mPipe.mhStdoutNew)
	{
		CloseHandle(mPipe.mhStdoutNew);
		mPipe.mhStdoutNew = NULL;
	}

	if(mPipe.mhStdinWrite)
	{
		CloseHandle(mPipe.mhStdinWrite);
		mPipe.mhStdinWrite = NULL;
	}

	if(mPipe.mhStdoutRead)
	{
		CloseHandle(mPipe.mhStdoutRead);
		mPipe.mhStdoutRead = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// ReadPipes
//
void Win32Runner::ReadPipes()
{
	char  buffer[1024]; buffer[0] = 0;
	DWORD dwCount      = 0;
	DWORD dwAvailable  = 0;
	DWORD dwRemaining  = 0;

	while(PeekNamedPipe(mPipe.mhStdoutRead, buffer, 1023, &dwCount, &dwAvailable, &dwRemaining) && dwCount)
	{
		if(ReadFile(mPipe.mhStdoutRead, buffer, 1023, &dwCount, NULL) && dwCount)
		{
			if(mPipe.msOutput.size() > 8000000)
				mPipe.msOutput.erase(mPipe.msOutput.begin(), mPipe.msOutput.begin() + 1000000); // Erase the first (oldest) N characters.

			mPipe.msOutput.insert(mPipe.msOutput.end(), buffer, buffer + dwCount);

			buffer[dwCount] = 0;
			TraceOccurred(buffer);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// FreeChildHandles
//
void Win32Runner::FreeChildHandles()
{
	// Close process and thread handles.
	if(mChildProcessInfo.hProcess)
	{
		if(!CloseHandle(mChildProcessInfo.hProcess))
			TraceLastError(GetLastError());

		mChildProcessInfo.hProcess = 0;
	}

	if(mChildProcessInfo.hThread)
	{
		if(!CloseHandle(mChildProcessInfo.hThread))
			TraceLastError(GetLastError());

		mChildProcessInfo.hThread = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
// ProcessDebugEvent
//
// struct DEBUG_EVENT {
//     DWORD dwDebugEventCode;
//     DWORD mLocalProcessId;
//     DWORD dwThreadId;
//     union {
//         EXCEPTION_DEBUG_INFO      Exception;
//         CREATE_THREAD_DEBUG_INFO  CreateThread;
//         CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
//         EXIT_THREAD_DEBUG_INFO    ExitThread;
//         EXIT_PROCESS_DEBUG_INFO   ExitProcess;
//         LOAD_DLL_DEBUG_INFO       LoadDll;
//         UNLOAD_DLL_DEBUG_INFO     UnloadDll;
//         OUTPUT_DEBUG_STRING_INFO  DebugString;
//         RIP_INFO                  RipInfo;
//     } u;
// };
//
void Win32Runner::ProcessDebugEvent(DEBUG_EVENT& debugEvent)
{
	switch (debugEvent.dwDebugEventCode)
	{
		case EXCEPTION_DEBUG_EVENT:
			//struct EXCEPTION_DEBUG_INFO {
			//    EXCEPTION_RECORD ExceptionRecord;
			//    DWORD dwFirstChance;
			//};
			//struct EXCEPTION_RECORD {
			//    DWORD             ExceptionCode;
			//    DWORD             ExceptionFlags;
			//    EXCEPTION_RECORD* ExceptionRecord;
			//    PVOID             ExceptionAddress;
			//    DWORD             NumberParameters;
			//    ULONG_PTR         ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
			//};

			// If the dwFirstChance member is nonzero, this is the first time the debugger has encountered the exception.
			if(debugEvent.u.Exception.dwFirstChance == 1)
			{
				if(debugEvent.u.Exception.ExceptionRecord.ExceptionCode == 0x80000003) // (0x80000003 == STATUS_BREAKPOINT)
					ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
				else
					ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
				return;
			}
			else
			{
				ExceptionOccurred(&debugEvent.u.Exception.ExceptionRecord);
				mExitReason = kExitReasonException;
				ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, 0);
				return;
			}

		case CREATE_THREAD_DEBUG_EVENT: 
			//struct CREATE_THREAD_DEBUG_INFO{
			//    HANDLE hThread;
			//    LPVOID lpThreadLocalBase;
			//    LPTHREAD_START_ROUTINE lpStartAddress;
			//};
			break;

		case CREATE_PROCESS_DEBUG_EVENT: 
			//struct CREATE_PROCESS_DEBUG_INFO {
			//    HANDLE hFile;
			//    HANDLE hProcess;
			//    HANDLE hThread;
			//    LPVOID lpBaseOfImage;
			//    DWORD  dwDebugInfoFileOffset;
			//    DWORD  nDebugInfoSize;
			//    LPVOID lpThreadLocalBase;
			//    LPTHREAD_START_ROUTINE lpStartAddress;
			//    LPVOID lpImageName;
			//    WORD   fUnicode;
			//};
			CloseHandle(debugEvent.u.CreateProcessInfo.hFile);
			break;
 
		case EXIT_THREAD_DEBUG_EVENT: 
			//struct EXIT_THREAD_DEBUG_INFO {
			//    DWORD dwExitCode;
			//};
			break;
 
		case EXIT_PROCESS_DEBUG_EVENT: 
			//struct EXIT_PROCESS_DEBUG_INFO {
			//    DWORD dwExitCode;
			//};
			mbProcessIsRunning = false;
			mExitCode = (int)(unsigned)debugEvent.u.ExitProcess.dwExitCode;
			break;
 
		case LOAD_DLL_DEBUG_EVENT: 
			//struct LOAD_DLL_DEBUG_INFO {
			//    HANDLE hFile;
			//    LPVOID lpBaseOfDll;
			//    DWORD  dwDebugInfoFileOffset;
			//    DWORD  nDebugInfoSize;
			//    LPVOID lpImageName;
			//    WORD   fUnicode;
			//};
			CloseHandle(debugEvent.u.LoadDll.hFile);
			break;
 
		case UNLOAD_DLL_DEBUG_EVENT: 
			//struct UNLOAD_DLL_DEBUG_INFO {
			//    LPVOID lpBaseOfDll;
			//};
			break;
 
		case OUTPUT_DEBUG_STRING_EVENT: 
		{
			//struct _OUTPUT_DEBUG_STRING_INFO {
			//    LPSTR lpDebugStringData;  // The debugging string in the calling process's address space. The debugger can use the ReadProcessMemory function to retrieve the value of the string.
			//    WORD  fUnicode;           // The format of the debugging string. If this member is zero, the debugging string is ANSI; if it is nonzero, the string is Unicode.
			//    WORD  nDebugStringLength; // The size of the debugging string, in characters. The length includes the string's terminating null character.
			//};

			if(debugEvent.u.DebugString.nDebugStringLength)
			{
				SIZE_T dwNumberOfBytesRead = 0;
				SIZE_T copySize;
				void*  pDestination;

				if(debugEvent.u.DebugString.fUnicode)
				{
					mDebugString16.resize((unsigned)debugEvent.u.DebugString.nDebugStringLength - 1);
					pDestination = (void*)&mDebugString16[0];
					copySize     = (unsigned)(debugEvent.u.DebugString.nDebugStringLength - 1) * 2;
				}
				else
				{
					mDebugString8.resize((unsigned)debugEvent.u.DebugString.nDebugStringLength - 1);
					pDestination = (void*)&mDebugString8[0];
					copySize     = (unsigned)(debugEvent.u.DebugString.nDebugStringLength - 1);
				}

				BOOL result = ReadProcessMemory(mChildProcessInfo.hProcess,
												debugEvent.u.DebugString.lpDebugStringData,
												pDestination,
												copySize,
												&dwNumberOfBytesRead);
				if(result)
				{
					if(debugEvent.u.DebugString.fUnicode)
					{
						// Convert 16 bit text to 8 bit text.
						// To do: Fix this to do proper text encoding conversion.
						mDebugString8.resize(mDebugString16.size());

						for(eastl_size_t i = 0, iEnd = mDebugString16.size(); i < iEnd; i++)
							mDebugString8[i] = (char)(unsigned char)mDebugString16[i];
					}

					TraceOccurred(mDebugString8.c_str());
				}
			}

			break;
		}

		case RIP_EVENT:
			//struct RIP_INFO {
			//    DWORD dwError;
			//    DWORD dwType;
			//};
			mbProcessIsRunning = false;
			mExitReason        = kExitReasonException; 
			break;

		default:
		{
			TraceOccurred("Error: Unknown debug event.\n");
			break;
		}
	}

	ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
}


///////////////////////////////////////////////////////////////////////////////
// ExceptionOccurred
//
void Win32Runner::ExceptionOccurred(const void* pExceptionInformation)
{
	const EXCEPTION_RECORD* const pExceptionRecord = (EXCEPTION_RECORD*)pExceptionInformation;

	// To do: We need to do more than this.
	// At the least we need to print a human readable string for the exception
	// code. We also need to have a mechanism whereby the user can specify
	// a process to spawn upon the exception occurring. This process might
	// be an exception report uploader or at least a package such as Symmogrifier.

	if(mbPrintOutput)
	{
		printf("Target process generated an exception.\n");
		printf(" Code     = 0x%08x\n", (uint32_t) pExceptionRecord->ExceptionCode);
		printf(" Address  = 0x%016" PRIX64" \n", (uint64_t) pExceptionRecord->ExceptionAddress);
		printf(" Flags    = 0x%08x\n", (uint32_t) pExceptionRecord->ExceptionFlags);
	}

	TerminateProcess(mChildProcessInfo.hProcess, 0xffff);
	mbProcessIsRunning = false;
}


///////////////////////////////////////////////////////////////////////////////
// TimeoutOccurred
//
void Win32Runner::TimeoutOccurred()
{
	const bool bUnilateralTimeoutOccurred = (GetRunnerTime() > mUnilateralTimeoutAbsolute);

	if(mbPrintOutput)
	{
		printf("Target process still running after the %s timeout period elapsed.\n", bUnilateralTimeoutOccurred ? "unilateral" : "response");
		printf("Response timeout = %d seconds; unilateral timeout = %d seconds.\n", (int)mResponseTimeoutSeconds, (int)mUnilateralTimeoutSeconds);
	}

	TerminateProcess(mChildProcessInfo.hProcess, 0xffff);
	mbProcessIsRunning = false;
}


///////////////////////////////////////////////////////////////////////////////
// CancelOccurred
//
void Win32Runner::CancelOccurred()
{
	if(mbPrintOutput)
		printf("User requested cancellation of run.\n");

	TerminateProcess(mChildProcessInfo.hProcess, 0xffff);
	mbProcessIsRunning = false;
}


///////////////////////////////////////////////////////////////////////////////
// CompletionOccurred
//
void Win32Runner::CompletionOccurred(int /*exitCode*/)
{
	// We do nothing at this time.
}


///////////////////////////////////////////////////////////////////////////////
// TraceOccurred
//
void Win32Runner::TraceOccurred(const char* pText)
{
	// Any input pushes back the respone timeout.
	if(strstr(pText, "keepalive"))
	{
		if(mUnilateralTimeoutAbsolute != kRunnerTimeMax)
			mUnilateralTimeoutAbsolute = GetRunnerTime() + (mUnilateralTimeoutSeconds * 1000); // Convert seconds to milliseconds.
	}

	if(mResponseTimeoutAbsolute != kRunnerTimeMax)
		mResponseTimeoutAbsolute = GetRunnerTime() + (mResponseTimeoutSeconds * 1000);
	
	if(mbPrintOutput)
	{
		// Echo the process output to our stdout.
		printf("%s", pText); // We could alternatively read msDebugTraceBuffer.
	 
		if(IsDebuggerPresent())
			OutputDebugStringA(pText);
	 
		// flush to make sure calling programs get output in real time, otherwise the output may be buffered.
		fflush(stdout);
	}
}


///////////////////////////////////////////////////////////////////////////////
// TraceLastError
//
void Win32Runner::TraceLastError(unsigned error)
{
	if(mbPrintOutput)
	{
		char* pErrorString;
		DWORD dwError = error ? error : GetLastError();

		if(FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
						  NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						  (LPSTR)&pErrorString, 0, NULL) != 0)
		{
			printf("%s", pErrorString);
			LocalFree(pErrorString);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// Run
//
int Win32Runner::Run()
{
	mExitCode   = kResultError;
	mExitReason = kExitReasonNone;
	mPipe.msOutput.clear();

	if(!msTargetProcessPath.empty())
	{
		// if(IsMachineNameLocal(msTargetMachineName))
			LocalRun();
		// else
		//     RemoteRun();
	}

	if(mExitReason == kExitReasonDone)
		CompletionOccurred(mExitCode);

	return mExitCode;
}


///////////////////////////////////////////////////////////////////////////////
// LocalRun
//
int Win32Runner::LocalRun()
{
	bool bUserCancelled = false;

	FixedString16 sTargetDirectory;
	FixedString16 sTargetFileName;

	GetPathDirectory(msTargetProcessPath, sTargetDirectory);
	GetPathFileName(msTargetProcessPath, sTargetFileName);

	bool bResult = AllocatePipes();

	if(bResult)
	{
		DWORD dwExitCode;
		DWORD dwCreationFlags = (DWORD)((mbEnableProcessConsole ? CREATE_NEW_CONSOLE : 0) | (mbActAsDebugger ? DEBUG_ONLY_THIS_PROCESS : 0));

		STARTUPINFOW childStartupInfo;
		memset(&childStartupInfo, 0, sizeof(childStartupInfo));
		childStartupInfo.cb = sizeof(STARTUPINFO);  

		// We have to specify a full set of handles.
		childStartupInfo.dwFlags    = STARTF_USESTDHANDLES;
		childStartupInfo.hStdOutput = mPipe.mhStdoutNew;
		childStartupInfo.hStdError  = mPipe.mhStdoutNew;
		childStartupInfo.hStdInput  = mPipe.mhStdinNew;

		mbProcessIsRunning = true;

		bResult = CreateProcessW(reinterpret_cast<LPCWSTR>(msTargetProcessPath.c_str()),        // lpApplicationName - if 0 the first arg of the cmdline is used and the path is searched.
								 (LPWSTR)msArguments.c_str(),                                   // lpCommandLine
								 &mSecurityAttributes,                                          // lpProcessAttributes
								 NULL,                                                          // lpThreadAttributes
								 TRUE,                                                          // bInheritHandles
								 dwCreationFlags,                                               // dwCreationFlags
								 NULL,														    // lpEnvironment - use current environment
								 reinterpret_cast<LPCWSTR>(sTargetDirectory.c_str()),           // lpCurrentDirectory
								 &childStartupInfo,                                             // lpStartupInfo
								 &mChildProcessInfo) != 0;                                      // child process information - structure populated by CreateProcess

		if(bResult)
		{
			mProcessStartTime = GetRunnerTime();

			// Set our initial timeout absolute values.
			if(mUnilateralTimeoutSeconds < mResponseTimeoutSeconds)
				mUnilateralTimeoutSeconds = mResponseTimeoutSeconds;

			if(mResponseTimeoutSeconds == kRunnerTimeMax)
				mResponseTimeoutAbsolute = kRunnerTimeMax;
			else
				mResponseTimeoutAbsolute = GetRunnerTime() + (mResponseTimeoutSeconds * 1000); // Convert seconds to milliseconds.

			if(mUnilateralTimeoutSeconds == kRunnerTimeMax)
				mUnilateralTimeoutAbsolute = kRunnerTimeMax;
			else
				mUnilateralTimeoutAbsolute = GetRunnerTime() + (mUnilateralTimeoutSeconds * 1000);

			if(mExitReason == kExitReasonNone)
				mExitReason = kExitReasonDone; // Set the reason to 'done' until some other exit reason occurs.

			// We run a loop reading the child process output and checking for child exit.
			while(mbProcessIsRunning)
			{
				if(mpCancelCheckFunction && mpCancelCheckFunction(this, mpCancelCheckFunctionContext))
				{
					bUserCancelled = true;
					if(mbPrintOutput)
						printf("Cancelling the run.\n");
					break;
				}

				if (GetRunnerTime() >= (mResponseTimeoutAbsolute < mUnilateralTimeoutAbsolute ? mResponseTimeoutAbsolute : mUnilateralTimeoutAbsolute)) // If we have a timeout...
					break;

				if(mbActAsDebugger)
				{
					DEBUG_EVENT debugEvent;

					if(WaitForDebugEvent(&debugEvent, 200))
						ProcessDebugEvent(debugEvent);
					else if(GetExitCodeProcess(mChildProcessInfo.hProcess, (LPDWORD)&dwExitCode) && (dwExitCode != STILL_ACTIVE))
						mbProcessIsRunning = false;
				}
				else
				{
					Sleep(200);

					if(GetExitCodeProcess(mChildProcessInfo.hProcess, (LPDWORD)&dwExitCode) && (dwExitCode != STILL_ACTIVE))
						mbProcessIsRunning = false;
					else
						TraceLastError(GetLastError());
				}

				ReadPipes();
			}

			if(mbProcessIsRunning) // If the process is still running but we cancelled it or we determined that the process timed out...
			{
				mbProcessIsRunning = false;

				if(bUserCancelled) // If the break out of the loop above occurred because the user cancelled...
				{
					mExitReason = kExitReasonCancel;
					CancelOccurred();
				}
				else
				{
					mExitReason = kExitReasonTimeout;
					TimeoutOccurred();
				}
			}

			FreeChildHandles();
		}
		else
		{
			mExitCode = -1;
			mbProcessIsRunning = false;
			TraceLastError(GetLastError());
		}
	}

	FreePipes();

	return mExitCode;
}


int Win32Runner::RunToCompletionWithOutput(const char16_t* pCommandString, int64_t responseTimeoutSeconds, int64_t unilateralTimeoutSeconds, EA::Callstack::CAString8& sOutput)
{
	int result(-1);

	// The following variables are for the sole purpose of parsing the command line string into app and arguments.
	std::basic_string<char16_t> s16(pCommandString);
	std::basic_string<char16_t> sAppPath;
	const char16_t*             pBeginOfArguments;
	const char16_t*             pToken;
	const char16_t*             pTokenEnd;

	if(EA::StdC::ParseDelimitedText(pCommandString, pCommandString + EA::StdC::Strlen(pCommandString), char16_t(' '), 
									pToken, pTokenEnd, &pBeginOfArguments))
	{
		sAppPath.assign(pToken, pTokenEnd);

		while((*pBeginOfArguments == ' ') || (*pBeginOfArguments == 't'))
			++pBeginOfArguments; // Eat any leading whitespace.

		Win32Runner runner;

		if(runner.Init())
		{
			runner.SetTargetProcessPath(sAppPath.c_str());
			runner.SetArgumentString(pBeginOfArguments);
			runner.SetResponseTimeout(responseTimeoutSeconds);
			runner.SetUnilateralTimeout(unilateralTimeoutSeconds);
			result = runner.Run();
			runner.GetOutput(sOutput); 
			runner.Shutdown();
		}
	}

	return result;
}


int Win32Runner::RunToCompletionWithOutput(const char* pCommandString, int64_t responseTimeoutSeconds, int64_t unilateralTimeoutSeconds, EA::Callstack::CAString8& sOutput)
{
	std::basic_string<char>  s8(pCommandString);
	std::basic_string<char16_t> s16(EA::StdC::ConvertString<std::basic_string<char>, std::basic_string<char16_t> >(s8));

	return RunToCompletionWithOutput(s16.c_str(), responseTimeoutSeconds, unilateralTimeoutSeconds, sOutput);
}




} // namespace Runner

} // namespace EA


#endif // #if defined(EA_PLATFORM_WINDOWS)




