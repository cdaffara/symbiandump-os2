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
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqlprivcage test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDb1, "c:\\private\\21212124\\t_sqlprivcage_1.db");
_LIT(KTestDb2, "c:\\private\\21212124\\t_sqlprivcage_2.db");
_LIT(KTestDbZ, "z:\\private\\21212124\\t_sqldb1.db");//Created outside this test
_LIT(KTestDb,  "\\private\\21212124\\t_sqlprivcage_3.db");
TParse TheFileNameParse;

static RCriticalSection ThreadCritSect;
static RCriticalSection MainCritSect;

_LIT(KPanicCategory, "TransFail");
const TInt KPanicCode = 1111;

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DeleteTestFiles()
	{
	if(TheFileNameParse.FullName().Length() > 0)
		{
		(void)RSqlDatabase::Delete(TheFileNameParse.FullName());
		}
	(void)RSqlDatabase::Delete(KTestDb2);
	(void)RSqlDatabase::Delete(KTestDb1);
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

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1764
@SYMTestCaseDesc		The test creates a database in the test application's private data cage.
						Then the test does some operations with the created private database:
						create table, insert records, select records, transactions, delete database.
						The tests verifies that the SQL server can create a database in the application's
						private data cage and can operate with the database.
@SYMTestPriority		High
@SYMTestActions			SQL, Private database test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SimpleDbOpTest()
	{
	//Create private database
	RSqlDatabase db;
	TInt err = db.Create(KTestDb1);
	TEST2(err, KErrNone);
	
	//Execute some operations with the private database
	err = db.Exec(_L8("CREATE TABLE A(ID INTEGER)"));
	TEST(err >= 0);	
	err = db.Exec(_L8("INSERT INTO A(ID) VALUES(1);INSERT INTO A(ID) VALUES(2);INSERT INTO A(ID) VALUES(3);"));
	TEST(err >= 0);	

	//Check written records
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 2);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 3);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();

	db.Close();
	//Open private database
	err = db.Open(KTestDb1);
	TEST2(err, KErrNone);

	//Execute a DELETE transaction
	err = db.Exec(_L8("BEGIN IMMEDIATE TRANSACTION"));
	TEST(err >= 0);	

	err = db.Exec(_L8("DELETE FROM A WHERE ID > 1"));
	TEST(err >= 0);	

	err = db.Exec(_L8("COMMIT TRANSACTION"));
	TEST(err >= 0);	

	//Check records left
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();

	//Open private database which is on drive Z and with the same name
	RSqlDatabase db2;
	err = db2.Open(KTestDbZ);
	TEST2(err, KErrNone);
	//An attempt to write to a read-only database
	err = db2.Exec(_L("INSERT INTO A VALUES(6)"));
	TheTest.Printf(_L(" === Read-only private database. RSqlDatabase::Exec() returned err=%d\r\n"), err);
	TEST(err != KErrNone);
	//Check records
	err = stmt.Prepare(db2, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	db2.Close();
	
	db.Close();
	err = RSqlDatabase::Delete(KTestDb1);
	TEST2(err, KErrNone);

	//Create private database on drive different than C:
	for(TInt drvNum=EDriveD;drvNum<=EDriveZ;++drvNum)
		{
		TDriveUnit drvUnit(drvNum);
		TPtrC drvName = drvUnit.Name();
		TheFileNameParse.Set(KTestDb, &drvName, 0);
		//Check if it is possible to create application's private data cage on drvNum drive.	
		err = TheFs.CreatePrivatePath(drvNum);
		if(err == KErrNone || err == KErrAlreadyExists)
			{
			(void)RSqlDatabase::Delete(TheFileNameParse.FullName());
			err = db.Create(TheFileNameParse.FullName());
			if(err == KErrNone)
				{
				//Execute some operations with the private database
				err = db.Exec(_L8("BEGIN IMMEDIATE TRANSACTION"));
				TEST(err >= 0);	
				err = db.Exec(_L8("CREATE TABLE A(ID INTEGER)"));
				TEST(err >= 0);	
				err = db.Exec(_L8("INSERT INTO A(ID) VALUES(1);INSERT INTO A(ID) VALUES(2);INSERT INTO A(ID) VALUES(3);"));
				TEST(err >= 0);	
				err = db.Exec(_L8("COMMIT TRANSACTION"));
				TEST(err >= 0);	
				db.Close();
				err = RSqlDatabase::Delete(TheFileNameParse.FullName());
				TEST2(err, KErrNone);
				break;
				}
			}
		TheFileNameParse.Set(KNullDesC, 0, 0);
		}
		
	//An attempt to create/open "C:[21212122]BBDb2.db" - this test has no enough rights to do that.
	//...open as a non-secure database
	err = db.Open(_L("C:[21212122]BBDb2.db"));
	TEST2(err, KErrPermissionDenied);
	//...create as a non-secure database
	err = db.Create(_L("C:[21212122]BBDb2.db"));
	TEST2(err, KErrArgument);//secure database name, no security policy
    //Very long private database name
    err = db.Create(_L("c:\\private\\21212124\\hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh.db"));
    TEST2(err, KErrBadName);
    //Private database name without drive letter
    err = db.Create(_L("\\private\\21212124\\dbname_no_drive_letter.db"));
    TEST2(err, KErrBadName);
    //Zero length private database name
    err = db.Create(_L(""));
    TEST2(err, KErrBadName);
    //Private database + very long config string
    _LIT8(KVeryLongConfig, "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj");
    err = db.Create(KTestDb2, &KVeryLongConfig);
    TEST2(err, KErrArgument);
	//...create as a secure database
	RSqlSecurityPolicy dbSecurity;
	TSecurityPolicy policy(TSecurityPolicy::EAlwaysPass);
	err = dbSecurity.Create(policy);
	TEST2(err, KErrNone);
	err = db.Create(_L("C:[21212122]BBDb2.db"), dbSecurity);
	TEST2(err, KErrPermissionDenied);
	dbSecurity.Close();

	//An attempt to delete "C:[21212122]BBDb2.db" - this test has no enough rights to do that.
	err = RSqlDatabase::Delete(_L("C:[21212122]BBDb2.db"));
	TEST2(err, KErrPermissionDenied);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1765
@SYMTestCaseDesc		The test creates two databases in the test application's private data cage.
						Then the test inserts some records in both databases using separate RSqlDatabase objects.
						The test closes both databases, then reopens the first one and attaches th second one.
						Again, the test inserts some records in both databases, using single RSqlDatabase object.
						The test reads the inserted records and verifies their column values.
						The tests verifies that the SQL server can create a database in the application's
						private data cage, can operate with the database and can attach private databases.
@SYMTestPriority		High
@SYMTestActions			SQL, Attach private database test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void AttachDbOpTest()
	{
	//Create private database 1
	RSqlDatabase db1;
	TInt err = db1.Create(KTestDb1);
	TEST2(err, KErrNone);
	//Execute some operations with private database 1
	err = db1.Exec(_L8("CREATE TABLE A(ID INTEGER)"));
	TEST(err >= 0);	
	err = db1.Exec(_L8("INSERT INTO A(ID) VALUES(1)"));
	TEST2(err, 1);
	db1.Close();

	//Create private database 2
	RSqlDatabase db2;
	err = db2.Create(KTestDb2);
	TEST2(err, KErrNone);
	//Execute some operations with private database 2
	err = db2.Exec(_L8("CREATE TABLE A(ID INTEGER, T TEXT)"));
	TEST(err >= 0);	
	err = db2.Exec(_L8("INSERT INTO A(ID, T) VALUES(1, 'NAME-NAME-NAME')"));
	TEST2(err, 1);
	db2.Close();

	//Open database 1
	RSqlDatabase db;
	err = db.Open(KTestDb1);
	TEST2(err, KErrNone);
	//An attempt to attach a database with zero length name
    err = db.Attach(_L(""), _L("Db2"));
    TEST2(err, KErrBadName);
    //Attach database 2
	err = db.Attach(KTestDb2, _L("Db2"));
	TEST2(err, KErrNone);

	//Insert some records
	err = db.Exec(_L8("BEGIN IMMEDIATE TRANSACTION"));
	TEST(err >= 0);	
	err = db.Exec(_L8("INSERT INTO Main.A(ID) VALUES(2);INSERT INTO Db2.A(ID, T) VALUES(2, 'AAA');"));
	TEST(err >= 0);	
	err = db.Exec(_L8("COMMIT TRANSACTION"));
	TEST(err >= 0);	
	
	err = db.Detach(_L("Db2"));
	TEST2(err, KErrNone);
	db.Close();
	
	//Verify inserted data in database 2
	err = db.Open(KTestDb2);
	TEST2(err, KErrNone);
	
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	TPtrC text;
	err = stmt.ColumnText(1, text);
	TEST2(err, KErrNone);
	TEST(text == _L("NAME-NAME-NAME"));

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 2);
	err = stmt.ColumnText(1, text);
	TEST2(err, KErrNone);
	TEST(text == _L("AAA"));
	
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	db.Close();
		
    //Open database 1, attach read-only database 2
    err = db.Open(KTestDb1);
    TEST2(err, KErrNone);
    //Make database 2 read-only.
    err = TheFs.SetAtt(KTestDb2, KEntryAttReadOnly, 0);
    TEST2(err, KErrNone);
    //Attach database 2
    err = db.Attach(KTestDb2, _L("Db2"));
    TEST2(err, KErrNone);
    //
    err = db.Exec(_L("INSERT INTO Db2.A(ID, T) VALUES(3, 'AAA')"));
    TPtrC errmsg = db.LastErrorMessage();
    TheTest.Printf(_L(" === Read-only private attached database. Msg=%S, err=%d\r\n"), &errmsg, err);
    TEST(err != KErrNone);
    TSqlScalarFullSelectQuery q(db);
    TBuf<20> text2;
    TRAP(err, q.SelectTextL(_L("SELECT T FROM Db2.A WHERE ID=2"), text2));
    TEST2(err, KErrNone);
    TEST(text2 == _L("AAA"));
    //
    err = db.Detach(_L("Db2"));
    TEST2(err, KErrNone);
    err = TheFs.SetAtt(KTestDb2, 0, KEntryAttReadOnly);
    TEST2(err, KErrNone);
    db.Close();
    
	err = RSqlDatabase::Delete(KTestDb2);
	TEST2(err, KErrNone);
	err = RSqlDatabase::Delete(KTestDb1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1766
@SYMTestCaseDesc		The test creates a database in the test application's private data cage.
						Then the test creates two RSqlDatabase obejcts connecting them to the same 
						private database. The test inserts some records using both connections.
						The test verifies that the inserted records are in the database file and
						verifies the column values.
						The test verifies that it is possible to make more than one connection to the
						same private database and operate with the database using the database connections.
@SYMTestPriority		High
@SYMTestActions			SQL, Two database connections to the same private database test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TwoDbOpTest()
	{
	//Create private database
	RSqlDatabase db1;
	TInt err = db1.Create(KTestDb1);
	TEST2(err, KErrNone);
	
	//Make a second connection with the test database
	RSqlDatabase db2;
	err = db2.Open(KTestDb1);
	TEST2(err, KErrNone);
	
	//Execute some operations with the private database
	err = db1.Exec(_L8("CREATE TABLE A(ID INTEGER)"));
	TEST(err >= 0);	
	err = db1.Exec(_L8("INSERT INTO A(ID) VALUES(1);"));
	TEST2(err, 1);
	err = db2.Exec(_L8("INSERT INTO A(ID) VALUES(2);"));
	TEST2(err, 1);

	//Verify inserted data
	RSqlStatement stmt;
	err = stmt.Prepare(db2, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 2);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();

	db2.Close();
	db1.Close();
	err = RSqlDatabase::Delete(KTestDb1);
	TEST2(err, KErrNone);
	}

//Test thread function.
//The test thread opens a database, begins a transaction and then simulates a crash within the transaction.
TInt ThreadFunc1(void*)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	//Open test database
	RSqlDatabase db;
	TInt err = db.Open(KTestDb1);
	TTEST2(err, KErrNone);

	RDebug::Print(_L("---:WorkThread: Begin transaction. Exec SQL...\r\n"));
	
	//Begin a transaction
	err = db.Exec(_L8("BEGIN IMMEDIATE TRANSACTION"));
	TTEST(err >= 0);	

	//Execute INSERT sql statements
	err = db.Exec(_L8("INSERT INTO A(ID) VALUES(2);INSERT INTO A(ID) VALUES(3);"));
	TTEST(err >= 0);	
	
	RDebug::Print(_L("---:WorkThread: Notify the main thread about the SQL statement execution\r\n"));
	MainCritSect.Signal();

	RDebug::Print(_L("---:WorkThread: Wait for permisson to continue...\r\n"));
	ThreadCritSect.Wait();

	User::SetJustInTime(EFalse);	// disable debugger panic handling

	//Panic current thread without commiting the transaction (crash simulation)
	RDebug::Print(_L("---:WorkThread: Panic!\r\n"));
	User::Panic(KPanicCategory, KPanicCode);

	delete tc;	
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1767
@SYMTestCaseDesc		The test creates a database in the test application's private data cage.
						The test does some operations with the database leaving it in state A.
						Then the test creates a test thread and runs the thread. The test thread
						opens a connection to the database, begins a transaction, inserts some data
						and then simulates a crash within the transaction (kills the thread).
						When the main thread takes the execution control, it reopens the database connection
						and verifies that the database is in the same state A as it was before.
@SYMTestPriority		High
@SYMTestActions			SQL, Private database - transaction recovery test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TransDbOpTest()
	{
	//Create private database
	RSqlDatabase db;
	TInt err = db.Create(KTestDb1);
	TEST2(err, KErrNone);
	
	//Execute some operations with the private database
	err = db.Exec(_L8("CREATE TABLE A(ID INTEGER)"));
	TEST(err >= 0);	
	err = db.Exec(_L8("INSERT INTO A(ID) VALUES(1)"));
	TEST2(err, 1);

	//Check written records
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();

	db.Close();

	//Run a test thread which will begin a transaction and then simulate a crash within the transaction
	TEST2(ThreadCritSect.CreateLocal(), KErrNone);
	ThreadCritSect.Wait();
	TEST2(MainCritSect.CreateLocal(), KErrNone);
	MainCritSect.Wait();
	RDebug::Print(_L("+++:MainThread: Create the worker thread\r\n"));
	_LIT(KThreadName, "WorkThrd");
	RThread thread;
	TEST2(thread.Create(KThreadName, &ThreadFunc1, 0x2000, 0x1000, 0x10000, NULL, EOwnerProcess), KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();
	RDebug::Print(_L("+++:MainThread: Wait SQL statement(s) to be executed...\r\n"));
	MainCritSect.Wait();
	RDebug::Print(_L("+++:MainThread: Notify the worker thread to simulate a crash...\r\n"));
	ThreadCritSect.Signal();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling
	TEST2(thread.ExitType(), EExitPanic);
	TEST2(thread.ExitReason(), KPanicCode);
	thread.Close();
	MainCritSect.Close();
	ThreadCritSect.Close();

	//Reopen the test database. The failed transaction must be rolled back.
	err = db.Open(KTestDb1);
	TEST2(err, KErrNone);
	//Verify that the database content is the same as before the failed transaction
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST2(stmt.ColumnInt(0), 1);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	db.Close();

	err = RSqlDatabase::Delete(KTestDb1);
	TEST2(err, KErrNone);
	}
	
void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1764 Simple private db operations "));
	SimpleDbOpTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1765 Private db operations - attach database "));
	AttachDbOpTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1766 Private db operations - 2 database connections "));
	TwoDbOpTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1767 Private db operations - transaction recovery "));
	TransDbOpTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();
	TheFs.Close();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
