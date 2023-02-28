/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_INTERNAL_CABREADER_H
#define EACALLSTACK_INTERNAL_CABREADER_H


#include <EABase/eabase.h>

#if defined(EA_PLATFORM_WINDOWS)

EA_DISABLE_ALL_VC_WARNINGS();

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <Windows.h>

EA_RESTORE_ALL_VC_WARNINGS();

#pragma pack(4)

	enum FDIERROR
	{
		FDIERROR_NONE,
		FDIERROR_CABINET_NOT_FOUND,
		FDIERROR_NOT_A_CABINET,
		FDIERROR_UNKNOWN_CABINET_VERSION,
		FDIERROR_CORRUPT_CABINET,
		FDIERROR_ALLOC_FAIL,
		FDIERROR_BAD_COMPR_TYPE,
		FDIERROR_MDI_FAIL,
		FDIERROR_TARGET_FILE,
		FDIERROR_RESERVE_MISMATCH,
		FDIERROR_WRONG_CABINET,
		FDIERROR_USER_ABORT
	};

	enum FDINOTIFICATIONTYPE
	{
		fdintCABINET_INFO,
		fdintPARTIAL_FILE,
		fdintCOPY_FILE,
		fdintCLOSE_FILE_INFO,
		fdintNEXT_CABINET,
		fdintENUMERATE
	};

	struct ERF
	{
		int  erfOper;
		int  erfType;
		BOOL fError;
	};

	struct FDICABINETINFO
	{
		long     cbCabinet;
		uint16_t cFolders;
		uint16_t cFiles;
		uint16_t setID;
		uint16_t iCabinet;
		BOOL     fReserve;
		BOOL     hasprev;
		BOOL     hasnext;
	};

	struct FDINOTIFICATION
	{
		long      cb;
		char*     psz1;
		char*     psz2;
		char*     psz3;
		void*     pv;
		int       hf;
		uint16_t  date;
		uint16_t  time;
		uint16_t  attribs;
		uint16_t  setID;
		uint16_t  iCabinet;
		uint16_t  iFolder;
		FDIERROR  fdie;
	};

	#define cpuUNKNOWN (-1)    // FDI does detection
	#define cpu80286   (0)     // 286 opcodes only
	#define cpu80386   (1)     // 386 opcodes used

	#define DIAMONDAPI __cdecl

	typedef int   (DIAMONDAPI *PFNFDINOTIFY) (FDINOTIFICATIONTYPE fdint, FDINOTIFICATION* pfdin);
	typedef int   (DIAMONDAPI *PFNOPEN)      (char* pFile, int oflag, int pmode);
	typedef UINT  (DIAMONDAPI *PFNREAD)      (int hf, void* pv, UINT cb);
	typedef UINT  (DIAMONDAPI *PFNWRITE)     (int hf, void* pv, UINT cb);
	typedef int   (DIAMONDAPI *PFNCLOSE)     (int hf);
	typedef long  (DIAMONDAPI *PFNSEEK)      (int hf, long dist, int seektype);
	typedef void* (DIAMONDAPI *PFNALLOC)     (ULONG cb);
	typedef void  (DIAMONDAPI *PFNFREE)      (void* pv);
	typedef int   (DIAMONDAPI *PFNFDIDECRYPT)(/*PFDIDECRYPT*/ void* pfdid);
	typedef void* (DIAMONDAPI *fFdiCreate)   (PFNALLOC, PFNFREE, PFNOPEN, PFNREAD, PFNWRITE, PFNCLOSE, PFNSEEK, int, ERF*);
	typedef BOOL  (DIAMONDAPI *fFdiCopy)     (void* hfdi, char*, char*, int, PFNFDINOTIFY, PFNFDIDECRYPT, void*);
	typedef BOOL  (DIAMONDAPI *fFdiIsCabinet)(void*, int, FDICABINETINFO*);
	typedef BOOL  (DIAMONDAPI *fFdiDestroy)  (void*);

#pragma pack()


///////////////////////////////////////////////////////////////////////////////
// CabReader
//
// Reads Windows .cab files
//
class CabReader
{
public:
	CabReader();
   ~CabReader();

	bool Init();
	void Shutdown();

	bool Copy(char* pCabFilePath, const char* pResultDirectoryPath, void* pContext = NULL);
	void Abort();
	int  GetLastError(int* pnExtra = NULL);

	struct CABINETINFO
	{
		char*  szNextCabinet;   // Name of the next cabinet excluding path info
		char*  szNextDisk;      // Name of the next disk
		char*  pFilePath;       // Path to the cabinet
		USHORT uSetID;          // The set ID of this cabinet
		USHORT uCabinet;        // The cabinet number within the cabinet set (starting at 0)
	};

	struct CABINETFILEINFO
	{
		char*    szFile;        // The name of the file
		LONG     lSize;         // Uncompressed file size
		FILETIME ftFile;        // Time and date of the file
		USHORT   uAttribs;      // Attributes of the file
	};

protected:
	static int   Open(const char* pFilePath, int oflag, int pmode);
	static int   Read(int fp, void* buffer, unsigned int count);
	static int   Write(int fp, const void* buffer, unsigned int count);
	static int   Close(int fp);
	static long  Seek(int fp, long offset, int origin);
	static void* Alloc(ULONG size);
	static void  Free(void* p);

	static int  FDICallback(FDINOTIFICATIONTYPE fdint, FDINOTIFICATION* pFDINotification);
	static void SetAttributesAndDate(LPCSTR lpszFileName, USHORT uDate, USHORT uTime, USHORT uAttribs);

	void OnCabinetInfo     (CABINETINFO& ci, LPVOID pContext);
	bool OnCopyFile        (CABINETFILEINFO& cfi, LPCSTR pDestPath, void* pContext);
	void OnCopyFileComplete(LPCSTR pDestPath, LPVOID pContext);
	void OnNextCabinet     (CABINETINFO& ci, FDIERROR fdiError, LPVOID pContext);

protected:
	HINSTANCE      mhCabinetDll;
	fFdiCreate     mfFdiCreate;
	fFdiCopy       mfFdiCopy;
	fFdiIsCabinet  mfFdiIsCabinet;
	fFdiDestroy    mfFdiDestroy;

	void*  mhFDIContext;
	ERF    mError;
	void*  mpContext;
	bool   mbShouldAbort;
	char   mDirectoryPath[MAX_PATH];

};  // class CabReader


#endif  // #if defined(EA_PLATFORM_WINDOWS)

#endif  // EACALLSTACK_INTERNAL_CABREADER_H


