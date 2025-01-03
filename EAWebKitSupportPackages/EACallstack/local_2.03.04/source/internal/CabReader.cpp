/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EACallstack/internal/CabReader.h>
#include <EACallstack/internal/Config.h>
#include <EAAssert/eaassert.h>

#if defined(EA_PLATFORM_WINDOWS)


CabReader::CabReader()
  : mhFDIContext(NULL),
	mhCabinetDll(NULL),
	mfFdiCreate(NULL),
	mfFdiCopy(NULL),
	mfFdiIsCabinet(NULL),
	mfFdiDestroy(NULL),
	mError(),
	mpContext(NULL),
	mbShouldAbort(false)
{
	mDirectoryPath[0] = 0;
	mError.fError = FALSE;
}


CabReader::~CabReader()
{
	Shutdown();
}


bool CabReader::Copy(char* pCabFilePath, const char* pResultDirectoryPath, void* pContext)
{
	char pFileName[MAX_PATH];
	char pFilePath[MAX_PATH];

	if(!mhFDIContext)
		return false;

	if(!pCabFilePath || (strlen(pCabFilePath) >= MAX_PATH))
		return false;

	if(!pResultDirectoryPath)
		pResultDirectoryPath = mDirectoryPath;  // Note by Paul Pedriana: This pathway doesn't make sense, as pResultDirectoryPath is unused after this.
	else
	{
		const size_t size = strlen(pResultDirectoryPath);

		if((size > 0) && (pResultDirectoryPath[size] != '\\') && ((size + 1) < MAX_PATH))
		{
			strcpy(mDirectoryPath, pResultDirectoryPath);
			strcat(mDirectoryPath, "\\");
		}
		else if(size < MAX_PATH)
			strcpy(mDirectoryPath, pResultDirectoryPath);
		else
			return false;
	}

	// Split the path to a filename and path
	const char* const p = strrchr(pCabFilePath, '\\');

	if(p == NULL)
	{
		strcpy(pFileName, pCabFilePath);
		strcpy(pFilePath, "");
	}
	else
	{
		strcpy(pFileName, p + 1);
		strncpy(pFilePath, pCabFilePath, (size_t)((p - pCabFilePath) + 1));
		pFilePath[(int)(p - pCabFilePath) + 1] = 0;
	}

	mpContext     = pContext;
	mbShouldAbort = false;

	BOOL bResult = mfFdiCopy(mhFDIContext, pFileName, pFilePath, 0, (PFNFDINOTIFY)FDICallback, NULL, this);
	
	return (bResult == TRUE);
}


void CabReader::Abort()
{
	mbShouldAbort = true;
}


int CabReader::GetLastError(int* pnExtra)
{
	if(!mError.fError)
		return 0;

	if(pnExtra)
		*pnExtra = mError.erfType;

	return mError.erfOper;
}


int CabReader::Open(const char* pFilePath, int oflag, int pmode)
{
	return _open(pFilePath, oflag, pmode);
}


int CabReader::Read(int fp, void* buffer, unsigned int count)
{
	return _read(fp, buffer, count);
}


int CabReader::Write(int fp, const void* buffer, unsigned int count)
{
	return _write(fp, buffer, count);
}


int CabReader::Close(int fp)
{
	return _close(fp);
}


long CabReader::Seek(int fp, long offset, int origin)
{
	return _lseek(fp, offset, origin);
}


void* CabReader::Alloc(ULONG size)
{
	return operator new(size);
}


void CabReader::Free(void* p)
{
	operator delete(p);
}


bool CabReader::Init()
{
	if(!mhCabinetDll)
	{
		// If the DLL is already attached to this process it will not be loaded a second time!
		mhCabinetDll   = LoadLibraryA("Cabinet.dll");
		EA_ASSERT(mhCabinetDll != NULL);

		mfFdiCreate    = (fFdiCreate)    (uintptr_t)GetProcAddress(mhCabinetDll, "FDICreate");
		mfFdiCopy      = (fFdiCopy)      (uintptr_t)GetProcAddress(mhCabinetDll, "FDICopy");
		mfFdiIsCabinet = (fFdiIsCabinet) (uintptr_t)GetProcAddress(mhCabinetDll, "FDIIsCabinet");
		mfFdiDestroy   = (fFdiDestroy)   (uintptr_t)GetProcAddress(mhCabinetDll, "FDIDestroy");

		if(!mfFdiCreate || !mfFdiCopy || !mfFdiIsCabinet || !mfFdiDestroy)
		{
			FreeLibrary(mhCabinetDll);
			mhCabinetDll = NULL;
		}
	}

	if(mhCabinetDll)
	{
		if(!mhFDIContext)
		{
			mhFDIContext = mfFdiCreate((PFNALLOC)Alloc, (PFNFREE)Free, (PFNOPEN)Open,
									   (PFNREAD)Read, (PFNWRITE)Write,
									   (PFNCLOSE)Close, (PFNSEEK)Seek, cpu80386, &mError);
		}
	}

	return (mhFDIContext != NULL);
}


void CabReader::Shutdown()
{
	if(mhFDIContext)
	{
		mfFdiDestroy(mhFDIContext);
		mhFDIContext = NULL;
	}

	if(mhCabinetDll)
	{
		FreeLibrary(mhCabinetDll);
		mhCabinetDll = NULL;
	}
}


int CabReader::FDICallback(FDINOTIFICATIONTYPE fdint, FDINOTIFICATION* pFDINotification)
{
	int nRet = 0; // Allow unsupported notifications to continue

	// Get pointer to the instance
	CabReader* pThis = (CabReader*)pFDINotification->pv;

	// Abort if necessary
	if(pThis->mbShouldAbort)
		return -1;

	// Relay all currently known notifications
	switch (fdint)
	{
		case fdintCABINET_INFO:
		{
			CABINETINFO ci = { pFDINotification->psz1, pFDINotification->psz2, pFDINotification->psz3, pFDINotification->setID, pFDINotification->iCabinet };

			pThis->OnCabinetInfo(ci, pThis->mpContext);    
			break;
		}

		case fdintCOPY_FILE:
		{
			CABINETFILEINFO cfi;
			char pFilePath[MAX_PATH]; pFilePath[0] = 0;

			// Copy target directory
			strcpy(pFilePath, pThis->mDirectoryPath);

			// Split the path to a filename and path
			char* p = strrchr(pFDINotification->psz1, '\\');

			if(p == NULL)
				cfi.szFile = pFDINotification->psz1;
			else
			{
				cfi.szFile = p + 1;
				size_t size = (int)(p - pFDINotification->psz1) + strlen(pFilePath) + 1;
				if(size + 1 >= MAX_PATH)
					return 0;
				strncat(pFilePath, pFDINotification->psz1, size);
				pFilePath[size] = 0;
			}

			// Fill rest of the struct
			cfi.lSize    = pFDINotification->cb;
			cfi.uAttribs = pFDINotification->attribs;
			DosDateTimeToFileTime(pFDINotification->date, pFDINotification->time, &cfi.ftFile);

			// Call notification function
			if(!pThis->OnCopyFile(cfi, pFilePath, pThis->mpContext))
				nRet = 0;
			else
			{
				char szFolder[MAX_PATH];

				// Make sure the directory exists
				p = strchr(pFilePath, '\\');

				while (p)
				{
					strncpy(szFolder, pFilePath, (size_t)(p - pFilePath));
					szFolder[p - pFilePath] = 0;
					CreateDirectoryA(szFolder, NULL);
					p = strchr(p + 1, '\\');
				}

				// Create the file
				strncat(pFilePath, cfi.szFile, MAX_PATH - 1 - strlen(pFilePath));
				nRet = Open(pFilePath, _O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL, _S_IREAD | _S_IWRITE);
			}

			break;
		}

		case fdintCLOSE_FILE_INFO:
		{
			char pFilePath[MAX_PATH]; pFilePath[0] = 0;

			// Close the file
			Close(pFDINotification->hf);

			// Set the file attributes
			strcpy(pFilePath, pThis->mDirectoryPath);
			strncat(pFilePath, pFDINotification->psz1, MAX_PATH - 1 - strlen(pFilePath));
			SetAttributesAndDate(pFilePath, pFDINotification->date, pFDINotification->time, pFDINotification->attribs);

			// Call notification function
			pThis->OnCopyFileComplete(pFDINotification->psz1, pThis->mpContext);
			nRet = TRUE;
			break;
		}

		case fdintNEXT_CABINET:
		{
			CABINETINFO ci = { pFDINotification->psz1, pFDINotification->psz2, pFDINotification->psz3, pFDINotification->setID, pFDINotification->iCabinet };

			pThis->OnNextCabinet(ci, pFDINotification->fdie, pThis->mpContext);
			break;
		}
	}

	return nRet;
}


void CabReader::SetAttributesAndDate(const char* pFilePath, USHORT uDate, USHORT uTime, USHORT uAttribs)
{
	HANDLE hFile = CreateFileA(pFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME ftFile;

		if(DosDateTimeToFileTime(uDate, uTime, &ftFile))
		{
			FILETIME ftLocal;

			if(LocalFileTimeToFileTime(&ftFile, &ftLocal))
				SetFileTime(hFile, &ftLocal, NULL, &ftLocal);
		}

		CloseHandle(hFile);
	}

	SetFileAttributesA(pFilePath, (DWORD)(uAttribs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE)));
}


void CabReader::OnCabinetInfo(CABINETINFO& /*ci*/, LPVOID /*pContext*/)
	{ }


bool CabReader::OnCopyFile(CABINETFILEINFO& /*cfi*/, const char* /*pDestPath*/, void* /*pContext*/)
	{ return true; } // Return value indicates if you want to extract the file or not.


void CabReader::OnCopyFileComplete(const char* /*pDestPath*/, LPVOID /*pContext*/)
	{ }


void CabReader::OnNextCabinet(CABINETINFO& /*ci*/, FDIERROR /*fdiError*/, LPVOID /*pContext*/)
	{ }


#endif // #if defined(EA_PLATFORM_WINDOWS)



