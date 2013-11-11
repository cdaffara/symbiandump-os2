// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlmulti test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlmulti.db");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine, TBool aPrintThreadName = EFalse)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		if(aPrintThreadName)
			{
			RThread th;
			TName name = th.Name();
			RDebug::Print(_L("*** Thread %S, Line %d\r\n"), &name, aLine);
			}
		else
			{
			RDebug::Print(_L("*** Line %d\r\n"), aLine);
			}
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine, TBool aPrintThreadName = EFalse)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		if(aPrintThreadName)
			{
			RThread th;
			TName name = th.Name();
			RDebug::Print(_L("*** Thread %S, Line %d Expected error: %d, got: %d\r\n"), &name, aLine, aExpected, aValue);
			}
		else
			{
			RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
			}
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)
#define TTEST(arg) ::Check1((arg), __LINE__, ETrue)
#define TTEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__, ETrue)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1612
@SYMTestCaseDesc		Two connections to the same database in the same thread. Create a test database
						and insert some records from both connections. Verify that all records were inserted
						successfully.
@SYMTestPriority		High
@SYMTestActions			Testing SQL engine behaviour when having mutiple connections to the same database
						in the same thread.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TestMultiConnSameThread()
	{
	//Connection 1
	RSqlDatabase db1;
	TInt err = db1.Create(KTestDbName1);
	TEST2(err, KErrNone);

	//Create test database
	RDebug::Print(_L("###Create test database\r\n"));
	_LIT8(KCreateSql, "CREATE TABLE A(Id INTEGER PRIMARY KEY AUTOINCREMENT, Data INTEGER)");
	err = db1.Exec(KCreateSql);
	TEST(err >= 0);
	
	//Connection 2
	RSqlDatabase db2;
	err = db2.Open(KTestDbName1);
	TEST2(err, KErrNone);
	
	//Insert some records using both connections
	RDebug::Print(_L("###Insert some records\r\n"));
	const TInt KRecNum = 100;
	_LIT8(KInsertSql, "INSERT INTO A(Data) VALUES(");
	for(TInt i=0;i<KRecNum;++i)
		{
		TBuf8<100> sql(KInsertSql);
		sql.AppendNum((TInt64)i + 1);
		sql.Append(_L(");"));
		err = (i%2) ? db1.Exec(sql) : db2.Exec(sql);
		if(err < 0)
			{
			TPtrC msg = (i%2) ? db1.LastErrorMessage() : db2.LastErrorMessage();
			RDebug::Print(_L("##Db Error msg: \"%S\"\n\r"), &msg);
			}
		TEST2(err, 1);
		}
		
	//Check the database content
	RDebug::Print(_L("###Check the database content\r\n"));
	_LIT8(KSelectSql, "SELECT * FROM A");
	RSqlStatement stmt;
	err = stmt.Prepare(db1, KSelectSql);
	TEST2(err, KErrNone);
	
	for(TInt j=0;j<KRecNum;++j)
		{
		err = stmt.Next();
		TEST2(err, KSqlAtRow);
		TEST(stmt.AtRow());
		TInt id = stmt.ColumnInt(0);
		TInt data = stmt.ColumnInt(1);
		TEST(id == data);
		}
	
	stmt.Close();

	//Cleanup	
	db2.Close();
	db1.Close();
	RDebug::Print(_L("###Delete the test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////

struct TThreadData
	{
	TInt							iTransType;
	RSqlDatabase::TIsolationLevel	iIsolationLevel;
	TInt							iLowRecNo;
	TInt							iHighRecNo;	
	};

TInt ThreadFunc(void* aData)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	TThreadData* data = static_cast<TThreadData*> (aData);
	TTEST(data != NULL);

	RSqlDatabase db;
	TInt err = db.Open(KTestDbName1);
	TTEST2(err, KErrNone);
	
	err = db.SetIsolationLevel(data->iIsolationLevel);
	TTEST2(err, KErrNone);
		
	if(data->iTransType == 1)
		{
		_LIT8(KBeginTrans, "BEGIN");
		err = db.Exec(KBeginTrans);
		TTEST(err >= 0);
		}

	_LIT8(KInsertSql, "INSERT INTO A(Id) VALUES(");	
	for(TInt id=data->iLowRecNo;id<=data->iHighRecNo;++id)
		{
		TBuf8<128> sql(KInsertSql);
		sql.AppendNum((TInt64)id);
		sql.Append(_L(")"));
		err = KSqlErrBusy;
		const TInt KAttempts = 20;
		for(TInt i=0;i<KAttempts&&err==KSqlErrBusy;++i)
			{
			err = db.Exec(sql);
			if(err == KSqlErrBusy)
				{
				RThread th;
				TName name = th.Name();
				RDebug::Print(_L("!!!Database locked, Thread: %S, Attempt %d, column value %d\r\n"), &name, i + 1, id);
				User::After(1000000);
				}
			}
		TTEST2(err, 1);
		}

	if(data->iTransType == 1)
		{
		_LIT8(KCommitTrans, "COMMIT");
		err = db.Exec(KCommitTrans);
		TTEST(err >= 0);
		}

	db.Close();		
	delete tc;
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1613
@SYMTestCaseDesc		Multiple connections to the same database from different threads. 
						Each thread inserts set of record to the same table. Verify that all expected records
						and their column values meet the expectations.
@SYMTestPriority		High
@SYMTestActions			Testing SQL engine behaviour when having mutiple connections to the same database
						from different threads.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TestMultiConnDiffThread()
	{
	//Create a test database
	RDebug::Print(_L("+++:MainThread: Create test database\r\n"));
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	
	//Create a test table
	RDebug::Print(_L("+++:MainThread: Create a table in the test database\r\n"));
	_LIT8(KCreateSql, "CREATE TABLE A(Id INTEGER PRIMARY KEY)");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);
	
	const TInt KThreadCnt = 4;
	const TInt KRange = 100;
	
	const TInt KIsolationLevelCnt = 2;
	TPtrC KIsolationLevelName[KIsolationLevelCnt] = {_L("Read Uncommitted"), _L("Serializable")};
	const RSqlDatabase::TIsolationLevel KIsolationLevels[KIsolationLevelCnt] = {
											RSqlDatabase::EReadUncommitted, RSqlDatabase::ESerializable};
											
	const TInt KTransTypeCnt = 2;											

	//Do the tests:
	// - doing each per thread database operation in a single transaction;
	// - doing all per thread database operations in a single transaction;
	for(TInt transType=0;transType<KTransTypeCnt;++transType)	
		{
		//For both supported isolation levels: read uncommitted and serializable
		for(TInt isolLevel=0;isolLevel<KIsolationLevelCnt;++isolLevel)
			{
			TInt low = 1;
			TInt high = KRange;
			
			RDebug::Print(_L("+++:MainThread: Test: thread count %d, records %d, trans type %d, isolation level: %S\r\n"), 
									KThreadCnt, KRange, transType, &KIsolationLevelName[isolLevel]);
									
			RThread thread[KThreadCnt];
			TRequestStatus status[KThreadCnt];
			TThreadData	data[KThreadCnt];

			//Create the test threads and run them. Each thread establishes a connection with the test database
			//and attempts to write set of records in the test table.
			TInt j;
			for(j=0;j<KThreadCnt;++j,low=high+1,high+=KRange)
				{
				data[j].iTransType = transType;
				data[j].iIsolationLevel = KIsolationLevels[isolLevel];
				data[j].iLowRecNo = low;
				data[j].iHighRecNo = high;
				
				_LIT(KThreadName,"Thr-");
				TBuf<32> threadName(KThreadName);
				threadName.AppendNum((TInt64)j + 1);
				
				TEST2(thread[j].Create(threadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&data[j], EOwnerThread), KErrNone);
				thread[j].Logon(status[j]);
				TEST2(status[j].Int(), KRequestPending);
				thread[j].Resume();
				}
			
			User::After(2000000);
			//Wait until threads finish the database operations and close them.				
			for(j=0;j<KThreadCnt;++j)
				{
				User::WaitForRequest(status[j]);
				TEST(thread[j].ExitType() != EExitPanic);
				thread[j].Close();
				}

			//Check that all records which are esupposed to be in the database, are there.
			RDebug::Print(_L("+++:MainThread: Check that all records have been written\r\n"));
			_LIT8(KSelectSql1, "SELECT COUNT(*) FROM A;");
			RSqlStatement stmt;
			err = stmt.Prepare(db, KSelectSql1);
			TEST2(err, KErrNone);
			err = stmt.Next();
			TEST2(err, KSqlAtRow);
			TInt cnt = stmt.ColumnInt(0);
			TEST2(cnt, KThreadCnt * KRange);
			stmt.Close();
			
			//Check that all records have expected column values.
			RDebug::Print(_L("+++:MainThread: Check that all records have expected column values\r\n"));
			_LIT8(KSelectSql2, "SELECT * FROM A;");
			err = stmt.Prepare(db, KSelectSql2);
			TEST2(err, KErrNone);
			for(TInt k=0;k<(KThreadCnt*KRange);++k)
				{
				err = stmt.Next();
				TEST2(err, KSqlAtRow);
				TInt val = stmt.ColumnInt(0);
				TEST(val > 0 && val <= (KThreadCnt * KRange));
				}
			stmt.Close();

			//Prepare for the next test run - delete all records.
			RDebug::Print(_L("+++:MainThread: Delete all records\r\n"));
			_LIT8(KDeleteSql, "DELETE FROM A");
			err = db.Exec(KDeleteSql);
			TEST(err >= 0);
			}//end of "for(TInt isolLevel=0;isolLevel<KIsolationLevelCnt;++isolLevel)"
		}//end of "for(TInt transType=0;transType<KTransTypeCnt;++transType)"
		
	db.Close();
	RDebug::Print(_L("+++:MainThread: Delete the test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////

static RCriticalSection UpdateThreadCrS;
static RCriticalSection MainThreadCrS;
const TInt KInitialValue = 10;
const TInt KUpdatedValue = 20;

TInt UpdateThreadFunc(void*)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	RSqlDatabase db;
	TInt err = db.Open(KTestDbName1);
	TTEST2(err, KErrNone);

	RDebug::Print(_L("---:UpdThread: Set the isolation level to \"Read uncommitted\"\r\n"));
	err = db.SetIsolationLevel(RSqlDatabase::EReadUncommitted);
	TTEST2(err, KErrNone);

	RDebug::Print(_L("---:UpdThread: Begin a write transaction\r\n"));
	_LIT8(KBeginTransSql, "BEGIN IMMEDIATE TRANSACTION");
	err = db.Exec(KBeginTransSql);
	TTEST(err >= 0);

	RDebug::Print(_L("---:UpdThread: Update the record\r\n"));
	_LIT8(KUpdateSql, "UPDATE A SET Id = ");
	TBuf8<64> sql(KUpdateSql);
	sql.AppendNum((TInt64)KUpdatedValue);
	err = db.Exec(sql);
	TTEST(err >= 0);

	RDebug::Print(_L("---:UpdThread: Notify the main thread about the update\r\n"));
	MainThreadCrS.Signal();
	
	RDebug::Print(_L("---:UpdThread: Wait for permisson to continue...\r\n"));
	UpdateThreadCrS.Wait();

	RDebug::Print(_L("---:UpdThread: Rollback the update\r\n"));
	_LIT8(KRollBackTransSql, "ROLLBACK TRANSACTION");
	err = db.Exec(KRollBackTransSql);
	TTEST(err >= 0);

	RDebug::Print(_L("---:UpdThread: Notify the main thread about the rollback\r\n"));
	MainThreadCrS.Signal();
	
	db.Close();
	delete tc;
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1614
@SYMTestCaseDesc		Verifying that when having 2 database connections in different threads, both set
						the isolation level to "Read Uncommitted", the reading thread can make "dirty read"
						operations (can read the updated but not committed yet record values made by the
						writing thread).
@SYMTestPriority		High
@SYMTestActions			Testing "Read Uncommitted" database isolation level.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TestIsolationLevel()
	{
	RDebug::Print(_L("+++:MainThread: Create critical sections\r\n"));
	TEST2(UpdateThreadCrS.CreateLocal(), KErrNone);
	UpdateThreadCrS.Wait();
	TEST2(MainThreadCrS.CreateLocal(), KErrNone);
	MainThreadCrS.Wait();
	
	RDebug::Print(_L("+++:MainThread: Create test database\r\n"));
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	RDebug::Print(_L("+++:MainThread: Set the isolation level to \"Read uncommitted\"\r\n"));
	err = db.SetIsolationLevel(RSqlDatabase::EReadUncommitted);
	TEST2(err, KErrNone);
	
	RDebug::Print(_L("+++:MainThread: Create a table in the test database\r\n"));
	_LIT8(KCreateSql, "CREATE TABLE A(Id INTEGER)");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);

	RDebug::Print(_L("+++:MainThread: Insert one record in the table\r\n"));
	_LIT8(KInsertSql, "INSERT INTO A(Id) VALUES(");
	TBuf8<64> sql(KInsertSql);
	sql.AppendNum((TInt64)KInitialValue);
	sql.Append(_L(")"));
	err = db.Exec(sql);
	TEST2(err, 1);

	RDebug::Print(_L("+++:MainThread: Create the \"update\" thread\r\n"));
	_LIT(KThreadName, "UpdTh");
	RThread thread;
	TEST2(thread.Create(KThreadName, &UpdateThreadFunc, 0x2000, 0x1000, 0x10000, NULL, EOwnerThread), KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();

	RDebug::Print(_L("+++:MainThread: Wait for record update completion...\r\n"));
	MainThreadCrS.Wait();

	RDebug::Print(_L("+++:MainThread: Read the record and check the data...\r\n"));
	_LIT8(KSelectSql, "SELECT * FROM A");
	RSqlStatement stmt;
	err = stmt.Prepare(db, KSelectSql);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt val = stmt.ColumnInt(0);
	TEST(val == KUpdatedValue);
	stmt.Close();

	RDebug::Print(_L("+++:MainThread: Notify the update thread that it can rollback\r\n"));
	UpdateThreadCrS.Signal();

	RDebug::Print(_L("+++:MainThread: Wait for  rollback  completion...\r\n"));
	MainThreadCrS.Wait();

	RDebug::Print(_L("+++:MainThread: Read the record and check the data...\r\n"));
	err = stmt.Prepare(db, KSelectSql);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt(0);
	TEST2(val, KInitialValue);
	stmt.Close();

	User::WaitForRequest(status);
	thread.Close();

	db.Close();
	RDebug::Print(_L("+++:MainThread: Delete the test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);

	RDebug::Print(_L("+++:MainThread: Close critical sections\r\n"));
	MainThreadCrS.Close();
	UpdateThreadCrS.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1612 Multiple connections, the same thread "));
	TestMultiConnSameThread();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1613 Multiple connections, different threads "));
	TestMultiConnDiffThread();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1614 Isolation level "));
	TestIsolationLevel();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestDir();
	DeleteTestFiles();
	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
