// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32buf.h>
#include <bautils.h>
#include <sqldb.h>
#include <stdio.h>
#include <stdlib.h>
#include "SqlResourceTester.h"      //TSqlResourceTester
#include "sqlite3.h"
#include "SqliteSymbian.h"

//In order to be able to compile the test, the following variables are defined (used inside the OS porting layer, 
//when _SQLPROFILER macro is defined)
#ifdef _SQLPROFILER
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;
#endif

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqloom6 test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KDbFile, "c:\\test\\t_sqloom6.db");

static RSqlDatabase TheDb;
static RSqlStatement TheStmt;

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	TheStmt.Close();
	TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
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
		RDebug::Print(_L("*** Expresssion evaluated to false\r\n"));
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MarkHandles()
    {
    RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
    }

static void MarkAllocatedCells()
    {
    TheAllocatedCellsCount = User::CountAllocCells();
    }

static void CheckAllocatedCells()
    {
    TInt allocatedCellsCount = User::CountAllocCells();
    TEST2(allocatedCellsCount, TheAllocatedCellsCount);
    }

static void CheckHandles()
    {
    TInt endProcessHandleCount;
    TInt endThreadHandleCount;
    
    RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

    TEST2(TheProcessHandleCount, endProcessHandleCount);
    TEST2(TheThreadHandleCount, endThreadHandleCount);
    }

static void OomPreStep(TInt aFailingAllocationNo)
    {
    MarkHandles();
    MarkAllocatedCells();
    __UHEAP_MARK;
    TSqlResourceTester::Mark();
    TSqlResourceTester::SetHeapFailure(RHeap::EBurstFailNext, aFailingAllocationNo);
    }

static void OomPostStep()
    {
    __UHEAP_RESET;
    TSqlResourceTester::SetHeapFailure(RHeap::ENone, 0);
    TSqlResourceTester::Check();
    CheckAllocatedCells();
    CheckHandles();
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//Creates a UTF8 encoded database with:
// - One table with three colums: A(ColumnName1234567890, Col2, Col3);
// - One record in the table with values: ('A1234567890', 'A12345', '');
void CreateTestDb(const TDesC& aDbName)
    {
    TBuf8<100> dbName8;
    dbName8.Copy(aDbName);
    sqlite3* db = 0;
    int rc = sqlite3_open((const char*)dbName8.PtrZ(), &db);
    TEST2(rc, SQLITE_OK);
    rc = sqlite3_exec(db, "CREATE TABLE A(ColumnName1234567890 TEXT, Col2 LONG TEXT, Col3 SMALL TEXT)", 0, 0, 0);
    TEST2(rc, SQLITE_OK);
    rc = sqlite3_exec(db, "INSERT INTO A VALUES('A1234567890', 'A12345', '')", 0, 0, 0);
    TEST2(rc, SQLITE_OK);
    sqlite3_close(db);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4176
@SYMTestCaseDesc        RSqlStatement::ColumnName() OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs RSqlStatement::ColumnName() in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the ColumnName() call behaves correctly 
                         when the related sqlite3_column_name16() call performed by the SQL server fails 
                         with "no memory".
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void ColumnNameOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the RSqlStatement::ColumnName() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A"));
        if(err != KErrNone)
            {
            goto LabelOomPostStep;
            }
        
        TPtrC name;
        err = TheStmt.ColumnName(0, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L("ColumnName1234567890"));
        
        err = TheStmt.ColumnName(1, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L("Col2"));
        
        err = TheStmt.ColumnName(2, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L("Col3"));
        
LabelStmtClose:        
        TheStmt.Close();
        
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===RSqlStatement::ColumnName() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4177
@SYMTestCaseDesc        RSqlStatement::ParameterName() OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs RSqlStatement::ParameterName() in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the ParameterName() call behaves correctly 
                         if the related sqlite3_bind_parameter_name() call performed by the SQL server fails 
                         with "no memory".
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void ParameterNameOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the RSqlStatement::ParameterName() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A WHERE Col2 != :Prm1234567890 AND Col3 != :Prm2 AND ColumnName1234567890 != ?"));
        if(err != KErrNone)
            {
            goto LabelOomPostStep;
            }
        
        TPtrC name;
        err = TheStmt.ParameterName(0, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L(":Prm1234567890"));
        
        err = TheStmt.ParameterName(1, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L(":Prm2"));
        
        err = TheStmt.ParameterName(2, name);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(name == _L("?2"));
        
LabelStmtClose:        
        TheStmt.Close();
        
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===RSqlStatement::ColumnName() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4178
@SYMTestCaseDesc        RSqlStatement::ColumnText() OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs RSqlStatement::ColumnText() in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the ColumnText() call behaves correctly 
                         when the related sqlite3_column_text16() call performed by the SQL server fails 
                         with "no memory" (or the sqlite3_column_bytes16() call).
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void ColumnTextOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the RSqlStatement::ColumnText() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A"));
        if(err != KErrNone)
            {
            goto LabelOomPostStep;
            }
        err = TheStmt.Next();
        if(err != KSqlAtRow)
            {
            goto LabelStmtClose;
            }
        
        TPtrC data;
        err = TheStmt.ColumnText(0, data);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(data == _L("A1234567890"));
        
        err = TheStmt.ColumnText(1, data);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(data == _L("A12345"));

        err = TheStmt.ColumnText(2, data);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST(data == _L(""));

LabelStmtClose:        
        TheStmt.Close();
                    
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===RSqlStatement::ColumnText() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4179
@SYMTestCaseDesc        RSqlColumnReadStream OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs RSqlColumnReadStream in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the RSqlColumnReadStream APIs behave correctly 
                         when the related sqlite3_column_text16() call performed by the SQL server fails 
                         with "no memory" (or the sqlite3_column_bytes16() call).
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void TextColumnReadStreamOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the RSqlColumnReadStream::ReadL() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A"));
        if(err != KErrNone)
            {
            goto LabelOomPostStep;
            }
        err = TheStmt.Next();
        if(err != KSqlAtRow)
            {
            goto LabelCloseStmt;
            }
        
        RSqlColumnReadStream strm;
        err = strm.ColumnText(TheStmt, 0);
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TBuf<50> data;
        TRAP(err, strm.ReadL(data, 11));
        strm.Close();
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TEST(data == _L("A1234567890"));

        err = strm.ColumnText(TheStmt, 1);
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TRAP(err, strm.ReadL(data, 6));
        strm.Close();
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TEST(data == _L("A12345"));

        err = strm.ColumnText(TheStmt, 2);
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TInt len = -1;
        TRAP(err, len = strm.Source()->SizeL());//The column value is with 0 length
        strm.Close();
        if(err != KErrNone)
            {
            goto LabelCloseStmt;
            }
        TEST2(len, 0);

LabelCloseStmt:            
        TheStmt.Close();
        
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===RSqlColumnReadStream OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4180
@SYMTestCaseDesc        TSqlScalarFullSelectQuery::SelectTextL() OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs TSqlScalarFullSelectQuery::SelectTextL() in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the SelectTextL() call behaves correctly 
                         when the related sqlite3_column_text16() call performed by the SQL server fails 
                         with "no memory" (or the sqlite3_column_bytes16() call).
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void ScalarColumnTextOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the TSqlScalarFullSelectQuery::SelectTextL() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        TSqlScalarFullSelectQuery query(TheDb);
        TBuf<50> data;
        TRAP(err, query.SelectTextL(_L("SELECT ColumnName1234567890 FROM A"), data));
        if(err != KErrNone)
            {
            goto LabelOomPostStep; 
            }
        TEST(data == _L("A1234567890"));

        TRAP(err, query.SelectTextL(_L("SELECT Col2 FROM A"), data));
        if(err != KErrNone)
            {
            goto LabelOomPostStep; 
            }
        TEST(data == _L("A12345"));
        
        TRAP(err, query.SelectTextL(_L("SELECT Col3 FROM A"), data));
        if(err != KErrNone)
            {
            goto LabelOomPostStep; 
            }
        TEST(data == _L(""));
        
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===TSqlScalarFullSelectQuery::SelectTextL() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4181
@SYMTestCaseDesc        RSqlStatement::DeclaredColumnType() OOM test.
@SYMTestPriority        High
@SYMTestActions         The test runs RSqlStatement::DeclaredColumnType() in an OOM simulation loop.
                        The difference betwee this test case and the similar test case in t_sqloom2 is that here:
                         - burst OOM simulation is used;
                         - UTF8 encoded database is used;
                         - only SQL server side OOM simulation is performed;
                         The purpose of the test is to verify that the DeclaredColumnType() call behaves correctly 
                         when the related sqlite3_column_name16() call performed by the SQL server fails 
                         with "no memory".
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145033
*/  
void DeclaredColumnTypeOomTest()
    {
    //This is not really a full OOM test, because the SQL server counts only the number of active statement and 
    //stream objects, the allocated memory cells are not counted.
    //The reason that the allocated memory cells are not counted, and in case of an OOM failure - checked, is
    //because the SQL server can make some per-connection memory allocations (cache pages, etc.)
    //and they will not be deallocated when the statement object is closed. 
    //But the result of the RSqlStatement::DeclaredColumnType() operation is checked. If there is a failed memory
    //allocation on the server side, the returned column name can be NULL and that will be tested.
    
    (void)RSqlDatabase::Delete(KDbFile);
    
    TheTest.Printf(_L("Iteration:\r\n"));

    CreateTestDb(KDbFile);//Creates UTF8 encoded database with one table with one record.
    
    TInt err = TheDb.Open(KDbFile);
    TEST2(err, KErrNone);
    
    TInt failingAllocationNo = 0;
    err = KErrNoMemory;
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        
        err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A"));
        if(err != KErrNone)
            {
            goto LabelOomPostStep;
            }
        
        TSqlColumnType colType = ESqlNull;
        err = TheStmt.DeclaredColumnType(0, colType);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST2(colType, ESqlText);
        
        colType = ESqlNull;
        err = TheStmt.DeclaredColumnType(1, colType);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST2(colType, ESqlText);
        
        colType = ESqlNull;
        err = TheStmt.DeclaredColumnType(2, colType);
        if(err != KErrNone)
            {
            goto LabelStmtClose;
            }
        TEST2(colType, ESqlText);
        
LabelStmtClose:        
        TheStmt.Close();
        
LabelOomPostStep:        
        OomPostStep();
        }

    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbFile);
    
    TEST2(err, KErrNone);   
    TheTest.Printf(_L("\r\n===RSqlStatement::DeclaredColumnType() OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

void DoTests()
	{
    TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-CT-4176 RSqlStatement::ColumnName() OOM test"));
    ColumnNameOomTest();

    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4177 RSqlStatement::ParameterName() OOM test"));
    ParameterNameOomTest();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4178 RSqlStatement::ColumnText() OOM test"));
    ColumnTextOomTest();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4179 RSqlColumnReadStream OOM test"));
    TextColumnReadStreamOomTest();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4180 TSqlScalarFullSelectQuery::SelectTextL() OOM test"));
    ScalarColumnTextOomTest();

    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4181 RSqlStatement::DeclaredColumnType() OOM test"));
    DeclaredColumnTypeOomTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DoTests();
	DestroyTestEnv();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
