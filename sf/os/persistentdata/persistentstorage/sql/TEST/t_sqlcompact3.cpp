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
#include <bautils.h>
#include <sqldb.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "SqliteSymbian.h"


///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlcompact3 test"));
RSqlDatabase TheDb;
RSqlStatement TheStmt;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlcompact3_1.db");
_LIT(KTestPrivDbName, "c:\\private\\21212124\\t_sqlcompact3_2.db");
_LIT(KTestPrivDbNameZ,"z:\\private\\21212124\\t_sqldb1.db");//Created outside the test app
_LIT(KTestPrivDbNameC,"c:\\private\\21212124\\t_sqldb1.db");
_LIT(KTestSecureDbName, "c:[21212124]t_sqlcompact3_3.db");
_LIT8(KTestFullSecureDbNameZ, "c:\\private\\10281e17\\[21212124]t_sqlcompact3_3.db\x0");

const TInt KAutoVacuum = 1;
const TInt KIncrementalVacuum = 2;
const TInt KSqlDefaultVacuum = KIncrementalVacuum;

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
	TheStmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestPrivDbNameC);
	(void)RSqlDatabase::Delete(KTestSecureDbName);
	(void)RSqlDatabase::Delete(KTestPrivDbName);
	(void)RSqlDatabase::Delete(KTestDbName1);
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

	err = fs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	fs.Close();

	sqlite3SymbianLibInit();
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4056
@SYMTestCaseDesc		Manual compaction - configuration test.
						The test creates a database with a manual compaction mode.
						The test reopens the database and verifies that the compaction mode is persistent and is still manual.
						Then the test reopens the database using different compaction mode in the configuration string and 
						verifies that the original (manual) compaction mode cannot be changed when the database is opened.
@SYMTestPriority		Medium
@SYMTestActions			Manual compaction - configuration test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10274
                        REQ10402
*/
void CompactConfigTest1L()
	{
	//Create a test database with "manual" compaction mode
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=manual");
	TInt err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "incremental"
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Close and open the database again. The SQLite vacuum mode should be "incremental".
	err = TheDb.Open(KTestDbName1);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Close and open the database again with a config string with "auto" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	_LIT8(KConfigStr2, "compaction=auto");
	err = TheDb.Open(KTestDbName1, &KConfigStr2);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4057
@SYMTestCaseDesc		Auto compaction - configuration test.
						The test creates a database with an auto compaction mode.
						The test reopens the database and verifies that the compaction mode is persistent and is still auto.
						Then the test reopens the database using different compaction mode in the configuration string and 
						verifies that the original (auto) compaction mode cannot be changed when the database is opened.
@SYMTestPriority		Medium
@SYMTestActions			Auto compaction - configuration test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10274
                        REQ10400
*/
void CompactConfigTest2L()
	{
	//Create a test database with "auto" compaction mode
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=auto");
	TInt err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Create a test database with "synchronous" compaction mode
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	_LIT8(KConfigStr3, "encoding=utf-8;compaction=synchronous");
	err = TheDb.Create(KTestDbName1, &KConfigStr3);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Close and open the database again. The SQLite vacuum mode should be "auto".
	err = TheDb.Open(KTestDbName1);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Close and open the database again with a config string with "background" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	_LIT8(KConfigStr2, "compaction=background");
	err = TheDb.Open(KTestDbName1, &KConfigStr2);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4058
@SYMTestCaseDesc		Background compaction - configuration test.
						The test creates a database with a background compaction mode.
						The test reopens the database and verifies that the compaction mode is persistent and is still background.
						Then the test reopens the database using different compaction mode in the configuration string and 
						verifies that the original (background) compaction mode cannot be changed when the database is opened.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction - configuration test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10274
*/
void CompactConfigTest3L()
	{
	//Create a test database with "background" compaction mode
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=background");
	TInt err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "incremental"
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Close and open the database again. The SQLite vacuum mode should be "incremental".
	err = TheDb.Open(KTestDbName1);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Close and open the database again with a config string with "manual" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	_LIT8(KConfigStr2, "compaction=manual");
	err = TheDb.Open(KTestDbName1, &KConfigStr2);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4059
@SYMTestCaseDesc		Background compaction - no configuration string test.
						The test creates a database without using a configuration string.
						The database should be created with default compaction mode - background.
						The test reopens the database and verifies that the compaction mode is persistent and is still background.
						Then the test reopens the database using different compaction mode in the configuration string and 
						verifies that the original (background) compaction mode cannot be changed when the database is opened.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction - no configuration string test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
*/
void CompactConfigTest4L()
	{
	//Create a test database without configuration string
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be KSqlDefaultVacuum
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	//Create a test database with invalid configuration string
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=backgrund");
	err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be KSqlDefaultVacuum
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	//Create a test database with invalid configuration string
	_LIT8(KConfigStr2, "compactin=background");
	err = TheDb.Create(KTestDbName1, &KConfigStr2);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be KSqlDefaultVacuum
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4060
@SYMTestCaseDesc		Private database and compaction configuration test.
						The test verifies that a private database can be created using auto, background or 
						manual compaction mode and that the compaction mode does not chage after reopening 
						the database.
						The test also verifies that if the database is legacy, read-only, then the compaction
						mode is auto.
						The test also verifies that if the database is legacy, r/w, then the compaction
						mode will be changed from auto to background.
@SYMTestPriority		Medium
@SYMTestActions			Private database and compaction configuration test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10400
                        REQ10402
*/
void CompactConfigTest5L()
	{
	//Create a private test database with "auto" compaction mode
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=auto");
	TInt err = TheDb.Create(KTestPrivDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Close and open the database again. The SQLite vacuum mode should be "auto".
	err = TheDb.Open(KTestPrivDbName);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Close and open the database again with a config string with "background" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	_LIT8(KConfigStr2, "compaction=background");
	err = TheDb.Open(KTestPrivDbName, &KConfigStr2);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestPrivDbName);
	TEST2(err, KErrNone);
	//Create a private test database - no config string
	err = TheDb.Create(KTestPrivDbName);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be KSqlDefaultVacuum
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Close and open the database again. The SQLite vacuum mode should be KSqlDefaultVacuum.
	err = TheDb.Open(KTestPrivDbName);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Close and open the database again with a config string with "auto" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	err = TheDb.Open(KTestPrivDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	//Delete database
	err = RSqlDatabase::Delete(KTestPrivDbName);
	TEST2(err, KErrNone);
	//Open an existing private database that is read-only (on drive Z:)
	err = TheDb.Open(KTestPrivDbNameZ, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite compact mode should be "auto" (this is an old database (pre-"background compact" era))
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TheDb.Close();
	//Open the same database after copying it to drive C: (r/w private database). The compaction mode should change from auto to background.
	RFs fs;
	err = fs.Connect();
	TEST2(err, KErrNone);
	CFileMan* fm = CFileMan::NewL(fs);
	TEST(fm != NULL);
	err = fm->Copy(KTestPrivDbNameZ, KTestPrivDbNameC);
	TEST2(err, KErrNone);
	//"Copy" operation executed without errors. Now it is a time to turn off the read-only
	//flag of the target file (which may be on if the source file is on a read-only drive)
	err = fs.SetAtt(KTestPrivDbNameC, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);
	delete fm;
	fs.Close();
	err = TheDb.Open(KTestPrivDbNameC);
	TEST2(err, KErrNone);
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KSqlDefaultVacuum);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestPrivDbNameC);
	}

RSqlSecurityPolicy CreateTestSecurityPolicy()
	{
	const TSecurityPolicy KDefaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	TInt err = securityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);
	return securityPolicy;
	}

void TestCompactMode(TInt aExpectedCompactMode)
	{
	sqlite3 *dbHandle = NULL;
	TInt rc = sqlite3_open((const char*)KTestFullSecureDbNameZ().Ptr(), &dbHandle);
	TEST2(rc, SQLITE_OK);

	sqlite3_stmt* stmtHandle = NULL;
	const char* stmtTailZ = NULL;
	rc = sqlite3_prepare_v2(dbHandle, "PRAGMA auto_vacuum", -1, &stmtHandle, &stmtTailZ);
	TEST2(rc, SQLITE_OK);

	rc = sqlite3_step(stmtHandle);
	TEST2(rc, SQLITE_ROW);
	TInt compact = sqlite3_column_int(stmtHandle, 0); 
	
	sqlite3_finalize(stmtHandle);
	sqlite3_close(dbHandle);
	
	TEST2(compact, aExpectedCompactMode);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4061
@SYMTestCaseDesc		Secure shared database and compaction configuration test.
						The test verifies that a secure shared database can be created using auto, background or 
						manual compaction mode and that the compaction mode does not chage after reopening 
						the database.
@SYMTestPriority		Medium
@SYMTestActions			Secure shared database and compaction configuration test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10400
                        REQ10402
*/
void CompactConfigTest6L()
	{
	//Create a secure test database with "auto" compaction mode.
	RSqlSecurityPolicy securityPolicy = CreateTestSecurityPolicy();
	_LIT8(KConfigStr1, "encoding=utf-8;compaction=auto");
	TInt err = TheDb.Create(KTestSecureDbName, securityPolicy, &KConfigStr1);
	TEST2(err, KErrNone);
	securityPolicy.Close();
	TheDb.Close();
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	TestCompactMode(KAutoVacuum);
	//Close and open the database again. The SQLite vacuum mode should be "auto".
	err = TheDb.Open(KTestSecureDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	TestCompactMode(KAutoVacuum);
	//Close and open the database again with a config string with "background" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	_LIT8(KConfigStr2, "compaction=background");
	err = TheDb.Open(KTestSecureDbName, &KConfigStr2);
	TEST2(err, KErrNone);
	TheDb.Close();
	TestCompactMode(KAutoVacuum);
	//Delete database
	err = RSqlDatabase::Delete(KTestSecureDbName);
	TEST2(err, KErrNone);
	//Create a private test database - no config string
	securityPolicy = CreateTestSecurityPolicy();
	err = TheDb.Create(KTestSecureDbName, securityPolicy);
	TEST2(err, KErrNone);
	securityPolicy.Close();
	TheDb.Close();
	//Check the vacuum mode. The SQLite vacuum mode should be KSqlDefaultVacuum.
	TestCompactMode(KSqlDefaultVacuum);
	//Close and open the database again. The SQLite vacuum mode should be KSqlDefaultVacuum.
	err = TheDb.Open(KTestSecureDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	TestCompactMode(KSqlDefaultVacuum);
	//Close and open the database again with a config string with "auto" compaction mode. 
	//The SQLite vacuum mode should stay unchanged.
	err = TheDb.Open(KTestSecureDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	TheDb.Close();
	TestCompactMode(KSqlDefaultVacuum);
	//Delete database
	err = RSqlDatabase::Delete(KTestSecureDbName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4062
@SYMTestCaseDesc		Compaction configuration test - multiple connections.
						The test creates a database with auto or background compaction mode.
						Then the test opens more connections to the same database.
						The test verifies that the compaction mode of the connections opened later is
						the same as the compaction mode of the database that was openen first.
						The test also verifies that the compactino mode cannot be changed even if the second
						connection is opened with a configuration string with a specified different compaction mode.
@SYMTestPriority		Medium
@SYMTestActions			Compaction configuration test - multiple connections.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10400
*/
void CompactConfigTest7L()
	{
	//Create a test database with "auto" compaction mode.
	_LIT8(KConfigStr1, "encoding=utf-8;compaction  =   auto");
	TInt err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	//Open a second connection to the same database - no config string
	RSqlDatabase db2;
	err = db2.Open(KTestDbName1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TSqlScalarFullSelectQuery scalarQuery2(db2);
	compact = scalarQuery2.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	//Open a third connection to the same database - "background" compaction mode config string
	_LIT8(KConfigStr2, "  encoding =    utf-8 ; ; ; compaction  =   background   ");
	RSqlDatabase db3;
	err = db3.Open(KTestDbName1, &KConfigStr2);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "auto"
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	compact = scalarQuery2.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	TSqlScalarFullSelectQuery scalarQuery3(db3);
	compact = scalarQuery3.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	//Close & Delete database
	db3.Close();
	db2.Close();
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	//
	//
	//Create a test database with "background" compaction mode
	_LIT8(KConfigStr3, "compaction  =   background   ;;;;");
	err = TheDb.Create(KTestDbName1, &KConfigStr3);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "incremental"
	scalarQuery.SetDatabase(TheDb);
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	//Open a second connection to the same database - no config string
	err = db2.Open(KTestDbName1);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "incremental"
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	scalarQuery2.SetDatabase(db2);
	compact = scalarQuery2.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	//Open a third connection to the same database - "auto" compaction mode config string
	_LIT8(KConfigStr4, "  encoding =    utf-16 ; compaction  =   auto   ; ; ; ");
	err = db3.Open(KTestDbName1, &KConfigStr4);
	TEST2(err, KErrNone);
	//Check the vacuum mode. The SQLite vacuum mode should be "incremental"
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	compact = scalarQuery2.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	scalarQuery3.SetDatabase(db3);
	compact = scalarQuery3.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KIncrementalVacuum);
	//Close & Delete database
	db3.Close();
	db2.Close();
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4063
@SYMTestCaseDesc		Compaction configuration test - attached database.
						The test creates a database with an auto compaction mode.
						Then the test attaches the same database.
						The test verifies that the compaction mode of the main and the attached database is the same - auto.
@SYMTestPriority		Medium
@SYMTestActions			Compaction configuration test - attached database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10273
                        REQ10274
                        REQ10400
*/
void CompactConfigTest8L()
	{
	//Create a test database with "auto" compaction mode
	_LIT8(KConfigStr1, "; ;; ; compaction  =   auto; ");
	TInt err = TheDb.Create(KTestDbName1, &KConfigStr1);
	TEST2(err, KErrNone);
	//Attach a database
	err = TheDb.Attach(KTestDbName1, _L("db2"));
	TEST2(err, KErrNone);
	//Check compact for both main and attached database
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	TInt compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	compact = scalarQuery.SelectIntL(_L("PRAGMA db2.auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	//Detach
	err = TheDb.Detach(_L("db2"));
	TEST2(err, KErrNone);
	//Check compact again
	compact = scalarQuery.SelectIntL(_L("PRAGMA auto_vacuum"));
	TEST2(compact, KAutoVacuum);
	//
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4056 Compaction configuration tests 1 - manual compact"));	
	CompactConfigTest1L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4057 Compaction configuration tests 2 - auto compact"));	
	CompactConfigTest2L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4058 Compaction configuration tests 3 - background compact"));	
	CompactConfigTest3L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4059 Compaction configuration tests 4 - invalid compact"));	
	CompactConfigTest4L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4060 Compaction configuration tests 5 - private databases"));	
	CompactConfigTest5L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4061 Compaction configuration tests 6 - secure databases"));	
	CompactConfigTest6L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4062 Compaction configuration tests 7 - multiple connections"));	
	CompactConfigTest7L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4063 Compaction configuration tests 8 - attached databases"));	
	CompactConfigTest8L();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	DestroyTestEnv();
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
