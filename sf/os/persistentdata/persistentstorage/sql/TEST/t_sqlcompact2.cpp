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

#include <f32file.h>
#include <e32test.h>
#include <hal.h>
#include <stdlib.h>
#include <sqldb.h>
#include "sqlite3.h"
#include "SqliteSymbian.h"
#include "SqlSrvStatementUtil.h"
#include "SqlPanic.h"
#include "SqlCompact.h"
#include "SqlCompactConn.h"
#include "SqlCompactEntry.h"
#include "SqlUtil.h"

const TInt KOperationCount = 20;
const TInt KFreePageThresholdKb = 5;
const TInt KFreePageThreshold = 5;

const TInt KCompactStepInterval = 5;//ms

TSqlCompactSettings TheCompactionSettings;

static RTest 			TheTest(_L ("t_sqlcompact2.exe"));
static CTrapCleanup*	TheTrapCleanup = NULL;
static RFs 				TheFs;
static TBuf<KMaxFileName + 1> TheFileNameZ;
static TBuf8<KMaxFileName + 1> TheFileNameZ8;

const TInt KBlobMaxSize = 1024 * 32;
static TBuf8<KBlobMaxSize> TheBlob;

static sqlite3* 		TheDbHandle = NULL;

_LIT8(KFreePageCountPragma, "PRAGMA freelist_count\x0");

TBuf<256>  TheCmd;
TDriveName TheDriveName;
TParse     TheParse;
TFileName  TheDbName;

class CSqlCompactTestActive;
CSqlCompactTestActive* TheTestActive = NULL;

const TTimeIntervalMicroSeconds32 KInterval(200000);

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
const TInt KBurstRate = 100;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	::CloseDbHandle(TheDbHandle);
	(void)TheFs.Delete(TheDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//////////////////////////////////////////////////////////////////////////////////////////////////

void TestEnvCreate()
	{	
	TInt err = sqlite3SymbianLibInit();
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	TheFs = sqlite3SymbianFs();
	for(TInt i=0;i<('Z'-'A');++i)
		{
		TheFs.CreatePrivatePath(i);
		}
	err = TheFs.MkDir(TheDbName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	DeleteTestFiles();
	}

void TestEnvDestroy()
	{
	DeleteTestFiles();
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MarkHandles()
	{
	RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
	}
	
void MarkAllocatedCells()
	{
	TheAllocatedCellsCount = User::CountAllocCells();
	}

void CheckHandles()
	{
	TInt processHandleCount = 0;
	TInt threadHandleCount = 0;

	RThread().HandleCount(processHandleCount, threadHandleCount);

	TEST(processHandleCount == TheProcessHandleCount);
	TEST(threadHandleCount == TheThreadHandleCount);
	}
	
void CheckAllocatedCells()
	{
	TInt allocatedCellsCount = User::CountAllocCells();
	TEST(allocatedCellsCount == TheAllocatedCellsCount);
	}
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const TDesC& FileNameZ(const TDesC& aFileName)
	{
	TheFileNameZ.Copy(aFileName);
	TheFileNameZ.ZeroTerminate();
	return TheFileNameZ;
	}

const TDesC8& FileNameZ8(const TDesC& aFileName)
	{
	TheFileNameZ8.Copy(aFileName);
	TheFileNameZ8.ZeroTerminate();
	return TheFileNameZ8;
	}

TInt FreePageCount()
	{
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = ::StmtPrepare8(TheDbHandle, KFreePageCountPragma, stmtHandle);
	TEST2(err, KErrNone);
	TEST(stmtHandle != NULL);
	err = ::StmtNext(stmtHandle);
	TEST2(err, KSqlAtRow);
	TInt pageCount = sqlite3_column_int(stmtHandle, 0);
	TEST(pageCount >= 0);
	::FinalizeStmtHandle(stmtHandle);
	return pageCount;
	}

void PrintInfo(TInt aProcessedPages, const TDesC& aMediaTypeName, TUint32 aStartTicks, TUint32 aEndTicks)
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
	TheTest.Printf(_L("####Media type: %S. Processed pages: %d. Ticks: %ld. Execution time: %d ms\r\n"), 
							&aMediaTypeName, aProcessedPages, diffTicks, us / 1000);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////        CSqlCompactTestActive declaration              /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSqlCompactTestActive : public CActive
	{
private:
	enum TCommand
		{
		ECmdInvalidTest,	
		ECmdBeginTest1,
		ECmdEndTest1,
		ECmdBeginTest2,
		ECmdEndTest2,
		ECmdBeginTest3,
		ECmdEndTest3,
		ECmdStopTests	
		};
	
public:
	static void New();
	virtual ~CSqlCompactTestActive();
	void OomTest();
	void FileIoErrTest();
	void PerformanceTest();
	void FreePageUpdateTest();
	
protected:		
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
private:
	CSqlCompactTestActive();
	void Complete(TCommand aNextCommand);
	void Schedule(TCommand aNextCommand, TTimeIntervalMicroSeconds32 aInterval);

	void CreateTestDatabase();
	void CreateTestDatabase2();
	void PrepareDb(TBool aNewDb);
	void InsertTestRecords(TInt aOpCount = KOperationCount);
	void UpdateTestRecords(TInt aOpCount = KOperationCount);
	void DeleteTestRecords(TInt aOpCount = KOperationCount);
	void DeleteTestRecords2();
	void TestEnd();
	
	void UpdateTestBegin();
	void UpdateTestEnd();
	void DeleteTestBegin();
	void DeleteTestEnd();
	void SingleOpCompactTestBegin();
	void SingleOpCompactTestEnd();
	void DoOomTest1();
	void DoOomTest2();
	void DoOomTest3();
	void DoOomTest4();
	void DoOomTest5();

private:
	TInt			iCommand;
	CSqlCompactor*	iCompactor;
	RTimer			iTimer;

	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////        CSqlCompactTestActive implementation             ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSqlCompactTestActive::New()
	{
	TheTestActive = new CSqlCompactTestActive;
	TEST(TheTestActive != NULL);
	}

CSqlCompactTestActive::~CSqlCompactTestActive()
	{
	Cancel();
	iTimer.Close();
	delete iCompactor;
	}
	
void CSqlCompactTestActive::DoCancel()
	{
	iTimer.Cancel();
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrNone);
	}
	
void CSqlCompactTestActive::RunL()
	{
	switch(iCommand)
		{
		case CSqlCompactTestActive::ECmdBeginTest1:
			TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4053 Update test"));
			UpdateTestBegin();
			Schedule(CSqlCompactTestActive::ECmdEndTest1, KInterval);
			break;
		case CSqlCompactTestActive::ECmdEndTest1:
			UpdateTestEnd();
			Complete(CSqlCompactTestActive::ECmdBeginTest2);
			break;
		case CSqlCompactTestActive::ECmdBeginTest2:
			TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4054 Delete test"));
			DeleteTestBegin();
			Schedule(CSqlCompactTestActive::ECmdEndTest2, KInterval);
			break;
		case CSqlCompactTestActive::ECmdEndTest2:
			DeleteTestEnd();
			Complete(CSqlCompactTestActive::ECmdBeginTest3);
			break;
		case CSqlCompactTestActive::ECmdBeginTest3:
			TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4055 Single operation - compaction test"));
			SingleOpCompactTestBegin();
			Schedule(CSqlCompactTestActive::ECmdEndTest3, KInterval);
			break;
		case CSqlCompactTestActive::ECmdEndTest3:
			SingleOpCompactTestEnd();
			Complete(CSqlCompactTestActive::ECmdStopTests);
			break;
		case CSqlCompactTestActive::ECmdStopTests:
			CActiveScheduler::Stop();
			break;
		case CSqlCompactTestActive::ECmdInvalidTest:
		default:
			TEST(0);
			break;
		}
	}

TInt CSqlCompactTestActive::RunError(TInt aError)
	{
	TEST2(aError, KErrNone);
	return aError;
	}

CSqlCompactTestActive::CSqlCompactTestActive() :
	CActive(CActive::EPriorityStandard),
	iCommand(CSqlCompactTestActive::ECmdInvalidTest),
	iCompactor(NULL)
	{
	TInt err = sqlite3_enable_shared_cache(1);
	TEST2(err, SQLITE_OK);
	TRAP(err, iCompactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
	TEST2(err, KErrNone);
	err = iTimer.CreateLocal();
	TEST2(err, KErrNone);
	CActiveScheduler::Add(this);
	Complete(CSqlCompactTestActive::ECmdBeginTest1);
	}

void CSqlCompactTestActive::Complete(CSqlCompactTestActive::TCommand aNextCommand)
	{
	TEST(!IsActive());
	iCommand = aNextCommand;
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrNone);
	SetActive();	
	}

void CSqlCompactTestActive::Schedule(TCommand aNextCommand, TTimeIntervalMicroSeconds32 aInterval)
	{
	TEST(!IsActive());
	iCommand = aNextCommand;
	iTimer.After(iStatus, aInterval);
	TEST2(iStatus.Int(), KRequestPending);
	SetActive();
	}

void CSqlCompactTestActive::CreateTestDatabase()
	{
	TInt err = ::CreateDbHandle8(::FileNameZ8(TheDbName), TheDbHandle);
	TEST2(err, KErrNone);
	_LIT8(KCreateTableSql, "CREATE TABLE A(I INTEGRER, B BLOB)\x0");
	err = ::DbExecStmt8(TheDbHandle, KCreateTableSql);
	TEST2(err, KErrNone);
	}

void CSqlCompactTestActive::InsertTestRecords(TInt aOpCount)
	{
	TheBlob.SetLength(SQLITE_DEFAULT_PAGE_SIZE);
	for(TInt i=0;i<aOpCount;++i)
		{
		_LIT8(KInsertSql, "INSERT INTO A VALUES(%d, :Prm)\x0");
		TBuf8<sizeof(KInsertSql) + 10> sqlBuf1;
		sqlBuf1.Format(KInsertSql, i + 1);
		sqlite3_stmt* stmtHandle = NULL;
		TInt err = ::StmtPrepare8(TheDbHandle, sqlBuf1, stmtHandle);
		TEST2(err, KErrNone);
		TEST(stmtHandle != NULL);
		err = sqlite3_bind_blob(stmtHandle, 1, TheBlob.Ptr(), SQLITE_DEFAULT_PAGE_SIZE, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		err = ::StmtExec(stmtHandle);
		TEST2(err, KErrNone);
		::FinalizeStmtHandle(stmtHandle);
		}
	}

//Creates a test database (with KDbName name). 
void CSqlCompactTestActive::CreateTestDatabase2()
    {
    //Create the database
    const TInt KPageSize = 1024;
    _LIT8(KConfigStr, "page_size=");
    TBuf8<100> config;
    config.Copy(KConfigStr);
    config.AppendNum(KPageSize);
    TInt err = KErrNone;
    err = ::CreateDbHandle8(::FileNameZ8(TheDbName), TheDbHandle);
    TEST2(err, KErrNone);  
    _LIT8(KCreateTableSql, "CREATE TABLE A(I INTEGER, T TEXT)\x0");
    err = ::DbExecStmt8(TheDbHandle, KCreateTableSql);
    TEST2(err, KErrNone);
    }

//Insert 1000 records. The record size is such that there is only two records per page.
void CSqlCompactTestActive::PrepareDb(TBool aDeleteRecords)
    {
    //Insert records
    const TInt KRecordCount = 1000;
    const TInt KTextLen = 400;
    TBuf<KTextLen> TheText;
    TBuf<KTextLen + 100> TheSqlBuf;
    TheText.SetLength(TheText.MaxLength());
    TheText.Fill(TChar('A'));
    for(TInt i=0;i<KRecordCount;++i)
        {
        TheSqlBuf.Format(_L("INSERT INTO A VALUES(%d, '%S')"), i + 1, &TheText);
        _LIT(KZero, "\x0");
        TheSqlBuf.Append(KZero);
        TInt err = ::DbExecStmt16(TheDbHandle, TheSqlBuf);
        TEST2(err, KErrNone);
        }
    if(aDeleteRecords)
        {   
        //Delete all records to make a lot of free pages. 
        _LIT(KDeleteAll, "DELETE FROM A WHERE 1\x0");
        TheSqlBuf = KDeleteAll;
        TInt err = ::DbExecStmt16(TheDbHandle, TheSqlBuf);
        TEST2(err, KErrNone);
        }
    }

void CSqlCompactTestActive::UpdateTestRecords(TInt aOpCount)
	{
	for(TInt i=0;i<aOpCount;++i)
		{
		_LIT8(KUpdateSql, "UPDATE A SET B=x'1122' WHERE I=%d\x0");
		TBuf8<sizeof(KUpdateSql) + 10> sqlBuf2;
		sqlBuf2.Format(KUpdateSql, i + 1);
		TInt err = ::DbExecStmt8(TheDbHandle, sqlBuf2);
		TEST2(err, KErrNone);
		}
	}

void CSqlCompactTestActive::DeleteTestRecords(TInt aOpCount)
	{
	for(TInt i=0;i<aOpCount;++i)
		{
		_LIT8(KDeleteSql, "DELETE FROM A WHERE I=%d\x0");
		TBuf8<sizeof(KDeleteSql) + 10> sqlBuf2;
		sqlBuf2.Format(KDeleteSql, i + 1);
		TInt err = ::DbExecStmt8(TheDbHandle, sqlBuf2);
		TEST2(err, KErrNone);
		}
	}

void CSqlCompactTestActive::DeleteTestRecords2()
	{
	_LIT8(KDeleteSql, "DELETE FROM A\x0");
	TInt err = ::DbExecStmt8(TheDbHandle, KDeleteSql);
	TEST2(err, KErrNone);
	}

void CSqlCompactTestActive::TestEnd()
	{
	TInt freePageCount = ::FreePageCount();
	TEST2(freePageCount, 0);
	iCompactor->ReleaseEntry(TheDbName);
	::CloseDbHandle(TheDbHandle);
	TheDbHandle = NULL;
	(void)TheFs.Delete(TheDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4053
@SYMTestCaseDesc		Background compaction scheduled by a set of UPDATE operations.
						The test uses the server background compaction classes directly.
						The test creates a database, inserts records and updates the records.
						The update operations free enough disk space to kick-off the background compaction.
						The test active object, that simulates the SQL server, is activated and the
						background compaction - executed.
						The test checks at the end that the background compaction really happened - in
						CSqlCompactTestActive::UpdateTestEnd().
@SYMTestPriority		Medium
@SYMTestActions			Background compaction scheduled by a set of UPDATE operations.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CSqlCompactTestActive::UpdateTestBegin()
	{
	CreateTestDatabase();
	TRAPD(err, iCompactor->AddEntryL(TheDbName, TheCompactionSettings));
	TEST2(err, KErrNone);
	InsertTestRecords();
	UpdateTestRecords();
	TInt freePageCount = ::FreePageCount();
	TEST(freePageCount > KFreePageThreshold);
	}

void CSqlCompactTestActive::UpdateTestEnd()
	{
	TestEnd();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4054
@SYMTestCaseDesc		Background compaction scheduled by a set of DELETE operations.
						The test uses the server background compaction classes directly.
						The test creates a database, inserts records and deletes the records.
						The delete operations free enough disk space to kick-off the background compaction.
						The test active object, that simulates the SQL server, is activated and the
						background compaction - executed.
						The test checks at the end that the background compaction really happened - in
						CSqlCompactTestActive::DeleteTestEnd().
@SYMTestPriority		Medium
@SYMTestActions			Background compaction scheduled by a set of DELETE operations.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CSqlCompactTestActive::DeleteTestBegin()
	{
	CreateTestDatabase();
	TRAPD(err, iCompactor->AddEntryL(TheDbName, TheCompactionSettings));
	TEST2(err, KErrNone);
	InsertTestRecords();
	DeleteTestRecords();
	TInt freePageCount = ::FreePageCount();
	TEST(freePageCount >= KFreePageThreshold);
	}
	
void CSqlCompactTestActive::DeleteTestEnd()
	{
	TestEnd();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4055
@SYMTestCaseDesc		Background compaction, initiated by a single operation.
						The test uses the server background compaction classes directly.
						The test creates a database, inserts records and deletes the records using just
						a single DELETE SQL statement.
						The test active object, that simulates the SQL server, schedules 
						CSqlCompactTestActive::SingleOpCompactTestEnd() for execution. The code in
						SingleOpCompactTestEnd() checks that the background compaction has been activated and closes the
						database connection. The "database close" operation should start the compaction
						because the total size of free pages is above the "free pages" threshold (in Kb).
@SYMTestPriority		Medium
@SYMTestActions			Background compaction, initiated by a single operation.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CSqlCompactTestActive::SingleOpCompactTestBegin()
	{
	CreateTestDatabase();
	TRAPD(err, iCompactor->AddEntryL(TheDbName, TheCompactionSettings));
	TEST2(err, KErrNone);
	InsertTestRecords();
	DeleteTestRecords2();
	TInt freePageCount = ::FreePageCount();
	TEST(freePageCount >= KFreePageThreshold);
	}

void CSqlCompactTestActive::SingleOpCompactTestEnd()
	{
	TestEnd();
	}

//Background compaction - OOM test.
//CSqlCompactor::NewL() is the function tested in an OOM simulation loop.
//The expectation is that if the iteration fails with KErrNoMemory, no memory leak will occur and the compactor object won't be created.
void CSqlCompactTestActive::DoOomTest1()
	{
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();
				
		__UHEAP_MARK;
		
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, ++failingAllocationNo, KBurstRate);
		
		CSqlCompactor* compactor = NULL;
		TRAP(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
		
		__UHEAP_RESET;

		if(err == KErrNone)	
			{
			TEST(compactor != NULL);
			delete compactor;
			}
		else
			{
			TEST(!compactor);
			TEST2(err, KErrNoMemory);	
			}
							
		__UHEAP_MARKEND;

		CheckAllocatedCells();	    	
		CheckHandles();	    	
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== CSqlCompactor::NewL() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	}

//Background compaction - OOM test.
//CSqlCompactor::AddEntryL() is the function tested in an OOM simulation loop.
//The expectation is that no memory leak will occur if OOM iteration fails with KErrNoMemory.
//The expectation also is that if the iteration fails with KErrNoMemory, no entry will be added to the compactor.
void CSqlCompactTestActive::DoOomTest2()
	{
	CSqlCompactor* compactor = NULL;
	TRAPD(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
	TEST2(err, KErrNone);
	TEST(compactor != NULL);
	
	err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();
				
		__UHEAP_MARK;
		
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, ++failingAllocationNo, KBurstRate);
		
		TRAP(err, compactor->AddEntryL(TheDbName, TheCompactionSettings));
		
		__UHEAP_RESET;

		if(err == KErrNone)	
			{
			TEST2(compactor->iEntries.Count(), 1);
			compactor->ReleaseEntry(TheDbName);
			compactor->iEntries.Compress();
			}
		else
			{
			TEST2(compactor->iEntries.Count(), 0);
			TEST2(err, KErrNoMemory);	
			}
							
		__UHEAP_MARKEND;

		CheckAllocatedCells();	    	
		CheckHandles();	    	
		}
	delete compactor;
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== CSqlCompactor::AddEntryL() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	}

//Background compaction - OOM test.
//CSqlCompactor::NewL() and CSqlCompactor::AddEntryL() are the functions tested in an OOM simulation loop.
//At the end of the iteration CSqlCompactor::ReleaseEntry() is not called. 
//The CSqlCompactor's destructor should properly release the entry if the compactor and the entry have been created successfully.
//The expectation is that no memory leak will occur if OOM iteration fails with KErrNoMemory.
void CSqlCompactTestActive::DoOomTest3()
	{
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();
				
		__UHEAP_MARK;
		
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, ++failingAllocationNo, KBurstRate);
		
		CSqlCompactor* compactor = NULL;
		TRAP(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
		if(err == KErrNone)
			{
			TRAP(err, (void)compactor->AddEntryL(TheDbName, TheCompactionSettings));
			}
		
		__UHEAP_RESET;

		if(err == KErrNone)	
			{
			TEST(compactor != NULL);
			}
		else
			{
			TEST2(err, KErrNoMemory);	
			}
		delete compactor;
							
		__UHEAP_MARKEND;

		CheckAllocatedCells();	    	
		CheckHandles();	    	
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== CSqlCompactor::NewL()+CSqlCompactor::AddEntryL() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	}

//Background compaction - OOM test.
//The test database is created inside the OOM loop, at the beginning of each OOM iteration. The database has enough free space.
//Then the Compact() method is called. The expectation is that if the iteration fails with KErrNoMemory error, no memory leak will occur.
void CSqlCompactTestActive::DoOomTest4()
	{
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		MarkHandles();
		MarkAllocatedCells();
						
		__UHEAP_MARK;
		
		(void)TheFs.Delete(TheDbName);
		CreateTestDatabase();
		CSqlCompactor* compactor = NULL;
		TRAP(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
		TEST2(err, KErrNone);
		TRAP(err, compactor->AddEntryL(TheDbName, TheCompactionSettings));
		TEST2(err, KErrNone);
		InsertTestRecords();
		DeleteTestRecords();
		TInt freePageCount = ::FreePageCount();
		TEST(freePageCount >= KFreePageThreshold);
		
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, ++failingAllocationNo, KBurstRate);

		CSqlCompactEntry* impl = compactor->iEntries[0];
		err = impl->Compact();
		
		__UHEAP_RESET;

		if(err != KErrNone)	
			{
			TEST2(err, KErrNoMemory);	
			}
		else
			{
			TInt freePageCount2 = ::FreePageCount();
			TEST(freePageCount2 < freePageCount);
			}
		delete compactor;
		::CloseDbHandle(TheDbHandle);
		TheDbHandle = NULL;
							
		__UHEAP_MARKEND;

		CheckAllocatedCells();	    	
		CheckHandles();	    	
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== CSqlCompactEntry::Compact() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	(void)TheFs.Delete(TheDbName);
	}

//Background compaction - OOM test.
//The test database is created outside the OOM loop. The database has enough free space.
//Then the Compact() method is called under OOM simulation. 
//The expectation is that if the iteration fails with KErrNoMemory error, no memory leak will occur and the number of 
//the free pages is the same as it was at the beginning of the OOM iteration.
void CSqlCompactTestActive::DoOomTest5()
	{
	__UHEAP_MARK;
	
	CreateTestDatabase();
	CSqlCompactor* compactor = NULL;
	TRAPD(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
	TEST2(err, KErrNone);
	TRAP(err, compactor->AddEntryL(TheDbName, TheCompactionSettings));
	TEST2(err, KErrNone);
	InsertTestRecords();
	DeleteTestRecords();
	TInt freePageCount = ::FreePageCount();
	TEST(freePageCount >= KFreePageThreshold);
	err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	while(err == KErrNoMemory)
		{
		TInt freePageCount2 = ::FreePageCount();
		
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, ++failingAllocationNo, KBurstRate);

		CSqlCompactEntry* impl = compactor->iEntries[0];
		impl->iPageCount = freePageCount2;
		err = impl->Compact();
		
		__UHEAP_RESET;

		if(err != KErrNone)	
			{
			TEST2(err, KErrNoMemory);	
			TInt freePageCount3 = ::FreePageCount();
			TEST2(freePageCount2, freePageCount3);
			}
	}
	TEST2(err, KErrNone);
	TInt freePageCount4 = ::FreePageCount();
	TEST(freePageCount4 < freePageCount);

	compactor->ReleaseEntry(TheDbName);
	delete compactor;
	::CloseDbHandle(TheDbHandle);
	TheDbHandle = NULL;
	
	__UHEAP_MARKEND;
	
	TheTest.Printf(_L("=== CSqlCompactEntry::Compact()-2 OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	(void)TheFs.Delete(TheDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4050
@SYMTestCaseDesc		Background compaction - OOM tests.
						The test uses directly the SQL server background compaction classes and does OOM tests for:
						creating the database compaction object (CSqlCompactor), adding a new background database connection, 
						calling directly the background compaction method.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction - OOM tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void CSqlCompactTestActive::OomTest()
	{
	CreateTestDatabase();
	::CloseDbHandle(TheDbHandle);
	TheDbHandle = NULL;
	
	DoOomTest1();
	DoOomTest2();
	DoOomTest3();

	(void)TheFs.Delete(TheDbName);
	
	DoOomTest4();
	DoOomTest5();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4051
@SYMTestCaseDesc		Background compaction - file I/O error simulation test.
						The test calls the background compaction method, CSqlCompactEntry::Compact(),
						in a file I/O error simulation loop.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction - file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void CSqlCompactTestActive::FileIoErrTest()
	{
	CreateTestDatabase();
	CSqlCompactor* compactor = NULL;
	TRAPD(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
	TEST2(err, KErrNone);
	TRAP(err, compactor->AddEntryL(TheDbName, TheCompactionSettings));
	TEST2(err, KErrNone);
	InsertTestRecords();
	DeleteTestRecords();
	TInt freePageCount = ::FreePageCount();
	TEST(freePageCount >= KFreePageThreshold);
	err = KErrGeneral;
	TInt ioCounter = 0;
	while(err != KErrNone)
		{
		TInt freePageCount2 = ::FreePageCount();
		if(freePageCount2 == 0)
			{
			err = KErrNone;
			break;	
			}
		
		(void)TheFs.SetErrorCondition(err, ioCounter++);

		CSqlCompactEntry* impl = compactor->iEntries[0];
		impl->iPageCount = freePageCount2;
		err = impl->Compact();
		
		(void)TheFs.SetErrorCondition(KErrNone);

		//check the database free pages count - all bets are off in a case of an I/O error. 
		//The free page count may actually have been reduced.
		TInt freePageCount3 = ::FreePageCount();
		TEST(freePageCount3  <= freePageCount2);
		}
	TEST2(err, KErrNone);
	TInt freePageCount4 = ::FreePageCount();

	compactor->ReleaseEntry(TheDbName);
	delete compactor;
	::CloseDbHandle(TheDbHandle);
	TheDbHandle = NULL;
	
	TheTest.Printf(_L("=== CSqlCompactEntry::Compact() \"file I/O\" error simulation test succeeded at iteration %d, free pages %d ===\r\n"), ioCounter, freePageCount4);
	(void)TheFs.Delete(TheDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4052
@SYMTestCaseDesc		Compaction - performance test.
						The test creates a test database (the default drive is C:, but different drive 
						can be specified as a test argument) and runs a compaction performance test. 
						The performance result is printed out.
@SYMTestPriority		Medium
@SYMTestActions			Compaction - performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CSqlCompactTestActive::PerformanceTest()
	{
	TInt err = TheParse.Set(TheDbName, NULL, NULL);
	TEST2(err, KErrNone);
	TPtrC driveName = TheParse.Drive();
	TEST(driveName.Length() > 0);
	TInt driveNumber = -1;
	err = RFs::CharToDrive(driveName[0], driveNumber);
	TEST2(err, KErrNone);
	TDriveNumber driveNo = static_cast <TDriveNumber> (driveNumber);
	TDriveInfo driveInfo;
	err = TheFs.Drive(driveInfo, driveNo);
	TEST2(err, KErrNone);
	
	_LIT(KType1, "Not present");
	_LIT(KType2, "Unknown");
	_LIT(KType3, "Floppy");
	_LIT(KType4, "Hard disk");
	_LIT(KType5, "CD ROM");
	_LIT(KType6, "RAM disk");
	_LIT(KType7, "Flash");
	_LIT(KType8, "ROM drive");
	_LIT(KType9, "Remote drive");
	_LIT(KType10,"NAND flash");
	_LIT(KType11,"Rotating media");
	TPtrC KMediaTypeNames[] = {KType1(), KType2(), KType3(), KType4(), KType5(), KType6(), KType7(), KType8(), KType9(), KType10(), KType11()};
	TheTest.Printf(_L("Drive: %C: %S. File: \"%S\"\r\n"), 'A' + driveNo, &KMediaTypeNames[driveInfo.iType], &TheDbName);

	(void)TheFs.Delete(TheDbName);
	CreateTestDatabase();
	const TInt KRecCount = 90;
	InsertTestRecords(KRecCount);
	DeleteTestRecords2();
	TInt freePageCount = ::FreePageCount();
	TInt processedPages = 0;
	TheTest.Printf(_L("   Free pages count = %d\r\n"), freePageCount);
	TUint32 start = User::FastCounter();
	err = ::DbCompact(TheDbHandle, KNullDesC, freePageCount, processedPages);
	TUint32 end = User::FastCounter();
	TEST2(err, KErrNone);
	TEST2(processedPages, freePageCount);
	::CloseDbHandle(TheDbHandle);
	TheDbHandle = NULL;
	(void)TheFs.Delete(TheDbName);
	PrintInfo(processedPages, KMediaTypeNames[driveInfo.iType], start, end);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4239
@SYMTestCaseDesc        Free page update test.
                        The test creates a database with some records and deletes them all. The records are inserted such that when
                        they get deleted, it leaves a great deal of free pages.
                        Then the test refill the pages which ware empty. After that, the test call ::DbCompact(...) with the number of free
                        pages previously. The free page count should be updated with "0" since all free pages have been refilled since. 
@SYMTestPriority        Medium
@SYMTestExpectedResults Test must not fail
*/
void CSqlCompactTestActive::FreePageUpdateTest()
    {
     (void)TheFs.Delete(TheDbName);
    
    //Create the database with 1000 records and then delete all of them
    CreateTestDatabase2();
    
    CSqlCompactor* compactor = NULL;
    TRAPD(err, compactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KCompactStepInterval));
    TEST2(err, KErrNone);
    TRAP(err, compactor->AddEntryL(TheDbName, TheCompactionSettings));
    TEST2(err, KErrNone);

    PrepareDb(ETrue);
    TInt freePageCount = ::FreePageCount();
    TEST(freePageCount > KSqlCompactFreePageThresholdKb);
    TheTest.Printf(_L("   Free pages count = %d\r\n"), freePageCount);
  
    //Refill the database
    PrepareDb(EFalse);
   
    CSqlCompactEntry* impl = compactor->iEntries[0];
    impl->iPageCount = freePageCount;
    err = impl->Compact();
    TEST2(err, KErrNone);
    TEST2(impl->iPageCount, 0);

    compactor->ReleaseEntry(TheDbName);
    delete compactor;
    ::CloseDbHandle(TheDbHandle);
    TheDbHandle = NULL;
    (void)TheFs.Delete(TheDbName);
    }

//////////////////////////////////////////////////////////////////////////////////////////////////

void DoTests()
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	TEST(scheduler != NULL);
	CActiveScheduler::Install(scheduler);
	
	CSqlCompactTestActive::New();
	
	TheCompactionSettings.iFreePageThresholdKb = KFreePageThresholdKb;

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4050 \"Out of memory\" test"));
	TheTestActive->OomTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4051 \"File I/O\" error simulation test"));
	TheTestActive->FileIoErrTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4052 Compaction - performance test"));
	TheTestActive->PerformanceTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4239 Free page update test"));
	TheTestActive->FreePageUpdateTest();

	CActiveScheduler::Start();
	
	delete TheTestActive;
	TheTestActive = NULL;
	delete scheduler;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

//Usage: "t_sqlcompact2 [<drive letter>:]"

TInt E32Main()
	{
	TheTest.Title();
	
	TheTrapCleanup = CTrapCleanup::New ();
	__ASSERT_ALWAYS(TheTrapCleanup != NULL, User::Invariant());

	__UHEAP_MARK;

	User::CommandLine(TheCmd);
	TheCmd.TrimAll();
	if(TheCmd.Length() > 0)
		{
		TheDriveName.Copy(TheCmd);
		}

	_LIT(KDbName, "c:\\test\\t_sqlcompact2_1.db");
	TheParse.Set(TheDriveName, &KDbName, 0);
	const TDesC& dbFilePath = TheParse.FullName();
	TheDbName.Copy(dbFilePath);
	
	TestEnvCreate();
	
	DoTests();

	TestEnvDestroy();
	
	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete TheTrapCleanup;

	return KErrNone;
	}
