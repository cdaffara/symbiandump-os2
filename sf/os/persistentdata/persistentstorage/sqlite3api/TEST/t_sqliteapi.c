/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sqlite3.h>
#include "sqliteTestUtl.h"

static sqlite3* 		TheDb = 0;
static sqlite3* 		TheDb2 = 0;
static sqlite3_stmt*	TheStmt = 0;
static sqlite3_stmt*	TheStmt2 = 0;
const char* const		TheTestDirName = "c:\\test";
const char* const		TheTestDbName  = "c:\\test\\a1.db";

const int 			KTestThreadCount = 2;
static int 			TheInsertRecCnt[KTestThreadCount] = {0, 0};
static int 			TheLockErrCnt[KTestThreadCount] = {0, 0};
const char* const 	KThreadNames[KTestThreadCount] = {"THRD1", "THRD2"};
static sem_t 		TheSemaphores[KTestThreadCount];

#define UNUSED_ARG(a) (a) = (a)

/* ///////////////////////////////////////////////////////////////////////////////////// */

/* The standard C library does not have abs() function with a double argument */
static double dabs(double arg)
	{
	return arg < 0.0 ? -arg : arg;
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

static void TestEnvDestroy()
	{
	if(TheStmt2)
		{
		(void)sqlite3_finalize(TheStmt2);
		TheStmt2 = 0;
		}
	if(TheStmt)
		{
		(void)sqlite3_finalize(TheStmt);
		TheStmt = 0;
		}
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
	(void)remove(TheTestDbName);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */
/* Test macros and functions */

static void Check1(int aValue, int aLine)
	{
	if(!aValue)
		{
		if(TheDb)
			{
			const char* errmsg = sqlite3_errmsg(TheDb);
			PrintS("*** DB1: SQLITE error message: %s\r\n", errmsg);
			}
		if(TheDb2)
			{
			const char* errmsg = sqlite3_errmsg(TheDb2);
			PrintS("*** DB2: SQLITE error message: %s\r\n", errmsg);
			}
		PrintI("*** Test check failed! Line=%d\r\n", aLine);
		TestEnvDestroy();
		TestAbort(aLine);
		}
	}
	
static void Check2(int aValue, int aExpected, int aLine)
	{
	if(aValue != aExpected)
		{
		if(TheDb)
			{
			const char* errmsg = sqlite3_errmsg(TheDb);
			PrintS("*** DB1: SQLITE error message: %s\r\n", errmsg);
			}
		if(TheDb2)
			{
			const char* errmsg = sqlite3_errmsg(TheDb2);
			PrintS("*** DB2: SQLITE error message: %s\r\n", errmsg);
			}
		PrintIII("*** Test check failed! Line=%d. Expected error: %d, got: %d\r\n", aLine, aExpected, aValue);
		TestEnvDestroy();
		TestAbort(aLine);
		}
	}
	
static void Check64(sqlite_int64 aValue, sqlite_int64 aExpected, int aLine)
	{
	if(aValue != aExpected)
		{
		if(TheDb)
			{
			const char* errmsg = sqlite3_errmsg(TheDb);
			PrintS("*** DB1: SQLITE error message: %s\r\n", errmsg);
			}
		if(TheDb2)
			{
			const char* errmsg = sqlite3_errmsg(TheDb2);
			PrintS("*** DB2: SQLITE error message: %s\r\n", errmsg);
			}
		PrintII64I64("*** Test check failed! Line=%ld. Expected error: %ld, got: %d\r\n", aLine, aExpected, aValue);
		TestEnvDestroy();
		TestAbort(aLine);
		}
	}
#define TEST(arg) Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) Check2(aValue, aExpected, __LINE__)
#define TEST64(aValue, aExpected) Check64(aValue, aExpected, __LINE__)

/* ///////////////////////////////////////////////////////////////////////////////////// */

static void TestEnvCreate()
	{
	int err; 
	(void)remove(TheTestDbName);
	err = mkdir(TheTestDirName, S_IREAD | S_IWRITE);
	if(err != 0)
		{
		err = errno;	
		}
	TEST(err == 0 || err == EEXIST);
	
	//Creating the private data cage directory here to suppress a capability warning
	CreatePrivateDir();
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */
/* Callbacks */

static int exec_callback(void* udata, int argc, char** argv, char** colname)
	{
	UNUSED_ARG(udata);
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);
	UNUSED_ARG(colname);
	return 0;
	}

static int authorizer_callback(void* udata, int optype, const char* name1, const char* name2, const char* name, const char* viewname)
	{
	UNUSED_ARG(udata);
	UNUSED_ARG(optype);
	UNUSED_ARG(name1);
	UNUSED_ARG(name2);
	UNUSED_ARG(name);
	UNUSED_ARG(viewname);
	return SQLITE_OK;
	}

static int commit_hook(void* udata)
	{
	UNUSED_ARG(udata);
	return SQLITE_OK;	
	}
	
static void rollback_hook(void* udata)
	{
	UNUSED_ARG(udata);
	}

static void update_hook(void* udata, int type, char const* dbname, char const* tblname, sqlite_int64 rowid)
	{
	UNUSED_ARG(udata);
	UNUSED_ARG(type);
	UNUSED_ARG(dbname);
	UNUSED_ARG(tblname);
	UNUSED_ARG(rowid);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4001
@SYMTestCaseDesc		sqlite3_exec() tests.
						List of called SQLITE3 functions:
						 - sqlite3_exec;
						 - sqlite3_errcode;
						 - sqlite3_errmsg;
						 - sqlite3_last_insert_rowid;
						 - sqlite3_changes;
						 - sqlite3_total_changes;
						 - sqlite3_get_autocommit;
@SYMTestPriority		High
@SYMTestActions			sqlite3_exec() tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TestExec()
	{
	int err;
	sqlite_int64 lastrowid;
	int val;
	
	TEST(TheDb != 0);
	
	err = sqlite3_exec(TheDb, "CREATE TABLE A(F1 INTEGER, F2 BIGINT, F3 REAL, F4 TEXT, F5 BLOB)", &exec_callback, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(1, 1234567891234, 56.12, 'TEXT', x'313233343536')", &exec_callback, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_errcode(TheDb);
	TEST2(err, SQLITE_OK);
	
	(void)sqlite3_errmsg(TheDb);
	
	lastrowid = sqlite3_last_insert_rowid(TheDb);
	TEST(lastrowid > 0LL);
	
	val = sqlite3_changes(TheDb);
	TEST2(val, 1);
	
	val = sqlite3_total_changes(TheDb);
	TEST(val >= 1);
	
	val = sqlite3_get_autocommit(TheDb);
	TEST(val != 0);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

static void DoTestStatement()
	{
	int err, val;
	sqlite_int64 val64;
	double dblval;
	const unsigned char* textval;
	const unsigned short* textval16;
	const unsigned char* blob;
	const char *coltype, *colname;
	const unsigned short *coltype16, *colname16;
	sqlite3* db;

	TEST(TheDb != 0);
	TEST(TheStmt != 0);
	
	val = sqlite3_column_count(TheStmt);
	TEST2(val, 5);
	
	db = sqlite3_db_handle(TheStmt);
	TEST2((unsigned int)db, (unsigned int)TheDb);
	
	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);
	
#ifdef SQLITE_ENABLE_COLUMN_METADATA
	sqlite3_column_database_name(TheStmt, 0);
	sqlite3_column_database_name16(TheStmt, 1);
	sqlite3_column_table_name(TheStmt, 2);
	sqlite3_column_table_name16(TheStmt, 3);
	sqlite3_column_origin_name(TheStmt, 4);
	sqlite3_column_origin_name16(TheStmt, 0);
#endif	

	coltype = sqlite3_column_decltype(TheStmt, 0);
	TEST2(strcmp(coltype, "INTEGER"), 0);
	
	coltype16 = (const unsigned short*)sqlite3_column_decltype16(TheStmt, 2);
	TEST2(wcscmp(coltype16, L"REAL"), 0);

	colname = sqlite3_column_name(TheStmt, 1);
	TEST2(strcmp(colname, "F2"), 0);
	
	colname16 = (const unsigned short *)sqlite3_column_name16(TheStmt, 4);
	TEST2(wcscmp(colname16, L"F5"), 0);

	val = sqlite3_column_int(TheStmt, 0);
	TEST2(val, 1);
	
	val64 = sqlite3_column_int64(TheStmt, 1);
	TEST64(val64, 1234567891234LL);
	
	dblval = sqlite3_column_double(TheStmt, 2);
	TEST(dabs(dblval - 56.12) < 0.00001);

	textval = sqlite3_column_text(TheStmt, 3);
	TEST2(strcmp((const char*)textval, "TEXT"), 0);

	textval16 = sqlite3_column_text16(TheStmt, 3);
	TEST2(wcscmp(textval16, L"TEXT"), 0);

	blob = (const unsigned char*)sqlite3_column_blob(TheStmt, 4);
	TEST2(memcmp(blob, "123456", 6), 0);

	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_DONE);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4002
@SYMTestCaseDesc		Statement handle SQLITE3 tests.
						List of called SQLITE3 functions:
						 - sqlite3_complete;
						 - sqlite3_complete16;
						 - sqlite3_prepare;
						 - sqlite3_finalize;
						 - sqlite3_column_count;
						 - sqlite3_db_handle;
						 - sqlite3_step;
						 - sqlite3_column_decltype;
						 - sqlite3_column_decltype16;
						 - sqlite3_column_name;
						 - sqlite3_column_name16;
						 - sqlite3_column_int;
						 - sqlite3_column_int64;
						 - sqlite3_column_double;
						 - sqlite3_column_text;
						 - sqlite3_column_text16;
						 - sqlite3_column_blob;
@SYMTestPriority		High
@SYMTestActions			Statement handle SQLITE3 tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TestStatement1()
	{
  	const char* tail = 0;
	int err;
	
	TEST(TheDb != 0);
	TEST(!TheStmt);

	err = sqlite3_complete("SELECT * FROM A;");
	TEST(err != 0);
	
	err = sqlite3_complete16(L"SELECT * FROM A;");
	TEST(err != 0);
	
	err = sqlite3_prepare(TheDb, "SELECT * FROM A", -1, &TheStmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)TheStmt);
	TEST(!tail || strlen(tail) == 0);
	
	DoTestStatement();
	
	err = sqlite3_finalize(TheStmt);
	TEST2(err, SQLITE_OK);
	TheStmt = 0;
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4003
@SYMTestCaseDesc		Statement handle SQLITE3 tests.
						List of called SQLITE3 functions:
						 - sqlite3_prepare;
						 - sqlite3_finalize;
						 - sqlite3_bind_int;
						 - sqlite3_bind_int64;
						 - sqlite3_bind_double;
						 - sqlite3_bind_text;
@SYMTestPriority		High
@SYMTestActions			Statement handle SQLITE3 tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TestStatement2()
	{
  	const char* tail = 0;
	int err;
	
	TEST(TheDb != 0);
	TEST(!TheStmt);
	
	err = sqlite3_prepare(TheDb, "SELECT * FROM A WHERE F1=? AND F2=? AND F3<? AND F4=?", -1, &TheStmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)TheStmt);
	TEST(!tail || strlen(tail) == 0);
	
	err = sqlite3_bind_int(TheStmt, 1, 1);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_bind_int64(TheStmt, 2, 1234567891234LL);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_bind_double(TheStmt, 3, 70.0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_bind_text(TheStmt, 4, "TEXT", -1, SQLITE_STATIC);
	TEST2(err, SQLITE_OK);
	
	DoTestStatement();
	
	err = sqlite3_finalize(TheStmt);
	TEST2(err, SQLITE_OK);
	TheStmt = 0;
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4038
@SYMTestCaseDesc		Database handle SQLITE3 tests.
						List of called SQLITE3 functions:
						 - sqlite3_db_status;
						 - sqlite3_file_control;
						 - sqlite3_limit;
						 - sqlite3_next_stmt;
						 - sqlite3_randomness;
@SYMTestPriority		High
@SYMTestActions			Database handle SQLITE3 tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void TestSqliteApi2()
	{
	int used = 0;
	int high = 0;
	int lock = -1;
	int limit = 0;
	sqlite3_stmt* next = 0;
	int err;
	unsigned char buf[10];
	
	TEST(TheDb != 0);
	
	err = sqlite3_db_status(TheDb, SQLITE_DBSTATUS_LOOKASIDE_USED, &used, &high, 0);
	TEST2(err, SQLITE_OK);
	PrintI("Lookaside slots: %d\r\n", used);
	PrintI("Max used lookaside slots: %d\r\n", high);
		
	err = sqlite3_file_control(TheDb, "main", SQLITE_FCNTL_LOCKSTATE, &lock);
	TEST2(err, SQLITE_OK);
	TEST2(lock, SQLITE_LOCK_NONE);
	
	limit = sqlite3_limit(TheDb, SQLITE_LIMIT_LENGTH, -1);
	TEST(limit > 0);
		
	next = sqlite3_next_stmt(TheDb, 0);
	TEST(!next);
		
	memset(buf, 0, sizeof(buf));
	sqlite3_randomness(8, buf);

	memset(buf, 0, sizeof(buf));
	sqlite3_randomness(7, buf);

	memset(buf, 0, sizeof(buf));
	sqlite3_randomness(3, buf);

	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4039
@SYMTestCaseDesc		SQLITE3 - blob API tests.
						List of called SQLITE3 functions:
						 - sqlite3_bind_zeroblob;
						 - sqlite3_blob_bytes;
						 - sqlite3_blob_close;
						 - sqlite3_blob_open;
						 - sqlite3_blob_read;
						 - sqlite3_blob_write;
						 - sqlite3_sql;
@SYMTestPriority		High
@SYMTestActions			SQLITE3 - blob API tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void TestSqliteBlobApi()
	{
	int err;
	const char* tail = 0;
	sqlite3_blob* blob = 0;
	int bytes = 0;
	const char KBlobData[] = "ABCDEFGH";
	char sql[100];
	const int KBlobLen = strlen(KBlobData);
	const char* sql2 = 0;
	const char KSqlFmt[] = "UPDATE BlobTbl SET B=:Prm WHERE ROWID=1";
	
	TEST(TheDb != 0);
	TEST(!TheStmt);

	/* Create the table, insert one record with a blob */
	
	err = sqlite3_exec(TheDb, "CREATE TABLE BlobTbl(I INTEGER PRIMARY KEY, B BLOB)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	sprintf(sql, "INSERT INTO BlobTbl VALUES(1, zeroblob(%d))", KBlobLen);
	err = sqlite3_exec(TheDb, sql, 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_prepare_v2(TheDb, KSqlFmt, -1, &TheStmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST(TheStmt != 0);

	sql2 = sqlite3_sql(TheStmt);
	TEST(sql2 != NULL);
	err = strcmp(sql2, KSqlFmt);
	TEST2(err, 0);

	err = sqlite3_bind_zeroblob(TheStmt, 1, KBlobLen);
	TEST2(err, SQLITE_OK);

	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_DONE);

	err = sqlite3_finalize(TheStmt);
	TEST2(err, SQLITE_OK);
	TheStmt = 0;

	/* Open the blob and write to it */
	
	err = sqlite3_blob_open(TheDb, "main", "BlobTbl", "B", 1, 1, &blob);
	TEST2(err, SQLITE_OK);
	TEST(blob != 0);

	bytes = sqlite3_blob_bytes(blob);
	TEST2(bytes, KBlobLen);

	err = sqlite3_blob_write(blob, KBlobData, KBlobLen, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_blob_close(blob);
	TEST2(err, SQLITE_OK);
	blob = 0;

	/* Open the blob and read from it */

	err = sqlite3_blob_open(TheDb, "main", "BlobTbl", "B", 1, 1, &blob);
	TEST2(err, SQLITE_OK);
	TEST(blob != 0);

	bytes = sqlite3_blob_bytes(blob);
	TEST2(bytes, KBlobLen);

	err = sqlite3_blob_read(blob, sql, KBlobLen, 0);
	TEST2(err, SQLITE_OK);
	sql[bytes] = 0;

	err = sqlite3_blob_close(blob);
	TEST2(err, SQLITE_OK);
	blob = 0;

	err = strcmp(sql, KBlobData);
	TEST2(err, 0);

	err = sqlite3_exec(TheDb, "DROP TABLE BlobTbl", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4040
@SYMTestCaseDesc		SQLITE3 - mutex API tests.
						List of called SQLITE3 functions:
						 - sqlite3_mutex_alloc;
						 - sqlite3_mutex_enter;
						 - sqlite3_mutex_free;
						 - sqlite3_mutex_held;
						 - sqlite3_mutex_leave;
						 - sqlite3_mutex_notheld;
						 - sqlite3_mutex_try;
@SYMTestPriority		High
@SYMTestActions			SQLITE3 - mutex API tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void TestSqliteMutexApi()
	{
	sqlite3_mutex* mtx = 0;
	int err;
	int type;
	
	TEST(TheDb != 0);
		
	for(type=SQLITE_MUTEX_FAST;type<=SQLITE_MUTEX_STATIC_LRU2;++type)
		{
		mtx = sqlite3_mutex_alloc(type);
		TEST(mtx != NULL);

		err = sqlite3_mutex_try(mtx);
		TEST(err == SQLITE_BUSY || err == SQLITE_OK);

		sqlite3_mutex_enter(mtx);

		sqlite3_mutex_leave(mtx);
		
		if(type <= SQLITE_MUTEX_RECURSIVE)
			{
			sqlite3_mutex_free(mtx);
			}
		mtx = 0;
		}
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4004
@SYMTestCaseDesc		Database handle SQLITE3 tests.
						List of called SQLITE3 functions:
						 - sqlite3_config;
						 - sqlite3_initialize;
						 - sqlite3_threadsafe;
						 - sqlite3_vfs_find;
						 - sqlite3_open;
						 - sqlite3_db_config;
						 - sqlite3_libversion;
						 - sqlite3_libversion_number;
						 - sqlite3_set_authorizer;
						 - sqlite3_commit_hook;
						 - sqlite3_rollback_hook;
						 - sqlite3_update_hook;
						 - sqlite3_close;
						 - sqlite3_shutdown;
@SYMTestPriority		High
@SYMTestActions			Database handle SQLITE3 tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TestSqliteApi()
	{
	void* prev = 0;
	const char* libverstr = 0;
	int libvernum = 0;
	int err;
	int threadSafe = -1;
	sqlite3_vfs* vfs = 0;

	TEST(!TheDb);
	
	TestStart("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4004: Test \"sqlite3_config()\"");
	err = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
	TEST2(err, SQLITE_OK);
	
	TestNext("Test \"sqlite3_initialize()\"");
	err = sqlite3_initialize();
	TEST2(err, SQLITE_OK);

	TestNext("Test \"sqlite3_threadsafe()\"");
	threadSafe = sqlite3_threadsafe();
	PrintI("SQLITE_THREADSAFE=%d\r\n", threadSafe);
	
	vfs = sqlite3_vfs_find(0);
	TEST(vfs != NULL);
	PrintS("Vfs name=\"%s\"\r\n", vfs->zName);
	
	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);
	
	err = sqlite3_db_config(TheDb, SQLITE_DBCONFIG_LOOKASIDE, 0, 128, 100);
	TEST2(err, SQLITE_OK);
	
	libverstr = sqlite3_libversion();
	libvernum = sqlite3_libversion_number();
	PrintSI("SQLITE version: \"%s\", Number: %d\r\n", libverstr, libvernum);
	
	err = sqlite3_set_authorizer(TheDb, &authorizer_callback, 0);
	TEST2(err, SQLITE_OK);
	
	prev = sqlite3_commit_hook(TheDb, &commit_hook, 0);
	TEST(!prev);
	prev = sqlite3_rollback_hook(TheDb, &rollback_hook, 0);
	TEST(!prev);
	prev = sqlite3_update_hook(TheDb, &update_hook, 0);
	TEST(!prev);
	
	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4001: Test \"sqlite3\" handle API");
	TestExec();
	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4002: Test \"sqlite3_stmt\" handle API-1");
	TestStatement1();
	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4003: Test \"sqlite3_stmt\" handle API-2");
	TestStatement2();
	TestNext("@SYMTestCaseID:PDS-SQLITE3-UT-4038: Test more sqlite3 API");
	TestSqliteApi2();
	TestNext("@SYMTestCaseID:PDS-SQLITE3-UT-4039: Test blob API");
	TestSqliteBlobApi();
	TestNext("@SYMTestCaseID:PDS-SQLITE3-UT-4040: Test mutex API");
	TestSqliteMutexApi();

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	
	TestNext("Test \"sqlite3_shutdown()\"");
	err = sqlite3_shutdown();
	TEST2(err, SQLITE_OK);
	
	err = remove(TheTestDbName);
	TEST2(err, 0);
	}

static void CreateTestDb()
	{
	int err;
	
	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	err = sqlite3_exec(TheDb, "CREATE TABLE A(F1 INTEGER, F2 BIGINT, F3 REAL, F4 TEXT, F5 BLOB)", &exec_callback, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(1, 1234567891234, 56.12, 'TEXT', x'313233343536')", &exec_callback, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	}
	
/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4005
@SYMTestCaseDesc		Two database connections in the same thread - "read operations" test. 
						The test creates two connections to the same database in the same thread.
						Both connections prepare a SELECT statement and call sqlite3_step() each
						thus testing that the file locking in the OS porting layer works properly
						(the SQLite library permits multiple connections to read from the database simultaneously,
						using a shared file locking mode).
						List of called SQLITE3 functions:
						 - sqlite3_open;
						 - sqlite3_prepare;
						 - sqlite3_step;
						 - sqlite3_finalize;
						 - sqlite3_close;
@SYMTestPriority		High
@SYMTestActions			Two database connections in the same thread - "read operations" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TwoReadersTest()
	{
	int err;
  	const char* tail = 0;

	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4005: Test two readers");
  	
	CreateTestDb();

	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	err = sqlite3_open(TheTestDbName, &TheDb2);
	TEST2(err, SQLITE_OK);
	TEST(TheDb2 != 0);

	/* ------------- */
	err = sqlite3_prepare(TheDb, "SELECT * FROM A", -1, &TheStmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)TheStmt);
	TEST(!tail || strlen(tail) == 0);

	err = sqlite3_prepare(TheDb2, "SELECT * FROM A", -1, &TheStmt2, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)TheStmt2);
	TEST(!tail || strlen(tail) == 0);

	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);

	err = sqlite3_step(TheStmt2);
	TEST2(err, SQLITE_ROW);
	
	/* ------------- */

	(void)sqlite3_finalize(TheStmt2);
	TheStmt2 = 0;
	
	(void)sqlite3_finalize(TheStmt);
	TheStmt = 0;

	err = sqlite3_close(TheDb2);
	TEST2(err, SQLITE_OK);
	TheDb2 = 0;

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	
	(void)remove(TheTestDbName);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4006
@SYMTestCaseDesc		Two database connections in the same thread - "write operations" test.
						The test creates two connections to the same database in the same thread.
						Both connections attempt to execute INSERT statements using an explicit transaction
						thus testing that the file locking in the OS porting layer works properly
						(the SQLite library permits only one connection at a time to write to the database).
						List of called SQLITE3 functions:
						 - sqlite3_open;
						 - sqlite3_exec;
						 - sqlite3_free;
						 - sqlite3_close;
						Test case steps:
						 - Two connections to the same database created;
						 - Both connections begin a deferred transaction;
						 - The first connection executes an INSERT statement. The database file should be locked
						   for a writing by this connection;
						 - The second connection attempts to execute an INSERT statement too. The operation should fail,
						   because the database file has been locked by the first connection;
						 - The first connection completes the transaction executing a ROLLBACK statement;
						 - The second connection makes second attempt to execute the INSERT statement. The execution should 
						   complete without errors;
@SYMTestPriority		High
@SYMTestActions			Two database connections in the same thread - "write operations" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TwoWritersTest()
	{
	int err;
	char* errmsg = 0;

	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4006: Test two writers");
  	
	CreateTestDb();
	
	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	err = sqlite3_open(TheTestDbName, &TheDb2);
	TEST2(err, SQLITE_OK);
	TEST(TheDb2 != 0);

	/* ------------- */
	Print("Two database connections, begin a transaction in each of them.\r\n");
	err = sqlite3_exec(TheDb, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb2, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	Print("Connection 1. Execute an \"INSERT\" statement. Success.\r\n");
	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, &errmsg);
	if(errmsg)
		{
		PrintSI("Err msg: %s. Err: %d.\r\n", errmsg, err);
		sqlite3_free(errmsg);
		}
	errmsg = 0;
	TEST2(err, SQLITE_OK);
	
	Print("Connection 2. Execute an \"INSERT\" statement. Failure. The database is locked.\r\n");
	err = sqlite3_exec(TheDb2, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, &errmsg);
	if(errmsg)
		{
		PrintSI("*** %s. Err: %d.\r\n", errmsg, err);
		sqlite3_free(errmsg);
		}
	errmsg = 0;
	TEST2(err, SQLITE_BUSY);

	Print("Connection 1. Rollback. The database is unlocked.\r\n");
	err = sqlite3_exec(TheDb, "ROLLBACK", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	Print("Connection 2. Execute an \"INSERT\" statement. Success.\r\n");
	err = sqlite3_exec(TheDb2, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, &errmsg);
	if(errmsg)
		{
		PrintSI("*** %s. Err: %d.\r\n", errmsg, err);
		sqlite3_free(errmsg);
		}
	errmsg = 0;
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb2, "ROLLBACK", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	Print("Close database connections.\r\n");
	/* ------------- */
	
	err = sqlite3_close(TheDb2);
	TEST2(err, SQLITE_OK);
	TheDb2 = 0;

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	
	(void)remove(TheTestDbName);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

static void* ThreadFunc(void* pname)
	{
	int records = 0, err, i;
	sqlite3* db;
	char* errmsg = 0;
	int threadIdx = -1;
	const int KRecordsCount = 500;
	const int KCommitRecordsCount = 2;
	
	for(i=0;i<KTestThreadCount;++i)
		{
		if(strcmp(pname, KThreadNames[i]) == 0)
			{
			threadIdx = i;
			break;
			}
		}
	
	srand((unsigned)&ThreadFunc);
	
	PrintS("Thread \"%s\" - begin\r\n", (char*)pname);
	err = sqlite3_open(TheTestDbName, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != 0);
	
	while(records < KRecordsCount)
		{
        if((records % 10) == 0)
            {
            PrintSI("Thread \"%s\", %d records.\r\n", (char*)pname, records);
            }
		err = sqlite3_exec(db, "BEGIN", 0, 0, &errmsg);
		if(err == SQLITE_OK)
			{
			err = sqlite3_exec(db, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, &errmsg);
			if(err == SQLITE_OK)
				{
				err = sqlite3_exec(db, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, &errmsg);
				if(err == SQLITE_OK)
					{
					err = sqlite3_exec(db, "COMMIT", 0, 0, &errmsg);
					}
				}
			}
		TEST(err == SQLITE_OK || err == SQLITE_BUSY);
		if(err == SQLITE_OK)
			{
			TheInsertRecCnt[threadIdx]	+= KCommitRecordsCount;
			records += KCommitRecordsCount;
			}
		else if(err == SQLITE_BUSY)
			{
			++TheLockErrCnt[threadIdx];
			(void)sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
			if(errmsg)
				{
                char fmt[100];
                strcpy(fmt, "Thread \"");
                strcat(fmt, (char*)pname);
                strcat(fmt, "\". Err msg: %s. Err: %d.\r\n");
				PrintSI(fmt, errmsg, err);
				sqlite3_free(errmsg);
				errmsg = 0;
				}
			usleep((rand() % 3000) + 500);
			}
		}

	err = sqlite3_close(db);
	TEST2(err, SQLITE_OK);
	
	PrintS("Thread \"%s\" - end\r\n", (char*)pname);
	return &TheInsertRecCnt[threadIdx];
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4007
@SYMTestCaseDesc		Two database connections from different threads - "write operations" test.
						The test creates two connections to the same database from two different threads.
						Both connections attempt to execute INSERT statements using an explicit transaction
						thus testing that the file locking in the OS porting layer works properly
						(the SQLite library permits only one connection at a time to write to the database).
						The threads do not use any thread synchronisation objects (mutexes, critical sections, etc...).
						List of called SQLITE3 functions:
						 - sqlite3_open;
						 - sqlite3_exec;
						 - sqlite3_free;
						 - sqlite3_close;
						Test case steps:
						 - Two threads created;
						 - Each thread opens a connection to the same database;
						 - No thread synchronisation is used;
						 - Each database connection attempts to insert records in a loop using an explicit transaction;
						 - If the database file is currently locked by the other database connection, the INSERT operation
						   should fail with SQLITE_BUSY error. Otherwise the operation should complete successfully.
@SYMTestPriority		High
@SYMTestActions			Two database connections from different threads - "write operations" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void ThreadsTest()
	{
	pthread_t threadIds[KTestThreadCount];
	int err, i;
	
	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4007: Test two writers in two threads");

	CreateTestDb();
	
	for(i=0;i<KTestThreadCount;++i)
		{
		err = pthread_create(&threadIds[i], 0, &ThreadFunc, (void*)KThreadNames[i]);
		TEST2(err, 0);
		}
	
	for(i=0;i<KTestThreadCount;++i)
		{
		(void)pthread_join(threadIds[i], 0);
		PrintIII("Thread %d records written: %d, \"db locked\" errors: %d\r\n", i + 1, TheInsertRecCnt[i], TheLockErrCnt[i]);
		}
	
	Print("Test two writers in two threads - end\r\n");
	(void)remove(TheTestDbName);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */
	
static void* ThreadFunc1(void* parg)
	{
	int err;
	sqlite3* db;

	UNUSED_ARG(parg);
	PrintS("Thread \"%s\" - begin\r\n", KThreadNames[0]);
	
	err = sqlite3_open(TheTestDbName, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != 0);
	
	(void)sem_wait(&TheSemaphores[0]);/* Wait for a notification from the main thread to begin */
	
	err = sqlite3_exec(db, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(db, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	(void)sem_post(&TheSemaphores[1]);/* The database is locked now. Notify thread 2 to attempt an INSERT operation */
	(void)sem_wait(&TheSemaphores[0]);/* Wait for a notification from thread 2 to continue with the COMMIT operation */

	err = sqlite3_exec(db, "COMMIT", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	(void)sem_post(&TheSemaphores[1]);/* The database is unlocked. Notify thread 2 to attempt the INSERT operation again */
	
	err = sqlite3_close(db);
	TEST2(err, SQLITE_OK);
	
	PrintS("Thread \"%s\" - end\r\n", KThreadNames[0]);
	return 0;
	}

static void* ThreadFunc2(void* parg)
	{
	int err;
	sqlite3* db;
	
	UNUSED_ARG(parg);
	PrintS("Thread \"%s\" - begin\r\n", KThreadNames[1]);
	
	err = sqlite3_open(TheTestDbName, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != 0);
	
	(void)sem_wait(&TheSemaphores[1]);/* Wait for a notification from thread 1 to attempt an INSERT operation */
	
	err = sqlite3_exec(db, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, 0);
	TEST2(err, SQLITE_BUSY);
	
	(void)sem_post(&TheSemaphores[0]);/* The database is locked. Notify thread 1 to commit and unlock the database */
	(void)sem_wait(&TheSemaphores[1]);/* Wait for a notification from thread 1 to attempt the INSERT operation again */

	err = sqlite3_exec(db, "INSERT INTO A VALUES(0,0,0.0,'',x'00')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_close(db);
	TEST2(err, SQLITE_OK);
	
	PrintS("Thread \"%s\" - end\r\n", KThreadNames[1]);
	return 0;
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4008
@SYMTestCaseDesc		Two database connections from different threads - synchronised "write operations" test.
						The test creates two connections to the same database from two different threads.
						Both connections attempt to execute INSERT statements using an explicit transaction
						thus testing that the file locking in the OS porting layer works properly
						(the SQLite library permits only one connection at a time to write to the database).
						The threads use semaphores in order to synchronise the INSERT statements execution.
						List of called SQLITE3 functions:
						 - sqlite3_open;
						 - sqlite3_exec;
						 - sqlite3_free;
						 - sqlite3_close;
						Test case steps:
						 - Two threads created;
						 - Each thread opens a connection to the same database;
						 - The first thread begins an explicit transaction and inserts one record to the database. 
						   The database file is locked. The first thread notifies the second thread and blocks
						   waiting for a notification from the second thread;
						 - The second thread attempts to execute an INSERT statement. Since the database file is locked 
						   by the first thread the INSERT statement execution fails with SQLITE_BUSY. The second thread 
						   sends a notification to the first thread to continue and blocks waiting for a notification;
						 - The first thread commits the transaction thus unlocking the database file. A notification is
						   sent to the second thread;
						 - The second thread makes second attempt to INSERT a record and this time the operation should 
						   complete successfully;
@SYMTestPriority		High
@SYMTestActions			Two database connections from different threads - synchronised "write operations" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TwoSyncThreadsTest()
	{
	const int KTestThreadCount = 2;
	int i, err;
	pthread_t threadIds[KTestThreadCount] = {0};
	thread_begin_routine threadFuncs[KTestThreadCount] = {&ThreadFunc1, &ThreadFunc2};

	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4008: Test two writers in two synchronized threads");
	
	CreateTestDb();

	for(i=0;i<KTestThreadCount;++i)
		{
		err = sem_init(&TheSemaphores[i], 0, 0);
		TEST2(err, 0);
		}
	
	for(i=0;i<KTestThreadCount;++i)
		{
		err = pthread_create(&threadIds[i], 0, threadFuncs[i], 0);
		TEST2(err, 0);
		}
	
	(void)sem_post(&TheSemaphores[0]);/* Notify thread 1 to begin */

	for(i=0;i<KTestThreadCount;++i)
		{
		(void)pthread_join(threadIds[i], 0);
		}
	
	(void)remove(TheTestDbName);
	
	for(i=0;i<KTestThreadCount;++i)
		{
		err = sem_destroy(&TheSemaphores[i]);
		TEST2(err, 0);
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4009
@SYMTestCaseDesc		Two database connections "RFileBuf64" test.
						This test verifies that the file buffer used by the SQLITE OS porting layer works properly.
						Test steps (in time order):
						1) Two connections to the same database are created.
						2) The first connection creates a table and executes a couple of INSERT statements.
						3) The second connection prepares a SELECT statement against the table, created in (2)
						Now, if the file buffer in the OS porting layer does not work as expected, step (3) fails,
						because at step (1) the database size is 0 and that is what the file buffer, used by the second 
						connection, "knows". The "prepare SQL statement" call will fail with a "no such table" error.
						But if the file buffer works properly, the buffer will be flushed during the "lock file" operation
						inside the OS porting layer and reloaded with the database data later.
@SYMTestPriority		High
@SYMTestActions			Two database connections "RFileBuf64" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void TwoConnectionsTest(void)
	{
	int err;
  	const char* tail = 0;
  	const char* errmsg = 0;

	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4009: Two database connections test (OS porting layer, RFileBuf64 related)");
	(void)remove(TheTestDbName);
	
	TEST(!TheDb);
	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	TEST(!TheDb2);
	err = sqlite3_open(TheTestDbName, &TheDb2);
	TEST2(err, SQLITE_OK);
	TEST(TheDb2 != 0);
	
	err = sqlite3_exec(TheDb, "CREATE TABLE t1(x);INSERT INTO t1 VALUES(1);INSERT INTO t1 VALUES(2);SELECT * FROM t1", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	TEST(!TheStmt);
	err = sqlite3_prepare(TheDb2, "SELECT * FROM t1", -1, &TheStmt, &tail);
	if(err != SQLITE_OK)
		{
		errmsg = sqlite3_errmsg(TheDb2);
		PrintSI("*** Stmt prepare err msg: \"%s\". Error: %d\r\n", errmsg, err);
		}
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)TheStmt);
	TEST(!tail || strlen(tail) == 0);

	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);

	err = sqlite3_finalize(TheStmt);
	TEST2(err, SQLITE_OK);
	TheStmt = 0;

	err = sqlite3_close(TheDb2);
	TEST2(err, SQLITE_OK);
	TheDb2 = 0;

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	(void)remove(TheTestDbName);
	}

static void UdfInsertFunc(sqlite3_context* aCtx, int aCnt, sqlite3_value** aValues)
	{
	int err;
  	const char* tail = 0;
  	sqlite3* db = 0;
  	
	TEST2(aCnt, 1);
	
	db = sqlite3_context_db_handle(aCtx);/* to test that sqlite3_context_db_handle() can be called */
	TEST(db != 0);
	
	TEST(!TheStmt);
	err = sqlite3_prepare(TheDb, "INSERT INTO t1(x) VALUES(:Val)", -1, &TheStmt, &tail);
	if(err == SQLITE_OK)
		{
		err = sqlite3_bind_value(TheStmt, 1, aValues[0]);
		if(err == SQLITE_OK)
			{
			err = sqlite3_step(TheStmt);
			}
		}
	(void)sqlite3_finalize(TheStmt);
	TheStmt = 0;
	
	sqlite3_result_int(aCtx, err);		
	}

/**
@SYMTestCaseID			SYSLIB-SQLITE3-UT-4027
@SYMTestCaseDesc		sqlite3_bind_value() and sqlite3_column_value() test.
						List of covered SQLITE3 functions:
						 - sqlite3_bind_value();
						 - sqlite3_column_value();
						Test case steps: 
						 - The test creates and calls a user defined function - UdfInsertFunc().
						 - The user defined function prepares and executes an INSERT statement,
							where the sqlite3_bind_value() call is used to bind the passed from the caller
							integer column value.
						 - After the user defined function call completes, the test prepares a SELECT
						    statement to verify the just inserted column value using sqlite3_column_int()
@SYMTestPriority		High
@SYMTestActions			sqlite3_bind_value() and sqlite3_column_int() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8782
*/
static void UdfTest()
	{
	int err;
	int val;
  	const char* tail = 0;
  	const int KTestColumnValue = 11234;
  	char sqlBuf[100];

	TestNext("@SYMTestCaseID:SYSLIB-SQLITE3-UT-4027: User defined function test");

	(void)remove(TheTestDbName);
	TEST(!TheDb);
	err = sqlite3_open(TheTestDbName, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	err = sqlite3_exec(TheDb, "CREATE TABLE t1(x INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_create_function(TheDb, "UdfInsert", 1, SQLITE_UTF8, NULL, &UdfInsertFunc, NULL, NULL);
	TEST2(err, SQLITE_OK);

	//Execute an INSERT statement via UDF
	TEST(!TheStmt2);
	sprintf(sqlBuf, "SELECT UdfInsert(%d)", KTestColumnValue);
	err = sqlite3_prepare(TheDb, sqlBuf, -1, &TheStmt2, &tail);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt2);
	TEST2(err, SQLITE_ROW);
	val = sqlite3_column_int(TheStmt2, 0);
	TEST2(val, SQLITE_DONE);
	(void)sqlite3_finalize(TheStmt2);
	TheStmt2 = 0;

	//Verify the inserted column value
	TEST(!TheStmt2);
	err = sqlite3_prepare(TheDb, "SELECT x FROM t1", -1, &TheStmt2, &tail);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt2);
	TEST2(err, SQLITE_ROW);
	val = sqlite3_column_int(TheStmt2,0);
	TEST2(val, KTestColumnValue);
	(void)sqlite3_finalize(TheStmt2);
	TheStmt2 = 0;

	err = sqlite3_close(TheDb);
	TEST2(err, SQLITE_OK);
	TheDb = 0;
	(void)remove(TheTestDbName);
	}

/* ///////////////////////////////////////////////////////////////////////////////////// */

int main(int argc, void** argv)
	{
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	TestOpen("t_sqliteapi test");
	TestTitle();

	TestHeapMark();
		
	TestEnvCreate();
	
	TestSqliteApi();
	TwoReadersTest();
	TwoWritersTest();
	ThreadsTest();
	TwoSyncThreadsTest();
	TwoConnectionsTest();
	UdfTest();
	
	TestEnvDestroy();

	TestHeapMarkEnd();

	TestEnd();
	TestClose();
	
	return 0;	
	}
