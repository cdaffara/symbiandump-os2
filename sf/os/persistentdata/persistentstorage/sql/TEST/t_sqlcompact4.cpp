// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sqldb.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "SqliteSymbian.h"
#include "SqlResourceTester.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlcompact4 test"));
TParse TheParse;
TDriveName TheDrive;

RSqlDatabase TheDb;
sqlite3* TheSqliteDb = NULL;
TBuf<256> TheCmd;

const TInt KTextLen = 1000;
const TInt KRecLen = 2000;

TBuf<KTextLen> TheText;
TBuf8<KRecLen> TheSqlQuery;
TBuf8<KRecLen> TheSqlFmt;
TBuf<KTextLen + 50> TheSqlTexLen;

_LIT(KDefaultDriveName, "c:");
_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbTemplate8, "c:\\test\\t_sqlcompact4_tmpl8.dat");
_LIT(KTestDbTemplate16, "c:\\test\\t_sqlcompact4_tmpl16.dat");
_LIT(KDbName, "c:\\test\\t_sqlcompact4_1.db");
_LIT(KDbName2, "c:\\test\\t_sqlcompact4_2.db");
_LIT(KRoDbName, "z:\\test\\testdb1.db");//Created outside the test app
TFileName TheTestDbName;

const TInt KMaxThreadCount = 100;
TInt32 TheTestThreadCount = 8;

const TInt KTestDbPageSize = 1024;

TInt TheOriginalDbSize8 = -1;
TInt TheCompactedDbSize8 = -1;

TInt TheOriginalDbSize16 = -1;
TInt TheCompactedDbSize16 = -1;

//In order to be able to compile the test, the following variables are defined (used inside the OS porting layer, when _SQLPROFILER macro is defined)
#ifdef _SQLPROFILER
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;
#endif

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	if(TheSqliteDb)
		{
		sqlite3_close(TheSqliteDb);
		TheSqliteDb = NULL;
		}
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName2);
	(void)RSqlDatabase::Delete(TheTestDbName);
	(void)RSqlDatabase::Delete(KTestDbTemplate16);
	(void)RSqlDatabase::Delete(KTestDbTemplate8);
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Test failure. Boolean expression evaluates to false.\r\n"));
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestEnv()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	TheParse.Set(TheDrive, &KTestDir, 0);

	err = fs.MkDir(TheParse.DriveAndPath());
	TEST(err == KErrNone || err == KErrAlreadyExists);

	fs.Close();

	sqlite3SymbianLibInit();
	}

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDatabase8()
	{
	TheTest.Printf(_L("Create UTF8 test database: %S\r\n"), &KTestDbTemplate8);
	(void)RSqlDatabase::Delete(KTestDbTemplate8);
	TBuf8<KMaxFileName> fname;
	fname.Copy(KTestDbTemplate8);
	TheSqliteDb = NULL;
	TInt rc = sqlite3_open((const char*)fname.PtrZ(), &TheSqliteDb);
	TEST2(rc, SQLITE_OK);
	TBuf8<100> sql;
	_LIT8(KSql, "PRAGMA page_size=%d\x0");
	sql.Format(KSql, KTestDbPageSize);
	rc = sqlite3_exec(TheSqliteDb, (const char*)sql.Ptr(), 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	rc = sqlite3_exec(TheSqliteDb, "CREATE TABLE A(Id INTEGER,Data BLOB)", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	//Insert records
	rc = sqlite3_exec(TheSqliteDb, "BEGIN", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	TheSqlQuery.Copy(_L8("INSERT INTO A VALUES(%d,x'"));
	for(TInt j=0;j<(KRecLen-50);++j)
		{
		TheSqlQuery.Append(_L8("A"));
		}
	TheSqlQuery.Append(_L8("')"));
	const TInt KRecCount = 100;	
	for(TInt i=0;i<KRecCount;++i)
		{
		TheSqlFmt.Format(TheSqlQuery, i + 1);
		rc = sqlite3_exec(TheSqliteDb, (const char*)TheSqlFmt.PtrZ(), 0, 0, 0);
		TEST2(rc, SQLITE_OK);
		}
	rc = sqlite3_exec(TheSqliteDb, "COMMIT", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	//Free some space
	rc = sqlite3_exec(TheSqliteDb, "DELETE FROM A WHERE Id > 10", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	sqlite3_close(TheSqliteDb);
	TheSqliteDb = NULL;
	}

void CreateTestDatabase16()
	{
	TheTest.Printf(_L("Create UTF16 test database: %S\r\n"), &KTestDbTemplate16);
	(void)RSqlDatabase::Delete(KTestDbTemplate16);
	TBuf<KMaxFileName> fname;
	fname.Copy(KTestDbTemplate16);
	TheSqliteDb = NULL;
	TInt rc = sqlite3_open16(fname.PtrZ(), &TheSqliteDb);
	TEST2(rc, SQLITE_OK);
	TBuf8<100> sql;
	_LIT8(KSql, "PRAGMA page_size=%d\x0");
	sql.Format(KSql, KTestDbPageSize);
	rc = sqlite3_exec(TheSqliteDb, (const char*)sql.Ptr(), 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	rc = sqlite3_exec(TheSqliteDb, "CREATE TABLE A(Id INTEGER,Data BLOB)", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	//Insert records
	rc = sqlite3_exec(TheSqliteDb, "BEGIN", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	TheSqlQuery.Copy(_L8("INSERT INTO A VALUES(%d,x'"));
	for(TInt j=0;j<(KRecLen-50);++j)
		{
		TheSqlQuery.Append(_L8("A"));
		}
	TheSqlQuery.Append(_L8("')"));
	const TInt KRecCount = 100;	
	for(TInt i=0;i<KRecCount;++i)
		{
		TheSqlFmt.Format(TheSqlQuery, i + 1);
		rc = sqlite3_exec(TheSqliteDb, (const char*)TheSqlFmt.PtrZ(), 0, 0, 0);
		TEST2(rc, SQLITE_OK);
		}
	rc = sqlite3_exec(TheSqliteDb, "COMMIT", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	//Free some space
	rc = sqlite3_exec(TheSqliteDb, "DELETE FROM A WHERE Id > 10", 0, 0, 0);
	TEST2(rc, SQLITE_OK);
	sqlite3_close(TheSqliteDb);
	TheSqliteDb = NULL;
	}
	
void CreateDatabase8(const TDesC& aTargetDbName)
	{
	RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);		
	CFileMan* fm = NULL;
	TRAP(err, fm = CFileMan::NewL(fs));
	TEST2(err, KErrNone);
	err = fm->Copy(KTestDbTemplate8, aTargetDbName);
	delete fm;
	fs.Close();
	TEST2(err, KErrNone);
	}

void CreateDatabase16(const TDesC& aTargetDbName)
	{
	RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);		
	CFileMan* fm = NULL;
	TRAP(err, fm = CFileMan::NewL(fs));
	TEST2(err, KErrNone);
	err = fm->Copy(KTestDbTemplate16, aTargetDbName);
	delete fm;
	fs.Close();
	TEST2(err, KErrNone);
	}

void CalculateMaxCompaction8()
	{
	TheTest.Printf(_L("UTF8 test database - calculate max compaction\r\n"));
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase8(TheTestDbName);
	TInt err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	RSqlDatabase::TSize size1;
	err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("UTF8.Database before compaction: size %ld, free space %ld\r\n"), size1.iSize, size1.iFree);
	err = TheDb.Compact(RSqlDatabase::EMaxCompaction);
	TEST2(err, size1.iFree);
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("UTF8.Database after compaction: size %ld, free space %ld\r\n"), size2.iSize, size2.iFree);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	TheOriginalDbSize8 = size1.iSize;
	TheCompactedDbSize8 = size2.iSize;
	TEST(TheOriginalDbSize8 > 0);
	TEST(TheCompactedDbSize8 > 0 && TheCompactedDbSize8 < TheOriginalDbSize8);
	}

void CalculateMaxCompaction16()
	{
	TheTest.Printf(_L("UTF16 test database - calculate max compaction\r\n"));
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase16(TheTestDbName);
	TInt err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	RSqlDatabase::TSize size1;
	err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("UTF16.Database before compaction: size %ld, free space %ld\r\n"), size1.iSize, size1.iFree);
	err = TheDb.Compact(RSqlDatabase::EMaxCompaction);
	TEST2(err, size1.iFree);
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("UTF16.Database after compaction: size %ld, free space %ld\r\n"), size2.iSize, size2.iFree);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	TheOriginalDbSize16 = size1.iSize;
	TheCompactedDbSize16 = size2.iSize;
	TEST(TheOriginalDbSize16 > 0);
	TEST(TheCompactedDbSize16 > 0 && TheCompactedDbSize16 < TheOriginalDbSize16);
	}

///////////////////////////////////////////////////////////////////////////////////////


enum TCompactionType {ESyncCompaction, EAsyncCompaction, EMaxCompactionType};

TInt DoCompact(TCompactionType aType, TInt aSize, const TDesC& aAttachDbName = KNullDesC)
	{
	TInt err = KErrGeneral;
	switch(aType)
		{
		case ESyncCompaction:
			err = TheDb.Compact(aSize, aAttachDbName);
			break;
		case EAsyncCompaction:
			{
			TRequestStatus stat;				
			TheDb.Compact(aSize, stat, aAttachDbName);
			User::WaitForRequest(stat);
			TEST(stat != KRequestPending);
			err = stat.Int();
			break;
			}
		default:
			TEST(0);
			break;	
		}
	return err;
	}

TInt DoManualCompaction(TCompactionType aType, const TDesC& aMainDb, TInt aSize, TBool aRoFlag = EFalse)
	{
	if(!aRoFlag)
		{
		(void)RSqlDatabase::Delete(aMainDb);
		CreateDatabase8(aMainDb);
		}

	TInt err = TheDb.Open(aMainDb);
	TEST2(err, KErrNone);
	
	err = DoCompact(aType, aSize);
		
	TheDb.Close();
	if(!aRoFlag)
		{
		(void)RSqlDatabase::Delete(aMainDb);
		}
	return err;
	}

TInt DoManualCompaction(TCompactionType aType, TInt aSize, const TDesC& aAttachDbName)
	{
	return DoCompact(aType, aSize, aAttachDbName);
	}

void DoManualCompaction(TCompactionType aType, TInt aSize, TInt aCompactedSize)
	{
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase8(TheTestDbName);

	TInt err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	
	err = DoCompact(aType, aSize);
	TEST(err >= 0);
	
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iSize, aCompactedSize);
		
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	}

void DoManualCompaction(TCompactionType aType, TInt aSize, TInt aCompactedSize, const TDesC& aAttachDbName)
	{
	TInt err = DoCompact(aType, aSize, aAttachDbName);
	TEST(err >= 0);
	
	RSqlDatabase::TSize size;
	err = TheDb.Size(size, aAttachDbName);
	TEST2(err, KErrNone);
	TEST2(size.iSize, aCompactedSize);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4064
@SYMTestCaseDesc		Manual compaction - negative tests.
						The test creates a database with a manual compaction mode.
						Then the test executes the following negative tests using both synchronous and
						asynchronous Compact() methods:
							- RSqlDatabase::Compact() called with aSize parameter value = KMinTInt;
							- RSqlDatabase::Compact() called with negative aSize parameter value;
							- RSqlDatabase::Compact() called on a read-only database;
							- RSqlDatabase::Compact() called on an attached read-only database;
							- RSqlDatabase::Compact() called on a nonexisting attached database with very long name;
							- RSqlDatabase::Compact() called with aSize = 0;
							- RSqlDatabase::Compact() called on a read-only database where the version number of symbian_settings table is 3;
@SYMTestPriority		Medium
@SYMTestActions			Manual compaction - negative tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10402
*/
void ManualCompactionNegativeTest()
	{
	for(TInt i=0;i<EMaxCompactionType;++i)
		{
		//Specifying KMaxTInt as aSize argument value.
		TInt err = DoManualCompaction((TCompactionType)i, TheTestDbName, KMaxTInt);
		TEST(err > 0);
		//Specifying KMinTInt as aSize argument value.
		err = DoManualCompaction((TCompactionType)i, TheTestDbName, KMinTInt);
		TEST2(err, KErrArgument);
		//Specifying another negative value as aSize argument value.
		err = DoManualCompaction((TCompactionType)i, TheTestDbName, -357);
		TEST2(err, KErrArgument);
		//Specifying zero as aSize argument value.
		err = DoManualCompaction((TCompactionType)i, TheTestDbName, 0);
		TEST2(err, 0);
		//Read-only database - old format (version 3 of symbian_settings table)
		err = DoManualCompaction((TCompactionType)i, KRoDbName, RSqlDatabase::EMaxCompaction, ETrue);
		TEST2(err, KSqlErrReadOnly);
		//
		(void)RSqlDatabase::Delete(TheTestDbName);
		CreateDatabase16(TheTestDbName);
		err = TheDb.Open(TheTestDbName);
		TEST2(err, KErrNone);
		_LIT(KAttachDbName, "Db");
		//Attached read-only database
		err = TheDb.Attach(KRoDbName, KAttachDbName);
		TEST2(err, KErrNone);
		err = DoManualCompaction((TCompactionType)i, RSqlDatabase::EMaxCompaction, KAttachDbName);
		TEST2(err, KSqlErrReadOnly);
		err = TheDb.Detach(KAttachDbName);
		TEST2(err, KErrNone);
		//Nonexisting attached database 
		err = DoManualCompaction((TCompactionType)i, RSqlDatabase::EMaxCompaction, _L("aaa"));
		TEST2(err, KSqlErrGeneral);
        //Very long name of a  nonexisting attached database 
        TBuf<KMaxFileName + 10> fname;
        fname.SetLength(fname.MaxLength());
        fname.Fill(0xDD);
        err = DoManualCompaction((TCompactionType)i, RSqlDatabase::EMaxCompaction, fname);
        TEST2(err, KErrBadName);
        //Invalid attached database name 
        fname.Copy(_L("c:\\|aaa|.db"));
        err = DoManualCompaction((TCompactionType)i, RSqlDatabase::EMaxCompaction, fname);
        TEST2(err, KSqlErrGeneral);
        //
		TheDb.Close();
		(void)RSqlDatabase::Delete(TheTestDbName);
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4065
@SYMTestCaseDesc		Manual compaction - functional tests.
						The test creates a database with a manual compaction mode.
						Then the test executes the following functional tests using both synchronous and
						asynchronous Compact() methods:
							- RSqlDatabase::Compact() called with aSize parameter value = RSqlDatabase::EMaxCompaction;
							- RSqlDatabase::Compact() called with aSize parameter value = 0. No pages should be removed;
							- RSqlDatabase::Compact() called with aSize parameter value = 1. 1 page should be removed;
							- RSqlDatabase::Compact() called with aSize parameter value = "db page size - 1". 1 page should be removed;
							- RSqlDatabase::Compact() called with aSize parameter value = "db page size * <cnt>". <cnt> pages should be removed;
							- RSqlDatabase::Compact() called with aSize parameter value > the free db space. All free pages should be removed;
						The same functional tests are repeated with an attached database.
@SYMTestPriority		Medium
@SYMTestActions			Manual compaction - functional tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10402
*/
void ManualCompactionTest()
	{
	for(TInt i=0;i<EMaxCompactionType;++i)
		{
		//Calling Compact() with aSize = RSqlDatabase::EMaxCompaction
		DoManualCompaction((TCompactionType)i, RSqlDatabase::EMaxCompaction, TheCompactedDbSize8);
		//Calling Compact() with aSize = 0. 0 pages expected to be removed
		DoManualCompaction((TCompactionType)i, 0, TheOriginalDbSize8);
		//Calling Compact() with aSize = 1. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, 1, TheOriginalDbSize8 - KTestDbPageSize);
		//Calling Compact() with aSize = KTestDbPageSize - 1. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize - 1, TheOriginalDbSize8 - KTestDbPageSize);
		//Calling Compact() with aSize = KTestDbPageSize. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize, TheOriginalDbSize8 - KTestDbPageSize);
		const TInt KPagesCnt1 = 17;
		//Calling Compact() with aSize = KTestDbPageSize * KPagesCnt1. KPagesCnt1 pages expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize * KPagesCnt1, TheOriginalDbSize8 - KTestDbPageSize * KPagesCnt1);
		//Calling Compact() with aSize > TheOriginalDbSize8. All free pages expected to be removed
		DoManualCompaction((TCompactionType)i, TheOriginalDbSize8 + 2000, TheCompactedDbSize8);
		//Attached database
		(void)RSqlDatabase::Delete(KDbName2);
		TInt err = TheDb.Create(KDbName2);
		TEST2(err, KErrNone);
		(void)RSqlDatabase::Delete(TheTestDbName);
		CreateDatabase16(TheTestDbName);
		_LIT(KAttachDbName, "Db");
		err = TheDb.Attach(TheTestDbName, KAttachDbName);
		TEST2(err, KErrNone);
		TInt newDatabaseSize = TheOriginalDbSize16;
		//Calling Compact() with aSize = 0. 0 pages expected to be removed
		DoManualCompaction((TCompactionType)i, 0, newDatabaseSize, KAttachDbName);
		//Calling Compact() with aSize = 1. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, 1, TheOriginalDbSize16 - KTestDbPageSize, KAttachDbName);
		newDatabaseSize -= KTestDbPageSize;
		//Calling Compact() with aSize = KTestDbPageSize - 1. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize - 1, newDatabaseSize - KTestDbPageSize, KAttachDbName);
		newDatabaseSize -= KTestDbPageSize;
		//Calling Compact() with aSize = KTestDbPageSize. 1 page expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize, newDatabaseSize - KTestDbPageSize, KAttachDbName);
		newDatabaseSize -= KTestDbPageSize;
		//Calling Compact() with aSize = KTestDbPageSize * KPagesCnt1. KPagesCnt1 pages expected to be removed
		DoManualCompaction((TCompactionType)i, KTestDbPageSize * KPagesCnt1, newDatabaseSize - KTestDbPageSize * KPagesCnt1, KAttachDbName);
		newDatabaseSize -= KTestDbPageSize * KPagesCnt1;
		//Calling Compact() with aSize > newDatabaseSize. All free pages expected to be removed
		DoManualCompaction((TCompactionType)i, newDatabaseSize + 2000, TheCompactedDbSize16, KAttachDbName);
		//
		err = TheDb.Detach(KAttachDbName);
		TEST2(err, KErrNone);
		TheDb.Close();
		(void)RSqlDatabase::Delete(KDbName2);
		}
	}


enum TSizeTestType {EManualSizeTest, EAutoSizeTest};

void DoCompactionDbSizeTest(TSizeTestType aType)
	{
	(void)RSqlDatabase::Delete(TheTestDbName);
	_LIT8(KConfig1, "compaction=manual");
	_LIT8(KConfig2, "compaction=auto");
	TInt err = TheDb.Create(TheTestDbName, aType == EManualSizeTest ? &KConfig1 : &KConfig2);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(T TEXT)"));
	TEST2(err, 1);
	//
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iFree, 0);
	//
	const TInt KRecCnt = 50;
	for(TInt i=0;i<KRecCnt;++i)
		{
		err = TheDb.Exec(_L("INSERT INTO A VALUES('AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaa')"));
		TEST2(err, 1);
		}
	//
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iFree, 0);
	//
	err = TheDb.Exec(_L("DELETE FROM A WHERE 1"));
	TEST2(err, KRecCnt);
	//
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	if(aType == EManualSizeTest)
		{
		TEST(size.iFree > 0);
		}
	else
		{
		TEST2(size.iFree, 0);
		}
	//
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4066
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) called on a database with manual compaction mode.
						The test creates a database with a manual compaction mode.
						Then the test inserts some records and deletes the records making some free database pages.
						The test calls RSqlDatabase::Size(TSize&) before and after the delete operation and verifies
						that the database file size stays unchanged.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Size(TSize&) called on a database with manual compaction mode.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void ManualCompactionSizeTest()
	{
	DoCompactionDbSizeTest(EManualSizeTest);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////         OOM testing        ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintEndOfOomTest(TInt aFailingAllocationNo)
	{
	TheTest.Printf(_L("=== OOM Test succeeded at heap failure rate of %d ===\r\n"), aFailingAllocationNo);
	}

void SetDbHeapFailure(TInt aFailingAllocationNo)
	{
	const TInt KDelayedDbHeapFailureMask = 0x1000;
	TSqlResourceTester::SetDbHeapFailure(RHeap::EDeterministic | KDelayedDbHeapFailureMask, aFailingAllocationNo);
	}
	
void ResetDbHeapFailure()
	{
	TSqlResourceTester::SetDbHeapFailure(RHeap::ENone, 0);
	}

static TInt TheHandleCount1B;
static TInt TheHandleCount2B;
static TInt TheAllocatedCellsCountB;

void MarkHandles()
	{
	RThread().HandleCount(TheHandleCount1B, TheHandleCount2B);
	}

void CheckHandles()
	{
	TInt endHandleCount1E;
	TInt endHandleCount2E;

	RThread().HandleCount(endHandleCount1E, endHandleCount2E);

	TEST(TheHandleCount1B == endHandleCount1E);
	TEST(TheHandleCount2B == endHandleCount2E);
	}

void MarkAllocatedCells()
	{
	TheAllocatedCellsCountB = User::CountAllocCells();
	}

void CheckAllocatedCells()
	{
	TInt allocatedCellsCountE = User::CountAllocCells();
	TEST(allocatedCellsCountE == TheAllocatedCellsCountB);
	}

typedef void (*TDbFuncPtrL)(const TDesC& aDbName);

void DoManualCompactionOomTest(TDbFuncPtrL aTestFunctionPtrL, const TDesC& aDbFileName, const TDesC& aAttachDbFileName, const TDesC& aDbName)
	{
	const TInt KDoDbOomTestAllocLimitServer = 1000;
	TInt failingAllocation = 0;
	TInt allocation = 0;
	TInt err = KErrNoMemory;
	while(allocation < KDoDbOomTestAllocLimitServer)
		{
		MarkHandles();
		MarkAllocatedCells();
		
		__UHEAP_MARK;

		SetDbHeapFailure(++allocation);

		err = TheDb.Open(aDbFileName);
		TEST2(err, KErrNone);
		if(aAttachDbFileName != KNullDesC)
			{
			TEST(aDbName != KNullDesC);
			err = TheDb.Attach(aAttachDbFileName, aDbName);
			TEST(err == KErrNone || err == KErrNoMemory);
			}
		if(err == KErrNone)
			{
			TRAP(err, (*aTestFunctionPtrL)(aDbName));
			if(err != KErrNoMemory)
				{			
				TEST2(err, KErrNone);
				}
			else
				{
				failingAllocation = allocation;	
				}
			}
			
		ResetDbHeapFailure();

		if(aAttachDbFileName != KNullDesC)
			{
			(void)TheDb.Detach(aDbName);
			}
		TheDb.Close();

		__UHEAP_MARKEND;

		CheckAllocatedCells();	    	
		CheckHandles();	
		}
	TEST2(err, KErrNone);
	PrintEndOfOomTest(failingAllocation + 1);
	}

void OomTest1L(const TDesC&)
	{
	User::LeaveIfError(TheDb.Compact(RSqlDatabase::EMaxCompaction));
	}

void OomTest2L(const TDesC& aDbName)
	{
	TEST(aDbName != KNullDesC);
	User::LeaveIfError(TheDb.Compact(RSqlDatabase::EMaxCompaction, aDbName));
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4068
@SYMTestCaseDesc		RSqlDatabase::Compact() - OOM test.
						The test creates a database with a manual compaction mode.
						Then the test calls Compact() in an OOM loop.
						The same OOM test is repeated for Compact() called an attached database.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Compact() - OOM test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
*/
void ManualCompactionOomTest()
	{
	TheTest.Printf(_L("Main database - manual compaction - OOM test\r\n"));
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase8(TheTestDbName);
	DoManualCompactionOomTest(&OomTest1L, TheTestDbName, KNullDesC, KNullDesC);
	TInt err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iSize, TheCompactedDbSize8);
	TheDb.Close();

	TheTest.Printf(_L("Attached database - manual compaction - OOM test\r\n"));
	(void)RSqlDatabase::Delete(KDbName2);
	err = TheDb.Create(KDbName2);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase16(TheTestDbName);
	DoManualCompactionOomTest(&OomTest2L, KDbName2, TheTestDbName, _L("Db"));
	err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iSize, TheCompactedDbSize16);
	TheDb.Close();

	(void)RSqlDatabase::Delete(KDbName2);
	(void)RSqlDatabase::Delete(TheTestDbName);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4067
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) called on a database with auto compaction mode.
						The test creates a database with an auto compaction mode.
						Then the test inserts some records and deletes the records.
						The test calls RSqlDatabase::Size(TSize&) after the delete operation and verifies
						that the database file does not contain any free pages.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Size(TSize&) called on a database with auto compaction mode.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
                        REQ10400
*/
void AutoCompactionSizeTest()
	{
	DoCompactionDbSizeTest(EAutoSizeTest);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4069
@SYMTestCaseDesc		Background compaction functional test.
						The test executes a 10 iterations loop with a "sleep" time 1000000 us at the beginning.
						The "sleep" time is divided by 2 on each iteration.
						In each iteration the test creates a database with free pages count big enough to kick-off the
						background compaction. Then the test executes enough Exec()s in order to kick-off the background compaction.
						Then the test "sleeps" the calculated "sleep" time and checks after that the database size and free pages
						count and prints them out. After the last iteration the same test is repeated with no "sleep" time.
						The test verifies how the client connection activity affects the possibility of the server to run the
						background compaction. 
@SYMTestPriority		Medium
@SYMTestActions			Background compaction functional test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10407
*/
void BackgroundCompactionTest()
	{
	TInt interval = 1000000;//us
	const TInt KIterationCnt = 10;
	TheTest.Printf(_L("===Sleep after Exec()\r\n"));
	for(TInt i=0;i<KIterationCnt;++i)
		{
		(void)RSqlDatabase::Delete(TheTestDbName);
		CreateDatabase8(TheTestDbName);
		TInt err = TheDb.Open(TheTestDbName);
		TEST2(err, KErrNone);
		RSqlDatabase::TSize size1;
		err = TheDb.Size(size1);
		TEST2(err, KErrNone);
		TheTest.Printf(_L("---------------------------------\r\n"));
		TheTest.Printf(_L("===Sleep time %d ms. Database before background compaction: size %ld, free space %ld\r\n"), interval / 1000, size1.iSize, size1.iFree);
		//Simulate Exec() activities
		for(TInt j=0;j<100;++j)
			{
			err = TheDb.Exec(_L8("SELECT Id FROM A LIMIT 1"));
			TEST(err >= 0);
			}
		User::After(interval);
		RSqlDatabase::TSize size2;
		err = TheDb.Size(size2);
		TEST2(err, KErrNone);
		TheTest.Printf(_L("===Database after background compaction: size %ld, free space %ld\r\n"), size2.iSize, size2.iFree);
		TEST(size2.iSize <= size1.iSize);
		TEST(size2.iFree <= size1.iFree);
		interval /= 2;
		TheDb.Close();
		}
	TheTest.Printf(_L("===No sleep\r\n"));
	(void)RSqlDatabase::Delete(TheTestDbName);
	CreateDatabase8(TheTestDbName);
	TInt err = TheDb.Open(TheTestDbName);
	TEST2(err, KErrNone);
	RSqlDatabase::TSize size1;
	err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Database before background compaction: size %ld, free space %ld\r\n"), size1.iSize, size1.iFree);
	//Simulate Exec() activities
	for(TInt j=0;j<100;++j)
		{
		err = TheDb.Exec(_L8("SELECT Id FROM A LIMIT 1"));
		TEST(err >= 0);
		}
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Database after background compaction: size %ld, free space %ld\r\n"), size2.iSize, size2.iFree);
	TEST(size2.iSize <= size1.iSize);
	TEST(size2.iFree <= size1.iFree);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheTestDbName);
	}

struct TThreadData
	{
	TThreadData(const TDesC& aFileName, TInt aSleepInterval) :
		iDbName(aFileName),
		iSleepInterval(aSleepInterval)
		{
		TInt err = iCritSection.CreateLocal();
		TEST2(err, KErrNone);
		iCritSection.Wait();
		Mem::FillZ(&iSize1, sizeof(iSize1));
		Mem::FillZ(&iSize2, sizeof(iSize2));
		}
	TFileName iDbName;	
	RCriticalSection iCritSection;
	RSqlDatabase::TSize iSize1;
	RSqlDatabase::TSize iSize2;
	TInt	iSleepInterval;
	};

TInt ThreadFunc(void* aPrm)
	{
	TEST(aPrm != NULL);

	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	//Wait for a signal from the main thread
	TThreadData* thrdat = (TThreadData*)aPrm;
	thrdat->iCritSection.Wait();

	RSqlDatabase db;
	TInt err = db.Open(thrdat->iDbName);
	TEST2(err, KErrNone);
	err = db.Size(thrdat->iSize1);
	TEST2(err, KErrNone);
	//Simulate Exec() activities
	for(TInt j=0;j<100;++j)
		{
		err = db.Exec(_L8("SELECT Id FROM A LIMIT 1"));
		TEST(err >= 0);
		if((j % 10) == 0 && thrdat->iSleepInterval > 0)
			{
			User::After(thrdat->iSleepInterval);
			}
		}
	err = db.Size(thrdat->iSize2);
	TEST2(err, KErrNone);
	db.Close();

	delete tc;	
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4070
@SYMTestCaseDesc		Background compaction load test.
						The test runs 8 threads. Each thread connects to a different database.
						Each database has space in the free pages above the "free pages" threshold - 
						the background compaction will be scheduled at the moment when the database is opened.
						Every thread executes some operations on the opened database - that will delay the background compaction.
						After every 10 operations the thread sleeps for a specified interval of a time.
						After all threads complete, the test checks the database size and free pages count and
						prints them out.
						The test verifies the ability of the SQL server to run the background compaction under a load.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction load test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10407
*/
void BackgroundCompactionLoadTest()
	{
	RThread threads[KMaxThreadCount];
	TThreadData* thrdata[KMaxThreadCount] = {NULL};
	TRequestStatus thrstat[KMaxThreadCount];
	
	const TInt KSleepInterval[] = {0, 50000, 100000, 300000, 500000, 800000};//us
	const TInt KTestCnt = sizeof(KSleepInterval) / sizeof(KSleepInterval[0]);
	
	for(TInt k=0;k<KTestCnt;++k)
		{
		TheTest.Printf(_L("=================================================\r\n"));
		TheTest.Printf(_L("===Sleep interval %d ms\r\n"), KSleepInterval[k] / 1000);
		TheTest.Printf(_L("=================================================\r\n"));
		//Create test databases and threads
		for(TInt i=0;i<TheTestThreadCount;++i)
			{
			//Database
			TBuf<16> fname;
			fname.Copy(_L("\\test\\a"));
			fname.AppendNum(i + 1);
			fname.Append(_L(".db"));
			TheParse.Set(TheDrive, &fname, 0);
			(void)RSqlDatabase::Delete(TheParse.FullName());
			CreateDatabase8(TheParse.FullName());
			//Thread data
			thrdata[i] = new TThreadData(TheParse.FullName(), KSleepInterval[k]);
			TEST(thrdata[i] != NULL);
			//Thread
			TBuf<16> thrname;
			thrname.Copy(_L("Thread"));
			thrname.AppendNum(i + 1);
			TInt err = threads[i].Create(thrname, &ThreadFunc, 0x2000, 0x1000, 0x10000, thrdata[i], EOwnerProcess);
			TEST2(err, KErrNone);
			threads[i].Logon(thrstat[i]);
			TEST2(thrstat[i].Int(), KRequestPending);
			threads[i].Resume();
			}
		//Enable the threads
		for(TInt i=0;i<TheTestThreadCount;++i)
			{
			thrdata[i]->iCritSection.Signal();	
			}
		//Wait for cmpletion
		for(TInt i=0;i<TheTestThreadCount;++i)
			{
			User::WaitForRequest(thrstat[i]);
			}
		//Report
		for(TInt i=0;i<TheTestThreadCount;++i)
			{
			TheTest.Printf(_L("===Thread %d, database %S\r\n"), i + 1, &(thrdata[i]->iDbName));
			TheTest.Printf(_L("===Before background compaction: size %6ld, free space %6ld\r\n"), thrdata[i]->iSize1.iSize, thrdata[i]->iSize1.iFree);
			TheTest.Printf(_L("===After background compaction:  size %6ld, free space %6ld\r\n"), thrdata[i]->iSize2.iSize, thrdata[i]->iSize2.iFree);
			TEST(thrdata[i]->iSize2.iSize <= thrdata[i]->iSize1.iSize);
			TEST(thrdata[i]->iSize2.iFree <= thrdata[i]->iSize1.iFree);
			}
		//Destroy
		for(TInt i=0;i<TheTestThreadCount;++i)
			{
			(void)RSqlDatabase::Delete(thrdata[i]->iDbName);
			thrdata[i]->iCritSection.Close();
			delete thrdata[i];
			thrdata[i] = NULL;
			CLOSE_AND_WAIT(threads[i]);
			}
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4071
@SYMTestCaseDesc		Background compaction in a DDL transaction test.
						The test creates a database, begins a transaction that modifies the database structure  
						and executes enough operations in order free enough space to kick-off the background compaction. 
						The test should not report any failures caused by the fact that the main database connection is
						in a DML transaction and at the same time the background connection may try to execute
						a "PRAGMA freelist_count" statement.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction in a DDL transaction test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void BackgroundCompactionInDDLTransactionTest()
	{
	const TInt KOperationCount = 100;
	(void)RSqlDatabase::Delete(KDbName);
	TInt err = TheDb.Create(KDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER, T TEXT)"));
	TEST2(err, 1);
	TheText.SetLength(KTextLen);
	TheText.Fill(TChar('A'));
	for(TInt i=0;i<=KOperationCount;++i)	
		{
		TheSqlTexLen.Format(_L("INSERT INTO A VALUES(%d, '%S')"), i + 1, &TheText);
		err = TheDb.Exec(TheSqlTexLen);
		TEST2(err, 1);
		}
	err = TheDb.Exec(_L("COMMIT"));
	TEST(err >= 0);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName);
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4209
@SYMTestCaseDesc		Corrupted database background compaction test.
						The test creates a database, inserts records, then deletes part of the records.
						The free pages count should be big enough to kick off the background compaction.
						But the database is closed immediatelly and then the db file is corrupted in a such
						way that during the "database open" operation the corruption is not detected.
						But the corruption is detected during the background compaction. The SQL server
						should detect during the compaction that the databas eis corrupted and should
						stop compacting the database (and draining the battery). Unfortunatelly, this 
						cannot be tested automatically, so a breakpoint should be set at the User::After()
						call, and then the SQL server side should be debugged in order to berify that the
						background compaction is really stopped for that database.  
@SYMTestPriority		High
@SYMTestActions			Corrupted database background compaction test.
@SYMTestExpectedResults Test must not fail
@SYMDEF					ou1cimx1#406830
*/
void CorruptedDbBckgCompactionTest()
	{
	//Step 1: Create a database with some records
	const TInt KOperationCount = 100;
	(void)RSqlDatabase::Delete(KDbName);
	TInt err = TheDb.Create(KDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER, T TEXT)"));
	TEST2(err, 1);
	TheText.SetLength(KTextLen);
	TheText.Fill(TChar('A'));
	for(TInt i=0;i<=KOperationCount;++i)	
		{
		TheSqlTexLen.Format(_L("INSERT INTO A VALUES(%d, '%S')"), i + 1, &TheText);
		err = TheDb.Exec(TheSqlTexLen);
		TEST2(err, 1);
		}
	err = TheDb.Exec(_L("COMMIT"));
	TEST(err >= 0);
	//Step 2: Delete some records to free some space
	err = TheDb.Exec(_L("DELETE FROM A WHERE (I % 2) = 0"));
	TEST(err > 0);
	//Step 3: Close the database
	TheDb.Close();
	//Step 4: Corrupt the database
    RFs fs;
	err = fs.Connect();
	TEST2(err, KErrNone);
	RFile file;
	err = file.Open(fs, KDbName, EFileRead | EFileWrite); 
	TEST2(err, KErrNone);
	TInt pos = 5000;
	err = file.Seek(ESeekStart, pos);
	TEST2(err, KErrNone);
	TheSqlQuery.SetLength(1000);
	for(TInt i=0;i<30;++i)
		{
		err = file.Write(TheSqlQuery);
		TEST2(err, KErrNone);
		}
	file.Close();
	//Step 5: Check the background compaction. Wait 10 seconds allowing the SQL server to try to compact the
	//        database. The SQL server should detect that the SQL database is corrupted and should stop trying to
	//        compact the database.
	err = TheDb.Open(KDbName);
	TEST2(err, KErrNone);
	User::After(10000000);
	//
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName);
	}

void DoTestsL()
	{
	CreateTestDatabase8();
	CalculateMaxCompaction8();
	CreateTestDatabase16();
	CalculateMaxCompaction16();

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4064 Manual Compact() - negative tests"));	
	ManualCompactionNegativeTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4065 Manual Compact() tests"));	
	ManualCompactionTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4066 Manual compaction db size test"));	
	ManualCompactionSizeTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4068 Manual compaction - OOM test"));	
	ManualCompactionOomTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4067 Auto compaction db size test"));	
	AutoCompactionSizeTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4069 Background compaction test"));	
	BackgroundCompactionTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4070 Background compaction - load test"));	
	BackgroundCompactionLoadTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4071 Background compaction activated inside a DDL transaction - test"));	
	BackgroundCompactionInDDLTransactionTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4209 Corrupted database background compaction test"));	
	CorruptedDbBckgCompactionTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	TheTest.Printf(_L("Usage:\r\n  t_sqlcompact4 [[drive:] [test thread count]]\r\n"));
	TheDrive.Copy(KDefaultDriveName);
	User::CommandLine(TheCmd);
	TheCmd.TrimAll();
	if(TheCmd.Length() > 0)
		{
		TInt pos = TheCmd.Locate(TChar(' '));
		TheTest(pos > 0);
		TPtrC prm1(TheCmd.Left(pos));
		TPtrC prm2(TheCmd.Mid(pos + 1));
		
		TheDrive.Copy(prm1);
		
		TLex lex(prm2);
		lex.Val(TheTestThreadCount);
		}
	TheParse.Set(TheDrive, &KDbName, 0);
	TheTestDbName.Copy(TheParse.FullName());
	TheTest.Printf(_L("Test database: %S\r\n"), &TheTestDbName);
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	TRAPD(err, DoTestsL());
	DestroyTestEnv();
	TEST2(err, KErrNone);
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
