/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sqlite3.h>
#include "t_sqliteperf.h"

static sqlite3* TheDb2 = 0;

static char TheSqlBuf2[2000];

#define UNUSED_VAR(a) (a) = (a)

/* ///////////////////////////////////////////////////////////////////////////////////// */

static void TestCleanup()
	{
	if(TheDb2)
		{
		sqlite3_close(TheDb2);
		TheDb2 = 0;
		}
	(void)remove(TestDbName());
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */
/* Test macros and functions */

static void Check1(int aValue, int aLine)
	{
	if(!aValue)
		{
		if(TheDb2)
			{
			const char* errmsg = sqlite3_errmsg(TheDb2);
			PrintS("*** SQLITE error message: %s\r\n", errmsg);
			}
		TestCleanup();
		PrintI("*** Test check failed! Line=%d\r\n", aLine);
		TestAbort(aLine);
		}
	}
static void Check2(int aValue, int aExpected, int aLine)
	{
	if(aValue != aExpected)
		{
		if(TheDb2)
			{
			const char* errmsg = sqlite3_errmsg(TheDb2);
			PrintS("*** SQLITE error message: %s\r\n", errmsg);
			}
		TestCleanup();
		PrintIII("*** Test check failed! Line=%d. Expected error: %d, got: %d\r\n", aLine, aExpected, aValue);
		TestAbort(aLine);
		}
	}
#define TEST(arg) Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) Check2(aValue, aExpected, __LINE__)

/* ///////////////////////////////////////////////////////////////////////////////////// */

//"PRAGMA cache_size=1024" and "PRAGMA locking_mode=EXCLUSIVE" statements executed only if 
//aPerfTestMode is EPerfTestSqliteMode (to match the Symbian SQL build time settings of SQLite)
static void ExecSqliteConfig(TPerfTestMode aPerfTestMode)
	{
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	if(aPerfTestMode == EPerfTestSqliteSqlMode)
		{
		int err;
		err = sqlite3_exec(TheDb2, "PRAGMA cache_size=1024", 0, 0, 0);
		TEST2(err, SQLITE_OK);
		err = sqlite3_exec(TheDb2, "PRAGMA locking_mode=EXCLUSIVE", 0, 0, 0);
		TEST2(err, SQLITE_OK);
		err = sqlite3_exec(TheDb2, "PRAGMA auto_vacuum=incremental", 0, 0, 0);
		TEST2(err, SQLITE_OK);
		err = sqlite3_exec(TheDb2, "PRAGMA journal_mode=PERSIST", 0, 0, 0);
		TEST2(err, SQLITE_OK);
		err = sqlite3_exec(TheDb2, "PRAGMA journal_size_limit=2048000", 0, 0, 0);
		TEST2(err, SQLITE_OK);
		}
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4018
@SYMTestCaseDesc		SQLite library multi-insert performance test.
						The test inserts 1000 records in a single transaction and stores 
						the execution time for later use (comparison and printing).
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4010 test case - "SQL server multi-insert performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-insert performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteMultiInsertTest(TPerfTestMode aPerfTestMode, const char aInsertSql[], int aInsertRecCnt)
	{
	int err;
	int i;
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;
	unsigned int fc;
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	err = sqlite3_prepare(TheDb2, aInsertSql, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	for(i=0;i<aInsertRecCnt;++i)
		{
		err = sqlite3_bind_int(stmt, 1, i + 1);
		TEST2(err, SQLITE_OK);

		err = sqlite3_step(stmt);
		TEST2(err, SQLITE_DONE);
		
		err = sqlite3_reset(stmt);		
		TEST2(err, SQLITE_OK);
		}

	err = sqlite3_exec(TheDb2, "COMMIT", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	StorePerfTestResult(aPerfTestMode, EPerfTestMultiInsert, FastCounterValue() - fc);

	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

static void FormatSqlStmt(char* aSqlBuf, const char aSql[], int aRecIds[], int aRecCnt)
	{
	int i;
	strcpy(aSqlBuf, aSql);
	strcat(aSqlBuf, "(");
	for(i=0;i<aRecCnt;++i)
		{
		char tmp[10];
		sprintf(tmp, "%d", aRecIds[i]);
		strcat(aSqlBuf, tmp);
		strcat(aSqlBuf, ",");
		}
	aSqlBuf[strlen(aSqlBuf) - 1] = ')';
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4019
@SYMTestCaseDesc		SQLite library multi-update performance test.
						The test updates 100 records and stores 
						the execution time for later use (comparison and printing).
						The IDs of the updated records are exactly the same as the IDs of the updated
						records, used by SYSLIB-SQLITE3-UT-4011 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4011 test case - "SQL server multi-update performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-update performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteMultiUpdateTest(TPerfTestMode aPerfTestMode, const char aUpdateSql[], int aUpdateRecIds[], int aUpdateRecCnt)
	{
	int err;
	unsigned int fc;
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	FormatSqlStmt(TheSqlBuf2, aUpdateSql, aUpdateRecIds, aUpdateRecCnt);

	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, TheSqlBuf2, 0, 0, 0);
	StorePerfTestResult(aPerfTestMode, EPerfTestMultiUpdate, FastCounterValue() - fc);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4020
@SYMTestCaseDesc		SQLite library multi-delete performance test.
						The test deletes 100 records and stores 
						the execution time for later use (comparison and printing).
						The IDs of the deleted records are exactly the same as the IDs of the deleted
						records, used by SYSLIB-SQLITE3-UT-4012 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4012 test case - "SQL server multi-delete performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-delete performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteMultiDeleteTest(TPerfTestMode aPerfTestMode, const char aDeleteSql[], int aDeleteRecIds[], int aDeleteRecCnt)
	{
	int err;
	unsigned int fc;
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	FormatSqlStmt(TheSqlBuf2, aDeleteSql, aDeleteRecIds, aDeleteRecCnt);

	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, TheSqlBuf2, 0, 0, 0);
	StorePerfTestResult(aPerfTestMode, EPerfTestMultiDelete, FastCounterValue() - fc);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4021
@SYMTestCaseDesc		SQLite library multi-select performance test.
						The test selects 100 records and stores 
						the execution time for later use (comparison and printing).
						The IDs of the selected records are exactly the same as the IDs of the selected
						records, used by SYSLIB-SQLITE3-UT-4013 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4013 test case - "SQL server multi-select performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-select performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteMultiSelectTest(TPerfTestMode aPerfTestMode, const char aSelectSql[], int aSelectRecIds[], int aSelectRecCnt)
	{
	int err;
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;
	int recCnt = 0;
	unsigned int fc;
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	FormatSqlStmt(TheSqlBuf2, aSelectSql, aSelectRecIds, aSelectRecCnt);

	err = sqlite3_prepare(TheDb2, TheSqlBuf2, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	fc = FastCounterValue();
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		__int64 i64;
		double d;
		const unsigned short* t;
		const unsigned char* b;
		
		i64 = sqlite3_column_int64(stmt, 0);
		UNUSED_VAR(i64);
		d = sqlite3_column_double(stmt, 1);
		UNUSED_VAR(d);
		t = (const unsigned short*)sqlite3_column_text16(stmt, 2);
		UNUSED_VAR(t);
		b = (const unsigned char*)sqlite3_column_blob(stmt, 3);
		UNUSED_VAR(b);
		++recCnt;
		}
	StorePerfTestResult(aPerfTestMode, EPerfTestMultiSelect, FastCounterValue() - fc);
	TEST2(err, SQLITE_DONE);
	TEST2(recCnt, aSelectRecCnt);

	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4022
@SYMTestCaseDesc		SQLite library single-insert performance test.
						The test inserts one record and stores 
						the execution time for later use (comparison and printing).
						The ID of the inserted record is exactly the same as the ID of the inserted
						record, used by SYSLIB-SQLITE3-UT-4014 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4014 test case - "SQL server single-insert performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library single-insert performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteSingleInsertTest(TPerfTestMode aPerfTestMode, const char aSingleInsertSql[], TInt aInsertRecId)
	{
	int err;
	unsigned int fc;
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	sprintf(TheSqlBuf2, aSingleInsertSql, aInsertRecId);
	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, TheSqlBuf2, 0, 0, 0);
	StorePerfTestResult(aPerfTestMode, EPerfTestSingleInsert, FastCounterValue() - fc);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4023
@SYMTestCaseDesc		SQLite library single-update performance test.
						The test updates one record and stores 
						the execution time for later use (comparison and printing).
						The ID of the updated record is exactly the same as the ID of the updated
						record, used by SYSLIB-SQLITE3-UT-4015 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4015 test case - "SQL server single-update performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library single-update performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteSingleUpdateTest(TPerfTestMode aPerfTestMode, const char aSingleUpdateSql[], TInt aUpdateRecId)
	{
	int err;
	unsigned int fc;
	char tmp[10];
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	sprintf(tmp, "%d", aUpdateRecId);
	strcpy(TheSqlBuf2, aSingleUpdateSql);
	strcat(TheSqlBuf2, tmp);
	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, TheSqlBuf2, 0, 0, 0);
	StorePerfTestResult(aPerfTestMode, EPerfTestSingleUpdate, FastCounterValue() - fc);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4024
@SYMTestCaseDesc		SQLite library single-delete performance test.
						The test deletes one record and stores 
						the execution time for later use (comparison and printing).
						The ID of the deleted record is exactly the same as the ID of the deleted
						record, used by SYSLIB-SQLITE3-UT-4016 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4016 test case - "SQL server single-delete performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library single-delete performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteSingleDeleteTest(TPerfTestMode aPerfTestMode, const char aSingleDeleteSql[], TInt aDeleteRecId)
	{
	int err;
	unsigned int fc;
	char tmp[10];
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	sprintf(tmp, "%d", aDeleteRecId);
	strcpy(TheSqlBuf2, aSingleDeleteSql);
	strcat(TheSqlBuf2, tmp);
	fc = FastCounterValue();
	err = sqlite3_exec(TheDb2, TheSqlBuf2, 0, 0, 0);
	StorePerfTestResult(aPerfTestMode, EPerfTestSingleDelete, FastCounterValue() - fc);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4025
@SYMTestCaseDesc		SQLite library single-select performance test.
						The test selects one record and stores 
						the execution time for later use (comparison and printing).
						The ID of the selected record is exactly the same as the ID of the selected
						record, used by SYSLIB-SQLITE3-UT-4017 test case.
						The results of this test case will be compared against the results of
						the SYSLIB-SQLITE3-UT-4017 test case - "SQL server single-select performance test".
@SYMTestPriority		High
@SYMTestActions			SQLite library single-select performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
void SqliteSingleSelectTest(TPerfTestMode aPerfTestMode, const char aSingleSelectSql[], TInt aSelectRecId)
	{
	int err;
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;
	int recCnt = 0;
	unsigned int fc;
	char tmp[10];
	
	TEST(aPerfTestMode > EPerfTestSqlMode && aPerfTestMode < EPerfTestModeCnt);
	TEST(!TheDb2);
	err = sqlite3_open(TestDbName(), &TheDb2);
	TEST2(err, SQLITE_OK);
	ExecSqliteConfig(aPerfTestMode);

	sprintf(tmp, "%d", aSelectRecId);
	strcpy(TheSqlBuf2, aSingleSelectSql);
	strcat(TheSqlBuf2, tmp);
	
	err = sqlite3_prepare(TheDb2, TheSqlBuf2, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	fc = FastCounterValue();
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		__int64 i64;
		double d;
		const unsigned short* t;
		const unsigned char* b;
		
		i64 = sqlite3_column_int64(stmt, 0);
		UNUSED_VAR(i64);
		d = sqlite3_column_double(stmt, 1);
		UNUSED_VAR(d);
		t = (const unsigned short*)sqlite3_column_text16(stmt, 2);
		UNUSED_VAR(t);
		b = (const unsigned char*)sqlite3_column_blob(stmt, 3);
		UNUSED_VAR(b);
		++recCnt;
		}
	StorePerfTestResult(aPerfTestMode, EPerfTestSingleSelect, FastCounterValue() - fc);
	TEST2(err, SQLITE_DONE);
	TEST2(recCnt, 1);

	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb2);
	TheDb2 = 0;
	}

void SqliteInitialize(TPerfTestMode aMode)
	{
	if(aMode == EPerfTestSqliteSqlMode)
		{
		const TInt KSqliteLookAsideCellSize = 128;
		const TInt KSqliteLookAsideCellCount = 512;
		int err;
		err = sqlite3_config(SQLITE_CONFIG_LOOKASIDE, KSqliteLookAsideCellSize, KSqliteLookAsideCellCount);
		TEST2(err, SQLITE_OK);
		sqlite3_soft_heap_limit(1024 * 1024);
		err = sqlite3_enable_shared_cache(1);
		TEST2(err, SQLITE_OK);
		}
	}

void SqliteFinalize(TPerfTestMode aMode)
	{
	if(aMode == EPerfTestSqliteSqlMode)
		{
		(void)sqlite3_enable_shared_cache(0);
		sqlite3_soft_heap_limit(0);
		}
	sqlite3_shutdown();
	}
