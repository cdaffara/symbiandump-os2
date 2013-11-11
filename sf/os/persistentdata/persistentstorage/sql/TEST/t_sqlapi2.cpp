// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32buf.h>				//MStreamBuf
#include <sqldb.h>
#include "SqlResourceProfiler.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlapi2 test"));
RSqlDatabase TheDb;
RSqlStatement TheStmt;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlapi2_1.db");
_LIT(KTestDbName2, "c:\\private\\1111C1EF\\t_sqlapi2_2.db");//t_sqlapi2 app - private database

_LIT(KDbInjectedName1, "DELETE FROM symbian_settings;c:\\test\\A.db");
_LIT(KDbInjectedName2, "c:\\test\\A.db;DELETE FROM symbian_settings;");

const TInt KBufLen = 8192;
TBuf<KBufLen> TheBuf;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	TheStmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbInjectedName2);
	(void)RSqlDatabase::Delete(KDbInjectedName1);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
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
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3512
@SYMTestCaseDesc		RSqlStatement::ColumnCount() - SELECT statements test
						The test creates a database with a table and then checks the ColumnCount()
						return result for the following statements:
						- select all columns;
						- select a subset;
						- select an expression;
						- select a constant;
						- multi-table select;
						- select a function;
						- select plus sub-query;
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ColumnCount() test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8035
*/
void ColumnCountTest()
	{

	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	//Create table 1
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT,Id2 INTEGER,Data BLOB)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(1,'AAA',6234567890,x'11AAFD0C771188')"));
	TEST2(err, 1);
		
	//Select all columns (SELECT *)
	err = TheStmt.Prepare(TheDb, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	TInt cnt = TheStmt.ColumnCount();
	TEST2(cnt, 4);
	TheStmt.Close();
	//Select all columns (SELECT a,b,c...)
	err = TheStmt.Prepare(TheDb, _L("SELECT Id,Name,Id2,Data FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 4);
	TheStmt.Close();
	//Select column subset
	err = TheStmt.Prepare(TheDb, _L("SELECT Id,Name,Data FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 3);
	TheStmt.Close();
	//Select column subset + expression
	err = TheStmt.Prepare(TheDb, _L("SELECT Id,Id+Id2 FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 2);
	TheStmt.Close();
	//Select column subset + constant
	err = TheStmt.Prepare(TheDb, _L("SELECT Id,Id2,345.78 FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 3);
	TheStmt.Close();
	//Select SQL function
	err = TheStmt.Prepare(TheDb, _L("SELECT COUNT(*) FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 1);
	TheStmt.Close();
	//Create table 2
	err = TheDb.Exec(_L("CREATE TABLE B(Id INTEGER, S INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO B VALUES(1,25)"));
	TEST2(err, 1);
	//Multitable select
	err = TheStmt.Prepare(TheDb, _L("SELECT A.Id,B.S FROM A,B WHERE A.Id = B.Id"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 2);
	TheStmt.Close();
	//Select + Subquery
	err = TheStmt.Prepare(TheDb, _L("SELECT Id FROM A WHERE (SELECT S FROM B WHERE A.Id = B.Id) > 10"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 1);
	TheStmt.Close();
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3513
@SYMTestCaseDesc		RSqlStatement::ColumnCount() - DDL and DML statements test
						The test creates a database with a table and then checks the ColumnCount() return result for
						DML statements (INSERT/UPDATE/DELETE) and DDL statements (CREATE TABLE/INDEX, DROP TABLE?INDEX).
						The column count for DML and DDL statements should be 0.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ColumnCount() test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8035
*/
void ColumnCountTest2()
	{
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	//Create table
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT,Id2 INTEGER,Data BLOB)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(1,'AAA',6234567890,x'11AAFD0C771188')"));
	TEST2(err, 1);
	//INSERT statement
	err = TheStmt.Prepare(TheDb, _L("INSERT INTO A(Id,Id2) VALUES(:P1,:P2)"));
	TEST2(err, KErrNone);
	TInt cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//UPDATE statement
	err = TheStmt.Prepare(TheDb, _L("UPDATE A SET Id2=100 WHERE Id=:P1"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//DELETE statement
	err = TheStmt.Prepare(TheDb, _L("DELETE FROM A WHERE Id=:P1"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//CREATE TABLE statement
	err = TheStmt.Prepare(TheDb, _L("CREATE TABLE B AS SELECT * FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//DROP TABLE statement
	err = TheStmt.Prepare(TheDb, _L("DROP TABLE A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//CREATE INDEX statement
	err = TheStmt.Prepare(TheDb, _L("CREATE INDEX I ON A(Id)"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	err = TheStmt.Exec();
	TEST(err >= 0);
	TheStmt.Close();
	//DROP INDEX statement
	err = TheStmt.Prepare(TheDb, _L("DROP INDEX I"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//CREATE TRIGGER statement
	err = TheStmt.Prepare(TheDb,
			_L("CREATE TRIGGER Trg BEFORE DELETE ON A \
	             BEGIN \
	                SELECT CASE WHEN ((SELECT Id2 FROM A WHERE A.Id = old.Id) > 0) \
	                            THEN RAISE (ABORT, 'Id2 > 0') \
	                END;\
	             END;"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//CREATE VIEW statement
	err = TheStmt.Prepare(TheDb, _L("CREATE VIEW V AS SELECT * FROM A"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	err = TheStmt.Exec();
	TEST(err >= 0);
	TheStmt.Close();
	//DROP VIEW statement
	err = TheStmt.Prepare(TheDb, _L("DROP VIEW V"));
	TEST2(err, KErrNone);
	cnt = TheStmt.ColumnCount();
	TEST2(cnt, 0);
	TheStmt.Close();
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3514
@SYMTestCaseDesc		RSqlStatement::DeclaredColumnType() test
						The test creates a database with a table and then checks the DeclaredColumnType() return result for:
						- select all column from the table and check their types;
						- multi-table select plus column type checks;
						- select expression - the expected column type is ESqlInt;
						- select constant - the expected column type is ESqlInt;
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ColumnCount() test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8035
*/
void DeclaredColumnTypeTest()
	{
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	const char* KColTypeNames[] =
		{"INTEGER", "LONG INTEGER", "INT", "SHORT INT", "SMALL INT", "TINY INT", "SHORT", "INT64",
		"TEXT", "LONGTEXT", "CLOB", "CHAR", "CHARACTER(20)", "LONG TEXT",
		"BINARY", "LONG BINARY", "LONGBINARY", "BLOB", "LONGBLOB", "LONG BLOB",
		"REAL", "FLOAT", "DOUBLE", "LONG DOUBLE",
		"LONG LONG", "BOO HOO"};
	const TSqlColumnType KColTypes[] =
		{ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,
		 ESqlText,ESqlText,ESqlText,ESqlText,ESqlText,ESqlText,
		 ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,
		 ESqlReal,ESqlReal,ESqlReal,ESqlReal,
		 ESqlInt,ESqlInt};
	const TInt KColTypeCnt = sizeof(KColTypes) / sizeof(KColTypes[0]);
	TEST2(sizeof(KColTypeNames) / sizeof(KColTypeNames[0]), KColTypeCnt);
	//Create table 1
	TBuf8<512> sql;
	sql.Copy(_L8("CREATE TABLE T("));
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		sql.Append(TChar('A'));
		sql.AppendNum(i + 1);
		sql.Append(TChar(' '));
		sql.Append((TUint8*)KColTypeNames[i], User::StringLength((TUint8*)KColTypeNames[i]));
		sql.Append(TChar(','));
		}
	sql.Replace(sql.Length() - 1, 1, _L8(")"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	//Select all columns (SELECT *)
	err = TheStmt.Prepare(TheDb, _L("SELECT * FROM T"));
	TEST2(err, KErrNone);
	TInt cnt = TheStmt.ColumnCount();
	TEST2(cnt, KColTypeCnt);
	TSqlColumnType colType;
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		TInt err = TheStmt.DeclaredColumnType(i, colType);
		TEST2(err, KErrNone);
		TEST2(colType, KColTypes[i]);
		}
	TheStmt.Close();
	//Create table 2
	err = TheDb.Exec(_L8("CREATE TABLE T2(Id INTEGER, DATA BLOB)"));
	TEST(err >= 0);
	//Multi-table select
	err = TheStmt.Prepare(TheDb, _L("SELECT T.A1,T2.Id,T.A9,T2.Data FROM T,T2"));
	TEST2(err, KErrNone);
	err = TheStmt.DeclaredColumnType(0, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	err = TheStmt.DeclaredColumnType(1, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	err = TheStmt.DeclaredColumnType(2, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlText);
	err = TheStmt.DeclaredColumnType(3, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlBinary);
	TheStmt.Close();
	//Select expression
	err = TheStmt.Prepare(TheDb, _L("SELECT (Id + Data) AS RES FROM t2"));
	TEST2(err, KErrNone);
	err = TheStmt.DeclaredColumnType(0, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	TheStmt.Close();
	//Select constant
	err = TheStmt.Prepare(TheDb, _L("SELECT (Id + Data) AS RES, 55.89 FROM t2"));
	TEST2(err, KErrNone);
	err = TheStmt.DeclaredColumnType(1, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	TheStmt.Close();
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4017
@SYMTestCaseDesc		RSqlStatement::ColumnName(TInt, TPtrC&) test
						The test creates a database with a table and then checks the ColumnName() return result for:
						- select all column from the table and check their names;
						- multi-table select plus column name checks;
						- select expression - the expected column name is RES
						- select constant - the expected column type is 55.89 
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ColumnName() test
@SYMTestExpectedResults Test must not fail
@SYMCR				    RMAD-7B7EV5
                        Add SQL Server APIs to retrieve column and parameter names
*/	
void ColumnNameTest()
	{
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	const char* KColTypeNames[] = 
		{"INTEGER", "LONG INTEGER", "INT", "SHORT INT", "SMALL INT", "TINY INT", "SHORT", "INT64",
		"TEXT", "LONGTEXT", "CLOB", "CHAR", "CHARACTER(20)", "LONG TEXT",
		"BINARY", "LONG BINARY", "LONGBINARY", "BLOB", "LONGBLOB", "LONG BLOB",
		"REAL", "FLOAT", "DOUBLE", "LONG DOUBLE",
		"LONG LONG", "BOO HOO"};
	const TSqlColumnType KColTypes[] = 
		{ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,ESqlInt,
		 ESqlText,ESqlText,ESqlText,ESqlText,ESqlText,ESqlText,
		 ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,ESqlBinary,
		 ESqlReal,ESqlReal,ESqlReal,ESqlReal,
		 ESqlInt,ESqlInt};
	const TInt KColTypeCnt = sizeof(KColTypes) / sizeof(KColTypes[0]);
	TEST2(sizeof(KColTypeNames) / sizeof(KColTypeNames[0]), KColTypeCnt);
	//Create table 1
	TBuf8<512> sql;
	sql.Copy(_L8("CREATE TABLE T("));
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		sql.Append(TChar('A'));
		sql.AppendNum(i + 1);
		sql.Append(TChar(' '));
		sql.Append((TUint8*)KColTypeNames[i], User::StringLength((TUint8*)KColTypeNames[i]));
		sql.Append(TChar(','));
		}
	sql.Replace(sql.Length() - 1, 1, _L8(")"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	//Select all columns (SELECT *)
	err = TheStmt.Prepare(TheDb, _L("SELECT * FROM T"));
	TEST2(err, KErrNone);
	TInt cnt = TheStmt.ColumnCount();
	TEST2(cnt, KColTypeCnt);
	TPtrC colName;
	TBuf<128> expectedColName;
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		expectedColName.Zero();
		expectedColName.Append(TChar('A'));
		expectedColName.AppendNum(i + 1);
		TInt err = TheStmt.ColumnName(i, colName);
		TEST2(err, KErrNone);
		TEST2(colName.Compare(expectedColName), 0);
		TSqlColumnType type;
		err = TheStmt.DeclaredColumnType(i, type);
		TEST2(err, KErrNone);
		TEST2(type, KColTypes[i]);
		}
	TheStmt.Close();
	//Create table 2
	err = TheDb.Exec(_L8("CREATE TABLE T2(Id INTEGER, DATA BLOB)"));
	TEST(err >= 0);
	//Multi-table select
	err = TheStmt.Prepare(TheDb, _L("SELECT T.A1,T2.Id,T.A9,T2.DATA FROM T,T2"));
	TEST2(err, KErrNone);
	err = TheStmt.ColumnName(0, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("A1")), 0);
	err = TheStmt.ColumnName(1, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("Id")), 0);
	err = TheStmt.ColumnName(2, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("A9")), 0);
	err = TheStmt.ColumnName(3, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("DATA")), 0);
	TheStmt.Close();
	//Select expression
	err = TheStmt.Prepare(TheDb, _L("SELECT (Id + Data) AS RES FROM t2"));
	TEST2(err, KErrNone);
	err = TheStmt.ColumnName(0, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("RES")), 0);
	//Too big column index
    err = TheStmt.ColumnName(1323, colName);
    TEST2(err, KErrNotFound);
    //Negative column index 
    err = TheStmt.ColumnName(-100, colName);
    TEST2(err, KErrNotFound);
	TheStmt.Close();
	//Select constant
	err = TheStmt.Prepare(TheDb, _L("SELECT (Id + Data) AS RES, 55.89 FROM t2"));
	TEST2(err, KErrNone);
	err = TheStmt.ColumnName(1, colName);
	TEST2(err, KErrNone);
	TEST2(colName.Compare(_L("55.89")), 0);
	TheStmt.Close();
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4018
@SYMTestCaseDesc		RSqlStatement::ParameterName(TInt, TPtrC&) and RSqlStatement::ParamName(TInt, TPtrC&) test
						DML test:
						The test creates a database with a table and prepares an insert query.
						The test then checks the ParameterName() and ParamName() return result for:
						- Named parameters - return the named param
						- Unnamed parameters - return ?<param-index>
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ParameterName() and RSqlStatement::ParamName() test
@SYMTestExpectedResults Test must not fail
@SYMCR					RMAD-7B7EV5
                        Add SQL Server APIs to retrieve column and parameter names
*/	
void ParamNameTest()
	{
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	const char* KColTypeNames[] = 
		{"INTEGER", "TEXT"};
	const TInt KColTypeCnt = sizeof(KColTypeNames) / sizeof(KColTypeNames[0]);
	//Create table 1
	TBuf8<256> sql;
	sql.Copy(_L8("CREATE TABLE T("));
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		sql.Append(TChar('A'));
		sql.AppendNum(i + 1);
		sql.Append(TChar(' '));
		sql.Append((TUint8*)KColTypeNames[i], User::StringLength((TUint8*)KColTypeNames[i]));
		sql.Append(TChar(','));
		}
	sql.Replace(sql.Length() - 1, 1, _L8(")"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	TheStmt.Close();
	
	// Create insert statement, then check param names
	err = TheStmt.Prepare(TheDb, _L("INSERT INTO T (A1, A2) VALUES (:prm1, :prm2)"));
	TEST2(err, KErrNone);
	TPtrC paramName;
	TBuf<128> expectedParamName;
	for(TInt i=0;i<KColTypeCnt;++i)
		{
		expectedParamName.Zero();
		expectedParamName.Append(_L(":prm"));
		expectedParamName.AppendNum(i + 1);
		TInt paramIndex = TheStmt.ParameterIndex(expectedParamName);
		TEST2(paramIndex, i);
		TInt err = TheStmt.ParameterName(i, paramName);
		TEST2(err, KErrNone);
		TEST2(paramName.Compare(expectedParamName), 0);
		err = TheStmt.ParamName(i, paramName);
		TEST2(err, KErrNone);
		TEST2(paramName.Compare(expectedParamName), 0);
		}
    //Too big parameter index
    err = TheStmt.ParamName(1323, paramName);
    TEST2(err, KErrNotFound);
    //Negative parameter index 
    err = TheStmt.ParamName(-100, paramName);
    TEST2(err, KErrNotFound);
	TheStmt.Close();
	
	//SQL statement without parameters
    err = TheStmt.Prepare(TheDb, _L("INSERT INTO T (A1, A2) VALUES (1, '1')"));
    TEST2(err, KErrNone);
    err = TheStmt.ParamName(0, paramName);
    TEST2(err, KErrNotFound);
    TheStmt.Close();

	// Create insert statement, then check param names
	err = TheStmt.Prepare(TheDb, _L("INSERT INTO T (A1, A2) VALUES (:prm1, ?)"));
	TEST2(err, KErrNone);
	
	expectedParamName.Zero();
	expectedParamName.Append(_L(":prm1"));
	TInt paramIndex = TheStmt.ParameterIndex(expectedParamName);
	TEST2(paramIndex, 0);
	err = TheStmt.ParameterName(0, paramName);
	TEST2(err, KErrNone);
	TEST2(paramName.Compare(expectedParamName), 0);
	err = TheStmt.ParamName(0, paramName);
	TEST2(err, KErrNone);
	TEST2(paramName.Compare(expectedParamName), 0);

	expectedParamName.Zero();
	expectedParamName.Append(_L("?1"));
	err = TheStmt.ParameterName(1, paramName);
	TEST2(err, KErrNone);
	paramIndex = TheStmt.ParameterIndex(expectedParamName);
	TEST2(paramIndex, 1);
	TEST2(paramName.Compare(expectedParamName), 0);
	
	err = TheStmt.ParamName(1, paramName);
	TEST2(err, KErrNone);
	TEST2(paramName.Compare(expectedParamName), 0);

	TheStmt.Close();
	
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}


/**
@SYMTestCaseID			SYSLIB-SQL-UT-4006
@SYMTestCaseDesc		Test for DEF115300 - SqlSrv.EXE::!SQL Server when preparing invalid LEFT JOIN sql query.
						The test does the following steps:
						1) Creates a 16-bit database and using 16-bit queries proves that the "GROUP BY GROUP BY" syntax error
						   does not cause an assert inside the SQLITE code.
						2) Creates a 8-bit database and using 8-bit queries proves that the "GROUP BY GROUP BY" syntax error
						   does not cause an assert inside the SQLITE code.
@SYMTestPriority		Medium
@SYMTestActions			Test for DEF115300 - SqlSrv.EXE::!SQL Server when preparing invalid LEFT JOIN sql query.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115300
*/
void DEF115300()
	{

	//Step 1: 16-bit statements
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);

	err = TheDb.Exec(_L("CREATE TABLE MAIN(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO MAIN(Id,Id1) VALUES(2,3)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO MAIN(Id,Id1) VALUES(3,4)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A(Id, Id1) VALUES(2,3)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A(Id, Id1) VALUES(4,4)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("CREATE TABLE B(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO B(Id, Id1) VALUES(2,3)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO B(Id, Id1) VALUES(5,4)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO B(Id, Id1) VALUES(5,4)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("CREATE VIEW v2 AS SELECT Id,Id1,F2 FROM B"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("CREATE VIEW v1 AS SELECT Id,Id1,F2 FROM A"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("CREATE VIEW v3 AS SELECT Id,Id1,F2 FROM B"));
	TEST2(err, 1);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT * FROM v2 LEFT JOIN MAIN ON v2.Id = MAIN.Id LEFT JOIN A ON MAIN.Id = A.Id GROUP BY GROUP BY MAIN.Id"));
	stmt.Close();
	TEST(err != KErrNone && err != KErrServerTerminated);

	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);

	//Step 2: 8-bit statements
	_LIT8(KServerConfigString1, "encoding=\"UTF-8\"");
	err = TheDb.Create(KTestDbName1, &KServerConfigString1);
	TEST2(err, KErrNone);

	err = TheDb.Exec(_L8("CREATE TABLE MAIN(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST2(err, 1);

	err = stmt.Prepare(TheDb, _L8("SELECT * FROM main GROUP BY GROUP BY main.Id"));
	stmt.Close();
	TEST(err != KErrNone && err != KErrServerTerminated);

	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4007
@SYMTestCaseDesc		Test for DEF115331 - SQL, bad code coverage for db reindexing if default collation has changed.
						The test does the following steps, using public shared and private secure database:
						1) Creates a test database with a table and one index using one of the collations.
						2) Updates the symbian_settings table, setting the collation dll name column value 
						   to be a "bbbababz" string (Simulates that there is no valid collation dll name).
						3) Reopens the database. This operation should cause a database reindexing, since the index uses
						   one of the user-defined collation methods.
						   The default system collation dll name should be stored in the symbian_settings table.
						4) Verifies that symbian_settings table contains only one record and that the collation dll name
						   column value has been updated.
@SYMTestPriority		Medium
@SYMTestActions			Test for DEF115331 - SQL, bad code coverage for db reindexing if default collation has changed.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115331
*/
void DEF115331L()
	{
	const TPtrC KDbNames[]		=	{KTestDbName1(),	KTestDbName2()};

	for(TInt i=0;i<(sizeof(KDbNames)/sizeof(KDbNames[0]));++i)
		{
		//Step 1: Create a test database with a table and one index using one of the collations.
		(void)RSqlDatabase::Delete(KDbNames[i]);
		TInt err = TheDb.Create(KDbNames[i]);
		TEST2(err, KErrNone);

		err = TheDb.Exec(_L("CREATE TABLE A(C TEXT)"));
		TEST2(err, 1);

		err = TheDb.Exec(_L("CREATE INDEX I ON A(C COLLATE CompareC1)"));
		TEST2(err, 1);

		//Step 2: Make sure that the collation dll name is set and unique (not the default collation).
		err = TheDb.Exec(_L("UPDATE symbian_settings SET CollationDllName='bbbababz'"));
		TEST2(err, 1);

		TheDb.Close();

		//Step 3: Reopen the database. That step should cause a database reindexing, because the default collation dll 
		//        name is not the one stored in the table.
		err = TheDb.Open(KDbNames[i]);
		TEST2(err, KErrNone);

		TSqlScalarFullSelectQuery query(TheDb);

		//Step 4: Check that the settigns table has only one record.
		TInt cnt = query.SelectIntL(_L("SELECT COUNT(*) FROM symbian_settings"));
		TEST2(cnt, 1);

		//Step 5: Check that the collation dll name in the settings table has been updated.
		TFileName collationDllName;
		err = query.SelectTextL(_L("SELECT CollationDllName FROM symbian_settings"), collationDllName);
		TEST2(err, KErrNone);
		_LIT(KTestCollationDllName, "bbbababz");//The same as the used in step 2 - above.
		TEST(collationDllName != KTestCollationDllName);

		TheDb.Close();
		err = RSqlDatabase::Delete(KDbNames[i]);
		TEST2(err, KErrNone);
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4079
@SYMTestCaseDesc		RSqlDatabase::Create() and RSqlDatabase::Open() - injection test
						The test attempts to create or open a database which name contains
						"DELETE FROM symbian_settings" statement.If it is possible to open or
						create a database with that name, the "symbian_settings" table content
						should stay unchanged.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Create() and RSqlDatabase::Open() - injection test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
*/
void InjectionTest()
	{
	TInt err = TheDb.Create(KDbInjectedName1);
	TEST(err != KErrNone);

	err = TheDb.Create(KDbInjectedName2);
	TEST2(err, KErrNone);
	
	TSqlScalarFullSelectQuery query(TheDb);
	TInt recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM symbian_settings")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	TheDb.Close();
	
	err = TheDb.Open(KDbInjectedName2);
	TEST2(err, KErrNone);
	recCount = 0;
	query.SetDatabase(TheDb);
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM symbian_settings")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	TheDb.Close();
		
	(void)RSqlDatabase::Delete(KDbInjectedName2);
	(void)RSqlDatabase::Delete(KDbInjectedName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4038
@SYMTestCaseDesc		Background compaction - two connections usability test.
						The test creates a database connection with a background compaction mode. The the test 
						locks the database in a transaction. Then the test creates a second connection
						to the same database while the first connection is in a transaction.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction - two connections usability test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void TwoConnectionsTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db1, db2;
	TInt err = db1.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = db1.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A VALUES(1)"));
	TEST2(err, 1);
	err = db1.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	err = db1.Exec(_L("INSERT INTO A VALUES(2)"));
	TEST2(err, 1);
	err = db2.Open(KTestDbName1);
	TEST2(err, KErrNone);
	err = db1.Exec(_L("COMMIT TRANSACTION"));
	TEST(err >= 0);
	db2.Close();		
	db1.Close();		
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

TInt StackOverflowThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TInt* cntptr = reinterpret_cast<TInt*> (aData);
	TEST(cntptr != NULL);
	TInt cnt = *cntptr;

	HBufC* buf = HBufC::New(cnt * 12 + 32);//enough for the "SELECT Id FROM A WHERE Id=v1 OR Id=v2 OR ..." string
	if(!buf)
		{
		delete tc;
		return KErrNoMemory;	
		}
	TPtr sql = buf->Des();
	
	TInt err = TheDb.Open(KTestDbName1);
	if(err != KErrNone)
		{
		delete buf;
		delete tc;
		return err;	
		}

	TTime now;
	now.UniversalTime();
	TInt64 seed = now.Int64();

	sql.Copy(_L("SELECT Id FROM A WHERE "));
	for(TInt i=0;i<cnt;++i)
		{
		sql.Append(_L("Id="));
		sql.AppendNum(Math::Rand(seed) % cnt);
		sql.Append(_L(" OR "));
		}
	sql.SetLength(sql.Length() - 4);//Remove the last " OR "

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, sql);
	stmt.Close();
	
	TheDb.Close();
	delete buf;
	delete tc;
	
	return err;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4080
@SYMTestCaseDesc		SQL server stack overflow test
						The test creates a database and runs a thread. The thread opens the database
						and attempts to execute a SELECT statement, which format is:
						"SELECT Id FROM A WHERE Id=1 OR Id=2 OR...OR Id=N",
						where N is a number passed as an argument from the main thread, starts from 100000
						and is increased or decreased on each test iteration, depending on the reported result from the thread.
						Finally, the main thread will report the max number of the OR subexpressions that can be included
						in the SELECT statement, without an error to be reported.
						The test should not crash the SQL server, if the server stack size and parsing tree depth has
						been properly configured.
@SYMTestPriority		Medium
@SYMTestActions			SQL server stack overflow test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
*/
void SqlServerStackOverflowTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER PRIMARY KEY AUTOINCREMENT)"));
	TEST2(err, 1);
	TheDb.Close();
	
	TInt prev = 0, next = 100000;
	while(Abs(next - prev) > 0)
		{
		TInt count = next;
		TheTest.Printf(_L("'OR' expr. count: %d\r\n"), count);
		RThread thread;
		_LIT(KThreadName,"ORThread");						//stack	minheap		maxheap
		err = thread.Create(KThreadName, &StackOverflowThreadFunc, 0x2000, 0x00100000, 0x02000000, &count);
		TEST2(err, KErrNone);
		
		TRequestStatus status;
		thread.Logon(status);
		TEST2(status.Int(), KRequestPending);
		thread.Resume();
		User::WaitForRequest(status);
		User::SetJustInTime(ETrue);	// enable debugger panic handling

		TInt exitType = thread.ExitType();
		const TDesC& exitCategory = thread.ExitCategory();
		TInt exitReason = thread.ExitReason();
		TheTest.Printf(_L("Exit type: %d, exit reason: %d, exit category: %S\r\n"), exitType, exitReason, &exitCategory);
		thread.Close();
		TEST(exitReason != KErrServerTerminated);
		TEST(exitType != EExitPanic);

		TInt tmp = next;		
		if(status.Int() != KErrNone)
			{//The number of the OR subexpressions is too big and cannot be parsed. Decrease the number, repeat the test.
			next -= Abs(next - prev) / 2;
			}
		else
			{//KErrNone: The number of the OR subexpressions has been successfully parsed. Increase the number, repeat the test.
			next += Abs(next - prev) / 2;
			}
		prev = tmp;
		}
	TheTest.Printf(_L("The test has succeeded with an expression with %d ORs\r\n"), prev);
	}

void AssertSettingsTable()
	{
	TSqlScalarFullSelectQuery query(TheDb);
	TInt recCount = 0;
	TRAPD(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM symbian_settings")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4086
@SYMTestCaseDesc		RSqlBlobWriteStream::OpenL() and RSqlBlobReadStream::OpenL() - injection test
						The test attempts to open a blob stream with an attached database name containing
						"DELETE FROM symbian_settings" statement. The test should not delete the content of the
						"symbian_settings" table.
						The test also attempts to open a blob stream with a set of very long database/table/column names.
@SYMTestPriority		Medium
@SYMTestActions			RSqlBlobWriteStream::OpenL() and RSqlBlobReadStream::OpenL() - injection test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ10410
						REQ10411
						REQ10418
*/
void BlobStreamInjectionTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Data BLOB)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(1, x'11223344556677889900')"));
	TEST2(err, 1);
	_LIT(KAttachDb, "AttachDb");
	err = TheDb.Attach(KTestDbName1, KAttachDb);
	TEST2(err, KErrNone);
	//RSqlBlobWriteStream::OpenL() - attached database  name injected
	RSqlBlobWriteStream strm1;
	TRAP(err, strm1.OpenL(TheDb, _L("A"), _L("Data"), 1, _L("Id;DELETE FROM symbian_settings;")));
	TEST(err != KErrNone);
	AssertSettingsTable();
	//RSqlBlobReadStream::OpenL() - attached database  name injected
	RSqlBlobReadStream strm2;
	TRAP(err, strm2.OpenL(TheDb, _L("A"), _L("Data"), 1, _L("Id;DELETE FROM symbian_settings;")));
	TEST(err != KErrNone);
	AssertSettingsTable();
	//Attempt to open a write blob stream with a set of very long database/table/column names.
	TBuf<KMaxFileName + 10> longName;
	longName.SetLength(longName.MaxLength());
	RSqlBlobWriteStream strm3;
	TRAP(err, strm3.OpenL(TheDb, longName, longName, 1, longName));
	TEST(err != KErrNone);
	//Attempt to open a read blob stream with a set of very long database/table/column names.
	RSqlBlobReadStream strm4;
	TRAP(err, strm4.OpenL(TheDb, longName, longName, 1, longName));
	TEST(err != KErrNone);
	//Attempt to open a write blob stream with a set of KNullDesC database/table/column names.
	RSqlBlobWriteStream strm5;
	TRAP(err, strm5.OpenL(TheDb, KNullDesC, KNullDesC, 1, KNullDesC));
	TEST(err != KErrNone);
	//Attempt to open a read blob stream with a set of KNullDesC database/table/column names.
	RSqlBlobReadStream strm6;
	TRAP(err, strm6.OpenL(TheDb, KNullDesC, KNullDesC, 1, KNullDesC));
	TEST(err != KErrNone);
	//Attempt to open a read blob stream, where the blob column name is invalid and contains non-convertible characters.
    TBuf<3> invName;
    invName.SetLength(3);
    invName[0] = TChar(0xD800); 
    invName[1] = TChar(0xFC00); 
    invName[2] = TChar(0x0000);
	RSqlBlobReadStream strm7;
	TRAP(err, strm7.OpenL(TheDb,  _L("A"), invName, 1, KNullDesC));
	TEST(err != KErrNone);
	//Attempt to open a read blob stream, where the table name is invalid and contains non-convertible characters.
	RSqlBlobReadStream strm8;
	TRAP(err, strm8.OpenL(TheDb, invName, _L("Data"), 1, KNullDesC));
	TEST(err != KErrNone);
	//Attempt to open a read blob stream, where the attached db name is invalid and contains non-convertible characters.
	RSqlBlobReadStream strm9;
	TRAP(err, strm9.OpenL(TheDb, _L("A"), _L("Data"), 1, invName));
	TEST(err != KErrNone);
	//
	err = TheDb.Detach(KAttachDb);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4087
@SYMTestCaseDesc		Bound parameter values test.
						The test verifies that bound parameters with big text/binary values retain their values after
						the RSqlStatement::Reset() call. The old bound paramegter values can be used for the next 
						RSqlStatement::Exec() call.
@SYMTestActions			Bound parameter values test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ5792
*/
void BoundParameterValuesTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A1(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("CREATE TABLE A2(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	
	RSqlStatement stmt1, stmt2;
	err = stmt1.Prepare(TheDb, _L("INSERT INTO A1 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	err = stmt2.Prepare(TheDb, _L("INSERT INTO A2 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	//Insert 1 record into table "A1". T2 = "ZZZZ.....".
	TheBuf.SetLength(KBufLen - 100);
	TheBuf.Fill(TChar('Z'));
	err = stmt1.BindText(0, TheBuf);
	TEST2(err, KErrNone);
	err = stmt1.BindText(1, TheBuf);
	TEST2(err, KErrNone);
	err = stmt1.Exec();
	TEST2(err, 1);
	err = stmt1.Reset();
	TEST2(err, KErrNone);
	//Insert 1 record into table "A2". T2 = "AAAAAAA.....".
	TheBuf.SetLength(KBufLen);
	TheBuf.Fill(TChar('A'));
	err = stmt2.BindText(0, TheBuf);
	TEST2(err, KErrNone);
	err = stmt2.BindText(1, TheBuf);
	TEST2(err, KErrNone);
	err = stmt2.Exec();
	TEST2(err, 1);
	err = stmt2.Reset();
	TEST2(err, KErrNone);
	//Insert 1 record into table "A1". T2 not set. T2 should be initialized with the previous bound value - "ZZZZZZ....".
	//If the problem is not fixed, the SQLITE will attempt to access an already deleted region of memory.
	TheBuf.SetLength(KBufLen - 100);
	TheBuf.Fill(TChar('B'));
	err = stmt1.BindText(0, TheBuf);
	TEST2(err, KErrNone);
	err = stmt1.Exec();
	TEST2(err, 1);
	err = stmt1.Reset();
	TEST2(err, KErrNone);
	
	stmt2.Close();
	stmt1.Close();
	
	//Check the inserted records.
	TSqlScalarFullSelectQuery q(TheDb);
	TRAP(err, q.SelectTextL(_L("SELECT T2 FROM A1 WHERE ROWID=1"), TheBuf));
	TEST2(err, KErrNone);
	TEST2(TheBuf.Length(), (KBufLen - 100));
	for(TInt i1=0;i1<(KBufLen - 100);++i1)
		{
		TEST2(TheBuf[i1], TChar('Z'));	
		}
	TRAP(err, q.SelectTextL(_L("SELECT T2 FROM A1 WHERE ROWID=2"), TheBuf));
	TEST2(err, KErrNone);
	TEST2(TheBuf.Length(), (KBufLen - 100));
	for(TInt i2=0;i2<(KBufLen - 100);++i2)
		{
		TEST2(TheBuf[i2], TChar('Z'));	
		}
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4076
@SYMTestCaseDesc		Bound parameter values test.
						The test verifies that when a RSqlParamWriteStream object is used for binding parameter values,
						it is safe to erverse the order of RSqlParamWriteStream::Close() and RSqlStatement::Close() calls.
@SYMTestActions			Bound parameter values test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ5792
*/
void BoundParameterValuesTest2()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A1(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO A1 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	RSqlParamWriteStream strm;
	err = strm.BindText(stmt, 0);
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 1);
	stmt.Close();
	strm.Close();

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4077
@SYMTestCaseDesc		Bound parameter values test.
						The test verifies that when a RSqlParamWriteStream object is used for binding parameter values,
						it is possible to write the parameter value, then call RSqlParamWriteStream::Close() and finally -
						RSqlStatement::Exec() to execute the operation (an INSERT statement). The test verifies that the record
						has really been inserted and the column value is equal to the bound parameter value
@SYMTestActions			Bound parameter values test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ5792
*/
void BoundParameterValuesTest3()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A1(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO A1 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	RSqlParamWriteStream strm;
	err = strm.BindText(stmt, 0);
	TEST2(err, KErrNone);
	_LIT(KText, "AAAA");
	TRAP(err, strm.WriteL(KText));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();
	err = stmt.Exec();
	TEST2(err, 1);
	stmt.Close();

	TSqlScalarFullSelectQuery q(TheDb);
	TRAP(err, q.SelectTextL(_L("SELECT T1 FROM A1"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText() == TheBuf);	

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4083
@SYMTestCaseDesc		Bound parameter values test.
						The test prepares an INSERT sql statement and inserts two records using streams to bind the parameter values.
						For the second INSERT no parameter value is bound to the first parameter. The expectation is that the value
						that has been bound for the first record will be used for the second record also.
@SYMTestActions			Bound parameter values test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ5792
*/
void BoundParameterValuesTest4()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A1(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO A1 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	
	RSqlParamWriteStream strm;
	err = strm.BindText(stmt, 0);
	TEST2(err, KErrNone);
	_LIT(KText1, "AAAA");
	TRAP(err, strm.WriteL(KText1));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();
	
	err = strm.BindText(stmt, 1);
	TEST2(err, KErrNone);
	_LIT(KText2, "BBBBBBBBBB");
	TRAP(err, strm.WriteL(KText2));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();
	
	err = stmt.Exec();
	TEST2(err, 1);
	err = stmt.Reset();
	TEST2(err, KErrNone);
	
	err = strm.BindText(stmt, 1);
	TEST2(err, KErrNone);
	_LIT(KText3, "CCCCCCCCCCC");
	TRAP(err, strm.WriteL(KText3));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();
	
	err = stmt.Exec();
	TEST2(err, 1);
	
	stmt.Close();

	TSqlScalarFullSelectQuery q(TheDb);
	TRAP(err, q.SelectTextL(_L("SELECT T1 FROM A1 WHERE ROWID=1"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText1() == TheBuf);	
	TRAP(err, q.SelectTextL(_L("SELECT T2 FROM A1 WHERE ROWID=1"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText2() == TheBuf);	

	TRAP(err, q.SelectTextL(_L("SELECT T1 FROM A1 WHERE ROWID=2"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText1() == TheBuf);	
	TRAP(err, q.SelectTextL(_L("SELECT T2 FROM A1 WHERE ROWID=2"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText3() == TheBuf);	

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4105
@SYMTestCaseDesc		Bound parameter values test.
						BC test. Even though it is correct to execute only one CommitL() on a parameter stream,
						it should be possible to execute more than one CommitL(). It should be possible also
						the stream data to be updated after the first commit operation and the expectation is that
						the updated parameter data should be used for the column value.
@SYMTestActions			Bound parameter values test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ5792
*/
void BoundParameterValuesTest5()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A1(T1 TEXT, T2 TEXT)"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO A1 VALUES(:Prm1, :Prm2)"));
	TEST2(err, KErrNone);
	
	RSqlParamWriteStream strm;
	err = strm.BindText(stmt, 0);
	TEST2(err, KErrNone);
	_LIT(KText1, "AAAA");
	TRAP(err, strm.WriteL(KText1));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	TRAP(err, strm.Sink()->SeekL(MStreamBuf::EWrite, EStreamBeginning, 0));
	TEST2(err, KErrNone);
	_LIT(KText2, "DTAA");
	TRAP(err, strm.WriteL(KText2));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();

	err = stmt.Exec();
	TEST2(err, 1);
	
	stmt.Close();

	TSqlScalarFullSelectQuery q(TheDb);
	TRAP(err, q.SelectTextL(_L("SELECT T1 FROM A1 WHERE ROWID=1"), TheBuf));
	TEST2(err, KErrNone);
	TEST(KText2() == TheBuf);	

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

void PrintConfig(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<128> config;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterConfig, config) == KErrNone)
		{
		_LIT(KCacheSize, "Cache size: %S pages\r\n");
		_LIT(KPageSize, "Page size: %S bytes\r\n");
		_LIT(KEncoding, "Encoding: %S\r\n");
		_LIT(KDefaultSoftHeapLimit, "Default soft heap limit: %S Kb\r\n");
		_LIT(KVacuumMode, "Vacuum mode: %S\r\n");
	
		TPtrC KText[] = {KCacheSize(), KPageSize(), KEncoding(), KDefaultSoftHeapLimit(), KVacuumMode()};
	
		for(TInt i=0;i<config.Length();++i)
			{
			if(config[i] == TChar(';'))	
				{
				config[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(config);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(KText[idx], &num);
			++idx;
			}
		}
	}

void PrintFileIo(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<300> countersValues;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterFileIO, countersValues) == KErrNone)
		{
		TheTest.Printf(_L("=========================\r\n"));
		_LIT(KFileCreate, "File Create");
		_LIT(KFileOpen, "File Open");
		_LIT(KFileClose, "File Close");
		_LIT(KFileDelete, "File Delete");
		_LIT(KFileRead, "File Read");
		_LIT(KFileWrite, "File Write");
		_LIT(KFileSeek, "File Seek");
		_LIT(KFileSize, "File Size");
		_LIT(KFileSetSize, "File SetSize");
		_LIT(KFileSync, "File Sync");
		_LIT(KFileDrive, "File Drive");
		_LIT(KFileAdopt, "File Adopt");
		_LIT(KFsClose, "Fs Close");
		_LIT(KFsConnect, "Fs Connect");
		_LIT(KFsGetSystemDrive, "Fs GetSystemDrive");
		_LIT(KFsCreatePrivatePath, "Fs CreatePrivatePath");
		_LIT(KFsPrivatePath, "Fs PrivatePath");
		_LIT(KFsVolumeIoParam, "Fs VolumeIoParam");
		_LIT(KFsEntry, "Fs Entry");
		_LIT(KFsAtt, "Fs Att");
		_LIT(KFileCreateTemp, "File CreateTemp");
		_LIT(KFileAttach, "File Attach");
		_LIT(KBytesWritten, "File Bytes Written");
		_LIT(KBytesRead, "File Bytes Read");
		TPtrC KText[] = 
			{
			KFileCreate(), KFileOpen(), KFileClose(), KFileDelete(), KFileRead(), KFileWrite(), KFileSeek(), KFileSize(),
			KFileSetSize(), KFileSync(), KFileDrive(), KFileAdopt(), KFsClose(), KFsConnect(), KFsGetSystemDrive(), 
			KFsCreatePrivatePath(), KFsPrivatePath(), KFsVolumeIoParam(), KFsEntry(), KFsAtt(), KFileCreateTemp(), 
			KFileAttach(), KBytesWritten(), KBytesRead()
			};
		
		for(TInt i=0;i<countersValues.Length();++i)
			{
			if(countersValues[i] == TChar(';'))	
				{
				countersValues[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(countersValues);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(_L("==Operation %S, count %S\r\n"), &KText[idx], &num);
			++idx;
			}
		}
	}

void PrintOsCall(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<300> countersValues;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterOsCall, countersValues) == KErrNone)
		{
		TheTest.Printf(_L("=========================\r\n"));
		_LIT(KEOsFileClose, "FileClose");
		_LIT(KEOsFileRead, "FileRead");
		_LIT(KEOsFileWrite, "FileWrite");
		_LIT(KEOsFileTruncate, "FileTruncate");
		_LIT(KEOsFileSync, "FileSync");
		_LIT(KEOsFileFileSize, "FileSize");
		_LIT(KEOsFileLock, "FileLock");
		_LIT(KEOsFileUnlock, "FileUnlock");
		_LIT(KEOsFileCheckReservedLock, "FileCheckReservedLock");
		_LIT(KEOsFileFileControl, "FileIoControl");
		_LIT(KEOsFileSectorSize, "FileSectorSize");
		_LIT(KEOsFileDeviceCharacteristics, "FileDeviceCharacteristics");
		_LIT(KEOsVfsOpen, "VfsOpen");
		_LIT(KEOsVfsDelete, "VfsDelete");
		_LIT(KEOsVfsAccess, "VfsAccess");
		_LIT(KEOsVfsFullPathName, "VfsFullPathName");
		_LIT(KEOsVfsRandomness, "VfsRandomnes");
		_LIT(KEOsVfsSleep, "VfsSleep");
		_LIT(KEOsVfsCurrentTime, "VfsCurrentTime");
		_LIT(KEOsVfsGetLastError, "VfsGetLastError");
		TPtrC KText[] = 
			{
			KEOsFileClose(), KEOsFileRead(), KEOsFileWrite(), KEOsFileTruncate(), KEOsFileSync(), 
			KEOsFileFileSize(), KEOsFileLock(), KEOsFileUnlock(), KEOsFileCheckReservedLock(), KEOsFileFileControl(), 
			KEOsFileSectorSize(), KEOsFileDeviceCharacteristics(), 
			KEOsVfsOpen(), KEOsVfsDelete(), KEOsVfsAccess(), KEOsVfsFullPathName(), KEOsVfsRandomness(), KEOsVfsSleep(), 
			KEOsVfsCurrentTime(), KEOsVfsGetLastError()};
		
		for(TInt i=0;i<countersValues.Length();++i)
			{
			if(countersValues[i] == TChar(';'))	
				{
				countersValues[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(countersValues);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(_L("==Operation %S, count %S\r\n"), &KText[idx], &num);
			++idx;
			}
		}
	}

void PrintOsCallTime(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<300> callTimes;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterOsCallTime, callTimes) == KErrNone)
		{
		TheTest.Printf(_L("=========================\r\n"));
		_LIT(KEOsFileClose, "FileClose");
		_LIT(KEOsFileRead, "FileRead");
		_LIT(KEOsFileWrite, "FileWrite");
		_LIT(KEOsFileTruncate, "FileTruncate");
		_LIT(KEOsFileSync, "FileSync");
		_LIT(KEOsFileFileSize, "FileSize");
		_LIT(KEOsFileLock, "FileLock");
		_LIT(KEOsFileUnlock, "FileUnlock");
		_LIT(KEOsFileCheckReservedLock, "FileCheckReservedLock");
		_LIT(KEOsFileFileControl, "FileIoControl");
		_LIT(KEOsFileSectorSize, "FileSectorSize");
		_LIT(KEOsFileDeviceCharacteristics, "FileDeviceCharacteristics");
		_LIT(KEOsVfsOpen, "VfsOpen");
		_LIT(KEOsVfsDelete, "VfsDelete");
		_LIT(KEOsVfsAccess, "VfsAccess");
		_LIT(KEOsVfsFullPathName, "VfsFullPathName");
		_LIT(KEOsVfsRandomness, "VfsRandomnes");
		_LIT(KEOsVfsSleep, "VfsSleep");
		_LIT(KEOsVfsCurrentTime, "VfsCurrentTime");
		_LIT(KEOsVfsGetLastError, "VfsGetLastError");
		TPtrC KText[] = 
			{
			KEOsFileClose(), KEOsFileRead(), KEOsFileWrite(), KEOsFileTruncate(), KEOsFileSync(), 
			KEOsFileFileSize(), KEOsFileLock(), KEOsFileUnlock(), KEOsFileCheckReservedLock(), KEOsFileFileControl(), 
			KEOsFileSectorSize(), KEOsFileDeviceCharacteristics(), 
			KEOsVfsOpen(), KEOsVfsDelete(), KEOsVfsAccess(), KEOsVfsFullPathName(), KEOsVfsRandomness(), KEOsVfsSleep(), 
			KEOsVfsCurrentTime(), KEOsVfsGetLastError()};
		
		for(TInt i=0;i<callTimes.Length();++i)
			{
			if(callTimes[i] == TChar(';'))	
				{
				callTimes[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(callTimes);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(_L("==Operation %S, time %S us\r\n"), &KText[idx], &num);
			++idx;
			}
		}
	}

void PrintIpc(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<300> countersValues;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterIpc, countersValues) == KErrNone)
		{
		TheTest.Printf(_L("=========================\r\n"));
		_LIT(KIpcRq, "IPC requests");
		_LIT(KIpcRead, "IPC read");
		_LIT(KIpcWrite, "IPC write");
		_LIT(KIpcReadBytes, "IPC read bytes");
		_LIT(KIpcWriteBytes, "IPC write bytes");
		TPtrC KText[] = 
			{
			KIpcRq(), KIpcRead(), KIpcWrite(), KIpcReadBytes(), KIpcWriteBytes()
			};
		
		for(TInt i=0;i<countersValues.Length();++i)
			{
			if(countersValues[i] == TChar(';'))	
				{
				countersValues[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(countersValues);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(_L("==Operation %S, count %S\r\n"), &KText[idx], &num);
			++idx;
			}
		}
	}

void PrintMemory(TSqlResourceProfiler& aProfiler)
	{
	TBuf8<300> countersValues;
	if(aProfiler.Query(TSqlResourceProfiler::ESqlCounterMemory, countersValues) == KErrNone)
		{
		TheTest.Printf(_L("=========================\r\n"));
		_LIT(KMemorySrvAllocatedCnt, "Server allocated cnt");
		_LIT(KMemorySrvAllocatedSize, "Server allocated size");
		_LIT(KMemorySrvFreeSpace, "Server free space");
		_LIT(KMemorySrvLargestBlockSize, "Server larges block size");
		_LIT(KMemorySQLiteAllocatedCnt, "SQLite allocated cnt");
		_LIT(KMemorySQLiteReallocatedCnt, "SQLite reallocated cnt");
		_LIT(KMemorySQLiteFreedCnt, "SQLite freed cnt");
		_LIT(KMemorySQLiteAllocatedBytes, "SQLite allocated bytes");
		_LIT(KMemorySQLiteFreedBytes, "SQLite freed bytes");
		_LIT(KMemorySQLiteAllocTime, "SQLite alloc, us");
		_LIT(KMemorySQLiteReallocTime, "SQLite realloc, us");
		_LIT(KMemorySQLiteFreeTime, "SQLite free, us");
		TPtrC KText[] = 
			{
			KMemorySrvAllocatedCnt(), KMemorySrvAllocatedSize(), KMemorySrvFreeSpace(), KMemorySrvLargestBlockSize(), 
			KMemorySQLiteAllocatedCnt(), KMemorySQLiteReallocatedCnt(), KMemorySQLiteFreedCnt(), 
			KMemorySQLiteAllocatedBytes(), KMemorySQLiteFreedBytes(),
			KMemorySQLiteAllocTime(), KMemorySQLiteReallocTime(), KMemorySQLiteFreeTime()
			};
		
		for(TInt i=0;i<countersValues.Length();++i)
			{
			if(countersValues[i] == TChar(';'))	
				{
				countersValues[i] = TChar(' ');	
				}
			}
		TInt idx = 0;
		TLex8 lex(countersValues);
		while (!lex.Eos() && idx < (sizeof(KText)/sizeof(KText[0])))
			{
			TPtrC8 num8 = lex.NextToken();
			TBuf<20> num;
			num.Copy(num8);
			TheTest.Printf(_L("==%S=%S\r\n"), &KText[idx], &num);
			++idx;
			}
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4088
@SYMTestCaseDesc		TSqlResouceProfiler - file I/O and configuration test.
						The test enables the file I/O profiling and then executes a simple INSERT statement
						and prints out the profiling results. The test also prints the current database configuration.
@SYMTestActions			TSqlResouceProfiler - file I/O and configuration test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ5792
*/
void ProfilerTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	TInt err = TheDb.Create(KTestDbName1);	
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, T TEXT)"));
	TEST2(err, 1);
	
	TSqlResourceProfiler profiler(TheDb);
	
	PrintConfig(profiler);
	
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterFileIO);
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterOsCall);
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterOsCallTime);
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterIpc);
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterMemory);
	
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterFileIO);
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterOsCall);
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterOsCallTime);
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterIpc);
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterMemory);

	err = TheDb.Exec(_L("INSERT INTO A VALUES(1, 'ABCDEEFGH')"));
	TEST2(err, 1);
	
	PrintFileIo(profiler);
	PrintOsCall(profiler);
	PrintOsCallTime(profiler);
	PrintIpc(profiler);
	PrintMemory(profiler);

	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterIpc);
	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterOsCallTime);
	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterOsCall);
	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterFileIO);
	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterMemory);

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

TInt CompoundSelectStackOverflowThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TInt* cntptr = reinterpret_cast<TInt*> (aData);
	TEST(cntptr != NULL);
	const TInt KSelectStmtCnt = *cntptr;

	HBufC* buf = HBufC::New(KSelectStmtCnt * 25 + 32);//enough for the "SELECT I FROM A UNION SELECT I FROM A..." string
	if(!buf)
		{
		delete tc;
		return KErrNoMemory;	
		}
	TPtr sql = buf->Des();

	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	if(err != KErrNone)
		{
		delete buf;
		return err;	
		}
	err = db.Exec(_L("CREATE TABLE A(I INTEGER);INSERT INTO A VALUES(1);"));
	if(err < 1)
		{
		delete buf;
		return err;	
		}
	
	for(TInt i=0;i<KSelectStmtCnt;i++)
		{
		sql.Append(_L("SELECT I FROM A UNION ")); 
		}
	sql.SetLength(sql.Length() - 7);
	RSqlStatement stmt;
	err = stmt.Prepare(db, sql);//This call can crash the server with "stack overflow"
	stmt.Close(); 
	
	db.Close();
	delete buf;
	(void)RSqlDatabase::Delete(KTestDbName1);
	return err;
	}

void CompoundSelectStackOverflowTest()
	{
	const TInt KMaxSelectStmtCnt = 64;
	for(TInt cnt=KMaxSelectStmtCnt;cnt>0;--cnt)
		{
		TheTest.Printf(_L("SELECT statement count: %d\r\n"), cnt);
		RThread thread;
		_LIT(KThreadName,"S2Thread");						//stack	minheap		maxheap
		TInt err = thread.Create(KThreadName, &CompoundSelectStackOverflowThreadFunc, 0x2000, 0x00100000, 0x02000000, &cnt);
		TEST2(err, KErrNone);
		
		TRequestStatus status;
		thread.Logon(status);
		TEST2(status.Int(), KRequestPending);
		thread.Resume();
		User::WaitForRequest(status);
		User::SetJustInTime(ETrue);	// enable debugger panic handling

		TInt exitType = thread.ExitType();
		const TDesC& exitCategory = thread.ExitCategory();
		TInt exitReason = thread.ExitReason();
		TheTest.Printf(_L("Exit type: %d, exit reason: %d, exit category: %S\r\n"), exitType, exitReason, &exitCategory);
		thread.Close();
		if(exitReason == KErrServerTerminated)	//SQL server --> stack overflow
			{
			continue;	
			}
		TEST2(exitReason, KErrNone);
		TheTest.Printf(_L("  The test has succeeded with SELECT statement count=%d\r\n"), cnt);
		break;
		}
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4198
@SYMTestCaseDesc		Expired SQL statements test.
						The test creates a database and opens 2 connections to that database.
						Connection 2 prepares couple of SELECT and INSERT statements (8-bit and 16-bit).
						Then connection 1 renames the table used in the already prepared statements.
						Connection 2 attempts to execute the prepared statements. The execution should fail
						because the database schema has changed after they were prepared.
@SYMTestActions			Expired SQL statements test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145236
*/
void ExpiredStmtTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	//Create a database and create db connection 1.
	TInt err = TheDb.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(C1 INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(C1) VALUES(1)"));
	TEST2(err, 1);
	
	//Create db connection 2 to the same database, as db connection 1.
	RSqlDatabase db2;
	err = db2.Open(KTestDbName1);
	TEST2(err, KErrNone);
	
	//Db connection 2. Prepare SELECT and INSERT, 8-bit and 16-bit statements. 
	RSqlStatement stmt1, stmt2, stmt3, stmt4;
	err = stmt1.Prepare(db2, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt2.Prepare(db2, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt3.Prepare(db2, _L("INSERT INTO A(C1) VALUES(2)"));
	TEST2(err, KErrNone);
	err = stmt4.Prepare(db2, _L8("INSERT INTO A(C1) VALUES(3)"));
	TEST2(err, KErrNone);
	
	//Modify the A table structure from the other connection
	//err = TheDb.Exec(_L("ALTER TABLE A ADD C2 INTEGER"));
	err = TheDb.Exec(_L("ALTER TABLE A RENAME TO B"));
	TEST(err >= 0);
	
	//Try to execute the already prepared statements.
	err = stmt1.Next();
	TEST2(err, KSqlErrSchema);
	err = stmt1.Next();
	TEST(err != KSqlAtRow);
	err = stmt2.Next();
	TEST(err != KSqlAtRow);
	err = stmt3.Exec();
	TEST(err < 0);
	err = stmt4.Exec();
	TEST(err < 0);
	//
	stmt4.Close();
	stmt3.Close();
	stmt2.Close();
	stmt1.Close();
	db2.Close();
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3512 RSqlStatement::ColumnCount() tests "));
	ColumnCountTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3513 RSqlStatement::ColumnCount(), non-SELECT tests "));
	ColumnCountTest2();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3514 RSqlStatement::DeclaredColumnType() tests "));
	DeclaredColumnTypeTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4017 RSqlStatement::ColumnName() tests"));	
	ColumnNameTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4018 RSqlStatement::ParameterName() and RSqlStatement::ParamName() tests"));	
	ParamNameTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4006 DEF115300 - SqlSrv.EXE::!SQL Server when preparing invalid LEFT JOIN sql query "));	
	DEF115300();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4007 DEF115331 - SQL, bad code coverage for db reindexing if default collation has changed "));
	DEF115331L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4079 RSqlDatabase::Create() and RSqlDatabase::Open() - injection tests"));	
	InjectionTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4038 Two connections test"));	
	TwoConnectionsTest();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4080 SQL server stack overflow test"));
	SqlServerStackOverflowTest();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4086 RSqlBlobWriteStream/RSqlBlobReadStream injection test"));
	BlobStreamInjectionTest();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4087 Bound parameter values test 1"));
	BoundParameterValuesTest();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4076 Bound parameter values test 2"));
	BoundParameterValuesTest2();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4077 Bound parameter values test 3"));
	BoundParameterValuesTest3();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4083 Bound parameter values test 4"));
	BoundParameterValuesTest4();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4105 Bound parameter values test 5"));
	BoundParameterValuesTest5();
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4088 TSqlResourceProfiler - file I/O and configuration test"));
	ProfilerTest();
	TheTest.Next( _L(" Compound SELECT, stack overflow test"));
	CompoundSelectStackOverflowTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4198 Expired statements test"));
	ExpiredStmtTest();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestEnv();
	DeleteTestFiles();
	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
