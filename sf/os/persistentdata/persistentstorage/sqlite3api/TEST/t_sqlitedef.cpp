// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32uid.h>
#include <f32file.h>
#include <e32math.h>
#include <sqlite3.h>

#include "sqliteTestUtl.h"

const char* const KTestName = "t_sqlitedef";

static RFs	TheFs;

static sqlite3* TheDb = NULL;
static sqlite3* TheDb2 = NULL;

const char* KTestDir = "c:\\test\\";
const char* KTestDb  = "c:\\test\\t_sqlitedef.db";
const char* KTestDb2  = "c:\\t_sqlitedef.db";

static void DeleteFile(const char* aFileName)
	{
	TFileName fname;
	fname.Copy(TPtrC8((const TUint8*)aFileName));
	(void)TheFs.Delete(fname);
	}

/**
 * Creates the database file and the directory that the test file will be stored.
 */
static void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TestTestLine(err == KErrNone, __LINE__);
	
	err = TheFs.ShareAuto();
	TestTestLine(err == KErrNone,__LINE__);

    TFileName testDir;
    testDir.Copy(TPtrC8((const TUint8*)KTestDir));
	err = TheFs.MkDir(testDir);
	TestTestLine(err == KErrNone || err == KErrAlreadyExists, __LINE__);
	
	TFileName fname;
	fname.Copy(TPtrC8((const TUint8*)KTestDb));
	(void)TheFs.Delete(fname);
	}
/**
 * Closes the database and erases the database file, but not the directory.
 */
static void DestroyTestEnv()
	{
	if(TheDb2)
		{
		(void)sqlite3_close(TheDb2);
		TheDb2 = 0;
		}
	if(TheDb)
		{
		(void)sqlite3_close(TheDb);
		TheDb = 0;
		}
	if(TheFs.Handle() != KNullHandle)
		{
		DeleteFile(KTestDb2);
		DeleteFile(KTestDb);
		}
	TheFs.Close();
	}


///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions

static void PrintErrMsg()
	{
	TBuf<256> buf;
	if(TheDb)
		{
		const char* msg = sqlite3_errmsg(TheDb);	
		buf.Copy(TPtrC8((const TUint8*)msg));
		RDebug::Print(_L("*** Db1 err msg: \"%S\"\r\n"), &buf);
		}
	if(TheDb2)
		{
		const char* msg = sqlite3_errmsg(TheDb2);	
		buf.Copy(TPtrC8((const TUint8*)msg));
		RDebug::Print(_L("*** Db2 err msg: \"%S\"\r\n"), &buf);
		}
	}

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		PrintErrMsg();
		DestroyTestEnv();
		TestTestLine(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		PrintErrMsg();
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TestTestLine(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
//

TInt ThreadFunc(void*)
	{
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

    TInt err = sqlite3_open(KTestDb, &TheDb2); 
	TEST2(err, SQLITE_OK);

    err = sqlite3_exec(TheDb2, "CREATE TABLE A(Id INTEGER,Name TEXT)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
    err = sqlite3_exec(TheDb2, "INSERT INTO A VALUES(1, 'AAA')", 0, 0, 0);
	TEST2(err, SQLITE_OK);

    sqlite3_close(TheDb2);
    TheDb2 = NULL;

	delete tc;	

	return 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4029
@SYMTestCaseDesc		Sqlite file handle test
						The test verifies that a database can be opened from different threads in the same process,
						when the shared page cache is enabled. In this case the database file handle is shared between the
						threads that open the database.
@SYMTestPriority		High
@SYMTestActions			Sqlite file handle test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
void FileHandleTest()
	{
	DeleteFile(KTestDb);
    sqlite3_enable_shared_cache(1);//this is a per-process setting (was per-thread in SQLite 3.3.17)
    TInt err = sqlite3_open(KTestDb, &TheDb); 
	TEST2(err, SQLITE_OK);
    
    err = sqlite3_exec(TheDb, "CREATE TABLE B(Id INTEGER,Name TEXT)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
    err = sqlite3_exec(TheDb, "INSERT INTO B VALUES(1, 'BBB')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	////////////////////////////////////////////////////////////
	// The created thread uses the heap of the creating thread
	// The same SQLite database can be accessed from different threads in
	// shared page cache mode only if the threads share the same heap.
	// The database file handle will be shared between threads.
	////////////////////////////////////////////////////////////
	RDebug::Print(_L("*** Shared heap\r\n"));	
	RThread thr;
    err = thr.Create(_L("TestThr"), &ThreadFunc, KDefaultStackSize, NULL, NULL);
	TEST2(err, KErrNone);
    thr.Resume();
	TRequestStatus stat;
	thr.Logon(stat);
	User::WaitForRequest(stat);
	User::SetJustInTime(ETrue);	// enable debugger panic handling
	
	TInt exitType = thr.ExitType();
	TInt exitReason = thr.ExitReason();
	thr.Close();
	TEST2(exitReason, 0);
	TEST2(exitType, EExitKill);
	////////////////////////////////////////////////////////////
	
    sqlite3_close(TheDb);
    TheDb = NULL;
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////         Sqlite3 DLL OOM test         ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-SQLITE3-CT-4028
@SYMTestCaseDesc		Sqlite OOM test 
						Precondition: none
						A standard OOM test checks the sqlite3 DLL for memory leaks documented 
						on the raised defect, to check if the applied fix is working. Before 
						the fix the test was failing with PANIC USER:84 on the second iteration 
						loop on DoTest()
@SYMTestPriority		Medium
@SYMTestActions			Sqlite OOM test - 
						Opens the database file.						
						Calls sqlite3_prepare16_v2()
						Closes database
						Checks memory leaks
						Repeats the above indefinitely until SQLITE_OK
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF121506
*/
void DEF121506()
	{
	RDebug::Print(_L("Iteration: \r\n"));
	for (TInt it = 1;  ; ++it)
		{
		RDebug::Print(_L("%d "), it);
		TInt c1 = User::CountAllocCells();
 		__UHEAP_SETFAIL(RHeap::EDeterministic, it);
		
 		TInt err = sqlite3_open(KTestDb,&TheDb);

		if(err == SQLITE_OK)
			{
	 		sqlite3_stmt* stmt = 0; 
	 		const void* tail = 0;	
			err = sqlite3_prepare16_v2(TheDb, 
						L"CREATE TABLE Sample(Id INTEGER PRIMARY KEY NOT NULL, Name TEXT NOT NULL UNIQUE COLLATE NOCASE);", 
						-1, &stmt, &tail);	
			(void)sqlite3_finalize(stmt);
			}
 		
		(void)sqlite3_close(TheDb);
		TheDb = NULL;
		__UHEAP_RESET;
		
 		TInt c2 = User::CountAllocCells();
 		if (c1 != c2)
			{	
			RDebug::Print(_L("\r\n*** OOM Test failed\r\n"));
			TEST(EFalse);
			} 		
 		else if (err == SQLITE_OK) 
 			{
 			RDebug::Print(_L("\r\n*** OOM Test passed\r\n"));
 			break; 			
 			}
 		TEST2(err, SQLITE_NOMEM);
 		}	
	}

/**
@SYMTestCaseID			PDS-SQLITE3-CT-4046
@SYMTestCaseDesc		[sqlite3] can't execute sql sequence in transcation. 
@SYMTestPriority		High
@SYMTestActions			The test deletes the test application private data cage.
						Then the test creates a database and attempts to execute a set
						of SQL statements, some of them will need a temporary file to be created.
						Since the test application private data cage (so the session path) does not exist,
						the SQLite OS porting layer will fail to create the requested temporary file and
						will fail with KErrPathNotFound error. 
						The OS porting layer was fixed to create the session path if the temporary file creation error
						is KErrPathNotFound. 
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF140020
*/
void DEF140020()
	{
	//Remove the private data cage
	CFileMan* fm = 0;
	TRAPD(err, fm = CFileMan::NewL(TheFs));
	TEST(fm != 0);
	
	TFileName privatePath;
	err = TheFs.SessionPath(privatePath);
	TEST2(err, KErrNone);
	err = fm->RmDir(privatePath);
	TEST(err == KErrNone || err == KErrPathNotFound);
	
	delete fm;
	fm = 0;

	TEST2((TUint)TheDb, 0);
	err = sqlite3_open(KTestDb2, &TheDb);
	TEST2(err, SQLITE_OK);
	
    const char * stmt[] ={
            "CREATE TABLE fortest (id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, intcol INTEGER NOT NULL, charcol CHAR(255) ) ",
            "INSERT INTO fortest(intcol, charcol) VALUES(1,'111');",
            "BEGIN TRANSACTION;",
            "CREATE TABLE t1_backup(id INTEGER, intcol INTEGER NOT NULL);",
            "INSERT INTO t1_backup SELECT id, intcol FROM fortest;",
            "DROP TABLE fortest;",
            "CREATE TABLE fortest (id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, intcol INTEGER NOT NULL);",
            "INSERT INTO fortest(id, intcol) SELECT id,intcol FROM t1_backup;",
            "DROP TABLE t1_backup;",
            "select count(*) from fortest;",
            "COMMIT;",
            "select count(*) from fortest;",
            "CREATE TABLE t1_backup(id INTEGER, intcol INTEGER NOT NULL);",
            "INSERT INTO t1_backup SELECT id, intcol FROM fortest;",
            "DROP TABLE fortest;",
            "CREATE TABLE fortest (id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, intcol INTEGER NOT NULL);",
            "INSERT INTO fortest(id, intcol) SELECT id,intcol FROM t1_backup;",
            "DROP TABLE t1_backup;",
            };

    char* msg = NULL;
    int i = 0;
    for (i = 0; i < sizeof(stmt) / sizeof(*stmt); i++)
        {
        err = sqlite3_exec(TheDb, stmt[i], NULL, NULL, &msg);
        TEST2(err, SQLITE_OK);
        }

    sqlite3_close(TheDb);
    TheDb = 0;
    DeleteFile(KTestDb2);
	}

/**
@SYMTestCaseID          PDS-SQLITE3-CT-4047
@SYMTestCaseDesc        Test for DEF143066: SQLITE, "CREATE INDEX" sql crashes the SQLite library.
                        The test creates a database with one empty table and establishes two connections
                        to that database. Then, while the first connection is at the middle of a read
                        transaction, the second connection attempts to create an index.
                        If the defect is not fixed, the SQLite library will crash.
@SYMTestPriority        High
@SYMTestActions         DEF143066: SQLITE, "CREATE INDEX" sql crashes the SQLite library.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143066
*/
void DEF143066()
    {
    DeleteFile(KTestDb);
    sqlite3_enable_shared_cache(1);
    int err = sqlite3_open(KTestDb, &TheDb); 
    TEST2(err, SQLITE_OK);
    
    err = sqlite3_exec(TheDb, "CREATE TABLE T0(Thread INTEGER, LocalIndex INTEGER, Inserts INTEGER, Updates INTEGER, IndexMod8 INTEGER)", 0, 0, 0);
    TEST2(err, SQLITE_OK);
    
    err = sqlite3_open(KTestDb, &TheDb2); 
    TEST2(err, SQLITE_OK);

    sqlite3_stmt* stmt = 0; 
    const char* tail = 0;   
    err = sqlite3_prepare_v2(TheDb, "SELECT COUNT(Thread) FROM T0 WHERE Thread = 0", -1, &stmt, &tail);
    TEST2(err, SQLITE_OK);
    
    err = sqlite3_step(stmt);
    TEST2(err, SQLITE_ROW);

    err = sqlite3_exec(TheDb2, "CREATE INDEX T0INDEX ON T0(Thread,IndexMod8)", 0, 0, 0);
    TEST2(err, SQLITE_LOCKED);
    
    (void)sqlite3_finalize(stmt);
    sqlite3_close(TheDb2);
    TheDb2 = NULL;
    sqlite3_close(TheDb);
    TheDb = NULL;
    DeleteFile(KTestDb);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4048
@SYMTestCaseDesc        Test for DEF143151: SQLite, strftime() returns incorrect result.
                        The test takes the current universal time (using TTime) 
                        and the current time retrieved from the SQLite library.
                        The test compares the times and expects the difference to be no more than
                        1 second. 
@SYMTestPriority        High
@SYMTestActions         DEF143151: SQLite, strftime() returns incorrect result
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143151
*/
void DEF143151()
    {
    DeleteFile(KTestDb);
    int err = sqlite3_open(KTestDb, &TheDb); 
    TEST2(err, SQLITE_OK);

    //Home date & time
    TBuf<50> dtstr1;
    TTime time;
    time.UniversalTime();
    TDateTime dt = time.DateTime();
    
    sqlite3_stmt* stmt = 0; 
    const char* tail = 0;   
    err = sqlite3_prepare_v2(TheDb, "SELECT strftime('%Y-%m-%d,%H:%M:%S','now')", -1, &stmt, &tail);
    TEST2(err, SQLITE_OK);
    err = sqlite3_step(stmt);
    TEST2(err, SQLITE_ROW);
    
    //SQLite date & time
    const unsigned char* s = sqlite3_column_text(stmt, 0);
    TEST(s != NULL);
    TBuf<50> dtstr2;
    dtstr2.Copy(TPtrC8(s));
    sqlite3_finalize(stmt);

    sqlite3_close(TheDb);
    TheDb = NULL;
    DeleteFile(KTestDb);
    
    dtstr1.Format(_L("%04d-%02d-%02d,%02d:%02d:%02d"), dt.Year(), dt.Month() + 1, dt.Day() + 1, dt.Hour(), dt.Minute(), dt.Second());

    // For the C-Style printout
    _LIT8(KUniversalTimeText,"Universal date&time=");
    _LIT8(KSQLiteTimeText,   "SQLite    date&time=");
    TBuf8<96> dtstr1print;
    TBuf8<96> dtstr2print;
    dtstr1print.Copy(dtstr1);
    dtstr2print.Copy(dtstr2);
    dtstr1print.Insert(0,KUniversalTimeText);
    dtstr2print.Insert(0,KSQLiteTimeText);
    TestPrintf((const char*)(dtstr1print.PtrZ()));
    TestPrintf((const char*)(dtstr2print.PtrZ()));
    
    //Comapare and fail if dates are not equal (+- 1 second)
    TLex lex;
    lex = dtstr2.Mid(0, 4);
    TInt sqlyear;
    err = lex.Val(sqlyear);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(5, 2);
    TInt sqlmonth;
    err = lex.Val(sqlmonth);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(8, 2);
    TInt sqlday;
    err = lex.Val(sqlday);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(11, 2);
    TInt sqlhour;
    err = lex.Val(sqlhour);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(14, 2);
    TInt sqlminute;
    err = lex.Val(sqlminute);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(17, 2);
    TInt sqlsecond;
    err = lex.Val(sqlsecond);
    TEST2(err, KErrNone);
    
    TDateTime sqldt(sqlyear, (TMonth)(sqlmonth - 1), sqlday - 1, sqlhour, sqlminute, sqlsecond, 0);
    TTime sqltime(sqldt);
    TTimeIntervalSeconds diff;
    err = sqltime.SecondsFrom(time, diff);
    TEST2(err, KErrNone);
    TEST(diff.Int() <= 1);
    }

void DoTest()
	{
	TestStart("@SYMTestCaseID:PDS-SQLITE3-UT-4029: SQLite file handle test");
	FileHandleTest();
	
	TestNext("@SYMTestCaseID:PDS-SQLITE3-CT-4028: DEF121506 test");
	DEF121506();

	TestNext("@SYMTestCaseID:PDS-SQLITE3-CT-4046: DEF140020 test");
	DEF140020();

    TestNext("@SYMTestCaseID:PDS-SQLITE3-CT-4047: SQLITE, \"CREATE INDEX\" sql crashes the SQLite library");
    DEF143066();

    TestNext(" @SYMTestCaseID:SYSLIB-SQL-CT-4048 DEF143151: SQLite, strftime() returns incorrect result");
    DEF143151();
	}

///////////////////////////////////////////////////////////////////////////////////////

TInt E32Main()
	{
	TestOpen(KTestName);
	TestTitle();
	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;
	
	CreateTestEnv();
	DoTest();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;

	TestEnd();	
	TestClose();
	delete tc;
	User::Heap().Check();
	return KErrNone;
	}
