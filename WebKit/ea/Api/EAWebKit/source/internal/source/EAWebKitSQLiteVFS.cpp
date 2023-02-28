/*
Copyright (C) 2013, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"

#if ENABLE(SQL_DATABASE) && defined(SQLITE_OS_OTHER)

#include <sqlite3.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitSystem.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKit_p.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <EAWebKit/EAWebKitThreadInterface.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <internal/include/EAWebKitAssert.h>
#include <wtf/CurrentTime.h>
#include <EAIO/EAFileBase.h>
#include <EASTL/list.h>

extern "C"
{


 // The default size of a disk sector
#ifndef SQLITE_DEFAULT_SECTOR_SIZE
# define SQLITE_DEFAULT_SECTOR_SIZE 512
#endif

/*
** The following values may be passed as the second argument to
** sqlite3OsLock(). The various locks exhibit the following semantics:
**
** SHARED:    Any number of processes may hold a SHARED lock simultaneously.
** RESERVED:  A single process may hold a RESERVED lock on a file at
**            any time. Other processes may hold and obtain new SHARED locks.
** PENDING:   A single process may hold a PENDING lock on a file at
**            any one time. Existing SHARED locks may persist, but no new
**            SHARED locks may be obtained by other processes.
** EXCLUSIVE: An EXCLUSIVE lock precludes all other locks.
**
** PENDING_LOCK may not be passed directly to sqlite3OsLock(). Instead, a
** process that requests an EXCLUSIVE lock may actually obtain a PENDING
** lock. This can be upgraded to an EXCLUSIVE lock by a subsequent call to
** sqlite3OsLock().
*/
const int NO_LOCK       = 0;
const int SHARED_LOCK   = 1;
const int RESERVED_LOCK = 2;
const int PENDING_LOCK  = 3;
const int EXCLUSIVE_LOCK = 4;

 // Macros for locking and unlocking files
#define LockFile(a,b,c,d,e)       EAWebKitSQLiteVFS_LockFile(reinterpret_cast<uintptr_t>(&a), b, c, d, e)
#define UnlockFile(a,b,c,d,e)     EAWebKitSQLiteVFS_UnlockFile(reinterpret_cast<uintptr_t>(&a), b, c, d, e)

const int PENDING_BYTE     = 0x40000000;  /* First byte past the 1GB boundary */
const int RESERVED_BYTE    = (PENDING_BYTE+1);
const int SHARED_FIRST     = (PENDING_BYTE+2);
const int SHARED_SIZE      = 510;

#ifndef FALSE
#define FALSE 0
#endif 

#ifndef TRUE
#define TRUE 1
#endif
typedef unsigned int BOOL;


const int EAWebKitSQLiteVFS_PATH_MAX = EA::WebKit::FileSystem::kMaxPathLength;
static int sLastError = SQLITE_OK;

#if _DEBUG 
static int sOpenCount = 0;  // Track open files
#endif 

// SQLiteEA lacks native support for file locking so we have to fake it
// with some code of our own.
typedef struct _SQLiteEALock
{
    EA::WebKit::IMutex*     mpMutex;   
    int nReaders;       // Number of reader locks obtained 
    BOOL bPending;      // Indicates a pending lock has been obtained 
    BOOL bReserved;     // Indicates a reserved lock has been obtained 
    BOOL bExclusive;    // Indicates an exclusive lock has been obtained 
} SQLiteEALock;

typedef struct _SQLiteEAFile 
{
    const sqlite3_io_methods*           pMethod;                // Must be first 
    EA::WebKit::FileSystem::FileObject  fileObject;	            // Handle for accessing the file 
    unsigned char                       LockType;			    // Type of lock currently held on this file 
    SQLiteEALock                        Local;			        // Locks obtained by this instance of SQLiteEAFile 
    SQLiteEALock*                       pShared;			    // Global Shared lock memory for the file  
    bool                                deleteOnClose;          // Delete on Close
    char                                path[EAWebKitSQLiteVFS_PATH_MAX+1]; // Store path name for delete on close  
} SQLiteEAFile;


// get a pointer to the SQLiteFile from its fileObject member by using offsetof()
#define FILEOBJECT_TO_SQLiteEAFile(a) (SQLiteEAFile*)&((char*)a)[-((int)(offsetof(SQLiteEAFile,fileObject)))]

// Basic implementation of a look-up system for locks protected by a Shared critical section
#define MAX_LOCKS	256
EA::WebKit::IMutex* g_FileSystemMutex;
SQLiteEALock	g_LockList[MAX_LOCKS];
char*			g_lockName[MAX_LOCKS];
int				g_LockRef[MAX_LOCKS];

    
// prototype for function defined later on in this file
static void EAWebKitSQLiteVFS_DestroyLock(SQLiteEAFile *pFile);

typedef eastl::list<sqlite3_file*, EA::WebKit::EASTLAllocator> SQLiteFileHandlesList;
static  SQLiteFileHandlesList& activeHandles()
{
	static SQLiteFileHandlesList activeHandlesList;
	return activeHandlesList;
}

// Close the specified file
//	sqlite3_file* pFile - File to close
static int EAWebKitSQLiteVFS_Close(sqlite3_file* pFile)
{
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
        
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    fileSystem->CloseFile(pFileEA->fileObject);
    if (pFileEA->deleteOnClose)
    {
        fileSystem->RemoveFile(pFileEA->path);
        pFileEA->deleteOnClose = false;
    }

    fileSystem->DestroyFileObject(pFileEA->fileObject); 
    pFileEA->fileObject = 0;    
    EAWebKitSQLiteVFS_DestroyLock(pFileEA);
    
	activeHandles().remove(pFile);
#if _DEBUG 
    sOpenCount--;
#endif
    return SQLITE_OK;   // (Because close and destroy return void).
}


// Read data from a file into a buffer.  Return SQLITE_OK if all
// bytes were read successfully and SQLITE_IOERR if anything goes
// wrong.
//		sqlite3_file* pFile - File to read from 
//		void* pBuf - Write content into this buffer 
//		int Amt - Number of bytes to read 
//		sqlite3_int64 Offset - Begin reading at this Offset 
static int EAWebKitSQLiteVFS_Read( sqlite3_file* pFile, void* pBuf, int Amt, sqlite3_int64 Offset )
{
    EAW_ASSERT( pFile !=0 );
    SQLiteEAFile *pFileEA = (SQLiteEAFile*) pFile;
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    bool status = fileSystem->SetFilePosition(pFileEA->fileObject,(uint64_t) Offset);
    if (!status)
    {
        sLastError = SQLITE_FULL;
        return SQLITE_FULL; 
    }

    int64_t got = fileSystem->ReadFile(pFileEA->fileObject, pBuf, (int64_t) Amt);
    if (got < 0)
    {
        sLastError = SQLITE_IOERR_READ;   
        return SQLITE_IOERR_READ;
    }

    if( got == (int64_t) Amt )
    {
        return SQLITE_OK;
    }
    else
    {
        // Failed to read the full amount clear the remaining bits that we didn't get.
        memset(&((char*)pBuf)[got], 0, (int64_t)Amt-got);
        sLastError = SQLITE_IOERR_SHORT_READ;
        return SQLITE_IOERR_SHORT_READ;
    }
}

// Write data from a buffer into a file.  Return SQLITE_OK on success
// or some other error code on failure.
//		sqlite3_file* pFile - File to write into 
//		const void* pBuf - The bytes to be written 
//		int Amt - Number of bytes to write 
//		sqlite3_int64 Offset - Offset into the file to begin writing at 
static int EAWebKitSQLiteVFS_Write( sqlite3_file* pFile, const void* pBuf, int Amt, sqlite3_int64 Offset )
{
    EAW_ASSERT( pFile != 0 );
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
        
    // Seek.
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    bool status = fileSystem->SetFilePosition(pFileEA->fileObject,(uint64_t) Offset);
    if (!status)
    {
        sLastError = SQLITE_FULL;
        return SQLITE_FULL; 
    }

    // Write
    int64_t toWrite = (int64_t) Amt;
    EAW_ASSERT( toWrite > 0 );
    if (toWrite > 0)
    {
        bool status = fileSystem->WriteFile(pFileEA->fileObject, pBuf, toWrite);
        if (!status)
        {
            sLastError =  SQLITE_IOERR_WRITE;
            return SQLITE_IOERR_WRITE;
        }
    }
      
    // Note: Our write only returns a bool currently so there is no full evaluation.
    return SQLITE_OK;
}

// Truncate an open file to a specified size
//		sqlite3_file* pFile - file to truncate
//		sqlite3_int64 nByte - size to truncate to
static int EAWebKitSQLiteVFS_Truncate( sqlite3_file* pFile, sqlite3_int64 nByte) 
{
    EAW_ASSERT( pFile != 0 );
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
      
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    bool status = fileSystem->SetFileSize(pFileEA->fileObject, (int64_t) nByte);
    if (!status) 
    {
        sLastError = SQLITE_IOERR;        
        return SQLITE_IOERR;
    }
    
    return SQLITE_OK;
}

// Make sure all writes to a particular file are committed to disk.
//		sqlite3_file* pFile - file
//		int Flags - Flags
static int EAWebKitSQLiteVFS_Sync( sqlite3_file* pFile, int Flags )
{
    SQLiteEAFile *pFileEA = (SQLiteEAFile*) pFile;
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();    
    fileSystem->FlushFile(pFileEA->fileObject);
    
#if defined(EA_PLATFORM_SONY) 
    fileSystem->CloseFile(pFileEA->fileObject);
    fileSystem->OpenFile( pFileEA->fileObject, NULL, EA::WebKit::FileSystem::kWrite|EA::WebKit::FileSystem::kRead, EA::WebKit::FileSystem::kCDOOpenAlways);
#endif


    return SQLITE_OK;
}

// Determine the current size of a file in bytes
//		sqlite3_file* pFile - file
//		sqlite3_int64* pSize - file size
static int EAWebKitSQLiteVFS_FileSize(sqlite3_file* pFile, sqlite3_int64* pSize)
{
    SQLiteEAFile *pFileEA = (SQLiteEAFile*) pFile;

    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    *pSize = fileSystem->GetFileSize(pFileEA->fileObject);

    return SQLITE_OK;
}

// Get lock structure with specifed pname
//		const char* pname - pname of lock to look up
//		SQLiteLockEA** lock - lock to return
static void EAWebKitSQLiteVFS_LockGet(const char* pName, SQLiteEALock** ppLock)
{
    // find a lock by pName
    int i=0;
    int freeLock = -1; 	
    if (g_FileSystemMutex)
        g_FileSystemMutex->Lock();

    while( i < MAX_LOCKS  )
    {
        if( g_LockRef[i] == 0 )
        {
            if( freeLock == -1 )
            {
                freeLock = i;
            }
        }
        else if( strcmp(g_lockName[i], pName) == 0 )
        {
            // found it ! 
            *ppLock = &g_LockList[i];
            g_LockRef[i]++;
            freeLock = -1;
            break;
        }
        i++;
    }

    // if we didn't find the file return the first free instead.
    if( freeLock != -1 )
    {
        int NameLength = 0;
        *ppLock = &g_LockList[freeLock];
        g_LockRef[freeLock]++;		

        // get the length of the name, create a buffer for it and then copy the name into the buffer
        NameLength = strlen(pName)+1;
        g_lockName[freeLock] = (char*) sqlite3_malloc( NameLength );
        EAW_ASSERT(g_lockName[freeLock] != NULL );
        g_lockName[freeLock] = strncpy( g_lockName[freeLock], pName, NameLength ); 
        EAW_ASSERT(g_lockName[freeLock] != NULL );

        EAW_ASSERT(!g_LockList[freeLock].mpMutex);
        EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
        EA::WebKit::IMutex* pMutex = pThread->CreateAMutex();    
        EAW_ASSERT(pMutex);
        g_LockList[freeLock].mpMutex = pMutex;


        g_LockList[freeLock].nReaders = 0;  
        g_LockList[freeLock].bPending = 0; 
        g_LockList[freeLock].bReserved = 0;
        g_LockList[freeLock].bExclusive = 0;
    }
    if (g_FileSystemMutex)
        g_FileSystemMutex->Unlock();
    return;
}

// free a lock
//		SQLiteLockEA* plock - lock to release
static void EAWebKitSQLiteVFS_LockReturn(SQLiteEALock* pLock)
{
    int i=0;
    if (g_FileSystemMutex)
        g_FileSystemMutex->Lock();

    while( i < MAX_LOCKS  )
    {
        if( &g_LockList[i] == pLock )
        {
            g_LockRef[i]--;
            if( g_LockRef[i] == 0 )
            {
                if (g_LockList[i].mpMutex)
                {
                    EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
                    pThread->DestroyAMutex(g_LockList[i].mpMutex);
                    g_LockList[i].mpMutex = NULL;
                }
                sqlite3_free( g_lockName[i] );
            }
            break;
        }
        i++;
    }
    if (g_FileSystemMutex)
        g_FileSystemMutex->Unlock();
    return;
}

//Acquire a lock on the handle pFile
//		SQLiteEALock* pLock - lock to acquire
static void EAWebKitSQLiteVFS_MutexAcquire(SQLiteEALock* pLock)
{
    pLock->mpMutex->Lock();
}

// Release a lock acquired by EAWebKitSQLiteVFS_MutexAcquire()
//		SQLiteEALock* pLock - lock to release
static void EAWebKitSQLiteVFS_MutexRelease(SQLiteEALock* pLock)
{
    pLock->mpMutex->Unlock();
}

//Create the mutex and Shared memory used for locking in the file
// descriptor pFile
//		const char* pzFilename - file name
//		SQLiteEAFile* pFile - lock to create
static BOOL EAWebKitSQLiteVFS_CreateLock(const char* pzFilename, SQLiteEAFile* pFile)
{
    char* pzTok;
    int FileNameLength;
    char* pzName;

    // need to make a temporary copy of pzFileName to modify
    FileNameLength = strlen(pzFilename)+1;
    pzName = (char*) sqlite3_malloc( FileNameLength );
    EAW_ASSERT( pzName != NULL );
    memcpy( (void*) pzName, pzFilename, FileNameLength );

    // Replace the backslashes and forward slashes from the filename and lowercase it
    // to derive a mutex name. 
    for (pzTok = pzName;*pzTok;pzTok++)
    {
        *pzTok = tolower(*pzTok);
        if ((*pzTok == '/') || (*pzTok == '\\') || (*pzTok == ':') || (*pzTok == '.') )
        {
            *pzTok = '_';
        }
    }

    // Initialize the Local lockdata 
    pFile->Local.nReaders = 0;
    pFile->Local.bPending = 0;
    pFile->Local.bReserved = 0;
    pFile->Local.bExclusive = 0;

    // Create/open the named mutex 
    EAWebKitSQLiteVFS_LockGet(pzName, &pFile->pShared);

    // free temporary name
    sqlite3_free( pzName );

    return TRUE;
}

// Destroy the part of SQLiteEAFile that deals with SQLiteEA locks
//		SQLiteEAFile* pFile - file to destroy lock for
static void EAWebKitSQLiteVFS_DestroyLock(SQLiteEAFile* pFile)
{
    if (pFile->pShared)
    {
        // clean up the Shared lock
        EAWebKitSQLiteVFS_MutexAcquire(pFile->pShared);
        if (pFile->Local.nReaders)
        {
            pFile->pShared->nReaders --;
        }
        if (pFile->Local.bReserved)
        {
            pFile->pShared->bReserved = FALSE;
        }
        if (pFile->Local.bPending)
        {
            pFile->pShared->bPending = FALSE;
        }
        if (pFile->Local.bExclusive)
        {
            pFile->pShared->bExclusive = FALSE;
        }
        EAWebKitSQLiteVFS_MutexRelease(pFile->pShared);    

        // release our ptr to the Shared lock
        EAWebKitSQLiteVFS_LockReturn(pFile->pShared);
        pFile->pShared = NULL;
    }
}

// An implementation of the LockFile() API of windows for SQLiteEA
//		int* pFileDes - file to lock
//		unsigned long dwFileOffsetLow -
//		unsigned long dwFileOffsetHigh -
//		unsigned long nNumberOfBytesToLockLow - 
//		unsigned long nNumberOfBytesToLockHigg -
static BOOL EAWebKitSQLiteVFS_LockFile(uintptr_t pFileDes, unsigned long dwFileOffsetLow, unsigned long dwFileOffsetHigh, unsigned long nNumberOfBytesToLockLow, unsigned long nNumberOfBytesToLockHigh )
{
    SQLiteEAFile *pFile = FILEOBJECT_TO_SQLiteEAFile(pFileDes);
    BOOL bReturn = FALSE;

    if (!pFile->pShared) 
        return TRUE;

    EAWebKitSQLiteVFS_MutexAcquire(pFile->pShared);

    // Wanting an exclusive lock? 
    if (dwFileOffsetLow == SHARED_FIRST && nNumberOfBytesToLockLow == SHARED_SIZE)
    {
        if (pFile->pShared->nReaders == 0 && pFile->pShared->bExclusive == 0)
        {
            pFile->pShared->bExclusive = TRUE;
            pFile->Local.bExclusive = TRUE;
            bReturn = TRUE;
        }
    }
    // Want a read-only lock? 
    else if ((dwFileOffsetLow >= SHARED_FIRST && dwFileOffsetLow < SHARED_FIRST + SHARED_SIZE) && nNumberOfBytesToLockLow == 1)
    {
        if (pFile->pShared->bExclusive == 0)
        {
            pFile->Local.nReaders ++;
            if (pFile->Local.nReaders == 1)
            {
                pFile->pShared->nReaders ++;
            }
            bReturn = TRUE;
        }
    }
    // Want a pending lock? 
    else if (dwFileOffsetLow == PENDING_BYTE && nNumberOfBytesToLockLow == 1)
    {
        // If no pending lock has been acquired, then acquire it 
        if (pFile->pShared->bPending == 0) 
        {
            pFile->pShared->bPending = TRUE;
            pFile->Local.bPending = TRUE;
            bReturn = TRUE;
        }
    }
    // Want a reserved lock? 
    else if (dwFileOffsetLow == RESERVED_BYTE && nNumberOfBytesToLockLow == 1)
    {
        if (pFile->pShared->bReserved == 0) 
        {
            pFile->pShared->bReserved = TRUE;
            pFile->Local.bReserved = TRUE;
            bReturn = TRUE;
        }
    }

    EAWebKitSQLiteVFS_MutexRelease(pFile->pShared);
    return bReturn;
}

// An implementation of the UnlockFile API of windows for SQLiteEA
//		int* pFileDes,
//		unsigned long dwFileOffsetLow,
//		unsigned long dwFileOffsetHigh,
//		unsigned long nNumberOfBytesToUnlockLow,
//		unsigned long nNumberOfBytesToUnlockHigh
static BOOL EAWebKitSQLiteVFS_UnlockFile(uintptr_t pFileDes, unsigned long dwFileOffsetLow, unsigned long dwFileOffsetHigh, unsigned long nNumberOfBytesToUnlockLow, unsigned long nNumberOfBytesToUnlockHigh )
{
    SQLiteEAFile *pFileEA = FILEOBJECT_TO_SQLiteEAFile(pFileDes);
    BOOL bReturn = FALSE;

    if (!pFileEA->pShared) 
    {
        return TRUE;
    }

    EAWebKitSQLiteVFS_MutexAcquire(pFileEA->pShared);

    // Releasing a reader lock or an exclusive lock 
    if (dwFileOffsetLow >= SHARED_FIRST && dwFileOffsetLow < SHARED_FIRST + SHARED_SIZE)
    {
        // Did we have an exclusive lock? 
        if (pFileEA->Local.bExclusive)
        {
            pFileEA->Local.bExclusive = FALSE;
            pFileEA->pShared->bExclusive = FALSE;
            bReturn = TRUE;
        }
        // Did we just have a reader lock? 
        else if (pFileEA->Local.nReaders)
        {
            pFileEA->Local.nReaders --;
            if (pFileEA->Local.nReaders == 0)
            {
                pFileEA->pShared->nReaders --;
            }
            bReturn = TRUE;
        }
    }
    // Releasing a pending lock 
    else if (dwFileOffsetLow == PENDING_BYTE && nNumberOfBytesToUnlockLow == 1)
    {
        if (pFileEA->Local.bPending)
        {
            pFileEA->Local.bPending = FALSE;
            pFileEA->pShared->bPending = FALSE;
            bReturn = TRUE;
        }
    }
    // Releasing a reserved lock 
    else if (dwFileOffsetLow == RESERVED_BYTE && nNumberOfBytesToUnlockLow == 1)
    {
        if (pFileEA->Local.bReserved) 
        {
            pFileEA->Local.bReserved = FALSE;
            pFileEA->pShared->bReserved = FALSE;
            bReturn = TRUE;
        }
    }

    EAWebKitSQLiteVFS_MutexRelease(pFileEA->pShared);
    return bReturn;
}

// get a readlock
//		SQLiteEAFile* pFile - file to lock
static int GetReadLock(SQLiteEAFile* pFileEA)
{
    int res;
    res = LockFile(pFileEA->fileObject, SHARED_FIRST, 0, 1, 0);
    return res;
}

// Undo a readlock
//		SQLiteEAFile* pFile - file to unlock
static int UnlockReadLock(SQLiteEAFile* pFileEA)
{
    int res;
    res = UnlockFile(pFileEA->fileObject, SHARED_FIRST , 0, 1, 0);
    return res;
}

// Lock the file with the lock specified by parameter LockType - one
// of the following:
//
//     (1) SHARED_LOCK
//     (2) RESERVED_LOCK
//     (3) PENDING_LOCK
//     (4) EXCLUSIVE_LOCK
//
// Sometimes when requesting one lock state, additional lock states
// are inserted in between.  The locking might fail on one of the later
// transitions leaving the lock state different from what it started but
// still short of its goal.  The following chart shows the allowed
// transitions and the inserted intermediate states:
//
//    UNLOCKED -> SHARED
//    SHARED -> RESERVED
//    SHARED -> (PENDING) -> EXCLUSIVE
//    RESERVED -> (PENDING) -> EXCLUSIVE
//    PENDING -> EXCLUSIVE
//
// This routine will only increase a lock.  The SQLiteEAUnlock() routine
// erases all locks at once and returns us immediately to locking level 0.
// It is not possible to lower the locking level one step at a time.  You
// must go straight to locking level 0.
//		sqlite3_file* pFile - file
//		int LockType - lock type
static int EAWebKitSQLiteVFS_Lock(sqlite3_file* pFile, int LockType)
{
    int rc = SQLITE_OK;    // Return code from subroutines 
    int res = 1;           // Result of a windows lock call 
    int newLocktype;       // Set pSQLiteEAFile->LockType to this value before exiting 
    int gotPendingLock = 0;// True if we acquired a PENDING lock this time 
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;

    EAW_ASSERT( pFileEA!=0 );

    // If there is already a lock of this type or more restrictive on the
    // OsFile, do nothing. Don't use the end_lock: exit path, as
    // sqlite3OsEnterMutex() hasn't been called yet.	
    if( pFileEA->LockType >= LockType )
    {
        return SQLITE_OK;
    }

    // Make sure the locking sequence is correct	
    EAW_ASSERT( pFileEA->LockType!=NO_LOCK || LockType==SHARED_LOCK );
    EAW_ASSERT( LockType!=PENDING_LOCK );
    EAW_ASSERT( LockType!=RESERVED_LOCK || pFileEA->LockType==SHARED_LOCK );

    // Lock the PENDING_LOCK byte if we need to acquire a PENDING lock or
    // a SHARED lock.  If we are acquiring a SHARED lock, the acquisition of
    // the PENDING_LOCK byte is temporary.	
    newLocktype = pFileEA->LockType;
    if( pFileEA->LockType==NO_LOCK || (LockType==EXCLUSIVE_LOCK && pFileEA->LockType==RESERVED_LOCK) )
    {
        int cnt = 3;
        while( cnt-->0 && (res = LockFile(pFileEA->fileObject, PENDING_BYTE, 0, 1, 0))==0 )
        {
            // Try 3 times to get the pending lock.  The pending lock might be
            // held by another reader process who will release it momentarily.
            EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
            pThread->SleepThread(1);
        }
        gotPendingLock = res;
    }
    // Acquire a pShared lock	
    if( LockType==SHARED_LOCK && res )
    {
        EAW_ASSERT( pFileEA->LockType==NO_LOCK );
        res = GetReadLock(pFileEA);
        if( res )
        {
            newLocktype = SHARED_LOCK;
        }
    }
    // Acquire a RESERVED lock	
    if( LockType==RESERVED_LOCK && res )
    {
        EAW_ASSERT( pFileEA->LockType==SHARED_LOCK );
        res = LockFile(pFileEA->fileObject, RESERVED_BYTE, 0, 1, 0);
        if( res )
        {
            newLocktype = RESERVED_LOCK;
        }
    }
    // Acquire a PENDING lock	
    if( LockType==EXCLUSIVE_LOCK && res )
    {
        newLocktype = PENDING_LOCK;
        gotPendingLock = 0;
    }

    // Acquire an EXCLUSIVE lock	
    if( LockType==EXCLUSIVE_LOCK && res )
    {
        EAW_ASSERT( pFileEA->LockType >= SHARED_LOCK );
        res = UnlockReadLock(pFileEA);
        res = LockFile(pFileEA->fileObject, SHARED_FIRST, 0, SHARED_SIZE, 0);
        if( res )
        {
            newLocktype = EXCLUSIVE_LOCK;
        }
        else
        {
            GetReadLock(pFileEA);
        }
    }

    // If we are holding a PENDING lock that ought to be released, then
    // release it now.	
    if( gotPendingLock && LockType==SHARED_LOCK )
    {
        UnlockFile(pFileEA->fileObject, PENDING_BYTE, 0, 1, 0);
    }

    // Update the state of the lock has held in the file descriptor then
    // return the appropriate result code.	
    if( res )
    {
        rc = SQLITE_OK;
    }
    else
    {
        sLastError = SQLITE_BUSY;
        rc = SQLITE_BUSY;
    }
    pFileEA->LockType = newLocktype;
    return rc;
}

// Lower the locking level on file descriptor id to LockType.  LockType
// must be either NO_LOCK or SHARED_LOCK.
//
// If the locking level of the file descriptor is already at or below
// the requested locking level, this routine is a no-op.
//
// It is not possible for this routine to fail if the second argument
// is NO_LOCK.  If the second argument is SHARED_LOCK then this routine
// might return SQLITE_IOERR;
//		sqlite3_file* pFile
//		int LockType - lock type
static int EAWebKitSQLiteVFS_Unlock(sqlite3_file* pFile, int LockType)
{
    int type;
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
    int rc = SQLITE_OK;
    EAW_ASSERT( pFileEA !=0 );
    EAW_ASSERT( LockType <= SHARED_LOCK );
    type = pFileEA->LockType;
    if( type >= EXCLUSIVE_LOCK )
    {
        UnlockFile(pFileEA->fileObject, SHARED_FIRST, 0, SHARED_SIZE, 0);
        if( LockType == SHARED_LOCK && !GetReadLock(pFileEA) )
        {
            // This should never happen.  We should always be able to
            // reacquire the read lock 
            sLastError = SQLITE_IOERR_UNLOCK;
            rc = SQLITE_IOERR_UNLOCK;
        }
    }
    if( type >= RESERVED_LOCK )
    {
        UnlockFile(pFileEA->fileObject, RESERVED_BYTE, 0, 1, 0);
    }
    if( LockType == NO_LOCK && type >= SHARED_LOCK )
    {
        UnlockReadLock(pFileEA);
    }
    if( type>=PENDING_LOCK )
    {
        UnlockFile(pFileEA->fileObject, PENDING_BYTE, 0, 1, 0);
    }
    pFileEA->LockType = LockType;
    return rc;
}

// This routine checks if there is a RESERVED lock held on the specified
// file by this or any other process. If such a lock is held, return
// non-zero, otherwise zero.
//		sqlite3_file* pFile - file
//		int *pResOut - non-zero for reserved lock, otherwise zero
static int EAWebKitSQLiteVFS_CheckReservedLock(sqlite3_file* pFile, int* pResOut)
{
    int rc;
    SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
    EAW_ASSERT( pFileEA !=0 );
    if( pFileEA->LockType >= RESERVED_LOCK )
    {
        rc = 1;
    }
    else
    {
        rc = LockFile(pFileEA->fileObject, RESERVED_BYTE, 0, 1, 0);
        if( rc )
        {
            UnlockFile(pFileEA->fileObject, RESERVED_BYTE, 0, 1, 0);
        }
        rc = !rc;
    }
    *pResOut = rc;
    return SQLITE_OK;
}

//	Control and query of the open file handle.
//		sqlite3_file* pFile - file
//		int op - instruction
//		void *pArg - argument
static int EAWebKitSQLiteVFS_FileControl(sqlite3_file *pFile, int Op, void* pArg)
{
    switch( Op )
    {
        case SQLITE_FCNTL_LOCKSTATE: 
        {
            SQLiteEAFile *pFileEA = (SQLiteEAFile*)pFile;
            *(int*) pArg = pFileEA->LockType;
            return SQLITE_OK;
        }
    }
    sLastError = SQLITE_ERROR;
    return SQLITE_ERROR;
}

// Return the sector size in bytes of the underlying block device for
// the specified file. This is almost always 512 bytes, but may be
// larger for some devices.
//
// SQLite code assumes this function cannot fail. It also assumes that
// if two files are created in the same file-system directory (i.e.
// a database and its journal file) that the sector size will be the
// same for both.
//		sqlite3_file* pFile - file
static int EAWebKitSQLiteVFS_SectorSize(sqlite3_file* pFile)
{
    return SQLITE_DEFAULT_SECTOR_SIZE;
}

// Return a vector of device characteristics.
//		sqlite3_file* pFile - file
static int EAWebKitSQLiteVFS_DeviceCharacteristics(sqlite3_file* pFile)
{
    return 0;
}

// This vector defines all the methods that can operate on an
// sqlite3_file for SQLiteEA systems.
static const sqlite3_io_methods EAWebKitSQLiteVFS_IoMethod = {
    1,                        // iVersion 
    EAWebKitSQLiteVFS_Close,
    EAWebKitSQLiteVFS_Read,
    EAWebKitSQLiteVFS_Write,
    EAWebKitSQLiteVFS_Truncate,
    EAWebKitSQLiteVFS_Sync,
    EAWebKitSQLiteVFS_FileSize,
    EAWebKitSQLiteVFS_Lock,
    EAWebKitSQLiteVFS_Unlock,
    EAWebKitSQLiteVFS_CheckReservedLock,
    EAWebKitSQLiteVFS_FileControl,
    EAWebKitSQLiteVFS_SectorSize,
    EAWebKitSQLiteVFS_DeviceCharacteristics
};


// TODO: this is crude, needs reworked
// Create a temporary file name in zBuf.  zBuf must be big enough to
// hold at pVfs->mxPathname characters.
static int GetTempName(int nBuf, char* pzBuf)
{
    static char zChars[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    char zTempPath[EAWebKitSQLiteVFS_PATH_MAX+1];
    size_t i, j;

    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    fileSystem->GetTempDirectory(zTempPath, EAWebKitSQLiteVFS_PATH_MAX);

    // append the SQLITE prefix for temporary 
    sqlite3_snprintf(nBuf-30, pzBuf, "%setilqs_", zTempPath);

    // append 20 random characters for a file name
    j = strlen(pzBuf);
    sqlite3_randomness(20, &pzBuf[j]);
    for(i=0; i<20; i++, j++)
    {
        pzBuf[j] = (char)zChars[ ((unsigned char)pzBuf[j])%(sizeof(zChars)-1) ];
    }
    pzBuf[j] = 0;

    return SQLITE_OK;
}


// Open a File
static int EAWebKitSQLiteVFS_Open(sqlite3_vfs* pVFS, const char* pzName, sqlite3_file* pFile, int Flags, int *pOutFlags)
{ 
    int fileOpenFlags = 0;
    const char *zUtf8Name = pzName;    // Filename in UTF-8 encoding 
    char zTmpname[EAWebKitSQLiteVFS_PATH_MAX+1];        // Buffer used to create temp filename 
    SQLiteEAFile *pFileEA = (SQLiteEAFile*) (pFile);

    // Build a temp file name if none provided.
    if (!zUtf8Name)
    {    
        GetTempName(EAWebKitSQLiteVFS_PATH_MAX+1, zTmpname);
        zUtf8Name = zTmpname;
    }
        

    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    int createDisposition = EA::WebKit::FileSystem::kCDOOpenExisting;
    if (Flags & SQLITE_OPEN_READONLY)        
    {
        fileOpenFlags = EA::WebKit::FileSystem::kRead;
    }

    if (Flags & SQLITE_OPEN_READWRITE)
    {
        createDisposition = EA::WebKit::FileSystem::kCDOOpenAlways;
        fileOpenFlags = (EA::WebKit::FileSystem::kRead | EA::WebKit::FileSystem::kWrite);
    }
    else if (Flags & SQLITE_OPEN_CREATE)
    {
        createDisposition = EA::WebKit::FileSystem::kCDOOpenAlways;
    }

    EA::WebKit::FileSystem::FileObject fileObject = fileSystem->CreateFileObject();
    bool status = fileSystem->OpenFile(fileObject, zUtf8Name, fileOpenFlags, createDisposition); 
    if (!status)
    {
        fileSystem->DestroyFileObject(fileObject);
        sLastError = SQLITE_CANTOPEN;
        return SQLITE_CANTOPEN;
    }
     
    if( pOutFlags )
    {
        *pOutFlags = SQLITE_OPEN_READONLY;
        if( Flags & SQLITE_OPEN_READWRITE )
        {
            *pOutFlags = SQLITE_OPEN_READWRITE;
        }
    }

    // Setup the file handle to return.
    memset(pFileEA, 0, sizeof(SQLiteEAFile));
    pFileEA->pMethod = &EAWebKitSQLiteVFS_IoMethod;
    pFileEA->fileObject = fileObject;

    // Store request and path to delete on close.
    if( Flags & SQLITE_OPEN_DELETEONCLOSE )
    {
        size_t size = strlen(pzName);
        if (size < EAWebKitSQLiteVFS_PATH_MAX)        
        {
            pFileEA->deleteOnClose = true;
            memcpy(pFileEA->path, pzName, size);
        }
    }

    if( (Flags & (SQLITE_OPEN_READWRITE|SQLITE_OPEN_MAIN_DB)) == (SQLITE_OPEN_READWRITE|SQLITE_OPEN_MAIN_DB) )
    {
        if( !EAWebKitSQLiteVFS_CreateLock(pzName, pFileEA) )
        {
            fileSystem->DestroyFileObject(fileObject);
            sLastError = SQLITE_CANTOPEN;
            return SQLITE_CANTOPEN;
        }
    }
   
#if _DEBUG 
    sOpenCount++;
#endif    

	activeHandles().push_back(pFile);
    return SQLITE_OK;
}
    
// Delete a File
static int EAWebKitSQLiteVFS_Delete(sqlite3_vfs* pVFS, const char* pzName, int SyncDir)
{ 
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();

    // Check first if file even exists since remove file only returns bool.
    bool status = fileSystem->FileExists(pzName);
    if (status)
    {
        status = fileSystem->RemoveFile(pzName);
        if (!status)
        {
            sLastError = SQLITE_IOERR_DELETE; 
            return SQLITE_IOERR_DELETE; 
        }
    }      
    return SQLITE_OK;
}

// TODO: this is crude, needs reworked
// Turn a relative pathname into a full pathname
static int EAWebKitSQLiteVFS_FullPathname(sqlite3_vfs* pVFS, const char* pzName, int nOut, char* pzOut)
{ 
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    EA::WebKit::utf8_t path[EAWebKitSQLiteVFS_PATH_MAX+1];
    fileSystem->GetTempDirectory(path,EAWebKitSQLiteVFS_PATH_MAX);
      
    // See if the name already has the full path.
    int size1 = (int) strlen(path);
    int size2 = (int) strlen(pzName);
    int size = size1 <= size2 ? size1 : size2;
    bool fullPath = true;
    for (int i = 0; i < size; ++i)
    {
        if (path[i] == ':') 
            break;
        if( tolower(path[i]) != tolower(pzName[i]))
        {
            fullPath = false; 
            break;
        }
    }  

    if (fullPath)
    {
        sqlite3_snprintf( nOut, pzOut, "%s", pzName );  
    }
    else
    {
        sqlite3_snprintf( nOut, pzOut, "%s%s%s", path, EA::IO::kFilePathSeparatorString8, pzName );
    }
    return SQLITE_OK;
}   

// Write up to nBuf bytes of randomness into zBuf.
int EAWebKitSQLiteVFS_Randomness(sqlite3_vfs* pVFS, int nByte, char* pzOut)
{ 
    int n=0;
    while( nByte > 0 )
    {
        int copySize;
        double   time = currentTime();
        copySize = sizeof(double);
        if( copySize > nByte )
        {
            copySize = nByte;
        }

        memcpy(&pzOut[n], &time, copySize);
        n += copySize;
        nByte -= copySize;
    }
    return SQLITE_OK;
}


// Sleep for a little while.  Return the amount of time slept.
int EAWebKitSQLiteVFS_Sleep(sqlite3_vfs* pVFS, int Microseconds)
{ 
    EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
    pThread->SleepThread((Microseconds+999)/1000);
    return ((Microseconds+999)/1000)*1000;
}

// Find the current time and write the current time and date as a Julian Day number 
// into *prNow and return 0.  Return 1 if the time and date cannot be found.
int EAWebKitSQLiteVFS_CurrentTime(sqlite3_vfs* pVFS, double* pNow)
{ 
    double time;

    time = currentTime();
    *pNow = time;
    return SQLITE_OK;
}

// This function is called after an error in this VFS, this function should populate the 
// buffer pointed to by zBuff of length nBugg bytes withe a null terminated UTF-8 encoded
// error message describing the last IO to have occurred within the calling thread.
int EAWebKitSQLiteVFS_GetLastError(sqlite3_vfs* pVFS, int nBuff, char* pzBuff)
{ 
    sqlite3_snprintf(nBuff, pzBuff, "OsError 0x%x (%u)", sLastError, sLastError);
    return SQLITE_OK;
}


// Check the existence and status of a file
static int EAWebKitSQLiteVFS_Access(sqlite3_vfs* pVFS, const char* pzName, int Flags, int *pResOut)
{ 
    BOOL result = FALSE;
     
    EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
    bool status = fileSystem->FileExists(pzName);
    if (!status)
    {
    *pResOut = FALSE;
    return SQLITE_OK;
    }

    switch( Flags )
    {
    case SQLITE_ACCESS_EXISTS:
        {
            result = TRUE;
        }
    case SQLITE_ACCESS_READ:
        {
            result = TRUE;
            break;
        }
    case SQLITE_ACCESS_READWRITE:
        {
            result = TRUE;
            break;
        }
    default:
        {
            EAW_ASSERT(!"Invalid Flags argument");
        }
    }
    *pResOut = result;
    return SQLITE_OK;	
}

    
/*
** Initialize and end the operating system interface.
*/
SQLITE_API int sqlite3_os_init(void)
{
	
    // Our VFS implementation structure for SQLiteEA
    static sqlite3_vfs sqlite_EA_VfS = {
        // VFS settings
        1,					// iVersion 
        sizeof(SQLiteEAFile),  // szOsFile 
        EAWebKitSQLiteVFS_PATH_MAX, // mxPathname 
        0,					// pNext 
        "PLATFORM_EA",      // zName 
        0,					// pAppData 

        // File access functions
        EAWebKitSQLiteVFS_Open,          //SQLiteEAOpen,			// xOpen 
        EAWebKitSQLiteVFS_Delete,        //SQLiteEADelete,		// xDelete 
        EAWebKitSQLiteVFS_Access,		// xAccess 
        EAWebKitSQLiteVFS_FullPathname,  //SQLiteEAFullPathname,	// xFullPathname 

        // these four functions are for opening a shared library, finding code entry points
        // with in the library and closing the library.  These are not currently supported!
        0,					// xDlOpen 
        0,					// xDlError 
        0,					// xDlSym 
        0,					// xDlClose 

        // Utility functions
        EAWebKitSQLiteVFS_Randomness,	// xRandomness 
        EAWebKitSQLiteVFS_Sleep,			// xSleep 
        EAWebKitSQLiteVFS_CurrentTime,	// xCurrentTime 
        EAWebKitSQLiteVFS_GetLastError	// xGetLastError 
    };

#if _DEBUG 
    sOpenCount = 0;
#endif

    // Create VFS critSection
    EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
    EA::WebKit::IMutex* pMutex = pThread->CreateAMutex();    
    if (!pMutex)
    {
        sLastError = SQLITE_ERROR;
        return SQLITE_ERROR;        
    }
    else
    {
        // Reset
        sLastError = SQLITE_OK;
    }
    g_FileSystemMutex = pMutex;

    // register our VFS with SQLite
    sqlite3_vfs_register(&sqlite_EA_VfS, 1);
    return SQLITE_OK; 
}
	
SQLITE_API int sqlite3_os_end(void)
{ 
    if (g_FileSystemMutex) 	
    {
        EA::WebKit::IThreadSystem* pThread = EA::WebKit::GetThreadSystem();
        pThread->DestroyAMutex(g_FileSystemMutex);
        g_FileSystemMutex = 0;
    }    
	
	SQLiteFileHandlesList::iterator iter = activeHandles().begin();
	while(iter != activeHandles().end())
	{
		EAWebKitSQLiteVFS_Close((*iter));
		iter = activeHandles().begin(); //Not a typo
	}
	
	activeHandles().clear();
	
    return SQLITE_OK;
}

} //  extern "C"

#endif 
