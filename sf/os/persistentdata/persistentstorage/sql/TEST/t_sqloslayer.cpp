// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32test.h>
#include <bautils.h>
#include <stdlib.h>
#include <string.h>
#include <hal.h>
#include "SqliteSymbian.h"
#ifdef __cplusplus
extern "C" {
#endif
	#include "sqliteInt.h"
	#include "os.h"
#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif
#include "SqliteUtil.h"

///////////////////////////////////////////////////////////////////////////////////////

const char* KSymbianVfsNameZ = "SymbianSql";

RTest TheTest(_L("t_sqloslayer test"));
RFs   TheFs;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestFile1, "c:\\test\\t_sqloslayer.bin");
_LIT(KTestFile3, "c:\\test\\t_sqloslayer.db");
const char* KTestFile1Z = "c:\\test\\t_sqloslayer.bin";
const char* KTestFile2Z = "z:\\test\\TestDb1.db";
const char* KTestFile3Z = "c:\\test\\t_sqloslayer.db";
_LIT(KPrivateDir, "c:\\private\\21F12127\\");
const char* KTestFile4Z = "c:\\test\\t_sqloslayer2.db";

//In order to be able to compile the test, the following variables are defined (used inside the OS porting layer, when _SQLPROFILER macro is defined)
#ifdef _SQLPROFILER
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;
#endif

#ifdef _DEBUG    
//SQLite panic category.
_LIT(KSqlitePanicCategory, "Sqlite");
#endif

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	(void)TheFs.Delete(KTestFile3);
	(void)TheFs.Delete(KTestFile1);
	}

void TestEnvDestroy()
	{
	sqlite3SymbianLibFinalize();
	DeleteTestFiles();
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TestEnvDestroy();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TestEnvDestroy();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
static const TInt KBurstRate = 20;
#endif

static void MarkHandles()
    {
    RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
    }

static void MarkAllocatedCells()
    {
    TheAllocatedCellsCount = User::CountAllocCells();
    }

static void CheckAllocatedCells()
    {
    TInt allocatedCellsCount = User::CountAllocCells();
    TEST2(allocatedCellsCount, TheAllocatedCellsCount);
    }

static void CheckHandles()
    {
    TInt endProcessHandleCount;
    TInt endThreadHandleCount;
    
    RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

    TEST2(TheProcessHandleCount, endProcessHandleCount);
    TEST2(TheThreadHandleCount, endThreadHandleCount);
    }

static void OomPreStep(TInt
#ifdef _DEBUG        
    aFailingAllocationNo
#endif
                      )
    {
    MarkHandles();
    MarkAllocatedCells();
    __UHEAP_MARK;
    __UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, aFailingAllocationNo, KBurstRate);
    }

static void OomPostStep()
    {
    __UHEAP_RESET;
    __UHEAP_MARKEND;
    CheckAllocatedCells();
    CheckHandles();
    }

///////////////////////////////////////////////////////////////////////////////////////

void TestEnvInit()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = sqlite3SymbianLibInit();
	TEST2(err, KErrNone);
	}

TInt CalcMs(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	static TInt freq = 0;
	if(freq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, freq), KErrNone);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
	return us / 1000;
	}
	
///////////////////////////////////////////////////////////////////////////////////////

//Create/open/close/delete a file
void Test1()
	{
	sqlite3_vfs* vfs = sqlite3_vfs_find(KSymbianVfsNameZ);
	TEST(vfs != NULL);

  	sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
	TEST(osFile != NULL);
		
	//Creating a new file
	int outFlags = 0;
	int err = sqlite3OsOpen(vfs, KTestFile1Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, &outFlags);
	TEST2(err, SQLITE_OK);
	TEST(outFlags & SQLITE_OPEN_READWRITE);
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//Opening an existing file for R/W
	err = sqlite3OsOpen(vfs, KTestFile1Z, osFile, SQLITE_OPEN_READWRITE, &outFlags);
	TEST2(err, SQLITE_OK);
	TEST(outFlags & SQLITE_OPEN_READWRITE);
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//Opening a read-only file
	err = sqlite3OsOpen(vfs, KTestFile2Z, osFile, SQLITE_OPEN_READWRITE, &outFlags);
	TEST2(err, SQLITE_OK);
	TEST(outFlags & SQLITE_OPEN_READONLY);
	//Truncate a read-only file
	err = osFile->pMethods->xTruncate(osFile, 0);
	TEST2(err, SQLITE_IOERR);
	//xAccess - read-only file
	int res = 0;
	err = vfs->xAccess(vfs, KTestFile2Z, SQLITE_ACCESS_READ, &res);
	TEST2(err, SQLITE_OK);
	TEST(res != 0);
	//xAccess - invalid request
	res = 0;
	err = vfs->xAccess(vfs, KTestFile2Z, 122, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	//
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//Creating a new file
	err = sqlite3OsOpen(vfs, KTestFile3Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, &outFlags);
	TEST2(err, SQLITE_OK);
	TEST(outFlags & SQLITE_OPEN_READWRITE);
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//Open a file for a read-only access
	err = sqlite3OsOpen(vfs, KTestFile1Z, osFile, SQLITE_OPEN_READONLY, &outFlags);
	TEST2(err, SQLITE_OK);
	TEST(outFlags & SQLITE_OPEN_READONLY);
	err = sqlite3OsWrite(osFile, "1234", 4, 0);
	TEST(err != SQLITE_OK);
	err = sqlite3SymbianLastOsError();
	TEST2(err, KErrAccessDenied);
	err = vfs->xGetLastError(vfs, 0, 0);
	TEST2(err, 0);//Default implementation
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//Delete KTestFile3Z file
	err = sqlite3OsDelete(vfs, KTestFile3Z, 0);
	TEST2(err, SQLITE_OK);
	res = 0;
	err = sqlite3OsAccess(vfs, KTestFile3Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	//Open a file for an exclusive access
	err = sqlite3OsOpen(vfs, KTestFile3Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_DELETEONCLOSE | SQLITE_OPEN_EXCLUSIVE, &outFlags);
	TEST2(err, SQLITE_OK);
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//The file should not exist now
	err = sqlite3OsAccess(vfs, KTestFile3Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	//Open a file for an exclusive access without deleting it after 
	err = sqlite3OsOpen(vfs, KTestFile3Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXCLUSIVE, &outFlags);
	TEST2(err, SQLITE_OK);
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//The file should exist now
	err = sqlite3OsAccess(vfs, KTestFile3Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 1);
	//Delete KTestFile3Z file
	err = sqlite3OsDelete(vfs, KTestFile3Z, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3OsAccess(vfs, KTestFile3Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	//
	User::Free(osFile);
	}

//Read/Write/Seek/Truncate test	
void Test2()
	{
	sqlite3_vfs* vfs = sqlite3_vfs_find(KSymbianVfsNameZ);
	TEST(vfs != NULL);

  	sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
	TEST(osFile != NULL);
	
	//Creating a new file
	int err = sqlite3OsDelete(vfs, KTestFile1Z, 0);
	TEST2(err, SQLITE_OK);
	int res = 0;
	err = sqlite3OsAccess(vfs, KTestFile1Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	err = sqlite3OsOpen(vfs, KTestFile1Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	TEST2(err, SQLITE_OK);
	//Writing at the beginning of the file
	err = sqlite3OsWrite(osFile, "123456", 6, 0);
	TEST2(err, SQLITE_OK);
	//Verify the written data
	char data[20];
	err = sqlite3OsRead(osFile, data, 6, 0);
	TEST2(err, SQLITE_OK);
	err = memcmp(data, "123456", 6);
	TEST2(err, 0);
	//Writing at beyond the end of the file
	err = sqlite3OsWrite(osFile, "abcdefgh", 8, 100);
	TEST2(err, SQLITE_OK);
	//Verify the written data
	err = sqlite3OsRead(osFile, data, 8, 100);
	TEST2(err, SQLITE_OK);
	err = memcmp(data, "abcdefgh", 8);
	TEST2(err, 0);
	//Truncate the file
	err = sqlite3OsTruncate(osFile, 3);
	TEST2(err, SQLITE_OK);
	//Write more data
	err = sqlite3OsWrite(osFile, "xyz", 3, 3);
	TEST2(err, SQLITE_OK);
	//Verify the written data
	err = sqlite3OsRead(osFile, data, 6, 0);
	TEST2(err, SQLITE_OK);
	err = memcmp(data, "123xyz", 6);
	TEST2(err, 0);
	//Check the file size
	TInt64 fileSize = 0;
	err = sqlite3OsFileSize(osFile, &fileSize);
	TEST2(err, SQLITE_OK);
	TEST(fileSize == 6);
	//FileControl - lock type
	int lockType = -1;
	err = osFile->pMethods->xFileControl(osFile, SQLITE_FCNTL_LOCKSTATE, &lockType);
	TEST2(err, SQLITE_OK);
	TEST2(lockType, NO_LOCK);
	//FileControl - set callback - NULL callback
	err = osFile->pMethods->xFileControl(osFile, KSqlFcntlRegisterFreePageCallback, 0);
	TEST2(err, SQLITE_ERROR);
	//FileControl - set callback - invalid callback object
	TSqlFreePageCallback cbck;
	err = osFile->pMethods->xFileControl(osFile, KSqlFcntlRegisterFreePageCallback, &cbck);
	TEST2(err, SQLITE_ERROR);
	//FileControl - invalid op-code
	err = osFile->pMethods->xFileControl(osFile, 90234, 0);
	TEST2(err, SQLITE_ERROR);
	//Close the file
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//
	err = sqlite3OsDelete(vfs, KTestFile1Z, 0);
	TEST2(err, SQLITE_OK);
	User::Free(osFile);
	}

//Miscellaneous tests
void Test3()
	{
	sqlite3_vfs* vfs = sqlite3_vfs_find(KSymbianVfsNameZ);
	TEST(vfs != NULL);
	//Full path name - the full file name is specified
	char fname[KMaxFileName];
	const char* testFileName1 = "c:\\test\\abv.db";
	int err = sqlite3OsFullPathname(vfs, testFileName1, KMaxFileName, fname);
	TEST2(err, SQLITE_OK);
	err = strncasecmp(fname, testFileName1, strlen(testFileName1));
	TEST2(err, 0);
	//Full path name - no drive, the full file name is not specified
	const char* testFileName2 = "abv.db";
	const char* expectedFileName2 = "c:\\private\\21F12127\\abv.db";//"21F12127", the UID of the current test app
	err = sqlite3OsFullPathname(vfs, testFileName2, KMaxFileName, fname);
	TEST2(err, SQLITE_OK);
	err = strncasecmp(fname, expectedFileName2, strlen(expectedFileName2));
	TEST2(err, 0);
	//Full path name - drive present, the full file name is not specified
	const char* testFileName3 = "z:abv.db";
	const char* expectedFileName3 = "z:\\private\\21F12127\\abv.db";//"21F12127", the UID of the current test app
	err = sqlite3OsFullPathname(vfs, testFileName3, KMaxFileName, fname);
	TEST2(err, SQLITE_OK);
	err = strncasecmp(fname, expectedFileName3, strlen(expectedFileName3));
	TEST2(err, 0);
	//Is directory writable - test 1
	int res = 0;
	err = sqlite3OsAccess(vfs, "c:\\test", SQLITE_ACCESS_READWRITE, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 1);
	//Is directory writable - test 2
	err = sqlite3OsAccess(vfs, "c:\\test\\", SQLITE_ACCESS_READWRITE, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 1);
	//Is directory writable - test 3
	//Not a valid test. It is the media that's read only, not the directory.
	//err = sqlite3OsAccess(vfs, "z:\\test\\", SQLITE_ACCESS_READWRITE, &res);
	//TEST2(err, SQLITE_OK);
	//TEST2(res, 0);
	//Random seed
	const int KBufLen = 100;
	char buf[KBufLen];
	err = sqlite3OsRandomness(vfs, KBufLen, buf);
	TEST2(err, KBufLen);
	//Sleep
	TUint32 start = User::FastCounter();
	const TInt KSleepTime = 200000;//us
	(void)sqlite3OsSleep(vfs, KSleepTime);
	TUint32 end = User::FastCounter();
	TInt ms = CalcMs(start, end);
	TheTest.Printf(_L(" === sqlite3OsSleep() time=%d ms\r\n"), ms);
	//TEST(ms >= 80 && ms <= 320);// -60%..+60%
	}

//Lock/unlock tests
void Test5()
	{
	sqlite3_vfs* vfs = sqlite3_vfs_find(KSymbianVfsNameZ);
	TEST(vfs != NULL);

  	sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
	TEST(osFile != NULL);
	
	//Creating a new file
	int res = 0;
	int err = sqlite3OsAccess(vfs, KTestFile1Z, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	err = sqlite3OsOpen(vfs, KTestFile1Z, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	TEST2(err, SQLITE_OK);
	//Lock/unlock
	//SHARED_LOCK
	err = sqlite3OsLock(osFile, SHARED_LOCK);
	TEST2(err, SQLITE_OK);
	err = sqlite3OsCheckReservedLock(osFile, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 0);
	//RESERVED_LOCK
	err = sqlite3OsLock(osFile, RESERVED_LOCK);
	TEST2(err, SQLITE_OK);
	err = sqlite3OsCheckReservedLock(osFile, &res);
	TEST2(err, SQLITE_OK);
	TEST2(res, 1);
	//PENDING_LOCK
	err = sqlite3OsLock(osFile, PENDING_LOCK);
	TEST2(err, SQLITE_OK);
	//EXCLUSIVE_LOCK
	err = sqlite3OsLock(osFile, EXCLUSIVE_LOCK);
	TEST2(err, SQLITE_OK);
	//back to SHARED_LOCK
	err = sqlite3OsLock(osFile, SHARED_LOCK);
	TEST2(err, SQLITE_OK);
	//UNLOCK
	err = sqlite3OsUnlock(osFile, NO_LOCK);
	//Close the file
	err = sqlite3OsClose(osFile);
	TEST2(err, SQLITE_OK);
	//
	err = sqlite3OsDelete(vfs, KTestFile1Z, 0);
	TEST2(err, SQLITE_OK);
	User::Free(osFile);
	}

//sqlite3SymbianLibInit() - OOM test
void sqlite3SymbianLibInitOomTest()
	{
	sqlite3SymbianLibFinalize();
	
	TInt failingAllocNum = 0;
	TInt err = KErrNoMemory;
	while(err == KErrNoMemory)
		{
		OomPreStep(++failingAllocNum);
		err = sqlite3SymbianLibInit();
        sqlite3SymbianLibFinalize();
        OomPostStep();
        if(err != KErrNoMemory)
            {
            TEST2(err, KErrNone);
            }
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== sqlite3SymbianLibInit() OOM test succeeded at allcoation %d\r\n"), failingAllocNum);
	}

//sqlite3SymbianLibInit() - 'File I/O error simulation' test
void sqlite3SymbianLibInitFsErrTest()
	{
	sqlite3SymbianLibFinalize();
	
	TInt sysDrive = static_cast<TInt>(RFs::GetSystemDrive());
	TDriveUnit drvUnit(sysDrive);
	TDriveName drvName = drvUnit.Name();
	
	TFileName path;
	TInt err = TheFs.PrivatePath(path);
	TEST2(err, KErrNone);
	
	TParse privDataCage;
	err = privDataCage.Set(drvName, &path, 0);
	TEST2(err, KErrNone);
	
	err = KErrNotFound;
	TInt cnt = 1;
	for(;err<KErrNone;++cnt)
		{
		for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
			{
			(void)TheFs.RmDir(privDataCage.FullName());
	
			TInt processHandleCnt = 0;
			TInt threadHandleCnt = 0;
			RThread().HandleCount(processHandleCnt, threadHandleCnt);
			TInt allocCellsCnt = User::CountAllocCells();
			
			(void)TheFs.SetErrorCondition(fsError, cnt);
			err = sqlite3SymbianLibInit();
			(void)TheFs.SetErrorCondition(KErrNone);
			
			if(err != KErrNone)
				{
				TInt processHandleCnt2 = 0;
				TInt threadHandleCnt2 = 0;
				RThread().HandleCount(processHandleCnt2, threadHandleCnt2);
				TEST2(processHandleCnt2, processHandleCnt);
				TEST2(threadHandleCnt2, threadHandleCnt);
				TInt allocCellsCnt2 = User::CountAllocCells();
				TEST2(allocCellsCnt2, allocCellsCnt);
				}
			else
				{
				sqlite3SymbianLibFinalize();
				}
			}
		}
	sqlite3SymbianLibFinalize();
	TheTest.Printf(_L("=== sqlite3SymbianLibInit() 'File I/O error simulation' test succeeded at iteration %d\r\n"), cnt);
	}

//If _SQLPROFILER macro is not defined then all profiling PS porting layer functions should return KErrNotSupported.
void ProfilerDisabledTest()
	{
#ifndef _SQLPROFILER
	TInt err = sqlite3SymbianProfilerStart(0);
	TEST2(err, KErrNotSupported);
	
	err = sqlite3SymbianProfilerStop(0);
	TEST2(err, KErrNotSupported);
	
	err = sqlite3SymbianProfilerReset(0);
	TEST2(err, KErrNotSupported);
	
	TBuf8<1> res;
	err = sqlite3SymbianProfilerQuery(0, res);
	TEST2(err, KErrNotSupported);
	res = res;
#else	
	TheTest.Printf(_L("  The _SQLPROFILER macro is defined. The Profliling OS porting layer functions are tested in t_sqlapi2\r\n"));
#endif
	}

void NegativeTest()
	{
	TInt err = sqlite3SymbianLibInit();
	TEST2(err, KErrNone);
	
	sqlite3_vfs* vfs = sqlite3_vfs_find(KSymbianVfsNameZ);
	TEST(vfs != NULL);

  	sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
	TEST(osFile != NULL);
		
	//Creating a new file - the name is too long
	const char* KLongFileNameZ = "c:\\test\\0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789-0123456789.bin";
	int outFlags = 0;
	err = sqlite3OsOpen(vfs, KLongFileNameZ, osFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, &outFlags);
	TEST2(err, SQLITE_CANTOPEN);

	//Open a file - the name contains the '|', but not at position 0 (private database - bad name)
	const char* KBadFileNameZ = "c:\\test\\01|23456.bin";
	err = sqlite3OsOpen(vfs, KBadFileNameZ, osFile, SQLITE_OPEN_READWRITE, &outFlags);
	TEST2(err, SQLITE_CANTOPEN);

	//FullPathName() - the output buffer is too small
	const char* KFileNameZ = "c:\\test\\0123456.bin";
	char buf[5];
	err = vfs->xFullPathname(vfs, KFileNameZ, 5, buf);
	TEST2(err, SQLITE_ERROR);

	//FullPathName() - NULL output buffer
	err = vfs->xFullPathname(vfs, KFileNameZ, 5, NULL);
	TEST2(err, SQLITE_ERROR);

	//FullPathName() - NULL file name
	err = vfs->xFullPathname(vfs, NULL, 5, buf);
	TEST2(err, SQLITE_ERROR);

	//FullPathName() - the file name is too long
	err = vfs->xFullPathname(vfs, KLongFileNameZ, 5, buf);
	TEST2(err, SQLITE_ERROR);

	//FullPathName() - NULL file name
	err = vfs->xFullPathname(vfs, NULL, 5, buf);
	TEST2(err, SQLITE_ERROR);

	//Dellete file - NULL file name
	err = vfs->xDelete(vfs, 0, 0);
	TEST2(err, SQLITE_ERROR);

	//Delete file - the output buffer for the unicode file name is too small	
	err = vfs->xDelete(vfs, KLongFileNameZ, 0);
	TEST2(err, SQLITE_ERROR);

	//Open file - NULL file name - this is a temp file name
	err = sqlite3OsOpen(vfs, NULL, osFile, SQLITE_OPEN_READWRITE, &outFlags);
	TEST2(err, SQLITE_OK);
	err = osFile->pMethods->xClose(osFile);
	TEST2(err, SQLITE_OK);
	
	//xAccess - too long file name
	int res = -1;
	err = vfs->xAccess(vfs, KLongFileNameZ, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_IOERR_ACCESS);

	//xAccess - NULL file name
	err = vfs->xAccess(vfs, 0, SQLITE_ACCESS_EXISTS, &res);
	TEST2(err, SQLITE_IOERR_ACCESS);

	User::Free(osFile);
	}

TInt DoDeleteTempFiles()
	{
    CFileMan* fm = NULL;
    TRAPD(err, fm = CFileMan::NewL(TheFs));
    TEST2(err, KErrNone);
    TFileName path;
    path.Copy(KPrivateDir);
    path.Append(_L("temp\\"));
    path.Append(_L("*.$$$"));
    err = fm->Delete(path);
    delete fm;
    return err;
	}

void VfsOpenTempFileOomTest()
    {
    //Delete all temp files in this test private data cage.
	TInt err = DoDeleteTempFiles();
    TEST(err == KErrNone || err == KErrNotFound);
    
    sqlite3_vfs* vfs = sqlite3_vfs_find(NULL);
    TEST(vfs != NULL);

    sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
    TEST(osFile != NULL);
    
    TheTest.Printf(_L("Iteration: "));
    TInt failingAllocNum = 0;
    err = SQLITE_IOERR_NOMEM;
    while(err == SQLITE_IOERR_NOMEM)
        {
        ++failingAllocNum;
        TheTest.Printf(_L("%d "), failingAllocNum);
        OomPreStep(failingAllocNum);
        int outFlags = 0;
        err = sqlite3OsOpen(vfs, NULL, osFile, SQLITE_OPEN_READWRITE, &outFlags);
        if(err == SQLITE_OK)
            {
			//Since this is a temp file, its creation will be delayed till the first file write operation.
			err = sqlite3OsWrite(osFile, "1234", 4, 0);
			(void)sqlite3OsClose(osFile);
            }
        OomPostStep();
        if(err != SQLITE_OK)
            {
            TEST2(err, SQLITE_IOERR_NOMEM);
            }
        //If the iteration has failed, then no temp file should exist in the test private data cage.
        //If the iteration has succeeded, then sqlite3OsClose() should have deleted the temp file.
        TInt err2 = DoDeleteTempFiles();
        TEST2(err2, KErrNotFound);
        }
    TEST2(err, SQLITE_OK);
    TheTest.Printf(_L("\r\n=== TVfs::Open(<temp file>) OOM test succeeded at allcoation %d\r\n"), failingAllocNum);
    
    User::Free(osFile);
    }

void VfsOpenTempFileFileIoErrTest()
	{
    //Delete all temp files in this test private data cage.
	TInt err = DoDeleteTempFiles();
    TEST(err == KErrNone || err == KErrNotFound);
    
    sqlite3_vfs* vfs = sqlite3_vfs_find(NULL);
    TEST(vfs != NULL);

    sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
    TEST(osFile != NULL);
    
	err = SQLITE_ERROR;
	TInt cnt = 1;
	while(err != SQLITE_OK)
		{
		TInt processHandleCnt = 0;
		TInt threadHandleCnt = 0;
		RThread().HandleCount(processHandleCnt, threadHandleCnt);
		TInt allocCellsCnt = User::CountAllocCells();

        TheTest.Printf(_L("%d "), cnt);
		(void)TheFs.SetErrorCondition(KErrGeneral, cnt);
        int outFlags = 0;
        err = sqlite3OsOpen(vfs, NULL, osFile, SQLITE_OPEN_READWRITE, &outFlags);
        if(err == SQLITE_OK)
            {
			//Since this is a temp file, its creation will be delayed till the first file write operation.
			err = sqlite3OsWrite(osFile, "1234", 4, 0);
			(void)sqlite3OsClose(osFile);
            }
		(void)TheFs.SetErrorCondition(KErrNone);
		if(err != SQLITE_OK)
			{
			TInt processHandleCnt2 = 0;
			TInt threadHandleCnt2 = 0;
			RThread().HandleCount(processHandleCnt2, threadHandleCnt2);
			TEST2(processHandleCnt2, processHandleCnt);
			TEST2(threadHandleCnt2, threadHandleCnt);
			TInt allocCellsCnt2 = User::CountAllocCells();
			TEST2(allocCellsCnt2, allocCellsCnt);
	        ++cnt;
			}
        //If the iteration has failed, then no temp file should exist in the test private data cage.
        //If the iteration has succeeded, then sqlite3OsClose() should have deleted the temp file.
        TInt err2 = DoDeleteTempFiles();
        TEST2(err2, KErrNotFound);
		}
    TEST2(err, SQLITE_OK);
    TheTest.Printf(_L("\r\n=== TVfs::Open(<temp file>) file I/O error simulation test succeeded at iteration %d\r\n"), cnt);
    User::Free(osFile);
	}

void VfsCreateDeleteOnCloseFileOomTest()
    {
    sqlite3_vfs* vfs = sqlite3_vfs_find(NULL);
    TEST(vfs != NULL);

    sqlite3_file* osFile = (sqlite3_file*)User::Alloc(vfs->szOsFile);
    TEST(osFile != NULL);
    
    TheTest.Printf(_L("Iteration: "));
    TInt failingAllocNum = 0;
    TInt err = SQLITE_IOERR_NOMEM;
    while(err == SQLITE_IOERR_NOMEM)
        {
        ++failingAllocNum;
        TheTest.Printf(_L("%d "), failingAllocNum);
        OomPreStep(failingAllocNum);
        int outFlags = 0;
        err = sqlite3OsOpen(vfs, KTestFile4Z, osFile, SQLITE_OPEN_CREATE | SQLITE_OPEN_DELETEONCLOSE, &outFlags);
        if(err == SQLITE_OK)
            {
            err = sqlite3OsClose(osFile);
            }
        OomPostStep();
        if(err != SQLITE_OK)
            {
            TEST2(err, SQLITE_IOERR_NOMEM);
            }
        //Whether the iteration has failed or succeeded, the file should not exist.
        TPtrC8 ptrname((const TUint8*)KTestFile4Z);
        TBuf<50> fname;
        fname.Copy(ptrname);
        TInt err2 = TheFs.Delete(fname);
        TEST2(err2, KErrNotFound);
        }
    TEST2(err, SQLITE_OK);
    TheTest.Printf(_L("\r\n=== TVfs::Open(<delete on close file>) OOM test succeeded at allcoation %d\r\n"), failingAllocNum);
    User::Free(osFile);
    }

///////////////////////////////////////////////////////////////////////////////////////

//Panic thread function. 
//It will cast aData parameter to a TFunctor pointer and call it.
//The expectation is that the called function will panic and kill the panic thread.
TInt ThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TFunctor* obj = reinterpret_cast<TFunctor*> (aData);
	TEST(obj != NULL);
	(*obj)();//call the panic function
	
	delete tc;
	
	return KErrNone;		
	}

//Panic test.
//PanicTest function will create a new thread - panic thread, giving it a pointer to the function which has to
//be executed and the expectation is that the function will panic and kill the panic thread.
//PanicTest function will check the panic thread exit code, exit category and the panic code.
void PanicTest(TFunctor& aFunctor, TExitType aExpectedExitType, const TDesC& aExpectedCategory, TInt aExpectedPanicCode)
	{
	RThread thread;
	_LIT(KThreadName,"OsLayerPanicThread");
	TEST2(thread.Create(KThreadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&aFunctor, EOwnerThread), KErrNone);
	
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling

	TEST2(thread.ExitType(), aExpectedExitType);
	TEST(thread.ExitCategory() == aExpectedCategory);
	TEST2(thread.ExitReason(), aExpectedPanicCode);
	
	CLOSE_AND_WAIT(thread);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Panic test functions    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG    

//Panic when calling COsLayerData::Create() is called and the OS layer data has been already created.
class TOsLayerDataDuplicated : public TFunctor
	{
private:		
	virtual void operator()()
		{
		(void)sqlite3SymbianLibInit();//This should crash the thread in debug mode (because the Os layer
		                              //data was created already in TestEnvInit()).
		}
	};
static TOsLayerDataDuplicated TheOsLayerDataDuplicated;

#endif //_DEBUG

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1650
@SYMTestCaseDesc		SQL, OS porting layer tests.
@SYMTestPriority		High
@SYMTestActions			SQL, OS porting layer tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void DoTests()
	{
	TheTest.Printf(_L("OS porting layer test - create/open/close/delete a file\r\n"));
	Test1();
	TheTest.Printf(_L("OS porting layer test - read/write/seek/truncate\r\n"));
	Test2();
	TheTest.Printf(_L("OS porting layer test - miscellaneous tests\r\n"));
	Test3();
	TheTest.Printf(_L("OS porting layer test - lock/unlock\r\n"));
	Test5();
	TheTest.Printf(_L("OS porting layer test - sqlite3SymbianLibInit() - OOM test\r\n"));
	sqlite3SymbianLibInitOomTest();
	TheTest.Printf(_L("OS porting layer test - sqlite3SymbianLibInit() - 'File I/O error simulation' test\r\n"));
	sqlite3SymbianLibInitFsErrTest();
	TheTest.Printf(_L("OS porting layer test - 'profiler disabled' tests\r\n"));
	ProfilerDisabledTest();
	TheTest.Printf(_L("OS porting layer test - negative tests\r\n"));
	NegativeTest();
    TheTest.Printf(_L("TVfs::Open(<temp file>) OOM test\r\n"));
    VfsOpenTempFileOomTest();
    TheTest.Printf(_L("TVfs::Open(<temp file>) file I/O error simulation test\r\n"));
    VfsOpenTempFileFileIoErrTest();
    TheTest.Printf(_L("TVfs::Open(<'delete on close' file>) OOM test\r\n"));
    VfsCreateDeleteOnCloseFileOomTest();
#ifdef _DEBUG    
	TheTest.Printf(_L("'An attempt to create the OS layer data again' panic\r\n"));
	PanicTest(TheOsLayerDataDuplicated, EExitPanic, KSqlitePanicCategory, ESqliteOsPanicOsLayerDataExists);
#endif //_DEBUG	
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	TestEnvInit();
	DeleteTestFiles();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1650 OS porting layer tests"));
	DoTests();
	TestEnvDestroy();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
