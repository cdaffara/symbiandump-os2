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
#include <e32math.h>
#include <bautils.h>
#include <sqldb.h>
#include "SqlResourceTester.h"

///////////////////////////////////////////////////////////////////////////////////////

#define UNUSED_VAR(a) (a) = (a)

RTest TheTest(_L("t_sqlload test"));

TDriveNumber KTestDrive = EDriveC;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlload_1.db");
_LIT(KTestDbName2, "c:\\test\\t_sqlload_2.db");
_LIT(KTestDbName3, "c:\\test\\t_sqlload_3.db");
_LIT(KTestDbName4, "c:\\test\\t_sqlload_4.db");
_LIT(KTestDbName5, "c:\\test\\t_sqlload_5.db");

//Test thread count
const TInt KTestThreadCnt = 4;

//Test database names
const TPtrC KTestDbNames[] =
	{
	KTestDbName1(),
	KTestDbName2(),
	KTestDbName3()
	};

//Test database count
const TInt KTestDbCnt = sizeof(KTestDbNames) / sizeof(KTestDbNames[0]);

//Test duration
const TInt KTestDuration = 120;//seconds

//Test record count
const TInt KRecordCnt = 100;
//Record count which will be used in the test SQL queries
const TInt KQueriedRecordCnt = 40;
//Every SQL query will be processed (stepped) in KTestStepCnt steps.
const TInt KTestStepCnt = 4;
//RSqlStatement object count which will be used in the tests
const TInt KStatementCnt = 10;
//Max allowed alive RSqlStatement objects per thread
const TInt KMaxStatementPerThread = 30;
//Binary data length
const TInt KBinDataLen = 2003;

//StatementMaxNumberTest() time limit in seconds.
const TInt KTestTimeLimit = 60;//seconds

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName5);
	RSqlDatabase::Delete(KTestDbName4);
	RSqlDatabase::Delete(KTestDbName3);
	RSqlDatabase::Delete(KTestDbName2);
	RSqlDatabase::Delete(KTestDbName1);
	}

void GetHomeTimeAsString(TDes& aStr)
	{
	TTime time;
	time.HomeTime();
	TDateTime dt = time.DateTime();
	aStr.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
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

//StatementMaxNumberTest() timeouts in WDP builds.
//This function is used to return the seconds passed from the start of the test case.
TTimeIntervalSeconds ExecutionTimeSeconds(TTime& aStartTime)
	{
	TTime currTime;
	currTime.HomeTime();
	
	TTimeIntervalSeconds s;
	TInt err = currTime.SecondsFrom(aStartTime, s);
	TEST2(err, KErrNone);
	return s;
	}

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = fs.CreatePrivatePath(KTestDrive);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDatabases()
	{
	HBufC8* recData = HBufC8::New(KBinDataLen * 2 + 50);//"* 2" - hex values for the INSERT SQL statement
	TEST(recData != NULL);
	TPtr8 sql = recData->Des();

	for(TInt dbIdx=0;dbIdx<KTestDbCnt;++dbIdx)
		{
		//Create test database
		RSqlDatabase db;
		TInt err = db.Create(KTestDbNames[dbIdx]);
		TEST2(err, KErrNone);

		//Create test table
		_LIT8(KCreateSql, "CREATE TABLE A(F1 INTEGER, F2 BLOB)");
		err = db.Exec(KCreateSql);
		TEST(err >= 0);

		//Insert records in the test table
		for(TInt recIdx=1;recIdx<=KRecordCnt;++recIdx)
			{
			_LIT8(KInsertSql, "INSERT INTO A(F1, F2) VALUES(");
			sql.Copy(KInsertSql);
			sql.AppendNum((TInt64)recIdx);
			sql.Append(_L(", X'"));
			for(TInt k=0;k<KBinDataLen;++k)
				{
				sql.AppendFormat(_L8("%02X"), recIdx);
				}
			sql.Append(_L("')"));
			err = db.Exec(sql);
			TEST2(err, 1);
			}

		db.Close();
		}

	delete recData;
	}

//Structure used by the test thread function for orginizing its set of test data.
struct TSqlStatement
	{
	RSqlStatement	iObj;
	TBool 			iAlive;			//Non-zero if iObj is alive
	TInt			iCurIndex;		//The number of the current record in the set controlled by iObj statement
	TInt			iEndIndex;		//The last record number in the set controlled by iObj statement
	TInt			iCount;			//Records count in the set controlled by iObj statement
	};
	
typedef RArray<TSqlStatement> RSqlStatementArray;	

//Inits the random numbers generator.
//Opens one of the test databases.
void PreTest(RSqlDatabase& aDb, TInt64& aSeed, TName& aThreadName)
	{
	RThread currThread;
	
	//Init the random numbers generator
	TTime now;
	now.UniversalTime();
	aSeed = now.Int64() + currThread.Id();

	//Open one of the test databases
	const TInt KDbIndex = Math::Rand(aSeed) % KTestDbCnt;

	aThreadName = currThread.Name();
	RDebug::Print(_L("=== Thread %S, database %S\r\n"), &aThreadName, &KTestDbNames[KDbIndex]);

	TInt err = aDb.Open(KTestDbNames[KDbIndex]);
	TTEST2(err, KErrNone);
	}

//Creates N statements, where 0 < N < KStatementCnt
TInt CreateStatements(RSqlDatabase& aDb, TInt64& aSeed, RSqlStatementArray& aStmtArray)
	{
	TInt stmtCount = Math::Rand(aSeed) % KStatementCnt;
	if(stmtCount == 0)
		{
		stmtCount = 1;
		}
	for(TInt i=0;i<stmtCount;++i)
		{
		TSqlStatement stmt;
		stmt.iAlive = EFalse;
		stmt.iCount = KQueriedRecordCnt;
		stmt.iCurIndex = Math::Rand(aSeed) % KRecordCnt;
		if(stmt.iCurIndex == 0)
			{
			stmt.iCurIndex = 1;
			}
		if(stmt.iCurIndex > (KRecordCnt - KQueriedRecordCnt))
			{
			stmt.iCurIndex = KRecordCnt - KQueriedRecordCnt;
			}
		stmt.iEndIndex = stmt.iCurIndex + KQueriedRecordCnt;
		TBuf8<100> sql;
		sql.Copy(_L8("SELECT * FROM A WHERE F1 >= "));
		sql.AppendNum(stmt.iCurIndex);
		sql.Append(_L8(" AND F1 < "));
		sql.AppendNum(stmt.iEndIndex);
		TInt err = stmt.iObj.Prepare(aDb, sql);
		TTEST2(err, KErrNone);
		stmt.iAlive = ETrue;
		err = aStmtArray.Append(stmt);
		TTEST2(err, KErrNone);
		}
	return stmtCount;		
	}
	
//For each alive statement object - do (TSqlStatement::iCount / KTestStepCnt)
//RSqlStatement::Next() calls. If the Next() call reaches the end - close the statement object.
TInt ProcessStatements(RSqlStatementArray& aStmtArray)
	{
	const TInt KTotalStmtCount = aStmtArray.Count();
	TInt alive = 0;
	TInt completed = 0;
	for(TInt k=0;k<KTotalStmtCount;++k)
		{
		TSqlStatement& stmt = aStmtArray[k];
		if(stmt.iAlive)
			{
			++alive;
			TInt endIndex = stmt.iCurIndex + stmt.iCount / KTestStepCnt;
			if(endIndex <= stmt.iEndIndex)
				{
				while(stmt.iCurIndex < endIndex)
					{
					TInt err = stmt.iObj.Next();
					TTEST2(err, KSqlAtRow);
					//test column values
					TInt val1 = stmt.iObj.ColumnInt(0);
					TTEST(val1 == stmt.iCurIndex);
					RSqlColumnReadStream strm;
					err = strm.ColumnBinary(stmt.iObj, 1);
					TTEST2(err, KErrNone);
					for(TInt ii=0;ii<KBinDataLen;++ii)
						{
						TUint8 byte = 0;
						TRAP(err, byte = strm.ReadUint8L());
						TTEST2(err, KErrNone);
						TTEST(byte == (TUint8)val1);
						}
					strm.Close();
					++stmt.iCurIndex;
					}
				}
			if(stmt.iCurIndex >= stmt.iEndIndex)
				{
				stmt.iObj.Close();
				stmt.iAlive = EFalse;
				++completed;
				}
			}
		}
	return completed;
	}

//Close up to N statements, where 0 < N < KStatementCnt
TInt CloseStatements(RSqlStatementArray& aStmtArray, TInt64& aSeed)
	{
	TInt stmtCount = Math::Rand(aSeed) % KStatementCnt;
	if(stmtCount == 0)
		{
		stmtCount = 1;
		}
	const TInt KTotalStmtCount = aStmtArray.Count();
	TInt closed = 0;
	for(TInt j=0;j<stmtCount;++j)
		{
		const TInt KIdx = Math::Rand(aSeed) % KTotalStmtCount;
		TInt idx = KIdx;
		while((idx = (++idx % KTotalStmtCount)) != KIdx)
			{
			if(aStmtArray[idx].iAlive)
				{
				aStmtArray[idx].iObj.Close();
				aStmtArray[idx].iAlive = EFalse;
				++closed;
				break;
				}
			}
		}
	return closed;
	}

//Counts the alive statements
TInt AliveStatementsCount(RSqlStatementArray& aStmtArray)
	{
	TInt aliveCnt = 0;
	const TInt KTotalStmtCount = aStmtArray.Count();
	for(TInt l=0;l<KTotalStmtCount;++l)
		{
		if(aStmtArray[l].iAlive)
			{
			++aliveCnt;
			}
		}
	return aliveCnt;
	}
	
//Close all alive statements
void CloseAllStatements(RSqlStatementArray& aStmtArray)
	{
	const TInt KTotalStmtCount = aStmtArray.Count();
	for(TInt i=0;i<KTotalStmtCount;++i)
		{
		if(aStmtArray[i].iAlive)
			{
			aStmtArray[i].iObj.Close();
			}
		}
	TTEST2(TSqlResourceTester::Count(), 0);
	}

//Removes the already closed statements and compresses the array
void RemoveDeadStatements(RSqlStatementArray& aStmtArray)
	{
	for(TInt i=aStmtArray.Count()-1;i>=0;--i)
		{
		if(!aStmtArray[i].iAlive)
			{
			aStmtArray.Remove(i);
			}
		}
	aStmtArray.Compress();		
	}

//Close statement objects, statements array and the database object
TInt PostTest(RSqlDatabase& aDb, RSqlStatementArray& aStmtArray)
	{
	TInt statementsAlive = AliveStatementsCount(aStmtArray);
	CloseAllStatements(aStmtArray);
	aStmtArray.Close();
	aDb.Close();
	return statementsAlive;
	}

//Test thread function
//The thread function works with a set of TSqlStatement objects
//The test consists of 4 steps:
//Step 1: the test thread creates m TSqlStatement objects, 0 < m < KStatementCnt.
//		  With each of the created TSqlStatement objects the test thread prepares SELECT SQL query
//        "SELECT * FROM A WHERE F1 >= K1 AND F1 < K2", where K1 is random generated number, such that:
//        0 < K1 < (KRecordCnt - KQueriedRecordCnt)
//        K2 = K1 + KQueriedRecordCnt
//		  All just created TSqlStatement objects are marked as alive.
//Step 2: For each alive TSqlStatement object the test thread calls iObj.Next() method KTestStepCnt times,
//        KTestStepCnt < KQueriedRecordCnt.
//        The column values are retrieved and checked.
//Step 3: the test thread closes n TSqlStatement objects, 0 < n < KStatementCnt.
//Step 4: the test thread counts how many alive TSqlStatement objects are there.
//        If this count > KMaxStatementPerThread then the test thread closes all alive TSqlStatement objects
//		  to avoid OOM errors during the test.
//
//		  Each test thread does steps 1..4 for a period of KTestDuration seconds.
//		  At the end all TSqlStatement objects are closed.
//
//		  The idea of the test is to load the SQL server creating several amount of statement and stream objects
//		  and see that it is working stable and without problems.
TInt ThreadFunc(void*)
	{
	__UHEAP_MARK;

	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	TInt64 seed = 0;
	RSqlDatabase db;
	TName threadName;
	RSqlStatementArray statements;
	
	//Init the random numbers generator, opens the database
	PreTest(db, seed, threadName);

	//Main test loop
	TInt iteration = 0;
	TTime currTime;
	currTime.UniversalTime();
	TTime endTime = currTime + TTimeIntervalSeconds(KTestDuration);
	while(currTime < endTime)
		{
		++iteration;
		///////////////////////////////////////////////////////////////////////
		TInt statementsAliveBegin = statements.Count();
		//Step 1: Create N statements, where 0 < N < KStatementCnt
		TInt statementsCreated = CreateStatements(db, seed, statements);
		///////////////////////////////////////////////////////////////////////
		//Step 2: For each alive statement object - do (TSqlStatement::iCount / KTestStepCnt)
		//        RSqlStatement::Next() calls. If the Next() call reaches the end - close the statement object.
		TInt statementsCompleted = ProcessStatements(statements);
		///////////////////////////////////////////////////////////////////////
		//Step 3: Close up to N statements, where 0 < N < KStatementCnt
		TInt statementsClosed = CloseStatements(statements, seed);
		///////////////////////////////////////////////////////////////////////
		//Step 4: If the alive statement count is more than KMaxStatementPerThread, then close them all
		TInt statementsAliveEnd = AliveStatementsCount(statements);
		if(statementsAliveEnd > KMaxStatementPerThread)
			{
			RDebug::Print(_L("!!! Thread %S, iteration %d, alive %d, close all\r\n"), &threadName, iteration, statementsAliveEnd);
			CloseAllStatements(statements);
			statementsAliveEnd = 0;
			}
		///////////////////////////////////////////////////////////////////////
		RemoveDeadStatements(statements);
		RDebug::Print(_L("=== Thread %S, iteration % 4d, begin: % 3d, created % 2d, closed % 2d, completed % 2d, end % 3d, \r\n"),
							&threadName, iteration, statementsAliveBegin, 
													statementsCreated, statementsClosed, statementsCompleted, 
													statementsAliveEnd);
		currTime.UniversalTime();
		}

	//Close statement objects and the database object
	TInt statementsAlive = PostTest(db, statements);

	delete tc;

	__UHEAP_MARKEND;

	RDebug::Print(_L("=== Thread %S exit, still alive %d\r\n"), &threadName, statementsAlive);

	return KErrNone;
	}

void CreateTestThreads(RThread aThreads[], TRequestStatus aStatuses[], TInt aMaxCount)
	{
	_LIT(KThreadName, "TstThr");
	for(TInt i=0;i<aMaxCount;++i)
		{
		TBuf<20> threadName(KThreadName);
		threadName.AppendNum((TInt64)(i + 1));
		TEST2(aThreads[i].Create(threadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, NULL, EOwnerProcess), KErrNone);
		aThreads[i].Logon(aStatuses[i]);
		TEST2(aStatuses[i].Int(), KRequestPending);
		}
	}

void ResumeTestThreads(RThread aThreads[], TInt aMaxCount)
	{
	for(TInt i=0;i<aMaxCount;++i)
		{
		aThreads[i].Resume();
		}
	}


void CloseTestThreads(RThread aThreads[], TRequestStatus aStatuses[], TInt aMaxCount)
	{
	for(TInt i=0;i<aMaxCount;++i)
		{
		User::WaitForRequest(aStatuses[i]);
		TEST(aThreads[i].ExitType() != EExitPanic);
		aThreads[i].Close();
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1627-0001
@SYMTestCaseDesc		SQL server load test. The test creates KTestThreadCnt threads, KTestDbCnt test databases and
						inserts in each of them KRecordCnt test records.
						Pre-test step: each test thread randomly chooses and opens one of the test databases.
						Then, each of the test threads is doing the following 4 test steps:
						Step 1: the test thread creates m TSqlStatement objects, 0 < m < KStatementCnt.
						With each of the created TSqlStatement objects the test thread prepares SELECT SQL query
						"SELECT * FROM A WHERE F1 >= K1 AND F1 < K2", where K1 is random generated number, such that:
						0 < K1 < (KRecordCnt - KQueriedRecordCnt)
						K2 = K1 + KQueriedRecordCnt
						All just created TSqlStatement objects are marked as alive.
						Step 2: For each alive TSqlStatement object the test thread calls iObj.Next() method KTestStepCnt times,
						KTestStepCnt < KQueriedRecordCnt.
						The column values are retrieved and checked.
						Step 3: the test thread closes n TSqlStatement objects, 0 < n < KStatementCnt.
						Step 4: the test thread counts how many alive TSqlStatement objects are there.
						If this count > KMaxStatementPerThread then the test thread closes all alive TSqlStatement objects
						to avoid OOM errors during the test.

						Each test thread does steps 1..4 for a period of KTestDuration seconds.
						At the end all TSqlStatement objects are closed.

						The idea of the test is to load the SQL server creating several amount of statement and stream objects
						and see that it is working stable and without problems.
@SYMTestPriority		High
@SYMTestActions			SQL server load test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void SqlLoadTest()
	{
	CreateTestDatabases();

	RThread threads[KTestThreadCnt];
	TRequestStatus statuses[KTestThreadCnt];

	CreateTestThreads(threads, statuses, KTestThreadCnt);

	ResumeTestThreads(threads, KTestThreadCnt);

	User::After(2000000);

	CloseTestThreads(threads, statuses, KTestThreadCnt);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4201
@SYMTestCaseDesc        Max number of SQL statements test.
@SYMTestPriority        High
@SYMTestActions         The test creates a table with couple of records and then
						creates as many as possible SQL statements. The expected result is
						that either the statement creation process will fail with KErrNoMemory or
						the max number of statements to be created is reached (100000).
						Then the test deletes 1/2 of the created statements objects and
						after that attempts to execute Next() on the rest of them.
						Note that the test has a time limit of 120 seconds. Otherwise on some platforms
						with WDP feature switched on the test may timeout.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145236
*/  
void StatementMaxNumberTest()
	{
	TBuf<30> time;
	GetHomeTimeAsString(time);
	TheTest.Printf(_L("=== %S: Create database\r\n"), &time);
	
	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A(I) VALUES(1); INSERT INTO A(I) VALUES(2);"));
	TEST(err >= 0);

	GetHomeTimeAsString(time);
	TheTest.Printf(_L("=== %S: Create statements array\r\n"), &time);
	
	//Reserve memory for the statement objects
	const TInt KMaxStmtCount = 100000;
	RSqlStatement* stmt = new RSqlStatement[KMaxStmtCount];
	TEST(stmt != NULL);

	TTime startTime;
	startTime.HomeTime();
	
	//Create as many statement objects as possible
	TInt idx = 0;
	err = KErrNone;
	for(;idx<KMaxStmtCount;++idx)
		{
		err = stmt[idx].Prepare(db, _L("SELECT * FROM A WHERE I>=0 AND I<10"));
		if(err != KErrNone)
			{
			break;
			}
		TTimeIntervalSeconds s = ExecutionTimeSeconds(startTime);
		if((idx % 100) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: Create % 5d statements. %d seconds.\r\n"), &time, idx + 1, s.Int());
			}
		if(s.Int() > KTestTimeLimit)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: The time limit reached.\r\n"), &time);
			++idx;//The idx-th statement is valid, the statement count is idx + 1.
			break;
			}
		}
	
	TInt stmtCnt = idx;
	TheTest.Printf(_L("%d created statement objects. Last error: %d.\r\n"), stmtCnt, err);
	TEST(err == KErrNone || err == KErrNoMemory);

	//Close 1/2 of the statements to free some memory
	idx = 0;
	for(;idx<(stmtCnt/2);++idx)
		{
		stmt[idx].Close();
		if((idx % 100) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: % 5d statements closed\r\n"), &time, idx + 1);
			}
		}
	
	//Now, there should be enough memory to be able to execute Next() on the rest of the statements
	for(TInt j=0;idx<stmtCnt;++idx,++j)
		{
		err = stmt[idx].Next();
		TEST2(err, KSqlAtRow);
		err = stmt[idx].Next();
		TEST2(err, KSqlAtRow);
		err = stmt[idx].Next();
		TEST2(err, KSqlAtEnd);
		GetHomeTimeAsString(time);
		TTimeIntervalSeconds s = ExecutionTimeSeconds(startTime);
		if((j % 100) == 0)
			{
			TheTest.Printf(_L("=== %S: % 5d statements processed. %d seconds.\r\n"), &time, j + 1, s.Int());
			}
		if(s.Int() > KTestTimeLimit)
			{
			TheTest.Printf(_L("=== %S: The time limit reached.\r\n"), &time);
			break;
			}
		}

	//Cleanup
	for(idx=0;idx<stmtCnt;++idx)
		{
		stmt[idx].Close();
		if((idx % 100) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: % 5d statements closed\r\n"), &time, idx + 1);
			}
		}
	delete [] stmt;
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	GetHomeTimeAsString(time);
	TheTest.Printf(_L("=== %S: Test case end\r\n"), &time);
	}

TInt CreateFileSessions(TInt& aIdx, RFs aFs[], TInt aMaxFsSessCount)
	{
	TBuf<30> time;
	TTime startTime;
	startTime.HomeTime();
	//Create as many file session objects as possible
	TInt err = KErrNone;
	for(;aIdx<aMaxFsSessCount;++aIdx)
		{
		err = aFs[aIdx].Connect();
		if(err != KErrNone)
			{
			break;
			}
		TTimeIntervalSeconds s = ExecutionTimeSeconds(startTime);
		if((aIdx % 500) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: Create % 5d file sessions. %d seconds.\r\n"), &time, aIdx + 1, s.Int());
			}
		if(s.Int() > KTestTimeLimit)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: The time limit reached.\r\n"), &time);
			++aIdx;//The idx-th file session object is valid, the file session count is idx + 1.
			break;
			}
		}
	return err;
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4237
@SYMTestCaseDesc        Max file session number test.
@SYMTestPriority        High
@SYMTestActions         The test creates as many as possible file session objects. The expected result is
						that either the file session creation process will fail with KErrNoMemory or
						the max number of file sessions to be created is reached (100000).
						Then the test attempts to create a database. If there is no memory, the test
						closes some of the file session objects. The test also attempts to copy
						the created database and to delete it after that, both operations performed
						with all file session objects still open. The expectation is that the test
						will not crash the SQL server or the client side SQL dll.
						Note that the test has a time limit of 120 seconds. Otherwise on some platforms
						with WDP feature switched on the test may timeout.
@SYMTestExpectedResults Test must not fail
*/  
void FileSessionMaxNumberTest()
	{
	TBuf<30> time;
	GetHomeTimeAsString(time);
	TheTest.Printf(_L("=== %S: Create file sessions\r\n"), &time);

	const TInt KMaxFsCount = 100000;
	RFs* fs = new RFs[KMaxFsCount];
	TEST(fs != NULL);

	//Create as many file session objects as possible
	TInt idx = 0;
	TInt err = CreateFileSessions(idx, fs, KMaxFsCount);
	TheTest.Printf(_L("%d created file session objects. Last error: %d.\r\n"), idx, err);
	TEST(err == KErrNone || err == KErrNoMemory);
	
	TBool dbCreated = EFalse;
	RSqlDatabase db;
	
	//An attempt to create a database
	while(idx > 0 && err == KErrNoMemory)
		{
		(void)RSqlDatabase::Delete(KTestDbName1);
		err = db.Create(KTestDbName1);
		if(err == KErrNone)
			{
			err = db.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A(I) VALUES(1); INSERT INTO A(I) VALUES(2);"));
			}
		TheTest.Printf(_L("Database creation. Last error: %d.\r\n"), err);
		TEST(err == KErrNoMemory || err >= 0);
		if(err == KErrNoMemory)
			{
			fs[--idx].Close();
			}
		else
			{
			dbCreated = ETrue;
			}
		db.Close();
		}

	if(dbCreated)
		{
		//Create again file session objects - as many as possible
		err = CreateFileSessions(idx, fs, KMaxFsCount);
		TEST(err == KErrNone || err == KErrNoMemory);
		//Try to copy the database
		err = RSqlDatabase::Copy(KTestDbName1, KTestDbName4);
		TheTest.Printf(_L("Copy database. Last error: %d.\r\n"), err);
		TEST(err == KErrNone || err == KErrNoMemory);
		//Try to delete the databases
		if(err == KErrNone)
			{
			err = RSqlDatabase::Delete(KTestDbName4);
			TheTest.Printf(_L("Delete database copy. Last error: %d.\r\n"), err);
			TEST(err == KErrNone || err == KErrNoMemory);
			}
		err = RSqlDatabase::Delete(KTestDbName1);
		TheTest.Printf(_L("Delete database. Last error: %d.\r\n"), err);
		TEST(err == KErrNone || err == KErrNoMemory);
		}
	
	//Cleanup
	for(TInt i=0;i<idx;++i)
		{
		fs[i].Close();
		if((i % 500) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: % 5d file sessions closed\r\n"), &time, i + 1);
			}
		}
	delete [] fs;
	err = RSqlDatabase::Delete(KTestDbName4);
	TEST(err == KErrNone || err == KErrNotFound);
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST(err == KErrNone || err == KErrNotFound);
	}

TInt CreateSqlConnections(TInt& aIdx, RSqlDatabase aDb[], TInt aMaxSqlConnCount)
	{
	TBuf<30> time;
	TTime startTime;
	startTime.HomeTime();
	//Create as many file session objects as possible
	TInt err = KErrNone;
	for(;aIdx<aMaxSqlConnCount;++aIdx)
		{
		err = aDb[aIdx].Open(KTestDbName1);
		if(err != KErrNone)
			{
			break;
			}
		TTimeIntervalSeconds s = ExecutionTimeSeconds(startTime);
		if((aIdx % 100) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: Create % 5d sql connections. %d seconds.\r\n"), &time, aIdx + 1, s.Int());
			}
		if(s.Int() > KTestTimeLimit)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: The time limit reached.\r\n"), &time);
			++aIdx;//The idx-th sql connection is valid, the sql connection count is idx + 1.
			break;
			}
		}
	return err;
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4238
@SYMTestCaseDesc        Max sql connection number test.
@SYMTestPriority        High
@SYMTestActions         The test creates as many as possible sql connection objects. The expected result is
						that either the sql connection creation process will fail with KErrNoMemory or
						the max number of sql connection to be created is reached (100000).
						Then the test attempts to create a database. If there is no memory, the test
						closes some of the sql connection objects. The test also attempts to copy
						the created database and to delete it after that, both operations performed
						with all sql connection objects still open. The expectation is that the test
						will not crash the SQL server or the client side SQL dll.
						Note that the test has a time limit of 120 seconds. Otherwise on some platforms
						with WDP feature switched on the test may timeout.
@SYMTestExpectedResults Test must not fail
*/  
void SqlConnectionMaxNumberTest()
	{
	TBuf<30> time;
	GetHomeTimeAsString(time);
	TheTest.Printf(_L("=== %S: Create sql connections\r\n"), &time);

	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db1;
	TInt err = db1.Create(KTestDbName1);//CreateSqlConnections() opens the already existing KTestDbName1 database
	TEST2(err, KErrNone);
	
	const TInt KMaxConnCount = 100000;
	RSqlDatabase* db = new RSqlDatabase[KMaxConnCount];
	TEST(db != NULL);

	//Create as many sql connection objects as possible
	TInt idx = 0;
	err = CreateSqlConnections(idx, db, KMaxConnCount);
	TheTest.Printf(_L("%d created sql connection objects. Last error: %d.\r\n"), idx, err);
	TEST(err == KErrNone || err == KErrNoMemory);
	
	TBool dbCreated = EFalse;
	RSqlDatabase db2;
	
	//An attempt to create a database
	while(idx > 0 && err == KErrNoMemory)
		{
		(void)RSqlDatabase::Delete(KTestDbName4);
		err = db2.Create(KTestDbName4);
		if(err == KErrNone)
			{
			err = db2.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A(I) VALUES(1); INSERT INTO A(I) VALUES(2);"));
			}
		TheTest.Printf(_L("Database creation. Last error: %d.\r\n"), err);
		TEST(err == KErrNoMemory || err >= 0);
		if(err == KErrNoMemory)
			{
			db[--idx].Close();
			}
		else
			{
			dbCreated = ETrue;
			}
		db2.Close();
		}
	
	if(dbCreated)
		{
		//Create again sql connection objects - as many as possible
		err = CreateSqlConnections(idx, db, KMaxConnCount);
		TEST(err == KErrNone || err == KErrNoMemory);
		//Try to copy the database
		err = RSqlDatabase::Copy(KTestDbName4, KTestDbName5);
		TheTest.Printf(_L("Copy database. Last error: %d.\r\n"), err);
		TEST(err == KErrNone || err == KErrNoMemory);
		//Try to delete the databases
		if(err == KErrNone)
			{
			err = RSqlDatabase::Delete(KTestDbName5);
			TheTest.Printf(_L("Delete database copy. Last error: %d.\r\n"), err);
			TEST(err == KErrNone || err == KErrNoMemory);
			}
		err = RSqlDatabase::Delete(KTestDbName4);
		TheTest.Printf(_L("Delete database. Last error: %d.\r\n"), err);
		TEST(err == KErrNone || err == KErrNoMemory);
		}
	
	//Cleanup
	for(TInt i=0;i<idx;++i)
		{
		db[i].Close();
		if((i % 100) == 0)
			{
			GetHomeTimeAsString(time);
			TheTest.Printf(_L("=== %S: % 5d sql connections closed\r\n"), &time, i + 1);
			}
		}
	delete [] db;
	db1.Close();
	err = RSqlDatabase::Delete(KTestDbName5);
	TEST(err == KErrNone || err == KErrNotFound);
	err = RSqlDatabase::Delete(KTestDbName4);
	TEST(err == KErrNone || err == KErrNotFound);
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST(err == KErrNone || err == KErrNotFound);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1627-0001 SQL server load test "));
	SqlLoadTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4201 Statement max number test"));
	StatementMaxNumberTest();
#if defined __WINS__ ||	defined __WINSCW__
	//The next two tests are likely to timeout on hardware because they create a lot of file sessions and sql connections.
	//The SQL server heap is 32Mb on hardware but only 6Mb on the Emulator. 
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4237 File session max number test"));
	FileSessionMaxNumberTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4238 Sql connection max number test"));
	SqlConnectionMaxNumberTest();
#endif	
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
