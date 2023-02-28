///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/EAAddressRep.h>
#include <EACallstack/MapFile.h>
#include <EACallstack/EASymbolUtil.h>
#include <EACallstack/Allocator.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAScanf.h>
#include <EAStdC/EACType.h>
#include <EAStdC/EATextUtil.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAStreamBuffer.h>
#include <EASTL/algorithm.h>
#include <EASTL/fixed_substring.h>
#include <stdlib.h>
#include <EAAssert/eaassert.h>


namespace EA
{
namespace Callstack
{

// This "bookmark" function is a known location in the library that we can use to
// get the module base address if address space layout randomization is enabled.
// C linkage is specified so that demangling is not an issue.
//
// Address space layout randomization on Apple platforms causes the entire module
// to be rebased in the interests of security but this causes issues when resolving
// names of functions to their addresses as often the addresses in the map file or
// debug information are based on the un-relocated addresses.
extern "C" EA_PREFIX_NO_INLINE void EACallstackBookmarkFunction() EA_POSTFIX_NO_INLINE;

void EACallstackBookmarkFunction()
{
}

const char *gpBookmarkFunctionSymbol = "EACallstackBookmarkFunction";

#if EACALLSTACK_MSVC_MAPFILE_ENABLED

///////////////////////////////////////////////////////////////////////////////
// MapFileMSVC
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// A Microsoft Xenon and VC8 .map file looks like the text below.
// The space at the beginning of each line is present in the map file.
///////////////////////////////////////////////////////////////////////////////
// ExceptionHandlerTest_Xenon
//
// Timestamp is 4470f2d5 (Sun May 21 16:08:05 2006)
//
// Preferred load address is 82000000
//
// Start         Length     Name                   Class
// 0001:00000000 000000f8H .idata$5                DATA
// 0001:00000100 00003766H .rdata                  DATA
// 0001:00003868 00000083H .rdata$debug            DATA
// 0001:000038f0 00000384H .xdata                  DATA
// 0002:00000000 00000c10H .pdata                  DATA
// 0003:00000000 000200b4H .text                   CODE
// 0004:00000050 000016e8H .data                   DATA
// 0004:00001740 00001548H .bss                    DATA
// 0005:00000000 0000003cH .idata$2                DATA
// 0005:0000003c 00000014H .idata$3                DATA
// 0006:00000000 00000030H .XBLD$V                 DATA
//
//  Address         Publics by Value              Rva+Base       Lib:Object
//
// 0000:00000000       ___safe_se_handler_count   00000000     <absolute>
// 0000:00000000       ___safe_se_handler_table   00000000     <absolute>
// 0001:00000000       __imp_XGetLanguage         82000400     XAPILIB:xam.xex@2920.0+1861.0
// 0001:00000004       __imp_XGetAVPack           82000404     XAPILIB:xam.xex@2920.0+1861.0
// 0001:00000008       __imp_XamTerminateTitle    82000408     XAPILIB:xam.xex@2920.0+1861.0
// 0001:0000000c       __imp_XamShowMessageBoxUIEx 8200040c     XAPILIB:xam.xex@2920.0+1861.0
// 0001:00000010       \177xam.xex@2920.0+1861_NULL_THUNK_DATA 82000410     XAPILIB:xam.xex@2920.0+1861.0
// 0001:00000014       __imp_DmReboot             82000414     XbDm:xbdm.xex@2920.0+1861.0
// 0001:00000018       __imp_DmIsDebuggerPresent  82000418     XbDm:xbdm.xex@2920.0+1861.0
// 0001:0000001c       __imp_DmWalkLoadedModules  8200041c     XbDm:xbdm.xex@2920.0+1861.0
// 0003:00000098       ?TestStackOverflow@@YAXPAX@Z 82010098 f   ExceptionHandlerTest.obj
// 0003:000000a8       ?TestStackCorruption@@YAXPAX@Z 820100a8 f   ExceptionHandlerTest.obj
// 0003:000000b8       main                       820100b8 f   ExceptionHandlerTest.obj
// 0003:00000218       ??0AllocatorInfo@?A0x76db9064@@QAA@XZ 82010218 f i MemoryMan.obj
// 0003:00000278       ??_U@YAPAXI@Z              82010278 f   MemoryMan.obj
// 0003:00000290       ??_V@YAXPAX@Z              82010290 f   MemoryMan.obj
// 0003:00000290       ??3@YAXPAX@Z               82010290 f   MemoryMan.obj
// 0003:000002b0       ??2@YAPAXIPBDHI0H@Z        820102b0 f   MemoryMan.obj
// 0003:000002c0       ??_GSharedAllocator@?A0x76db9064@@QAAPAXI@Z 820102c0 f i MemoryMan.obj
// 0003:00000328       ?ShutdownSharedAllocator@Allocator@EA@@YAXXZ 82010328 f   MemoryMan.obj
// 0003:00000388       ?Create@SharedAllocator@?A0x76db9064@@SAPAUOSGlobalNode@EA@@XZ 82010388 f i MemoryMan.obj
// 0003:000003d8       ?InitSharedAllocator@Allocator@EA@@YA_NXZ 820103d8 f   MemoryMan.obj
// 0003:000204bc       DmIsDebuggerPresent        820304bc f   XbDm:xbdm.xex@2920.0+1861.0
// 0003:000204cc       DmReboot                   820304cc f   XbDm:xbdm.xex@2920.0+1861.0
// 0004:00000000       __xri_a                    82040000     XAPILIB:xapi0dat.obj
// 0004:0000000c       __xri_z                    8204000c     XAPILIB:xapi0dat.obj
// 0004:00000010       __xc_a                     82040010     XAPILIB:xapi0dat.obj
// 0004:00000040       __xt_a                     82040040     LIBCMT:crt0init.obj
// 0004:00000044       __xt_z                     82040044     LIBCMT:crt0init.obj
// 0004:0000005c       ?gpReportFunction@UnitTest@EA@@3P6AXPBD@ZA 8204005c     UTFCombined_Xenon:EATest.obj
// 0004:00000078       ?kStrTrapCondition@PowerPC@Dasm@EA@@3PAPBDA 82040078     UTFCombined_Xenon:EADasm.obj
// 0004:000000f8       ?kStrCondition@PowerPC@Dasm@EA@@3PAPBDA 820400f8     UTFCombined_Xenon:EADasm.obj
// 0004:00000118       ?kStrCompare@PowerPC@Dasm@EA@@3PAPBDA 82040118     UTFCombined_Xenon:EADasm.obj
// 0004:00002b60       __pioinfo                  82042b60     <common>
// 0004:00002c60       __onexitend                82042c60     <common>
// 0004:00002c7c       __@@_PchSym_@00@UcvmlmUnzbAGUxlivUkirezgvUxlivoryhUcigoUkirezgvUlyqUcylcUcigokxsOlyq@xrtlpp 82042c7c     <common>
// 0004:00002c80       XapiCurrentTopLevelFilter  82042c80     <common>
// 0004:00002c84       NtGlobalFlag               82042c84     <common>
// 0005:00000000       __IMPORT_DESCRIPTOR_xam.xex@2920.0+1861 82050000     XAPILIB:xam.xex@2920.0+1861.0
// 0005:00000014       __IMPORT_DESCRIPTOR_xboxkrnl.exe@2920.0+1861 82050014     XBOXKRNL:xboxkrnl.exe@2920.0+1861.0
// 0005:00000028       __IMPORT_DESCRIPTOR_xbdm.xex@2920.0+1861 82050028     XbDm:xbdm.xex@2920.0+1861.0
// 0005:0000003c       __NULL_IMPORT_DESCRIPTOR   8205003c     XAPILIB:xam.xex@2920.0+1861.0
// 0006:00000000       __CrtBuildNumber           82060000     LIBCMT:onexit.obj
// 0006:00000010       XapiBuildNumber            82060010     XAPILIB:xapipch.obj
// 0006:00000020       XboxKrnlBuildNumber        82060020     XBOXKRNL:bldnum.obj
//
// entry point at        0003:0001e018
//
// Static symbols
//
// 0003:000060c0       ?ReadFormat8@@YAPBDPBDPAUFormatData@@PAPAD@Z 820160c0 f   UTFCombined_Xenon:EASprintf.obj
// 0003:00006550       ?WriteLong8@@YAPADABUFormatData@@JPAD@Z 82016550 f   UTFCombined_Xenon:EASprintf.obj
// 0003:000067b8       ?WriteLongLong8@@YAPADABUFormatData@@_JPAD@Z 820167b8 f   UTFCombined_Xenon:EASprintf.obj
// 0003:00006a28       ?WriteDouble8@@YAPADABUFormatData@@NPAD@Z 82016a28 f   UTFCombined_Xenon:EASprintf.obj
// 0003:00006ef8       ?VprintfCore8@@YAHP6AIPBDIPAX@Z10PAD@Z 82016ef8 f   UTFCombined_Xenon:EASprintf.obj
// 0003:00007f84       $$$00003                   82017f84 f   LIBCMT:crtgpr.obj
// 0003:00010f00       ?_fpcvt@@YAHPADIPAU_strflt@@HPAH2@Z 82020f00 f   LIBCMT:fcvt.obj
// 0003:00011650       doexit                     82021650 f   LIBCMT:crt0dat.obj
// 0003:00011f28       flsall                     82021f28 f   LIBCMT:fflush.obj
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// A Microsoft VC7 .map file looks like the text below.
// It is similar to a VC8 and Xenon .map file except it has additional
// information that allows you to tell file/line information.
// The space at the beginning of each line is present in the map file.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ExceptionHandlerTest_Win32
//
// Timestamp is 44f5d60c (Wed Aug 30 11:16:44 2006)
//
// Preferred load address is 00400000
//
// Start         Length     Name                   Class
// 0001:00000000 0006919aH .text                   CODE
// 0001:000691a0 0000026fH .text$yc                CODE
// 0001:00069410 000004f6H .text$yd                CODE
// 0002:00000000 000003d0H .idata$5                DATA
// 0002:000003d0 00008108H .rdata                  DATA
// 0002:000084d8 00000081H .rdata$debug            DATA
// 0002:0000855c 00000044H .rdata$r                DATA
// 0002:000085a0 00000004H .rdata$sxdata           DATA
// 0002:000085a4 00000004H .rtc$IAA                DATA
// 0002:000085a8 000000a4H .rtc$IMZ                DATA
// 0002:0000864c 00000004H .rtc$IZZ                DATA
// 0002:00008650 00000004H .rtc$TAA                DATA
// 0002:00008654 000000a4H .rtc$TMZ                DATA
// 0002:000086f8 00000004H .rtc$TZZ                DATA
// 0002:000086fc 00000054H .xdata$x                DATA
// 0002:00008750 000000b4H .idata$2                DATA
// 0002:00008804 00000014H .idata$3                DATA
// 0002:00008818 000003d0H .idata$4                DATA
// 0002:00008be8 00000e9cH .idata$6                DATA
// 0002:00009a84 00000000H .edata                  DATA
// 0003:00000000 00000004H .CRT$XCA                DATA
// 0003:00000004 00000004H .CRT$XCAA               DATA
// 0003:00000008 00000008H .CRT$XCC                DATA
// 0003:00000010 00000010H .CRT$XCF                DATA
// 0003:00000020 00000010H .CRT$XCL                DATA
// 0003:00000030 00000018H .CRT$XCU                DATA
// 0003:00000048 00000004H .CRT$XCZ                DATA
// 0003:0000004c 00000004H .CRT$XIA                DATA
// 0003:00000050 00000004H .CRT$XIZ                DATA
// 0003:00000060 00000348H .data                   DATA
// 0003:000003a8 00002698H .bss                    DATA
// 0004:00000000 00000058H .rsrc$01                DATA
// 0004:00000060 00000298H .rsrc$02                DATA
// 0004:00000060 00000298H .rsrc$02                DATA
//
//  Address         Publics by Value              Rva+Base     Lib:Object
//
// 0000:00000000       __except_list              00000000     <absolute>
// 0000:00000001       ___safe_se_handler_count   00000001     <absolute>
// 0000:00009876       __fltused                  00009876     <absolute>
// 0000:00009876       __ldused                   00009876     <absolute>
// 0001:00000000       ?InitSharedAllocator@Allocator@EA@@YA_NXZ 00401000 f   ExceptionHandlerTest.obj
// 0001:0006917c       _SHGetFolderPathW@20       0046a17c f   shell32:SHELL32.dll
// 0001:00069182       _CoUninitialize@0          0046a182 f   ole32:ole32.dll
// 0001:00069188       _CoCreateInstance@20       0046a188 f   ole32:ole32.dll
// 0001:0006918e       _CoInitialize@4            0046a18e f   ole32:ole32.dll
// 0001:00069194       __ecvt                     0046a194 f   MSVCRTD:MSVCR71D.dll
// 0002:00000000       __imp__GetUserNameA@8      0046b000     advapi32:ADVAPI32.dll
// 0002:00000004       \177ADVAPI32_NULL_THUNK_DATA 0046b004     advapi32:ADVAPI32.dll
// 0002:00000008       __imp__GetProcessHeap@0    0046b008     kernel32:KERNEL32.dll
// 0002:0000000c       __imp__HeapFree@12         0046b00c     kernel32:KERNEL32.dll
// 0002:00000010       __imp__DuplicateHandle@28  0046b010     kernel32:KERNEL32.dll
// 0002:00000014       __imp__TlsAlloc@0          0046b014     kernel32:KERNEL32.dll
// 0002:00000018       __imp__TlsGetValue@4       0046b018     kernel32:KERNEL32.dll
// 0002:0000001c       __imp__TlsSetValue@8       0046b01c     kernel32:KERNEL32.dll
// 0002:00000020       __imp__GetExitCodeThread@8 0046b020     kernel32:KERNEL32.dll
// 0002:000087b4       __IMPORT_DESCRIPTOR_USER32 004737b4     user32:USER32.dll
// 0002:000087c8       __IMPORT_DESCRIPTOR_ADVAPI32 004737c8     advapi32:ADVAPI32.dll
// 0002:000087dc       __IMPORT_DESCRIPTOR_SHELL32 004737dc     shell32:SHELL32.dll
// 0002:000087f0       __IMPORT_DESCRIPTOR_ole32  004737f0     ole32:ole32.dll
// 0002:00008804       __NULL_IMPORT_DESCRIPTOR   00473804     kernel32:KERNEL32.dll
// 0003:00000000       ___xc_a                    00475000     MSVCRTD:cinitexe.obj
// 0003:00000048       ___xc_z                    00475048     MSVCRTD:cinitexe.obj
// 0003:0000004c       ___xi_a                    0047504c     MSVCRTD:cinitexe.obj
// 0003:00000050       ___xi_z                    00475050     MSVCRTD:cinitexe.obj
// 0003:00000060       ??_R0?AVexception@@@8      00475060     ExceptionHandlerTest.obj
// 0003:00000078       ??_R0?AVbad_alloc@std@@@8  00475078     ExceptionHandlerTest.obj
// 0003:00000094       ?gpReportFunction@UnitTest@EA@@3P6AXPBD@ZA 00475094     UTFCombined_Win32:EATest.obj
// 0003:000000f4       ?gpAssertionFailureFunction@eastl@@3P6AXPBDPAX@ZA 004750f4     UTFCombined_Win32:assert.obj
// 0003:000000f8       ?gpEmptyBucketArray@eastl@@3PAPAXA 004750f8     UTFCombined_Win32:hashtable.obj
// 0003:00002a3c       ___onexitbegin             00477a3c     <common>
//
// entry point at        0001:00067460
//
// Static symbols
//
// 0001:00019a40       ?GetNextPathComponent@IO@EA@@YA_NABV?$fixed_string@_W$0BAE@$00@eastl@@AAV34@AAI@Z 0041aa40 f   UTFCombined_Win32:EAFilePath.obj
// 0001:0001e4d0       ?PPM_VERIFY@@YAHQBVGeneralAllocator@Allocator@EA@@_N@Z 0041f4d0 f   UTFCombined_Win32:EAGeneralAllocator.obj
// 0001:00040480       ?VprintfCore8@@YAHP6AIPBDIPAX@Z10PAD@Z 00441480 f   UTFCombined_Win32:EASprintf.obj
// 0001:00040ff0       ?ReadFormat8@@YAPBDPBDPAUFormatData@@PAPAD@Z 00441ff0 f   UTFCombined_Win32:EASprintf.obj
// 0001:00041630       ?WriteLong8@@YAPADABUFormatData@@JPAD@Z 00442630 f   UTFCombined_Win32:EASprintf.obj
//
//Line numbers for \Projects\Helios\Ctg_ears\Dl\obj\Win32 Debug\ExceptionHandlerTest_Win32\ExceptionHandlerTest.obj(f:\projects\helios\ctg_ears\dl\utfkernel\eathread\include\eathread\x86\eathread_atomic_x86.h) segment .text
//
//   116 0001:00000f60
//
//Line numbers for \Projects\Helios\Ctg_ears\Dl\obj\Win32 Debug\ExceptionHandlerTest_Win32\ExceptionHandlerTest.obj(f:\projects\helios\ctg_ears\dl\utfkernel\utffoundation\include\foundation\eafilepath.h) segment .text
//
//   547 0001:00000ea0   550 0001:00000ec6   552 0001:00000ecd   553 0001:00000ee5
//   554 0001:00000ee8
//
//Line numbers for \Projects\Helios\Ctg_ears\Dl\obj\Win32 Debug\ExceptionHandlerTest_Win32\ExceptionHandlerTest.obj(f:\projects\helios\ctg_ears\dl\utfkernel\utffoundation\include\foundation\eatest.h) segment .text
//
//   188 0001:00000e50   189 0001:00000e53   191 0001:00000e5b   192 0001:00000e68
//   194 0001:00000e88   195 0001:00000e8b
//
//Line numbers for \Projects\Helios\Ctg_ears\Dl\obj\Win32 Debug\ExceptionHandlerTest_Win32\ExceptionHandlerTest.obj(f:\projects\helios\ctg_ears\dl\utfdiagnostics\eacallstack\include\eacallstack\eaaddressrep.h) segment .text
//
//   224 0001:00000e20
//
//Line numbers for c:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\lib\MSVCRTD.lib(F:\VS70Builds\3077\vc\crtbld\crt\src\intel\ullshr.asm) segment .text
//
//    48 0001:00067980    42 0001:00067980    49 0001:00067983    54 0001:00067985
//    55 0001:00067988    56 0001:0006798a    57 0001:0006798d    58 0001:0006798f
//    64 0001:00067990    65 0001:00067992    66 0001:00067994    67 0001:00067997
//    68 0001:00067999    74 0001:0006799a    75 0001:0006799c    76 0001:0006799e
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// MapFileMSVC
//
MapFileMSVC::MapFileMSVC(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mbLoadAttempted(false),
	mbLowMemoryUsage(false),
	mbOfflineLookup(false),
	mbSymbolEndFound(false),
	msMapFilePath(),
	mBaseAddress(0),
	mMapFileBaseAddress(0),
	mFileBufferSize(4096),
	mFile(),
	mFileBuffer(IO::StreamBuffer::kBufferSizeUnspecified, IO::StreamBuffer::kBufferSizeUnspecified, NULL, mpCoreAllocator),
	mFileLowMemoryUsage(),
	mFileBufferLowMemoryUsage(IO::StreamBuffer::kBufferSizeUnspecified, IO::StreamBuffer::kBufferSizeUnspecified, NULL, mpCoreAllocator),
	mFileSize(0),
	mPhase(kPhaseIdle),
	mSymbolsPerLowMemSample(0), // Will be initialized in Load.
	mAddressToSymbolMap  (EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator)),
	mLineFilenameList    (EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator)),
	mAddressToFileLineMap(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator)),
	mSectionArray        (EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator)),
	mCodeGroupArray          (EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator)),
	mRVAToFilePosArray   (EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", mpCoreAllocator))
{
	mFile.AddRef(); // This prevents it from being released away by anybody.
	mFileLowMemoryUsage.AddRef();
}


///////////////////////////////////////////////////////////////////////////////
// ~MapFileMSVC
//
MapFileMSVC::~MapFileMSVC()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void MapFileMSVC::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;

	mAddressToSymbolMap  .get_allocator().set_allocator(pCoreAllocator);
	mLineFilenameList    .get_allocator().set_allocator(pCoreAllocator);
	mAddressToFileLineMap.get_allocator().set_allocator(pCoreAllocator);
	mSectionArray        .get_allocator().set_allocator(pCoreAllocator);
	mCodeGroupArray      .get_allocator().set_allocator(pCoreAllocator);
	mRVAToFilePosArray   .get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool MapFileMSVC::Init(const wchar_t* pMapFilePath, bool bDelayLoad)
{
	if(pMapFilePath)
		msMapFilePath = pMapFilePath;

	if(!bDelayLoad)
		Load();

	return (bDelayLoad || !mAddressToSymbolMap.empty());
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool MapFileMSVC::Shutdown()
{
	mbLoadAttempted     = false;
	mbSymbolEndFound    = false;
	mBaseAddress        = kBaseAddressUnspecified;
	mMapFileBaseAddress = 0; // Set to 0 and not kBaseAddressUnspecified, as this isn't something the user specifies.

	mFile.Close();
	mFileBuffer.SetStream(NULL);
	mFileBuffer.SetBufferSizes(0, 0);

	mFileLowMemoryUsage.Close();
	mFileBufferLowMemoryUsage.SetStream(NULL);
	mFileBufferLowMemoryUsage.SetBufferSizes(0, 0);

	mFileSize = 0;
	mPhase    = kPhaseIdle;

	mAddressToSymbolMap.clear();
	mLineFilenameList.clear();
	mAddressToFileLineMap.clear();
	mSectionArray.set_capacity(0);
	mCodeGroupArray.set_capacity(0);
	mRVAToFilePosArray.set_capacity(0);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* MapFileMSVC::AsInterface(int id)
{
	if(id == MapFileMSVC::kTypeId)
		return static_cast<MapFileMSVC*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void MapFileMSVC::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t MapFileMSVC::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* MapFileMSVC::GetDatabasePath() const
{
	return msMapFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void MapFileMSVC::SetOption(int option, int value)
{
	if(option == kOptionLowMemoryUsage)
	{
		EA_ASSERT(!mbLoadAttempted); // Must set this option before loading anything or doing any lookups.
		mbLowMemoryUsage = (value != 0);
	}
	else if(option == kOptionFileBufferSize)
		mFileBufferSize = (uint32_t)value;
	// else if(option == kOptionCopyFileToMemory) // Not currently supported.
	//    mbUseMemoryFile = (value != 0);
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int MapFileMSVC::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(!mbLoadAttempted)
		Load();

	// If the user hasn't set the base address, then we have no choice but to assume it's equal to the map file default base address.
	uint64_t baseAddressToUse = ((mBaseAddress == kBaseAddressUnspecified) ? mMapFileBaseAddress : mBaseAddress);

	// We need to make the following check here, as the code below doesn't account for it.
	// We don't need to make an upper-bound check, as the code below does account for it.
	uint64_t nMaxAddress; // nMaxAddress is an absolute address (same as mBaseAddress and nAddress).

	if(mbLowMemoryUsage) // If we haven't read the entire .map file ahead of time but instead are reading it incrementally...
	{
		// We don't know what the max address is without having read through the entire .map file, which by definition we aren't doing.
		// So we use a rule of thumb which in practice will yield no false failures for 99% of apps (100% on some platforms).
		#if defined(EA_PLATFORM_WIN64)
			nMaxAddress = baseAddressToUse + 300000000;    // ~300 MB
		#elif defined(EA_PLATFORM_MOBILE)
			nMaxAddress = baseAddressToUse +  70000000;    //  ~70 MB
		#elif defined(EA_PLATFORM_CONSOLE)
			nMaxAddress = baseAddressToUse + 250000000;    // ~250 MB
		#else
			nMaxAddress = baseAddressToUse + 100000000;    // ~100 MB
		#endif
	}
	else
	{
		nMaxAddress  = mAddressToSymbolMap.rbegin()->first;
		nMaxAddress += 10000;       // Add the length of the symbol, whatever that is. This 10000 is a guess, as we don't know the actual size of this last symbol.
		nMaxAddress += (baseAddressToUse - mMapFileBaseAddress);
	}

	if((nAddress < baseAddressToUse) || (nAddress > nMaxAddress))
		return 0;

	// Convert nAddress from actual address to map file address. The map file has a "preferred load address"
	// that all absolute symbol addresses are based off of. Usually the application will in fact load
	// in memory at this preferred address. Under Windows, however, it's possible that the module can
	// be relocated and the nAddress value will be in the relocated place. mBaseAddress (baseAddressToUse) refers to the
	// base of the relocated area, and mMapFileBaseAddress refers to the map file "preferred load address".
	// Very often we will have (mBaseAddress == mMapFileBaseAddress).
	const uint64_t nMapFileAddress = (nAddress + (mMapFileBaseAddress - baseAddressToUse));

	if(addressRepTypeFlags & (1 << kARTFileLine))
	{
		if(!mAddressToFileLineMap.empty()) // If there is any file/line info (in fact there won't be for VS2005+ and XBox 360, since Microsoft killed this feature)...
		{
			AddressToFileLineMap::iterator it(mAddressToFileLineMap.upper_bound(nMapFileAddress));

			if(it != mAddressToFileLineMap.begin())
			{
				const AddressToFileLineMap::mapped_type& lineInfo = (*--it).second;

				pRepArray[kARTFileLine]      = lineInfo.first->c_str(); // This is the source file name with no path
				pIntValueArray[kARTFileLine] = lineInfo.second;
				returnFlags                 |= (1 << kARTFileLine);
			}
		}
	}

	if(addressRepTypeFlags & kARTFlagFunctionOffset)
	{
		Symbol symbol;
		const Symbol* pSymbol = NULL;

		if(mbLowMemoryUsage)
		{
			// In this case, we are iteratively reading the file instead of looking at our (otherwise possibly very large) mAddressToSymbolMap.
			EA_ASSERT(!mRVAToFilePosArray.empty()); // We always will have at least one entry.
			RVAToFilePos* pRVAPosEnd = &mRVAToFilePosArray.back();

			if((nMapFileAddress >= pRVAPosEnd->mRVA) && !mbSymbolEndFound) // If this address is beyond where we've yet read in the file and if there's anything else left to read in the file...
			{
				FillRVAToFilePosArray(nMapFileAddress);
				pRVAPosEnd = &mRVAToFilePosArray.back();
			}

			if(nMapFileAddress < pRVAPosEnd->mRVA) // If this address is somewhere within the map file...
			{
				if(FindRVAToFilePosArray(nMapFileAddress, symbol))
					pSymbol = &symbol;
			}
		}
		else
		{
			AddressToSymbolMap::const_iterator it(mAddressToSymbolMap.upper_bound(nMapFileAddress));

			if(it != mAddressToSymbolMap.begin())
			{
				const Symbol& s = (*--it).second;

				// We assign pSymbol from a scoped variable (s) and iterator (it) and then use pSymbol
				// outside of the scopes. This is safe because pSymbol is pointing to constant data
				// within mAddressToSymbolMap.
				pSymbol = &s;
			}
		}

		if(pSymbol)
		{
			if( IsDRMInternalLabel(pSymbol->mSymbol.c_str()))
			{
				// "So in case your callstack (can happen in some cases) shows any “Label_xxxx” just scroll up in disassembly and locate the first “LSIxxxxx__....” label that comes from the game. This will indicate where it crashed."
				return GetAddressRep(addressRepTypeFlags, nAddress - 1, pRepArray, pIntValueArray); // note the address -1 is "up" in the addresses of map file
			}
			else
			{
				for (GroupArray::const_iterator itGroup(mCodeGroupArray.begin()); itGroup != mCodeGroupArray.end(); ++itGroup)
				{
					const Group&   g = *itGroup;
					const intptr_t offset = (intptr_t)nMapFileAddress - (intptr_t)pSymbol->mRVA;

					if ((g.mSegment == pSymbol->mSegment) && ((pSymbol->mOffset - g.mOffset) < g.mLength) && (offset >= 0) && (offset < 65536))  // If it belongs to this group and the offset isn't pathologically huge (which can mistakenly happen at the very end of the symbols)...
					{
						pRepArray[kARTFunctionOffset] = pSymbol->mSymbol.c_str();
						pIntValueArray[kARTFunctionOffset] = (int)offset;  // This is the value reported in the map file, which will be offset from the value in the running app if the app was relocated by the OS at runtime..
						returnFlags |= kARTFlagFunctionOffset;
						break;
					}
				}
			}
		}
	}

	return returnFlags;
}



// This function is used when mbLowMemoryUsage == true.
// This function fills in mRVAToFilePosArray until nAddress is found.
// Thus it builds a cache of info (mRVAToFilePosArray) about where symbols are.
// It doesn't read every symbol line into a mRVAToFilePosArray entry,
// but rather reads and stores one every N of them in order to save memory.
// Since the symbols are in order by address this can be used to tell us
// roughly where a symbol can later be found in the file in the FindRVAToFilePosArray function.
bool MapFileMSVC::FillRVAToFilePosArray(uint64_t nAddress)
{
	bool bAddressFound = false;

	// Assert that mRVAToFilePosArray doesn't contain nAddress (otherwise we wouldn't be calling this function).
	EA_ASSERT(!mRVAToFilePosArray.empty() && (nAddress >= mRVAToFilePosArray.back().mRVA));

	// We assume that mFileBuffer is positioned at the line after the last location we read a mRVAToFilePosArray entry.
	while(!mbSymbolEndFound && !bAddressFound)
	{
		IO::off_type filePosPrev = mFileBuffer.GetPosition(EA::IO::kPositionTypeBegin);
		uint64_t     rva         = UINT64_C(0xffffffffffffffff);

		// We unilaterally read mSymbolsPerLowMemSample lines or until the end of the symbol lines.
		// Thus we will always append at least one new mRVAToFilePosArray entry.
		for(uint32_t i = 0; !mbSymbolEndFound && (i < mSymbolsPerLowMemSample); i++)
		{
			//  seg   offset  symbol                                    rva     flags source
			//  xxxxx:xxxxxxxxsssssssssssssssssssssssssssssssssssssssssxxxxxxxxx
			// " 0001:005a2bb4       ?InsertSeries@cGZBarGraph@@UAEXK@Z 014a9bb4 f   GZWin:GZGraph.obj"

			const size_t  kLineSize = 2600;
			char          line[kLineSize];
			unsigned      seg = 0;
			filePosPrev = mFileBuffer.GetPosition(EA::IO::kPositionTypeBegin);

			IO::size_type lineSize = IO::ReadLine(&mFileBuffer, line, kLineSize);

			if(lineSize < IO::kSizeTypeDone)  // If the line was read without error and wasn't at the end of the file...
			{
				const int readCount = EA::StdC::Sscanf(line, "%x:", &seg);

				if(readCount && seg)  // If the line appears to be good...
				{
					if(1 != EA::StdC::Sscanf(line, "%*x:%*x %*s %I64x", &rva))
					{
						// We could be here due to the line capacity not being large enough due to a huge symbol string.
						// So we currently leave rva as-is with the same value as the previous line.
						// It would be better if we could tell why it was the sscanf failed, then we could know whether
						// we should continue or fail.
					}
				}
			}

			// To consider: Add code here to check that seg is after our CODE segments and there is no more chance of CODE symbol lines.

			if(!seg)  // If the line was bad or was at the end of the file...
			{
				mbSymbolEndFound = true;
				rva = UINT64_C(0xffffffffffffffff); // Set it to this value so mAddressToFilePosArray below has a valid final entry.

				// We are done with mFileBuffer.
				mFileBuffer.SetStream(NULL);
				mFileBuffer.SetBufferSizes(0, 0);
				mFile.Close();
			}


		}

		RVAToFilePos rvaPos = { (uint64_t)rva, (uint64_t)filePosPrev };
		mRVAToFilePosArray.push_back(rvaPos);

		bAddressFound = (nAddress < rva);
	}

	return bAddressFound;
}


// This function is used when mbLowMemoryUsage == true.
// This function finds an appropriate starting position within the file and
// starts reading symbol lines from it till it gets to the one that corresponds
// to nAddress.
bool MapFileMSVC::FindRVAToFilePosArray(uint64_t nAddress, Symbol& s)
{
	bool bResult = false;
	RVAToFilePos rvaPos = { nAddress, 0 };

	// Find the first symbol line in the file which might correspond to nAddress.
	// We will start with this line and read successive lines until we find the
	// one that corresponds to our symbol. This function should not be being called
	// unless we have found that nAddress is represented with lines we have read
	// into mRVAToFilePosArray.
	EA_ASSERT(!mRVAToFilePosArray.empty());
	RVAToFilePosArray::iterator it = eastl::upper_bound(mRVAToFilePosArray.begin(), mRVAToFilePosArray.end(), rvaPos, RVAToFilePosCompare());

	if(it != mRVAToFilePosArray.begin())
		--it;

	rvaPos = *it; // Start at this line.
	mFileBufferLowMemoryUsage.SetPosition((IO::off_type)rvaPos.mFilePos, IO::kPositionTypeBegin);

	// Read lines until we get to the first line that is beyond nAddress.
	const size_t  kLineSize = 2600;
	char          line[kLineSize];
	IO::size_type lineSize;
	uint64_t      filePosPrev    = rvaPos.mFilePos;
	uint64_t      filePosCurrent = filePosPrev;

	while((lineSize = IO::ReadLine(&mFileBufferLowMemoryUsage, line, kLineSize)) < IO::kSizeTypeDone) // While there are more lines...
	{
		//  seg   offset  symbol                                    rva     flags source
		//  xxxxx:xxxxxxxxsssssssssssssssssssssssssssssssssssssssssxxxxxxxxx
		// " 0001:005a2bb4       ?InsertSeries@cGZBarGraph@@UAEXK@Z 014a9bb4 f   GZWin:GZGraph.obj"

		if((lineSize == 0) || !EA::StdC::Isxdigit(line[1])) // I don't think this should ever occur, unless we bump up against the end of the file.
		{
			// We don't set mbSymbolEndFound here, because that's the job of FillRVAToFilePosArray.
			// break; Should we break here? It's not clear that doing so is a major win.
		}
		else
		{
			unsigned seg;
			unsigned offset;
			char     symbolMangled[2048+1]; // If you change this 2048, change the sscanf limit below.
			uint64_t rva;

			if(1 == EA::StdC::Sscanf(line, "%*x:%*x %*s %I64x", &rva))
			{
				if(rva > nAddress)  // If we have passed the line that refers to the symbol, then the previous line is the symbol line.
				{
					// We go back to the previous line and read its symbol info, as that is the line we want.
					mFileBufferLowMemoryUsage.SetPosition((IO::off_type)filePosPrev, IO::kPositionTypeBegin);
					IO::ReadLine(&mFileBufferLowMemoryUsage, line, kLineSize);

					if(4 == EA::StdC::Sscanf(line, "%x:%x %2048s %I64x", &seg, &offset, symbolMangled, &rva))
					{
						// We unmangle the symbolMangled string into the line buffer. We re-use the line buffer because we don't need it any more.
						UnmangleSymbol(symbolMangled, line, kLineSize, kCompilerTypeMSVC);

						s.mSymbol.get_allocator().set_allocator(mpCoreAllocator);
						s.mSegment   = seg;
						s.mOffset    = offset;
						s.mSymbol    = line;
						s.mRVA       = (uint64_t)rva;

						bResult = true;
					}

					break;
				}

				filePosPrev    = filePosCurrent; // Save this in case we'll need it on the next loop.
				filePosCurrent = (uint64_t)mFileBufferLowMemoryUsage.GetPosition(EA::IO::kPositionTypeBegin);
			}
		}
	}

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// Load
//
bool MapFileMSVC::Load()
{
	bool bSuccess = true;

	if(!mbLoadAttempted)
	{
		mbLoadAttempted = true;

		mFile.SetPath(msMapFilePath.c_str());

		if(mFile.Open())
		{
			mFileBuffer.SetBufferSizes(mFileBufferSize, IO::StreamBuffer::kBufferSizeUnspecified);
			mFileBuffer.SetStream(&mFile);

			mFileSize = mFile.GetSize();

			if(mbLowMemoryUsage)
			{
				mSymbolsPerLowMemSample = (uint32_t)(mFileSize / 40000); // This number is qualitative. Used only for the case of mbLowMemoryUsage.
				if(mSymbolsPerLowMemSample < 1)
					mSymbolsPerLowMemSample = 1;
				mRVAToFilePosArray.reserve(256);
			}

			// We put a buffer in front of the file stream in order to
			// speed up our reads, which will be entirely sequential.
			CAString8*      pCurrentFilename = NULL;
			const size_t    kLineSize = 2600;
			// declare this variable as static to avoid injection of __chkstk when stack size exceeds page limit which causes stackoverflow crash in ExceptionHandler code
			// that uses custom callstack.
			// NOTE. this code relies on one instance of lookup table initialized.
			static char     line[kLineSize];
			IO::size_type   lineSize;
			IO::off_type    filePosition = 0;         // Denotes the file position for the beginning of the current line.

			for(bool bContinueReading = true;
				bContinueReading && (lineSize = IO::ReadLine(&mFileBuffer, line, kLineSize)) < IO::kSizeTypeDone; // While we should continue reading and while there are more lines...
				filePosition = mFileBuffer.GetPosition(EA::IO::kPositionTypeBegin))
			{
				if(lineSize == 0) // If the line is empty, skip it.
					continue;

				eastl::fixed_substring<FixedString::value_type> linebuf(line, line + lineSize);
				eastl::fixed_substring<FixedString::value_type>::size_type firstNoWhitespace = linebuf.find_first_not_of(" ");

				if (firstNoWhitespace != FixedString::npos)
					linebuf.assign(linebuf, firstNoWhitespace, linebuf.length() - firstNoWhitespace);

				// " Start   Length   Name   Class"
				if(linebuf.compare(0, 5, "Start") == 0)
					mPhase = kPhaseCapturingGroups;

				// " Preferred load address is 10000000"
				if(linebuf.compare(0, 9, "Preferred") == 0)
				{
					uint64_t address;
					if(1 == EA::StdC::Sscanf(&linebuf[9], " load address is %I64x", &address))
					{
						mMapFileBaseAddress = address;
						continue;
					}
				}

				// "  Address   Publics by Value   Rva+Base   Lib:Object"
				if(linebuf.compare(0, 7, "Address") == 0)
				{
					mPhase = kPhaseCapturingSymbols;
					continue;
				}

				// " Line numbers for C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\atlmfc\lib\atls.lib(f:\vs70builds\3077\vc\mfcatl\ship\atlmfc\include\atlsimpcoll.h) segment .text"
				if(linebuf.compare(0, 12, "Line numbers") == 0)
				{
					mPhase = kPhaseIdle;

					if(!mbLowMemoryUsage) // We don't read line numbers in low memory usage mode.
					{
						int pos1, pos2;
						char c;

						if((1 == EA::StdC::Sscanf(&linebuf[12], " for %*[^(](%n%*[^)]%n%c\n", &pos1, &pos2, &c)) && (c == ')'))
						{
							eastl::fixed_substring<FixedString::value_type> filename(&linebuf[12 + (unsigned)pos1], (unsigned)(pos2 - pos1));
							eastl::fixed_substring<FixedString::value_type>::size_type last = filename.find_last_of("/\\:");

							if (last != FixedString::npos)
								filename.assign(filename, last + 1, filename.length() - (last + 1));

							mPhase = kPhaseCapturingLineNumbers;

							mLineFilenameList.push_back();
							pCurrentFilename = &mLineFilenameList.back(); // We'll use this below.

							#if EASTL_NAME_ENABLED
								pCurrentFilename->get_allocator().set_name(EACALLSTACK_ALLOC_PREFIX "MapFileMSVC");
							#endif

							pCurrentFilename->assign(filename.c_str(), filename.length());
						}
					}

					continue;
				}

				// " Exports"
				if(linebuf.compare(0, 7, "Exports") == 0)
				{
					mPhase = kPhaseIdle;
					continue;
				}

				if(!linebuf.empty())
				{
					// From here on we process lines that contain data. After each of the above header lines
					// we will various lines of data under each header. This will be very obvious if you look
					// at a .map file.
					switch(mPhase)
					{
						case kPhaseCapturingGroups:
						{
							// " 0001:00000000 000fae9eH .text                   CODE"
							char  className[32 + 1] = {}; // This will be something like "CODE" or "DATA". // If you change this 32, change the %31s sscanf limit below.
							Group g;

							if(4 == EA::StdC::Sscanf(linebuf.c_str(), "%x:%x %xH %*s %32s", (unsigned*)&g.mSegment, (unsigned*)&g.mOffset, (unsigned*)&g.mLength, className))
							{
								if(strcmp(className, "CODE") == 0) // If it is CODE...
								{
									mCodeGroupArray.push_back(g);

									if(g.mSegment >= mSectionArray.size())
										mSectionArray.resize(g.mSegment + 1, 0);
								}
							}

							break;
						}

						case kPhaseCapturingSymbols:
						{
							//  seg   offset  symbol                                    rva     flags source
							//  xxxxx:xxxxxxxxsssssssssssssssssssssssssssssssssssssssssxxxxxxxxx
							// " 0001:005a2bb4       ?InsertSeries@cGZBarGraph@@UAEXK@Z 014a9bb4 f   GZWin:GZGraph.obj"

							unsigned seg;
							unsigned offset;
							char     symbolMangled[2048+1]; // If you change this 2048, change the sscanf limit below.
							uint64_t rva;

							if(4 == EA::StdC::Sscanf(linebuf.c_str(), "%x:%x %2048s %I64x", &seg, &offset, symbolMangled, &rva))
							{
								// We only need to pay attention to CODE symbols, we check to see that the segment
								// this symbol belongs to is one in our mCodeGroupArray (which contains only the CODE segments).
								bool bSegmentIsCode = false;

								for(eastl_size_t i = 0, iEnd = mCodeGroupArray.size(); (i != iEnd) && !bSegmentIsCode; ++i)
								{
									if(seg == mCodeGroupArray[i].mSegment)
										bSegmentIsCode = true;
								}

								if(bSegmentIsCode) // Usually this is true the first time through, since VC++ .map files usually (always?) implement code symbols before data symbols.
								{
									if(mbLowMemoryUsage)
									{
										// In low memory mode, we stop loading right here, after the first symbol.
										// We will start reading the symbol lines as needed when the user calls GetAddressRep.
										EA_ASSERT(mRVAToFilePosArray.empty());

										RVAToFilePos entry = { (uint64_t)rva, (uint64_t)filePosition };
										mRVAToFilePosArray.push_back(entry);

										// We need to open the same file a second time. The reason for this is that we use mFile to
										// read symbol lines from filePosition position till the end of the file. This reading is
										// done periodically as we need to extend our sampling of symbol lines. We have this other
										// file so we can go back and read lines dynamically between our sampling positions.
										// We don't want to use the same file object for both, as we would get file cache thrashing
										// if we did so.
										mFileLowMemoryUsage.SetPath(msMapFilePath.c_str());
										mFileLowMemoryUsage.Open();
										mFileBufferLowMemoryUsage.SetBufferSizes(mFileBufferSize, IO::StreamBuffer::kBufferSizeUnspecified);
										mFileBufferLowMemoryUsage.SetStream(&mFileLowMemoryUsage);

										bContinueReading = false;
									}
									else
									{
										// We unmangle the symbolMangled string into the line buffer. We re-use the line buffer because we don't need it any more.
										UnmangleSymbol(symbolMangled, line, kLineSize, kCompilerTypeMSVC);

										Symbol s;
										s.mSymbol.get_allocator().set_allocator(mpCoreAllocator);

										s.mSegment   = seg;
										s.mOffset    = offset;
										s.mSymbol    = line;
										s.mRVA       = (uint64_t)rva;
									  //s.mSourceObj = source;

										const AddressToSymbolMap::value_type value((uint64_t)rva, s);
										mAddressToSymbolMap.insert(value);

										if(seg < mSectionArray.size())
											mSectionArray[seg] = (uint32_t)(rva - offset);
									}
								}
							}

							break;
						}

						case kPhaseCapturingLineNumbers: // Map files from VS2005 and later and XBox 360 never have line numbers.
						{
							unsigned lineNumber[4], section[4], offset[4];

							int count = EA::StdC::Sscanf(linebuf.c_str(), "%u %x:%x %u %x:%x %u %x:%x %u %x:%x"
																		, &lineNumber[0], &section[0], &offset[0]
																		, &lineNumber[1], &section[1], &offset[1]
																		, &lineNumber[2], &section[2], &offset[2]
																		, &lineNumber[3], &section[3], &offset[3]);

							count /= 3;

							for(int i = 0; i < count; ++i)
							{
								if(section[i] < mSectionArray.size())
									mAddressToFileLineMap[offset[i] + mSectionArray[section[i]]] =
											AddressToFileLineMap::mapped_type(pCurrentFilename, (int)lineNumber[i]);
							}

							break;
						}

						case kPhaseCount:
						case kPhaseIdle:
						default:
							break;
					} // switch
				} // if(!linebuf.empty())
			} // while ...

			if(mbLowMemoryUsage)
				bSuccess = !mRVAToFilePosArray.empty();
			else
				bSuccess = !mAddressToSymbolMap.empty();
		} // if mFile.Open()
	} // if(!mbLoadAttempted)

	if(!mbLowMemoryUsage)
	{
		// In the case of low memory usage, we leave the file open for iterative reading.
		// Otherwise we clear the stream buffer and file stream, as we won't need them any more.
		mFileBuffer.SetStream(NULL);
		mFileBuffer.SetBufferSizes(0, 0);
		mFile.Close();

		// mFileLowMemoryUsage shouldn't be open.
		EA_ASSERT(!mFileLowMemoryUsage.GetAccessFlags());
	}

	if(bSuccess)
	{
		// If the base address hasn't been set and we are reading our own map file (!mbOfflineLookup) as we run this app,
		// we set the map file base address based on the current module base address. In this case there's no reason for
		// the user to call SetBaseAddress, and even if the user did then the only practical base address the user could
		// specify would be the same one we auto-detect below. The user can still call SetBaseAddress to set it a different value.
		if((mBaseAddress == kBaseAddressUnspecified) && !mbOfflineLookup)
		{
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, msMapFilePath.c_str(), IO::kMaxPathLength);

			mBaseAddress = Callstack::GetModuleBaseAddress(pPath8);
			if(mBaseAddress == kBaseAddressInvalid)
				mBaseAddress = kBaseAddressUnspecified;
		}
	}

	return bSuccess;
}

#endif // EACALLSTACK_MSVC_MAPFILE_ENABLED



#if EACALLSTACK_GCC_MAPFILE_ENABLED

///////////////////////////////////////////////////////////////////////////////
// MapFileGCC3
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// A GCC3 (and later) .map file looks like the text below.
// We want to read the .text and .gnu.linkonce entries.
//
///////////////////////////////////////////////////////////////////////////////
//  .text          0x00000000000216d0      0x658 C:/Development/Platform/PS3/cell/082_012/target/ppu/lib/noeh/libc.a(xlv2tls.o)
//                 0x00000000000216d0                .TlsGetValue
//                 0x0000000000021bc0                ._Tlsfree
//                 0x0000000000021940                ._Destroytls
//                 0x000000000002178c                .TlsSetValue
//                 0x0000000000021a34                ._Tlsalloc
//                 0x0000000000021cd0                ._Tlsget
//                 0x0000000000021c68                ._Tlsset
//  .text._sys_ppu_thread_create
//                 0x00000000000d8c30       0x2c C:/Development/Platform/PS3/cell/082_012/target/ppu/lib/liblv2.a(syscall_stub_ppu_thread.o)
//                 0x00000000000d8c30                ._sys_ppu_thread_create
//  .text._ZN5eastl4sortIPN2EA4Jobs10JobMetricsEPFbRKS3_S6_EEEvT_S9_T0_
//                 0x00000000000147a0       0x2b /home/mburke/p4/jm/packages/job_test/dev/build/job_test/dev/unix64-clang-dev-debug/build/jobtest/source/tests/test_job_thread_wake_up.cpp.obj
//                 0x00000000000147a0                void eastl::sort<EA::Jobs::JobMetrics*, bool (*)(EA::Jobs::JobMetrics const&, EA::Jobs::JobMetrics const&)>(EA::Jobs::JobMetrics*, EA::Jobs::JobMetrics*, bool (*)(EA::Jobs::JobMetrics const&, EA::Jobs::JobMetrics const&))
//  *fill*         0x00000000000147cb        0x5 90909090
//  .gnu.linkonce.t._ZN5eastl9allocator8set_nameEPKc
//                 0x0000000000027b18       0x3c c:/Projects/Helios/Ctg_ears/Dl/lib/PS3DEB~1/UTFCOM~1/UTFCOM~1.lib(EAAddressRep.obj)
//                 0x0000000000027b18                .eastl::allocator::set_name(char const*)
//  .gnu.linkonce.t._ZN5eastleqERKNS_9allocatorES2_
//                 0x0000000000027b54       0x34 c:/Projects/Helios/Ctg_ears/Dl/lib/PS3DEB~1/UTFCOM~1/UTFCOM~1.lib(EAAddressRep.obj)
//                 0x0000000000027b54                .eastl::operator==(eastl::allocator const&, eastl::allocator const&)
//  .gnu.linkonce.d._ZTVN2EA4Dasm19DisassemblerPowerPCE
//                 0x000000000010a8a8       0x18 c:/Projects/Helios/Ctg_ears/Dl/lib/PS3DEB~1/UTFCOM~1/UTFCOM~1.lib(EADasm.obj)
//                 0x000000000010a8a8                vtable for EA::Dasm::DisassemblerPowerPC
//  .gnu.linkonce.d._ZTVN2EA4Dasm15DisassemblerX86E
//                 0x000000000010a8c0       0x18 c:/Projects/Helios/Ctg_ears/Dl/lib/PS3DEB~1/UTFCOM~1/UTFCOM~1.lib(EADasm.obj)
//                 0x000000000010a8c0                vtable for EA::Dasm::DisassemblerX86
//  .opd           0x0000000000112508        0x8 C:/Development/Platform/PS3/cell/082_012/target/ppu/lib/noeh/libm.a(xdint.o)
//                 0x0000000000112508                _Dint
//  .init          0x00000000000100b0        0xc c:/development/platform/ps3/cell/082_012/host-win32/ppu/bin/../lib/gcc/ppu-lv2/4.0.2/ecrti.o
//                 0x00000000000100b0                .__init
//  .bss           0x0000000000115c58        0x8 C:/Development/Platform/PS3/cell/082_012/target/ppu/lib/noeh/libc.a(xvalues.o)
//                 0x0000000000115c58                _Zero
//  .toc           0x00000000001145c8        0x8 c:/development/platform/ps3/cell/082_012/host-win32/ppu/bin/../lib/gcc/ppu-lv2/4.0.2\libsupc++.a(eh_catch.o)
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// MapFileGCC3
//
MapFileGCC3::MapFileGCC3(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mbLoadAttempted(false),
	mbLowMemoryUsage(false),
	mbOfflineLookup(false),
	msMapFilePath(),
	mEntryPairArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileGCC3", mpCoreAllocator)),
	mBaseAddress(kBaseAddressUnspecified),
	mFileBufferSize(4096)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~MapFileGCC3
//
MapFileGCC3::~MapFileGCC3()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void MapFileGCC3::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;

	mEntryPairArray.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool MapFileGCC3::Init(const wchar_t* pMapFilePath, bool bDelayLoad)
{
	if(pMapFilePath)
		msMapFilePath = pMapFilePath;

	// We delay the reading of mBaseAddress until Load.

	if(!bDelayLoad)
		Load();

	return (bDelayLoad || !mEntryPairArray.empty());
}


///////////////////////////////////////////////////////////////////////////////
// Load
//
bool MapFileGCC3::Load()
{
	if(!mbLoadAttempted)
	{
		mbLoadAttempted = true;

		IO::FileStream fs(msMapFilePath.c_str());

		if(fs.Open())
		{
			// We put a buffer in front of the file stream in order to
			// speed up our reads, which will be entirely sequential.
			fs.AddRef();
			IO::StreamBuffer streamBuffer(mFileBufferSize, 0, &fs);

			// We'll almost certainly need a few hundred entries, so reserve some up-front.
			mEntryPairArray.reserve(8192);

			// Read the map file data here.
			const size_t      kLineSize = 512;
			char              line[kLineSize];
			bool              bReadLines = false;
			int               nLineCount = 0;       // Assists in debugging.

			while(IO::ReadLine(&streamBuffer, line, kLineSize) < IO::kSizeTypeDone) // While there are more lines...
			{
				nLineCount++;

				if((line[0] == ' ') && (line[1] == '.')) // If this line represents the beginning of a new section...
				{
					// We check for a line like this:
					//     " .text   0x00000000000caa08     0x421c c:/Projects/Blah.lib(Blah.obj)"
					// In which case it will usually be followed with lines like this:
					//                 0x00000000000216d0                .TlsGetValue
					//                 0x0000000000021bc0                .SomeFunctionName
					//                 0x0000000000021940                .SomeOtherFunctionName
					if((EA::StdC::Strstr(line, " .text ")        == line) || // Would be better if we had a 'begins_with() function'.
					   (EA::StdC::Strstr(line, " .gnu.linkonce") == line))
					{
						if(line[2] == 'g') // If it is a .gnu.linkonce... line, then skip the next line. See the above example data to see why.
						{
							IO::ReadLine(&streamBuffer, line, kLineSize);
							nLineCount++;
						}
						bReadLines = true;
					}
					// GCC 4.x can have line triplets like this:
					//     " .text._ZN2EA9Callstack13InitCallstackEv"
					//     "                0x0001efa0        0x8 C:/Projects/EAOS/UTF/build/EACallstack/dev/android-gcc-dev-debug/lib/libEACallstack.a(BB_runtime.EACallstack.cpp.1bTSKDR4gEYy7C1ztALMjA.obj)"
					//     "                0x0001efa0                EA::Callstack::InitCallstack()"
					else if(EA::StdC::Strstr(line, " .text._Z") == line)
					{
						// Take into account the possibility that there aren't two address entry lines (e.g. like above) but
						// there are one or even zero. I think this could happen with minimal debug information being present.
						IO::size_type posSaved = (IO::size_type)streamBuffer.GetPosition(); // Saved position of the beginning of the line after .text._Z.
						IO::size_type lineSize = IO::ReadLine(&streamBuffer, line, kLineSize);
						// nLineCount++; Don't do this because we are just going to rewind it below.

						if(lineSize >= 37) // We need enough for at least "                0x0001efa0        0x8 "
						{
							if(EA::StdC::Strstr(line, "        ") == line) // If it begins with a bunch of spaces... it is presumably an address line.
							{
								if(StdC::Isxdigit(line[36])) // If this is the file name line (it has a number in the right place)... move to the next line.
								{
									posSaved = (IO::size_type)streamBuffer.GetPosition();
									lineSize = IO::ReadLine(&streamBuffer, line, kLineSize); // Move to next line, which will be the second address line or will be a new symbol entry altogether.
									nLineCount++; // Do this because the rewind below will rewind only one line, not two. See above related comment.
								}

								if(EA::StdC::Strstr(line, "        ") == line) // If this is an address line... (it presumably is the function name address line).
									bReadLines = true;
							}
						}

						streamBuffer.SetPosition((IO::off_type)posSaved); // posSaved is either at the beginning of the line after .text._Z or two lines after .text._Z
					}
					else
					{
						//  We may have a "*fill*    ..." line as in the example way above. Or a new section like ".bss    ..."
						bReadLines = false;
					}
				}
				else if(bReadLines)
				{
					// We have a line that is like this (with the address being 32 or 64 bits depending on the platform):
					//     "        0x00000000000d8c84       .EA::CommandLine::CommandLine(int, wchar_t**, EA::Allocator::ICoreAllocator*)"
					char*          pAddressEnd;
					const uint64_t address = EA::StdC::StrtoU64(line, &pAddressEnd, 16);
					char           symbol[256];
					size_t         symbolLength;

					while(*pAddressEnd == ' ') // Skip past whitespace.
						++pAddressEnd;

					symbolLength = EA::StdC::Strlcpy(symbol, pAddressEnd, sizeof(symbol));
					if(symbolLength >= sizeof(symbol))
						symbolLength = sizeof(symbol) - 1;

					// To do: We probably want to sanitize/simplify the symbol string.

					if(address && symbolLength) // If we have valid data...
					{
						// We leave the symbol mangled here; we'll unmangle it in GetAddressRep.
						EntryPair entryPair;
						entryPair.mAddress = address;
						entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);
						entryPair.mSymbol = symbol;

						EntryPairArray::iterator it =
							eastl::lower_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

						mEntryPairArray.insert(it, std::move(entryPair)); // Much of the time, this will be an append, because the entries are largely sorted already.
					}
				}
			}
		}
	}

	if(!mEntryPairArray.empty())
	{
		if((mBaseAddress == kBaseAddressUnspecified) && !mbOfflineLookup) // If the base address hasn't been set and we are reading our own mapfile at runtime...
		{
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, msMapFilePath.c_str(), IO::kMaxPathLength);

			// The bookmark function is called to prevent it from being deadstripped.
			EACallstackBookmarkFunction();

			uint64_t bookmarkAddress = (uint64_t)(void *)&EACallstackBookmarkFunction;

			for (EntryPairArray::const_iterator it = mEntryPairArray.begin(), itEnd = mEntryPairArray.end();
					it != itEnd; ++it)
			{
				// The linker-derived address of the bookmark function is fetched from the map file.
				// This address is then turned into an offset based on the actual address of the
				// bookmark function after the loader has potentially relocated it which then allows
				// the callstack lookup code to resolve proper addresses.
				if (EA::StdC::Strstr((it->mSymbol).c_str(), gpBookmarkFunctionSymbol) != 0)
				{
					mBaseAddress = bookmarkAddress - it->mAddress;
					break;
				}
			}
		}
	}

	return !mEntryPairArray.empty();
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool MapFileGCC3::Shutdown()
{
	// Clear our member data.
	mbLoadAttempted = false;
	mEntryPairArray.set_capacity(0);
	mBaseAddress = kBaseAddressUnspecified;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* MapFileGCC3::AsInterface(int id)
{
	if(id == MapFileGCC3::kTypeId)
		return static_cast<MapFileGCC3*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void MapFileGCC3::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t MapFileGCC3::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* MapFileGCC3::GetDatabasePath() const
{
	return msMapFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void MapFileGCC3::SetOption(int option, int value)
{
	if(option == kOptionLowMemoryUsage)
	{
		EA_ASSERT(!mbLoadAttempted); // Must set this option before loading anything or doing any lookups.
		mbLowMemoryUsage = (value != 0);
	}
	else if(option == kOptionFileBufferSize)
		mFileBufferSize = (uint32_t)value;
	// else if(option == kOptionCopyFileToMemory) // Not currently supported.
	//    mbUseMemoryFile = (value != 0);
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int MapFileGCC3::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(!mbLoadAttempted)
		Load();

	uint64_t baseAddressToUse = ((mBaseAddress == kBaseAddressUnspecified) ? 0 : mBaseAddress);

	// We need to make the following check here, as the code below doesn't account for it.
	// We don't need to make an upper-bound check, as the code below does account for it.
	if(nAddress < baseAddressToUse)
		return 0;

	// GCC .map files can give you only function/offset information.
	// You need to read the DWARF information from a GCC-generated .elf file to get file/line information.
	if(addressRepTypeFlags & kARTFlagFunctionOffset)
	{
		uint64_t  nAddressRelative = (nAddress - baseAddressToUse);
		EntryPair entryPair;
		entryPair.mAddress = nAddressRelative;
		entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);

		EntryPairArray::iterator it =
			eastl::upper_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

		if(it != mEntryPairArray.begin())
		{
			entryPair = *--it;

			const int addressOffset = (int)(unsigned)(nAddressRelative - entryPair.mAddress);

			if(abs(addressOffset) < 65536) // Do a sanity check.
			{
				char buffer[512]; // The symbol may in fact already be demangled, so UnmangleSymbol may just do a strcpy.
				UnmangleSymbol(entryPair.mSymbol.c_str(), buffer, sizeof(buffer), kCompilerTypeGCC);

				pRepArray[kARTFunctionOffset]      = buffer;
				pIntValueArray[kARTFunctionOffset] = addressOffset;
				returnFlags                       |= kARTFlagFunctionOffset;
			}
		}
	}

	return returnFlags;
}


#endif // EACALLSTACK_GCC_MAPFILE_ENABLED


#if EACALLSTACK_APPLE_MAPFILE_ENABLED


///////////////////////////////////////////////////////////////////////////////
// MapFileApple
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Apple GCC .map files looks like the text below.
// We want to read the "# Symbols:" entries that are in __TEXT segments.
//
///////////////////////////////////////////////////////////////////////////////
//
// # Path: /Users/PPedriana/Projects/EAOS/UTF/build/EACallstack/dev/CMakeOSXBuild/Debug/EACallstackTest
// # Arch: i386
// # Object files:
// [  0] linker synthesized
// [  1] /Developer/SDKs/MacOSX10.7.sdk/usr/lib/crt1.10.6.o
// [  2] /Users/PPedriana/Projects/EAOS/UTF/build/EACallstack/dev/CMakeOSXBuild/EACallstackTestProject.build/Debug/EACallstackTest.build/Objects-normal/i386/EACallstackTest.o
// [  3] /Users/PPedriana/Projects/EAOS/UTF/build/EACallstack/dev/CMakeOSXBuild/EACallstackTestProject.build/Debug/EACallstackTest.build/Objects-normal/i386/TestCallstackRecorder.o
// . . .
// [ 61] /Users/PPedriana/Projects/EAOS/UTF/build/EACallstack/dev/CMakeOSXBuild/EACallstack/Debug/libEACallstack.a(EACallstackChoose.o)
// [ 62] /Developer/SDKs/MacOSX10.7.sdk/usr/lib/i686-apple-darwin11/4.2.1/libstdc++.dylib
// [ 63] /Developer/SDKs/MacOSX10.7.sdk/usr/lib/libSystem.dylib
// # Sections:
// # Address    Size        Segment    Section
// 0x00002470    0x000DFED8    __TEXT    __text
// 0x000E2350    0x00000210    __TEXT    __const_coal
// 0x000E2560    0x0000326A    __TEXT    __symbol_stub
// 0x000E57CC    0x0000052C    __TEXT    __stub_helper
// 0x000E5CF8    0x0000769C    __TEXT    __gcc_except_tab
// 0x000ED3A0    0x00010B37    __TEXT    __cstring
// 0x000FDEE0    0x00000AFC    __TEXT    __const
// 0x000FE9DC    0x00000142    __TEXT    __ustring
// 0x000FEB1E    0x00002724    __TEXT    __unwind_info
// 0x00101248    0x00018DB4    __TEXT    __eh_frame
// 0x0011A000    0x00000014    __DATA    __program_vars
// 0x0011A014    0x000000FC    __DATA    __nl_symbol_ptr
// 0x0011A110    0x0000219C    __DATA    __la_symbol_ptr
// 0x0011C2AC    0x00000030    __DATA    __mod_init_func
// 0x0011C2E0    0x00000C78    __DATA    __data
// 0x0011CF60    0x00001020    __DATA    __const
// 0x0011DF80    0x00000C50    __DATA    __bss
// 0x0011EBE0    0x000017D0    __DATA    __common
// # Symbols:
// # Address    Size        File  Name
// 0x00002470    0x0000003E    [  1] start
// 0x000024B0    0x00000170    [  2] __ZN2EA9Allocator19InitSharedAllocatorEv
// 0x00002620    0x00000010    [  2] __ZN2EA9Allocator23ShutdownSharedAllocatorEv
// 0x00002630    0x00000060    [  2] __ZdlPvPKcijS1_i
// 0x00002690    0x00000060    [  2] __ZdaPvPKcijS1_i
// 0x000026F0    0x00000080    [  2] __ZdlPvmmPKcijS1_i
// 0x00002770    0x00000080    [  2] __ZdaPvmmPKcijS1_i
// 0x000027F0    0x00000032    [  2] __ZL3Absl
//  . . .
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// MapFileApple
//
MapFileApple::MapFileApple(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mbLoadAttempted(false),
	mbLowMemoryUsage(false),
	mbOfflineLookup(false),
	msMapFilePath(),
	mEntryPairArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileApple", mpCoreAllocator)),
	mBaseAddress(kBaseAddressUnspecified),
	mFileBufferSize(4096)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~MapFileApple
//
MapFileApple::~MapFileApple()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void MapFileApple::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;

	mEntryPairArray.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool MapFileApple::Init(const wchar_t* pMapFilePath, bool bDelayLoad)
{
	if(pMapFilePath)
		msMapFilePath = pMapFilePath;

	// We delay the reading of mBaseAddress until Load.

	if(!bDelayLoad)
		Load();

	return (bDelayLoad || !mEntryPairArray.empty());
}


///////////////////////////////////////////////////////////////////////////////
// Load
//

bool MapFileApple::Load()
{
	if(!mbLoadAttempted)
	{
		mbLoadAttempted = true;

		IO::FileStream fs(msMapFilePath.c_str());

		if(fs.Open())
		{
			// We put a buffer in front of the file stream in order to
			// speed up our reads, which will be entirely sequential.
			fs.AddRef();
			IO::StreamBuffer streamBuffer(mFileBufferSize, 0, &fs);

			// We'll almost certainly need a few hundred entries, so reserve some up-front.
			mEntryPairArray.reserve(8192);

			// Read the map file data here.
			const size_t kLineSize = 512;
			char         line[kLineSize];
			bool         bReadLines = false;
			int          nLineCount = 0;       // Assists in debugging.

			while(IO::ReadLine(&streamBuffer, line, kLineSize) < IO::kSizeTypeDone) // While there are more lines...
			{
				nLineCount++;

				if(line[0] == '#') // If this line represents the beginning of a new section...
				{
					// We check for a line like this:
					//     "# Symbols:"
					if((EA::StdC::Strstr(line, "# Symbols:") == line) )
					{
						// Skip the next lineas it's the table header. See the above example data to see why.
						IO::ReadLine(&streamBuffer, line, kLineSize);
						bReadLines = true;
					}
					else
						bReadLines = false;
				}
				else if(bReadLines)
				{
					// We have a line that is like this (with the address being 32 or 64 bits depending on the platform):
					//     "0x00002780  0x0000003E  [ 1]  start"

					char*          pAddressEnd;
					const uint64_t address = EA::StdC::StrtoU64(line, &pAddressEnd, 16);
				  //const uint64_t lSize = EA::StdC::StrtoU64(pAddressEnd, &pAddressEnd, 16);
					char           symbol[256];
					size_t         symbolLength;

					while(*pAddressEnd != ']') // Skip past the end of the file number.
						++pAddressEnd;

					++pAddressEnd;

					while(*pAddressEnd == ' ') // Skip past whitespace.
						++pAddressEnd;

					symbolLength = EA::StdC::Strlcpy(symbol, pAddressEnd, sizeof(symbol));
					if(symbolLength >= sizeof(symbol))
						symbolLength = sizeof(symbol) - 1;

					// To do: We probably want to sanitize/simplify the symbol string.

					if(address && symbolLength) // If we have valid data...
					{
						// We leave the symbol mangled here; we'll unmangle it in GetAddressRep.
						EntryPair entryPair;
						entryPair.mAddress = address;
						entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);
						entryPair.mSymbol = symbol;

						EntryPairArray::iterator it =
							eastl::lower_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

						mEntryPairArray.insert(it, std::move(entryPair)); // Much of the time, this will be an append, because the entries are largely sorted already.
					}
				}
			}
		}
	}

	if(!mEntryPairArray.empty())
	{
		if((mBaseAddress == kBaseAddressUnspecified) && !mbOfflineLookup) // If the base address hasn't been set...
		{
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, msMapFilePath.c_str(), IO::kMaxPathLength);

			// The bookmark function is called to prevent it from being deadstripped.
			EACallstackBookmarkFunction();

			uint64_t bookmarkAddress = (uint64_t)(void *)&EACallstackBookmarkFunction;

			for (EntryPairArray::const_iterator it = mEntryPairArray.begin(), itEnd = mEntryPairArray.end();
					it != itEnd; ++it)
			{
				// The linker-derived address of the bookmark function is fetched from the map file.
				// This address is then turned into an offset based on the actual address of the
				// bookmark function after the loader has potentially relocated it which then allows
				// the callstack lookup code to resolve proper addresses.
				if (EA::StdC::Strstr((it->mSymbol).c_str(), gpBookmarkFunctionSymbol) != 0)
				{
					mBaseAddress = bookmarkAddress - it->mAddress;
					break;
				}
			}
		}
	}

	return !mEntryPairArray.empty();
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool MapFileApple::Shutdown()
{
	// Clear our member data.
	mbLoadAttempted = false;
	mEntryPairArray.set_capacity(0);
	mBaseAddress = kBaseAddressUnspecified;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* MapFileApple::AsInterface(int id)
{
	if(id == MapFileApple::kTypeId)
		return static_cast<MapFileApple*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void MapFileApple::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t MapFileApple::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* MapFileApple::GetDatabasePath() const
{
	return msMapFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void MapFileApple::SetOption(int option, int value)
{
	if(option == kOptionLowMemoryUsage)
	{
		EA_ASSERT(!mbLoadAttempted); // Must set this option before loading anything or doing any lookups.
		mbLowMemoryUsage = (value != 0);
	}
	else if(option == kOptionFileBufferSize)
		mFileBufferSize = (uint32_t)value;
	// else if(option == kOptionCopyFileToMemory) // Not currently supported.
	//    mbUseMemoryFile = (value != 0);
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int MapFileApple::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(!mbLoadAttempted)
		Load();

	uint64_t baseAddressToUse = ((mBaseAddress == kBaseAddressUnspecified) ? 0 : mBaseAddress);

	// We need to make the following check here, as the code below doesn't account for it.
	// We don't need to make an upper-bound check, as the code below does account for it.
	if(nAddress < baseAddressToUse)
		return 0;

	// GCC .map files can give you only function/offset information.
	// You need to read the DWARF information from a GCC-generated .elf file to get file/line information.
	if(addressRepTypeFlags & kARTFlagFunctionOffset)
	{
		uint64_t  nAddressRelative = (nAddress - baseAddressToUse);
		EntryPair entryPair;
		entryPair.mAddress = nAddressRelative;
		entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);


		EntryPairArray::iterator it =
			eastl::upper_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

		if(it != mEntryPairArray.begin())
		{
			entryPair = *--it;

			const int addressOffset = (int)(unsigned)(nAddressRelative - entryPair.mAddress);

			if(abs(addressOffset) < 65536) // Do a sanity check.
			{
				char buffer[512]; // The symbol may in fact already be demangled, so UnmangleSymbol may just do a strcpy.
				UnmangleSymbol(entryPair.mSymbol.c_str(), buffer, sizeof(buffer), kCompilerTypeGCC);

				pRepArray[kARTFunctionOffset]      = buffer;
				pIntValueArray[kARTFunctionOffset] = addressOffset;
				returnFlags                       |= kARTFlagFunctionOffset;
			}
		}
	}

	return returnFlags;
}


#endif // EACALLSTACK_APPLE_MAPFILE_ENABLED



#if EACALLSTACK_SN_MAPFILE_ENABLED

///////////////////////////////////////////////////////////////////////////////
// MapFileSN
// Also works for Sony linkers of clang-based builds.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// An SN .map file looks like the text below.
// We want to read the .text and .gnu.linkonce entries.
// Within the .text section, File entries are sorted by address from lowest
// to highest. But the symbols within a File section aren't necessarily
// in order.
//
//
///////////////////////////////////////////////////////////////////////////////
// Address  Size     Align Out     In      File    Symbol
// =================================================================
// UNDEFINED
//                                                 ._lock_finalize
//                                                 ._debug_agent_shm_init
// 00010200 00000000     1 .interp
// 00010200 0000002c     4 .init
// 00010200 0000000c     4         .init
// 00010200 0000000c     4                 C:\packages\ps3sdk\220.002-411-lite\installed\host-win32\ppu\lib\gcc\ppu-lv2\4.1.1\fno-exceptions\fno-rtti\ecrti.o (TOC#1,1304)
// 00010200 00000000     0                         .__init
// 0001020c 00000008     1                 C:\packages\ps3sdk\220.002-411-lite\installed\host-win32\ppu\lib\gcc\ppu-lv2\4.1.1\fno-exceptions\fno-rtti\crtbegin.o (TOC#1,1304)
// 0001021c 00000010     1                 C:\packages\ps3sdk\220.002-411-lite\installed\host-win32\ppu\lib\gcc\ppu-lv2\4.1.1\fno-exceptions\fno-rtti\ecrtn.o (TOC#1,1304)
// 00010230 0008f5d8     8 .text
// 00010230 00000390     8         .text
// 00010230 00000390     8                 C:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\build\test\EAStdCTest\test\source\EAStdCTest.cpp.obj (TOC#1,1304)
// 00010348 00000094     0                         operator new(unsigned int, unsigned int, unsigned int, char const*, int, unsigned int, char const*, int)
// 000103e0 000001dc     0                         .main
// 000105c0 00003d98     8                 C:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\build\test\EAStdCTest\test\source\EATest.cpp.obj (TOC#1,1304)
// 000105c0 00000124     0                         EA::UnitTest::Test::Test(char const*, void (*)(char const*))
// 000106e8 00000068     0                         EA::UnitTest::Test::Test(char const*, void (*)(char const*))
// 0009f684 0000006c     1         .gnu.linkonce.t._ZN5eastl8distanceIPKjEENS_15iterator_traitsIT_E15difference_typeES4_S4_
// 0009f684 0000006c     1                 C:\Projects\EAOS\UTF\build\EASTL\dev\ps3-sn-dev-debug\lib\libEASTL.a(BB_runtime.EASTL.cpp.j7iZvRTCETjAkhs9uEOuw.obj) (TOC#1,1304)
// 0009f684 0000006c     0                         eastl::iterator_traits< unsigned int const* >::difference_type eastl::distance< unsigned int const* >(unsigned int const*, unsigned int const*)
// 0009f6f0 00000050     1         .gnu.linkonce.t._ZN5eastl13distance_implIPKjEENS_15iterator_traitsIT_E15difference_typeES4_S4_St26random_access_iterator_tag
// 0009f6f0 00000050     1                 C:\Projects\EAOS\UTF\build\EASTL\dev\ps3-sn-dev-debug\lib\libEASTL.a(BB_runtime.EASTL.cpp.j7iZvRTCETjAkhs9uEOuw.obj) (TOC#1,1304)
// 0009f6f0 00000050     0                         eastl::iterator_traits< unsigned int const* >::difference_type eastl::distance_impl< unsigned int const* >(unsigned int const*, unsigned int const*, std::random_access_iterator_tag)
// 0009f808 00000024     4 .fini
// 0009f808 0000000c     4         .fini
// 0009f808 0000000c     4                 C:\packages\ps3sdk\220.002-411-lite\installed\host-win32\ppu\lib\gcc\ppu-lv2\4.1.1\fno-exceptions\fno-rtti\ecrti.o (TOC#1,1304)
// 0009f808 00000000     0                         .__fini
// 0009f81c 00000010     1                 C:\packages\ps3sdk\220.002-411-lite\installed\host-win32\ppu\lib\gcc\ppu-lv2\4.1.1\fno-exceptions\fno-rtti\ecrtn.o (TOC#1,1304)
// 0009f82c 00000160     4 .sceStub.text
// 0009f82c 00000020     4         .sceStub.text
// 0009f82c 00000020     4                 C:\packages\ps3sdk\220.002-411-lite\installed\target\ppu\lib\liblv2_stub.a(_sysPrxForUser_0001_7.o) (TOC#1,1304)
// 0009f82c 00000020     0                         .sys_lwmutex_lock
// 00000000 00000000     1 .debug_frame
// 00000000 0001352b     1 .debug_str
// 00000000 0001352b     1         .debug_str
// 00000000 0001352b     1                 Dwarf1 (TOC#1,1304)
// 00000000 00000000     1 .debug_loc
// 00000000 00000000     1 .pgo_info
// 00000000 00000000     1 /DISCARD/
// 00000000 000000dc     4 .psp_libgen_markfunc
// 00000000 00000014     4         .psp_libgen_markfunc
// 00000000 00000014     4                 C:\packages\ps3sdk\220.002-411-lite\installed\target\ppu\lib\liblv2_stub.a(_sysPrxForUser_0001_7.o) (TOC#1,1304)
// 00000000 00000014     0                         sys_lwmutex_lock
// 00000014 00000014     4                 C:\packages\ps3sdk\220.002-411-lite\installed\target\ppu\lib\liblv2_stub.a(_sysPrxForUser_0001_10.o) (TOC#1,1304)
// 00000014 00000014     0                         sys_lwmutex_unlock
// 00000000 00000000     1 .psp_libgen_markvar
// 00000000 00000000     1 .psp_libgen_marktls
// 00000000 00000000     1 /DISCARD/
// 00000000 00000000     1 /DISCARD/
// 000c4e8c 00000000     0                         _edata
// 000c4e8c 00000000     0                         __bss_start
// 10000000 00000000     0                         _end
// 10000000 00000000     0                         __end
// 00010000 00000000     0                         __executable_start
// 0009f98c 00000000     0                         __etext
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// MapFileSN
//
MapFileSN::MapFileSN(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mbLoadAttempted(false),
	mbLowMemoryUsage(false),
	mbOfflineLookup(false),
	mbSymbolEndFound(false),
	mbImageBaseIsZero(false),
	msMapFilePath(),
	mEntryPairArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileSN", mpCoreAllocator)),
	mBaseAddress(kBaseAddressUnspecified),
	mFileBufferSize(4096),
	mFile(),
	mFileBuffer(IO::StreamBuffer::kBufferSizeUnspecified, IO::StreamBuffer::kBufferSizeUnspecified, NULL, mpCoreAllocator),
	mFileLowMemoryUsage(),
	mFileBufferLowMemoryUsage(IO::StreamBuffer::kBufferSizeUnspecified, IO::StreamBuffer::kBufferSizeUnspecified, NULL, mpCoreAllocator),
	mAddressToFilePosArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileSN", mpCoreAllocator)),
	mFileSize(0)
{
	mFile.AddRef(); // This prevents it from being released away by anybody.
	mFileLowMemoryUsage.AddRef();
}


///////////////////////////////////////////////////////////////////////////////
// ~MapFileSN
//
MapFileSN::~MapFileSN()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void MapFileSN::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;


	mEntryPairArray       .get_allocator().set_allocator(pCoreAllocator);
	mAddressToFilePosArray.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool MapFileSN::Init(const wchar_t* pMapFilePath, bool bDelayLoad)
{
	if(pMapFilePath)
		msMapFilePath = pMapFilePath;

	// We delay the reading of mBaseAddress until Load.

	if(!bDelayLoad)
		Load();

	return (bDelayLoad || !mEntryPairArray.empty());
}


// This function is used when mbLowMemoryUsage == true.
// This function fills in mAddressToFilePosArray until nAddress is found.
// Thus it builds a cache of info (mAddressToFilePosArray) about where symbols are.
// It doesn't read every symbol line into a mAddressToFilePosArray entry,
// but rather reads and stores one every N of them in order to save memory.
// Since the symbols are in order by address this can be used to tell us
// roughly where a symbol can later be found in the file in the FindAddressToFilePosArray function.
bool MapFileSN::FillAddressToFilePosArray(uint64_t nAddress)
{
	bool bAddressFound = false;

	// Assert that mAddressToFilePosArray doesn't contain nAddress (otherwise we wouldn't be calling this function).
	EA_ASSERT(!mAddressToFilePosArray.empty() && (nAddress >= mAddressToFilePosArray.back().mAddress));

	// We assume that mFileBuffer is positioned at the line after the last location we read a mAddressToFilePosArray entry.
	while(!mbSymbolEndFound && !bAddressFound)
	{
		IO::off_type filePos           = 0;
		uint64_t     symbolAddress     = UINT64_C(0xffffffffffffffff);
		unsigned     symbolSize        = 0;
		uint64_t     lastSymbolEnd     = 0;
		bool         bFileSectionFound = false;

		// We unilaterally read until the next File section or until the end of the symbol lines.
		// Thus we will always append at least one new mAddressToFilePosArray entry.
		// See a .map file to understand this; it's pretty simple.
		for(uint32_t i = 0; !mbSymbolEndFound && !bFileSectionFound; i++)
		{
			//Address  Size     Align Out     In      File    Symbol
			//=================================================================
			// . . .
			//000686b8 00000b08     8                 D:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\lib\libEAStdC.a(EAHashCRC.cpp.obj) (TOC#1,1288)
			//000686b8 000000f0     0                         EA::StdC::CRC16(void const*, unsigned int, unsigned short, bool)
			//000687a8 00000130     0                         EA::StdC::CRC24(void const*, unsigned int, unsigned int, bool)
			//000691c0 000012b8     8                 D:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\lib\libEAStdC.a(EAHashString.cpp.obj) (TOC#1,1288)
			//000698c8 000002c0     0                         EA::StdC::FNV1_String8(char const*, unsigned int, EA::StdC::CharCase)
			//000691c0 000000c8     0                         EA::StdC::DJB2(void const*, unsigned int, unsigned int)
			// . . .
			//000c4e54 00000000     1 .something

			const size_t  kLineSize = 2600;
			char          line[kLineSize];
			IO::size_type lineSize;

			lineSize = IO::ReadLine(&mFileBuffer, line, kLineSize);

			if(lineSize < IO::kSizeTypeDone)  // If the line was read without error and wasn't at the end of the file...
			{
				const bool bNewOutSection  = (line[kOutMinLength  - 1] != ' ');
				const bool bNewFileSection = (line[kFileMinLength - 1] != ' ');

				if(bNewOutSection)
				{
					mbSymbolEndFound = true;
					symbolAddress    = lastSymbolEnd;
				}
				else if(bNewFileSection)
				{
					bFileSectionFound = true;
					EA::StdC::Sscanf(line, "%I64x %x", &symbolAddress, &symbolSize);
					lastSymbolEnd = symbolAddress + symbolSize;
					filePos       = mFileBuffer.GetPosition(EA::IO::kPositionTypeBegin);
					break;
				}
				// else skip the line. It's a symbol line but we aren't compiling individual symbol lines in low memory mode.
			}
			else
			{
				// We've reached the end of the file. Normally this shouldn't happen, as we should hit the bNewOutSection stuff above first.
				mbSymbolEndFound = true;
				symbolAddress    = lastSymbolEnd;
			}
		}

		if(mbSymbolEndFound)  // If the line was bad or was at the end of the file...
		{
			// We are done with mFileBuffer.
			mFileBuffer.SetStream(NULL);
			mFileBuffer.SetBufferSizes(0, 0);
			mFile.Close();
		}
		else
		{
			AddressToFilePos addressPos = { (uint64_t)symbolAddress, (uint64_t)filePos };
			mAddressToFilePosArray.push_back(addressPos);

			bAddressFound = (nAddress < symbolAddress);
		}
	}

	return bAddressFound;
}


// This function is used when mbLowMemoryUsage == true.
// This function finds an appropriate starting position within the file and
// starts reading symbol lines from it till it gets to the one that corresponds
// to nAddress.
bool MapFileSN::FindAddressToFilePosArray(uint64_t nAddress, CAString8& s)
{
	bool bResult = false;
	AddressToFilePos addressPos = { nAddress, 0 };

	// Find the first File line in the file which might correspond to nAddress.
	// We store only File lines in mAddressToFilePosArray, in order to save memory
	// for this low memory mode and since File entries seem to be sorted by ascending
	// address, whereas symbols within File sections aren't necessarily sorted.
	// We will start with this line and read successive lines until we find the
	// one that corresponds to our symbol. This function should not be being called
	// unless we have found that nAddress is represented with lines we have read
	// into mAddressToFilePosArray. We should need to read only as far as the next
	// File section.
	EA_ASSERT(!mAddressToFilePosArray.empty());
	AddressToFilePosArray::iterator it = eastl::upper_bound(mAddressToFilePosArray.begin(), mAddressToFilePosArray.end(), addressPos, AddressToFilePosCompare());

	if(it != mAddressToFilePosArray.begin())
		--it;

	if(nAddress < mAddressToFilePosArray[0].mAddress)
		return false;

	addressPos = *it; // Start at this line, which should be a File line.
	mFileBufferLowMemoryUsage.SetPosition((IO::off_type)addressPos.mFilePos, IO::kPositionTypeBegin);

	// Read lines until we get to the first line that is beyond nAddress.
	const size_t  kLineSize = 2600;
	char          line[kLineSize];
	IO::size_type lineSize;

	while((lineSize = IO::ReadLine(&mFileBufferLowMemoryUsage, line, kLineSize)) < IO::kSizeTypeDone) // While there are more lines...
	{
		//Address  Size     Align Out     In      File    Symbol
		//=================================================================
		//. . .
		//0006a478 000024d0     8                 D:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\lib\libEAStdC.a(EAMemory.cpp.obj) (TOC#1,1288)
		//0006afc8 000000b4     0                         EA::StdC::Memcheck8(void const*, unsigned char, unsigned int)
		//0006b080 000000e0     0                         EA::StdC::Memcheck16(void const*, unsigned short, unsigned int)
		//0006a938 00000268     0                         EA::StdC::Memset32(void*, unsigned int, unsigned int)
		//0006c948 00001268     8                 D:\Projects\EAOS\UTF\build\EAStdC\dev\ps3-sn-dev-debug\lib\libEAStdC.a(EARandom.cpp.obj) (TOC#1,1288)
		//0006ca50 000000ec     0                         EA::StdC::RandomLinearCongruential::RandomUint32Uniform(unsigned int)
		//0006c9e8 00000068     0                         EA::StdC::RandomLinearCongruential::RandomUint32Uniform()
		//0006cc28 00000078     0                         EA::StdC::RandomMersenneTwister::RandomMersenneTwister(unsigned int)
		//. . .

		if((lineSize == 0) || !EA::StdC::Isxdigit(line[0])) // I don't think this should ever occur, unless we bump up against the end of the file.
		{
			// We don't set mbSymbolEndFound here, because that's the job of FillAddressToFilePosArray.
			bResult = false;
			// break; Should we break here? It's not clear that doing so is a major win.
		}
		else if(line[kFileMinLength - 1] != ' ') // If we have reached a new File section...
		{
			// We didn't find the symbol within the current File section.
			break;
		}
		else
		{
			uint64_t address;
			unsigned size;

			if(2 == EA::StdC::Sscanf(line, "%I64x %x", &address, &size))
			{
				if((nAddress >= address) && (nAddress < (address + size)))  // If we have passed the line that refers to the symbol, then the previous line is the symbol line.
				{
					// We leave the symbol mangled here; we'll unmangle it in GetAddressRep.
					s       = &line[kSymbolMinLength-1];
					bResult = true;

					break;
				}
			}
		}
	}

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// Load
//
bool MapFileSN::Load()
{
	bool bSuccess = false;

	if(!mbLoadAttempted)
	{
		mbLoadAttempted = true;

		mFile.SetPath(msMapFilePath.c_str());

		if(mFile.Open())
		{
			// We put a buffer in front of the file stream in order to
			// speed up our reads, which will be entirely sequential.
			mFileBuffer.SetBufferSizes(mFileBufferSize, IO::StreamBuffer::kBufferSizeUnspecified);
			mFileBuffer.SetStream(&mFile);

			mFileSize = mFile.GetSize();

			if(mbLowMemoryUsage)
			{
				// Possibly do something.
			}
			else
			{
				// We'll almost certainly need a few hundred entries, so reserve some up-front.
				mEntryPairArray.reserve(4096);
			}

			// Read the map file data here.
			// We read each line from the file until we get to the .text section line. At that point each
			// line in this section is either a module name entry or is a symbol name entry. See the example .map file.
			// We stop when we see the end of the .text section due to the beginning of a new section.
			// We add to our mEntryPairArray from each symbol line. In mbLowMemoryUsage mode, we don't read all
			// the lines, but rather we take note of where in the file the .text section begins and then exit;
			// we'll read the lines later on demand.

			bool              bReadLines = false;
			const size_t      kLineSize = 768;          // We may want to allocate this on the heap instead of the stack.
			char              line[kLineSize + 1];
			IO::size_type     lineSize;
			IO::off_type      filePosition = 0;         // Denotes the file position for the beginning of the current line.

			for(bool bContinueReading = true;
				bContinueReading && (lineSize = IO::ReadLine(&mFileBuffer, line, kLineSize)) < IO::kSizeTypeDone; // While we should continue reading and while there are more lines...
				filePosition = mFileBuffer.GetPosition(EA::IO::kPositionTypeBegin))
			{
				if(lineSize >= kOutMinLength)
				{
					const char* const pOutBegin = &line[kOutMinLength - 1];

					if((EA::StdC::Strstr(pOutBegin, ".text") == pOutBegin)) // If this line represents the beginning of a .text section ...
					{
						bReadLines = true;
					}
					else if((EA::StdC::Strstr(pOutBegin, "__text_end") != NULL))
					{
						// we need to ignore this odd non function where address is actually size
					}
					else if((EA::StdC::Strstr(pOutBegin, ".interp") == pOutBegin)) // If this line represents the beginning of a .interp section ...
					{
						const char* const pAddressBegin = &line[0];
						const uint64_t address = EA::StdC::StrtoU64(pAddressBegin, NULL, 16);

						mbImageBaseIsZero = (address == 0);
					}
					else if(pOutBegin[0] != ' ') // Else if we have a section but it isn't .text ...
					{
						bReadLines = false;
					}
					else if(bReadLines && (lineSize >= kSymbolMinLength) && (EA::StdC::Strstr(pOutBegin, "                        ") == pOutBegin)) // Else if we are reading lines and the current line is a symbol...
					{
						const char* const pAddressBegin = &line[0];
						const char* const pSymbolBegin  = &line[kSymbolMinLength-1];

						// We have a line that is like this:
						//     "000105c0 00000124     0                         SomeFunction(int)"
						const uint64_t address = EA::StdC::StrtoU64(pAddressBegin, NULL, 16);
						char           symbol[256];
						size_t         symbolLength;

						if(mbLowMemoryUsage)
						{
							// In low memory mode, we stop loading right here, after the first symbol.
							// We will start reading the symbol lines as needed when the user calls GetAddressRep.
							EA_ASSERT(mAddressToFilePosArray.empty());

							AddressToFilePos entry = { address, (uint64_t)filePosition };
							mAddressToFilePosArray.push_back(entry);

							// We need to open the same file a second time. The reason for this is that we use mFile to
							// read symbol lines from filePosition position till the end of the file. This reading is
							// done periodically as we need to extend our sampling of symbol lines. We have this other
							// file so we can go back and read lines dynamically between our sampling positions.
							// We don't want to use the same file object for both, as we would get file cache thrashing
							// if we did so.
							mFileLowMemoryUsage.SetPath(msMapFilePath.c_str());
							mFileLowMemoryUsage.Open(); // This should always succeed. If the open fails then we'll catch it later during read.
							mFileBufferLowMemoryUsage.SetBufferSizes(mFileBufferSize, IO::StreamBuffer::kBufferSizeUnspecified);
							mFileBufferLowMemoryUsage.SetStream(&mFileLowMemoryUsage);

							bContinueReading = false;
						}
						else
						{
							symbolLength = EA::StdC::Strlcpy(symbol, pSymbolBegin, sizeof(symbol));
							if(symbolLength >= sizeof(symbol))
								symbolLength = sizeof(symbol) - 1;

							if(address && symbolLength) // If we have valid data...
							{
								// We leave the symbol mangled here; we'll unmangle it in GetAddressRep.
								EntryPair entryPair;
								entryPair.mAddress = address;
								entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);
								entryPair.mSymbol = symbol;

								EntryPairArray::iterator it =
									eastl::lower_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

								mEntryPairArray.insert(it, std::move(entryPair)); // Much of the time, this will be an append, because the entries are largely sorted already.
							}
						}
					}
				} // if(lineSize...
			} // for ...

			if(mbLowMemoryUsage)
				bSuccess = !mAddressToFilePosArray.empty();
			else
				bSuccess = !mEntryPairArray.empty();
		}
	}

	if(!mbLowMemoryUsage)
	{
		// In the case of low memory usage, we leave the file open for iterative reading.
		// Otherwise we clear the stream buffer and file stream, as we won't need them any more.
		mFileBuffer.SetStream(NULL);
		mFileBuffer.SetBufferSizes(0, 0);
		mFile.Close();

		// mFileLowMemoryUsage shouldn't be open.
		EA_ASSERT(!mFileLowMemoryUsage.GetAccessFlags());
	}

	if(bSuccess)
	{
		if((mBaseAddress == kBaseAddressUnspecified) && !mbOfflineLookup) // If the base address hasn't been set...
		{
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, msMapFilePath.c_str(), IO::kMaxPathLength);

			mBaseAddress = Callstack::GetModuleBaseAddress(pPath8);
			if(mBaseAddress == kBaseAddressInvalid)
				mBaseAddress = kBaseAddressUnspecified;
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool MapFileSN::Shutdown()
{
	// Clear our member data.
	mbLoadAttempted = false;
	mbSymbolEndFound = false;
	mEntryPairArray.set_capacity(0);
	mAddressToFilePosArray.set_capacity(0);
	mBaseAddress = kBaseAddressUnspecified;

	mFile.Close();
	mFileBuffer.SetStream(NULL);
	mFileBuffer.SetBufferSizes(0, 0);

	mFileLowMemoryUsage.Close();
	mFileBufferLowMemoryUsage.SetStream(NULL);
	mFileBufferLowMemoryUsage.SetBufferSizes(0, 0);

	mFileSize = 0;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* MapFileSN::AsInterface(int id)
{
	if(id == MapFileSN::kTypeId)
		return static_cast<MapFileSN*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void MapFileSN::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t MapFileSN::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* MapFileSN::GetDatabasePath() const
{
	return msMapFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void MapFileSN::SetOption(int option, int value)
{
	if(option == kOptionLowMemoryUsage)
	{
		EA_ASSERT(!mbLoadAttempted); // Must set this option before loading anything or doing any lookups.
		mbLowMemoryUsage = (value != 0);
	}
	else if(option == kOptionFileBufferSize)
		mFileBufferSize = (uint32_t)value;
	// else if(option == kOptionCopyFileToMemory) // Not currently supported.
	//    mbUseMemoryFile = (value != 0);
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int MapFileSN::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(!mbLoadAttempted)
		Load();

	uint64_t baseAddressToUse = ((mBaseAddress == kBaseAddressUnspecified) ? 0 : mBaseAddress);

	// We need to make the following check here, as the code below doesn't account for it.
	// We don't need to make an upper-bound check, as the code below does account for it.
	if(nAddress < baseAddressToUse)
		return 0;

	// SN .map files can give you only function/offset information.
	// You need to read the DWARF information from a .elf file to get file/line information.
	if(addressRepTypeFlags & kARTFlagFunctionOffset)
	{
		if(nAddress >= baseAddressToUse) // Possibly we can also check to see that the address is less than some ceiling address. But the code below will take care of that.
		{
			// need to use non relative addressing if the map file was built against a static address
			uint64_t  nAddressMapRelative = mbImageBaseIsZero ? (nAddress - baseAddressToUse) : nAddress;
			EntryPair entryPair;
			entryPair.mAddress = nAddressMapRelative;
			entryPair.mSymbol.get_allocator().set_allocator(mpCoreAllocator);
			CAString8 symbol;
			symbol.get_allocator().set_allocator(mpCoreAllocator);

			if(mbLowMemoryUsage)
			{
				// In this case, we are iteratively reading the file instead of looking at our (otherwise possibly very large) mAddressToSymbolMap.
				EA_ASSERT(!mAddressToFilePosArray.empty()); // We always will have at least one entry.
				AddressToFilePos* pAddressPosEnd = &mAddressToFilePosArray.back();

				if((nAddressMapRelative >= pAddressPosEnd->mAddress) && !mbSymbolEndFound) // If this address is beyond where we've yet read in the file and if there's anything else left to read in the file...
				{
					FillAddressToFilePosArray(nAddressMapRelative);
					pAddressPosEnd = &mAddressToFilePosArray.back();
				}

				if(nAddressMapRelative <= (pAddressPosEnd->mAddress + 65536)) // If this address is somewhere within the map file...
				{
					if(FindAddressToFilePosArray(nAddressMapRelative, symbol))
						entryPair.mSymbol = symbol; // We'll demangle below.
				} // Else the address is not present in the map file.
			}
			else
			{
				EntryPairArray::iterator it = eastl::upper_bound(mEntryPairArray.begin(), mEntryPairArray.end(), entryPair, EntryPairCompare());

				if(it != mEntryPairArray.begin())
					entryPair = *--it;
			}

			if(!(entryPair.mSymbol.empty())) // If the above succeeded...
			{
				const int addressOffset = (int)(unsigned)(nAddressMapRelative - entryPair.mAddress);

				if(abs(addressOffset) < 65536) // Do a sanity check.
				{
					char buffer[512]; // The symbol may in fact already be demangled, so UnmangleSymbol may just do a strcpy.
					UnmangleSymbol(entryPair.mSymbol.c_str(), buffer, sizeof(buffer), kCompilerTypeSN);

					pRepArray[kARTFunctionOffset]      = buffer;
					pIntValueArray[kARTFunctionOffset] = addressOffset;
					returnFlags                       |= kARTFlagFunctionOffset;
				}
			}
		}
	}

	return returnFlags;
}

#endif // EACALLSTACK_SN_MAPFILE_ENABLED

} // namespace Callstack
} // namespace EA
