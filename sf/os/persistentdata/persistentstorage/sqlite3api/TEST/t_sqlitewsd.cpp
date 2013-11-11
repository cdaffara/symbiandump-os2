// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_sqlitewsd.h"
#include "sqliteTestUtl.h"

#include <spawn.h>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////

const char* const KTestName = "t_sqlitewsd";
static RFs		TheFs;

static pid_t	TheKSqliteWsdProc2Pid = 0;
const char* 	KSqliteWsdProc2Name = "z:\\sys\\bin\\t_sqlitewsd2.exe";

const char* KTestDir = "c:\\test\\";
const char* KTestDb  = "c:\\test\\t_sqlitewsd.db";

sqlite3* TheDb = 0;

///////////////////////////////////////////////////////////////////////////////////////

static void DestroyTestEnv()
	{
	if(TheDb)
		{
		(void)sqlite3_close(TheDb);
		TheDb = 0;
		}
	if(TheFs.Handle() != KNullHandle)
		{
		TFileName fname;
		fname.Copy(TPtrC8((const TUint8*)KTestDb));
		(void)TheFs.Delete(fname);
		}
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		TestTestLine(EFalse, aLine);
		}
	}
	
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		const char* errMsg = sqlite3_errmsg(TheDb);
		if(errMsg)
			{
			TBuf<200> msgBuf;
			msgBuf.Copy(TPtrC8((const TUint8*)errMsg));
			RDebug::Print(_L("*** SQLITE error msg: \"%S\".\r\n"), &msgBuf);
			}
		DestroyTestEnv();
		TestTestLine(EFalse, aLine);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

    TFileName testDir;
    testDir.Copy(TPtrC8((const TUint8*)KTestDir));
	err = TheFs.MkDir(testDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	TFileName fname;
	fname.Copy(TPtrC8((const TUint8*)KTestDb));
	(void)TheFs.Delete(fname);

	err = sqlite3_open(KTestDb, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CreateDb()
	{
	TEST(TheDb != 0);
	TInt err = sqlite3_exec(TheDb, "CREATE TABLE A(F1 INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	}

static void RunSqliteWsd2()
	{
	TInt err = posix_spawn(&TheKSqliteWsdProc2Pid, KSqliteWsdProc2Name, 0, 0, 0, 0);
	TEST2(err, 0);
	}
	
static void DestroySqliteWsd2()
	{
	(void)waitpid(TheKSqliteWsdProc2Pid, 0, 0);
	}

void DoVerify()
	{
	sqlite3_stmt* stmt = 0;
  	const char* tail = 0;
	TInt err = sqlite3_prepare(TheDb, "SELECT * FROM A", -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST(!tail || tail[0] == 0);
	TInt proc1Id1recCnt = 0;
	TInt proc1Id2recCnt = 0;
	TInt proc2Id1recCnt = 0;
	TInt proc2Id2recCnt = 0;
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		TInt val = sqlite3_column_int(stmt, 0);
		switch(val)
			{
			case KWsdProc1RecId1: 
				++proc1Id1recCnt;
				break;
			case KWsdProc1RecId2: 
				++proc1Id2recCnt;
				break;
			case KWsdProc2RecId1: 
				++proc2Id1recCnt;
				break;
			case KWsdProc2RecId2: 
				++proc2Id2recCnt;
				break;
			default:
				TEST(0);
				break;
			}
		}
	sqlite3_finalize(stmt);
	TEST2(err, SQLITE_DONE);
	TEST2((proc1Id1recCnt + proc1Id2recCnt), KTestRecordCnt);
	TEST2((proc2Id1recCnt + proc2Id2recCnt), KTestRecordCnt);
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4026
@SYMTestCaseDesc		SQLITE OS porting layer - WSD test.
						The test verifies that the WSD object allocation and access	inside the OS porting layer 
						works properly on both the emulator and the hardware.
						The test runs two separate processes. Each process establishes a connection to the same
						database and inserts 500 records simultaneously.
						During the inserts, the SQLITE OS porting layer will use a mutex to synchronise the database
						operations between the two processes. If the WSD implementation does not work properly,
						then the mutex object won't be allocated per process and the same mutex instance will be used by
						both processes on the emulator. This will lead to panics/asserts inside the OS porting layer.
						The number of the inserted record and record ids is verified at the end of the test.
@SYMTestPriority		High
@SYMTestActions			SQLITE OS porting layer - WSD test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void DoWsdTests()
	{
	TestStart(" @SYMTestCaseID:SYSLIB-SQLITE3-UT-4026 Create the test database ");
	CreateDb();
	TestNext("Run the second process: t_sqlitewsd2");
	RunSqliteWsd2();
	TestNext("Insert the records");
	DoInserts(KWsdProc1Id, KWsdProc1RecId1, KWsdProc1RecId2);
	DestroySqliteWsd2();
	TestNext("Verify the inserted records");
	DoVerify();
	}

///////////////////////////////////////////////////////////////////////////////////////

TInt E32Main()
	{
	TestOpen(KTestName);
	TestTitle();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DoWsdTests();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;
	
	TestEnd();
	TestClose();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
