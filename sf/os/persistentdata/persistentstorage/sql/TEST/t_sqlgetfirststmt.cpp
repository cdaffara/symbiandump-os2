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

#include <e32debug.h>
#include <e32test.h>
#include <f32file.h>
#include "sqlite3.h"
#include <stdlib.h>
#include "SqliteSymbian.h"
#include "SqlAssert.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_SqlGetFirstStmt test"));
RFs TheFs;
sqlite3* TheSqliteDb;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDatabase, "c:\\test\\DEF104744.db");
_LIT8(KTestDatabaseZ, "c:\\test\\DEF104744.db\x0");

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DestroyTestEnv()
	{
	if(TheSqliteDb)
		{
		sqlite3_close(TheSqliteDb);	
		TheSqliteDb = NULL;
		}
	(void)TheFs.Delete(KTestDatabase);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

extern TPtrC GetFirstSqlStmt(TPtr& aString);

#define PTR_ARG(buf) const_cast <TUint16*> (buf.Ptr()), buf.Length(), buf.Length()

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1628
@SYMTestCaseDesc		GetFirstSqlStmt() test
						Tests the GetFirstSqlStmt() behaviour with a set of various SQL statements.
@SYMTestPriority		High
@SYMTestActions			GetFirstSqlStmt() test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TestGetFirstSqlStmt()
	{
	TPtrC res;

	TBuf<1> b2; b2.Append(TChar(0));
	TPtr p2(PTR_ARG(b2));
	res.Set(GetFirstSqlStmt(p2));
	//Expected result: res = "\x0", p2 is NULL
	TEST(res == b2);
	TEST(!p2.Ptr());
		
	TBuf<2> b3; b3.Append(TChar(' ')); b3.Append(TChar(0));
	TPtr p3(PTR_ARG(b3));
	res.Set(GetFirstSqlStmt(p3));
	//Expected result: res = " \x0", p3 is NULL
	TEST(res == b3);
	TEST(!p3.Ptr());
	
	TBuf<7> b4(_L(";; ;  ")); b4.Append(TChar(0));
	TPtr p4(PTR_ARG(b4));
	res.Set(GetFirstSqlStmt(p4));
	//Expected result: res = "\x0", p4 = "; ;  \x0"
	TEST(res.Length() == 1 && (TInt)res[0] == 0);
	TInt accLen = res.Length();
	TEST(p4 == b4.Right(b4.Length() - accLen));

	res.Set(GetFirstSqlStmt(p4));
	//Expected result: res = "\x0", p4 = " ;  \x0"
	TEST(res.Length() == 1 && (TInt)res[0] == 0);
	accLen += res.Length();
	TEST(p4 == b4.Right(b4.Length() - accLen));
	
	res.Set(GetFirstSqlStmt(p4));
	//Expected result: res = " \x0", p4 = "  \x0"
	TEST((TInt)res[0] == (TInt)TChar(' ') && (TInt)res[1] == 0);
	accLen += res.Length();
	TEST(p4 == b4.Right(b4.Length() - accLen));
	
	res.Set(GetFirstSqlStmt(p4));
	//Expected result: res = "  \x0", p4 is NULL
	TEST((TInt)res[0] == (TInt)TChar(' ') && (TInt)res[1] == (TInt)TChar(' ') && (TInt)res[2] == 0);
	TEST(!p4.Ptr());
	
	TBuf<20> b5(_L("SELECT * FROM A")); b5.Append(TChar(0));
	TPtr p5(PTR_ARG(b5));
	res.Set(GetFirstSqlStmt(p5));
	//Expected result: res = "SELECT * FROM A\x0", p5 is NULL
	TEST(res == b5);
	TEST(!p5.Ptr());
	
	TBuf<20> b6(_L("SELECT * FROM A;")); b6.Append(TChar(0));
	TPtr p6(PTR_ARG(b6));
	res.Set(GetFirstSqlStmt(p6));
	//Expected result: res = "SELECT * FROM A\x0", p6 = "\x0"
	TEST(res == b6.Left(b6.Length() - 1));
	TEST(p6.Length() == 1 && p6[0] == 0);

	TBuf<40> b7(_L("/** Comment */ SELECT * FROM A;")); b7.Append(TChar(0));
	TPtr p7(PTR_ARG(b7));
	res.Set(GetFirstSqlStmt(p7));
	//Expected result: res = "/** Comment */ SELECT * FROM A\x0", p7 = "\x0"
	TEST(res == b7.Left(b7.Length() - 1));
	TEST(p7.Length() == 1 && p7[0] == 0);

	TBuf<40> b8(_L(" SELECT * FROM --Comment \r\n A;")); b8.Append(TChar(0));
	TPtr p8(PTR_ARG(b8));
	res.Set(GetFirstSqlStmt(p8));
	//Expected result: res = " SELECT * FROM --Comment \r\n A\x0", p8 = "\x0"
	TEST(res == b8.Left(b8.Length() - 1));
	TEST(p8.Length() == 1 && p8[0] == 0);

	TBuf<40> b9(_L("SELECT * FROM A; SELECT * FROM B")); b9.Append(TChar(0));
	TPtr p9(PTR_ARG(b9));
	res.Set(GetFirstSqlStmt(p9));
	//Expected result: res = "SELECT * FROM A\x0", p9 = " SELECT * FROM B\x0"
	TEST(res.Left(res.Length() - 1) == b9.Left(res.Length() - 1) && (TInt)res[res.Length() - 1] == 0);
	accLen = res.Length();
	TEST(p9 == b9.Right(b9.Length() - accLen));

	res.Set(GetFirstSqlStmt(p9));
	//Expected result: res = " SELECT * FROM B\x0", p9 is NULL
	TEST(res == b9.Right(b9.Length() - accLen));
	TEST(!p9.Ptr());

	//Defect INC113060	
	TBuf<255> b10(_L("UPDATE Playlist SET Name=';',Time='2007-09-20 12:31:33' WHERE UniqueId=640397473"));
	TPtr p10(PTR_ARG(b10));
	res.Set(GetFirstSqlStmt(p10));
	//Expected results: res= original string
	TEST(res.Compare(b10)==0);
	TEST(!p10.Ptr());
	
	TBuf<255> firstStmt(_L("SELECT * FROM PlayList"));firstStmt.Append(TChar(0));
	TBuf<255> b11(_L("SELECT * FROM PlayList;UPDATE Playlist SET Name=';',Time='2007-09-20 12:31:33' WHERE UniqueId=640397473"));
	TPtr p11(PTR_ARG(b11));
	res.Set(GetFirstSqlStmt(p11));
	TEST(res.Compare(firstStmt)==0);
	TEST(p11.Compare(b10)==0);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3433
@SYMTestCaseDesc		Test for DEF104744 - RSqlStatement::Next() SQL Server crashes on ORDER BY clause.
						The test creates a database with a table with 30 integer columns, then inserts 100
						records. After that, sets the soft heap limit to be very low - 10K 
						(to get sqlite3_release_memory() called by SQLITE ), creates a statement object 
						and attempts to retrieve the inserted records in descending order.
@SYMTestPriority		High
@SYMTestActions			Test for DEF104744 - RSqlStatement::Next() SQL Server crashes on ORDER BY clause.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF104744
*/
void DEF104744()
	{
	(void)TheFs.Delete(KTestDatabase);
	TheSqliteDb = NULL;
	TInt err = sqlite3_open((const char*)KTestDatabaseZ().Ptr(), &TheSqliteDb);
	TEST2(err, SQLITE_OK);
	
	_LIT8(KCreateTblSqlZ, "CREATE TABLE A1(F1 INTEGER,F2 INTEGER,F3 INTEGER,F4 INTEGER,F5 INTEGER,F6 INTEGER,F7 INTEGER,F8 INTEGER,F9 INTEGER,F10 INTEGER,F11 INTEGER,F12 INTEGER,F13 INTEGER,F14 INTEGER,F15 INTEGER,F16 INTEGER,F17 INTEGER,F18 INTEGER,F19 INTEGER,F20 INTEGER,F21 INTEGER,F22 INTEGER,F23 INTEGER,F24 INTEGER,F25 INTEGER,F26 INTEGER,F27 INTEGER,F28 INTEGER,F29 INTEGER,F30 INTEGER)\x0");
	err = sqlite3_exec(TheSqliteDb, (const char*)KCreateTblSqlZ().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);

	//Insert a 100 records
	const TInt KTestRecCnt = 100;
	_LIT8(KInsertSqlZ, "INSERT INTO A1(F1,F2 ,F3 ,F4 ,F5 ,F6 ,F7 ,F8 ,F9 ,F10,F11,F12,F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24,F25,F26,F27,F28,F29,F30) VALUES(:Prm1,:Prm2,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296,4294967296)\x0");
	sqlite3_stmt* stmt1 = NULL;
	err = sqlite3_prepare_v2(TheSqliteDb, (const char*)(KInsertSqlZ().Ptr()), -1, &stmt1, NULL);
	TEST2(err, SQLITE_OK);
	
	_LIT8(KBeginSqlZ, "BEGIN\x0");
	err = sqlite3_exec(TheSqliteDb, (const char*)KBeginSqlZ().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	for(TInt i=0;i<KTestRecCnt;++i)
		{ 
		err = sqlite3_bind_int(stmt1, 1, i);
		TEST2(err, SQLITE_OK);
		err = sqlite3_bind_int(stmt1, 2, i + 1);
		TEST2(err, SQLITE_OK);
		err = sqlite3_step(stmt1);
		TEST2(err, SQLITE_DONE);
		err = sqlite3_reset(stmt1);
		TEST2(err, SQLITE_OK);
		TInt cnt = sqlite3_changes(TheSqliteDb);
		TEST2(cnt, 1);
		}
		
	_LIT8(KCommitSqlZ, "COMMIT\x0");
	err = sqlite3_exec(TheSqliteDb, (const char*)KCommitSqlZ().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);
	sqlite3_finalize(stmt1);

	sqlite3_soft_heap_limit(10 * 1024);//Setting very low soft heap limit - 10K

	// Get the inserted record data in descending order.
	sqlite3_stmt* stmt2 = NULL;
	_LIT8(KSelectSqlZ,"SELECT * FROM A1 ORDER BY F1 DESC");
	err = sqlite3_prepare_v2(TheSqliteDb, (const char*)(KSelectSqlZ().Ptr()), -1, &stmt2, NULL);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(stmt2);
	TEST2(err, SQLITE_ROW);
	sqlite3_finalize(stmt2);

	sqlite3_close(TheSqliteDb); 
	TheSqliteDb = NULL;
	(void)TheFs.Delete(KTestDatabase);
	}

enum TStmtType {EStmt8, EStmt16};

//This function attempts to execute sqlite3_prepare_v2() or sqlite3_prepare16_v2() in an "out of memory loop".
//If the prepare call fails, the statement handle is expected to be NULL.
void StmtHandleTest(TStmtType aStmtType)
	{
	TEST(TheSqliteDb != NULL);
	for(TInt failingAllocationNo=1;;++failingAllocationNo)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failingAllocationNo);
		sqlite3_stmt* stmt = NULL;
		TInt err = SQLITE_NOMEM;
		if(aStmtType == EStmt8)
			{
			_LIT8(KSelectSqlZ,"SELECT * FROM A\x0");
			err = sqlite3_prepare_v2(TheSqliteDb, (const char*)(KSelectSqlZ().Ptr()), -1, &stmt, NULL);
			}
		else
			{
			_LIT(KSelectSqlZ,"SELECT * FROM A\x0");
			err = sqlite3_prepare16_v2(TheSqliteDb, (const char*)(KSelectSqlZ().Ptr()), -1, &stmt, NULL);
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		if(err != SQLITE_OK)
			{//The statement handle should be NULL if err is not SQLITE_OK
			TEST(!stmt);
			}
		else
			{
			TEST(stmt != NULL);
			sqlite3_finalize(stmt);
			break;
			}
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3466
@SYMTestCaseDesc		Test for DEF105444 SQL, sqlite3_prepare_v2() demonstrates non-atomic behaviour (handle leak).
						The test creates a database with a table and inserts one record.
						After that the test attempts to prepare a SELECT statement handle inside an "ouut of memory"
						loop. If the prepare operation fails, the statement handle should be NULL.
@SYMTestPriority		High
@SYMTestActions			DEF105444 SQL, sqlite3_prepare_v2() demonstrates non-atomic behaviour (handle leak) 
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF105444
*/
void DEF105444()
	{
	(void)TheFs.Delete(KTestDatabase);
	TheSqliteDb = NULL;
	TInt err = sqlite3_open((const char*)KTestDatabaseZ().Ptr(), &TheSqliteDb);
	TEST2(err, SQLITE_OK);
	
	_LIT8(KCreateTblSqlZ, "CREATE TABLE A(Id INTEGER)\x0");
	err = sqlite3_exec(TheSqliteDb, (const char*)KCreateTblSqlZ().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);

	_LIT8(KInsertSqlZ, "INSERT INTO A(Id) VALUES(1)\x0");
	err = sqlite3_exec(TheSqliteDb, (const char*)KInsertSqlZ().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);

	StmtHandleTest(EStmt8);
	StmtHandleTest(EStmt16);
	
	sqlite3_close(TheSqliteDb); 
	TheSqliteDb = NULL;
	(void)TheFs.Delete(KTestDatabase);
	}

void UtilFileNameTest()
	{
#ifdef _ASSERTIONS	
	_LIT(KFileName, "bbb.db");
	
	_LIT(KFileName1, "c:\\aaa\\bbb.db\x0");
	TPtrC p1 = Util::Filename(KFileName1().Ptr());
	TEST(p1 == KFileName);
	
	_LIT(KFileName2, "c:/aaa/bbb.db\x0");
	TPtrC p2 = Util::Filename(KFileName2().Ptr());
	TEST(p2 == KFileName);
	
	_LIT(KFileName3, "bbb.db\x0");
	TPtrC p3 = Util::Filename(KFileName3().Ptr());
	TEST(p3 == KFileName);
#else
	TheTest.Printf(_L(" === This test case works only if the test is built with the _ASSERTIONS macro defined!\r\n"));
#endif	
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1628 GetFirstSqlStmt() test "));
	TestGetFirstSqlStmt();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3433 DEF104744 SQL Server crashes on ORDER BY clause "));
	DEF104744();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3466 DEF105444 SQL, sqlite3_prepare_v2() demonstrates non-atomic behaviour (handle leak) "));
	DEF105444();

	TheTest.Next(_L(" Util::Filename() test"));
	UtilFileNameTest();
	}

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

TInt E32Main()
	{
	TheSqliteDb = NULL;
	
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;

	CreateTestEnv();
	TInt err = sqlite3SymbianLibInit();
	TEST2(err, KErrNone);
	DoTests();
	sqlite3SymbianLibFinalize();
	DestroyTestEnv();

	CloseSTDLIB();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
