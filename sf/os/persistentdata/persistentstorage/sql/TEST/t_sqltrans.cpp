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

#define UNUSED_VAR(a) (a) = (a)

RTest TheTest(_L("t_sqltrans test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:\\test\\t_sqltrans.db");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName);
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

_LIT8(KTestSql1, "INSERT INTO A(Id) VALUES(1); INSERT INTO A(Id) VALUES(2);");

const TPtrC8 KSqls[] = {KTestSql1()};

static RCriticalSection ThreadCritSect;
static RCriticalSection MainCritSect;

static TInt TheSqlIdx = 0;

_LIT(KPanicCategory, "TransFail");
const TInt KPanicCode = 0x1234;

//Test thread function
TInt ThreadFunc1(void*)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	__ASSERT_ALWAYS(TheSqlIdx >= 0 && TheSqlIdx < (TInt)(sizeof(KSqls) / sizeof(KSqls[0])), User::Invariant());
	const TPtrC8 sql = KSqls[TheSqlIdx];

	//Open test database
	RSqlDatabase db;
	TInt err = db.Open(KTestDbName);
	TTEST2(err, KErrNone);

	RDebug::Print(_L("---:WorkThread: Begin transaction. Exec SQL...\r\n"));
	
	//Begin a transaction
	_LIT8(KBeginTrans, "BEGIN");
	err = db.Exec(KBeginTrans);
	TTEST(err >= 0);	

	//Execute the SQL statement(s)
	err = db.Exec(sql);
	TTEST(err >= 0);	

	RDebug::Print(_L("---:WorkThread: Notify the main thread about the SQL statement execution\r\n"));
	MainCritSect.Signal();

	RDebug::Print(_L("---:WorkThread: Wait for permisson to continue...\r\n"));
	ThreadCritSect.Wait();

	User::SetJustInTime(EFalse);	// disable debugger panic handling

	//Panic current thread without commiting the transaction
	RDebug::Print(_L("---:WorkThread: Panic!\r\n"));
	User::Panic(KPanicCategory, KPanicCode);

	delete tc;	
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1623
@SYMTestCaseDesc		Transaction atomicity test.
						Create a test database with a table. 
						Create a worker thread and make some "insert record" operations in a transaction from 
						that thread. Before commiting the transaction notify the main thread that the
						insert operation completed and wait for a notification from the main thread.
						The main thread notifies the worker thread to panic and checks the test table 
						content. No records should be found there.
@SYMTestPriority		High
@SYMTestActions			Transaction atomicity test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TransactionTest1()
	{
	RDebug::Print(_L("+++:MainThread: Create critical sections\r\n"));
	TEST2(ThreadCritSect.CreateLocal(), KErrNone);
	ThreadCritSect.Wait();
	TEST2(MainCritSect.CreateLocal(), KErrNone);
	MainCritSect.Wait();
	
	RDebug::Print(_L("+++:MainThread: Create test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName);
	TEST2(err, KErrNone);

	RDebug::Print(_L("+++:MainThread: Create a table in the test database\r\n"));
	_LIT8(KCreateSql, "CREATE TABLE A(Id INTEGER)");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);	
	
	db.Close();

	RDebug::Print(_L("+++:MainThread: Create the worker thread\r\n"));
	_LIT(KThreadName, "WorkThrd");
	RThread thread;
	TheSqlIdx = 0;
	TEST2(thread.Create(KThreadName, &ThreadFunc1, 0x2000, 0x1000, 0x10000, NULL, EOwnerProcess), KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();

	RDebug::Print(_L("+++:MainThread: Wait SQL statement(s) to be executed...\r\n"));
	MainCritSect.Wait();

	RDebug::Print(_L("+++:MainThread: Notify the worker thread to panic...\r\n"));
	ThreadCritSect.Signal();
	
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling

	TEST2(thread.ExitType(), EExitPanic);
	TEST2(thread.ExitReason(), KPanicCode);
	
	thread.Close();

	RDebug::Print(_L("+++:MainThread: Check the database content...\r\n"));
	err = db.Open(KTestDbName);
	TEST2(err, KErrNone);
	_LIT8(KSelectSql, "SELECT COUNT(*) AS CNT FROM A");
	RSqlStatement stmt;
	err = stmt.Prepare(db, KSelectSql);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt val = stmt.ColumnInt(0);
	TEST(val == 0);
	stmt.Close();
	db.Close();

	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1624
@SYMTestCaseDesc		Transaction consistency test.
						Create a test database with a table with a field with a CHECK constraint. 
						Try to insert some records in a transaction violating the CHECK constraint.
						The transaction should fail.
						No records should be found in the test table.
@SYMTestPriority		High
@SYMTestActions			Transaction atomicity test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TransactionTest2()
	{
	//Create a test database
	(void)RSqlDatabase::Delete(KTestDbName);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName);
	TEST2(err, KErrNone);

	//Create a test table
	_LIT8(KCreateSql, "CREATE TABLE A(Id INTEGER, CHECK(Id > 10 AND Id <= 20))");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);	

	//Begin a transaction
	_LIT8(KBeginTrans, "BEGIN");
	err = db.Exec(KBeginTrans);
	TEST(err >= 0);	
	
	//Exec SQL, viloate constraint.
	_LIT8(KInsertSql, "INSERT INTO A(Id) VALUES(15); INSERT INTO A(Id) VALUES(38);");
	err = db.Exec(KInsertSql);
	TEST2(err, KSqlErrConstraint);

	//Rollback transaction
	_LIT8(KRollbackTrans, "ROLLBACK");
	err = db.Exec(KRollbackTrans);
	TEST(err >= 0);	

	//Check the database content
	_LIT8(KSelectSql, "SELECT COUNT(*) AS CNT FROM A");
	RSqlStatement stmt;
	err = stmt.Prepare(db, KSelectSql);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt val = stmt.ColumnInt(0);
	TEST2(val, 0);
	stmt.Close();
	
	db.Close();

	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1623 Transaction test 1 "));
	TransactionTest1();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1624 Transaction test 2 "));
	TransactionTest2();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestDir();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
