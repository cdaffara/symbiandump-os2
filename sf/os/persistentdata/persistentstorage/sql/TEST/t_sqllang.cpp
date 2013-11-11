// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqllang test"));
_LIT(KTestDir, "c:\\test\\");

_LIT(KTestDbName, "c:\\test\\t_sqllang_1.db");
_LIT(KTestDbName2, "c:\\test\\t_sqllang_2.db");

RSqlDatabase TheDb;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName2);
	RSqlDatabase::Delete(KTestDbName);
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
void Check2(TInt64 aValue, TInt64 aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %ld, got: %ld\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
#define TEST3(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}
	
///////////////////////////////////////////////////////////////////////////////////////

void PrintLastMsgIfError(TInt aErr)
	{
	if(aErr < 0 && SqlRetCodeClass(aErr) == ESqlDbError)
		{
		const TPtrC& msg = TheDb.LastErrorMessage();
		RDebug::Print(_L("Last error msg: \"%S\"\r\n"), &msg);	
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1611
@SYMTestCaseDesc		Create a table with INTEGER, SMALLINT, REAl, DOUBLE PRECISION, FLOAT, DECIMAL,...
						columns. Insert some records, retrieve the column values, verify the column values,
						try some mathematical operations with the column values, executed in the SQL
						statement.
@SYMTestPriority		High
@SYMTestActions			Testing SQL engine behaviour with different numerical types. 
						Testing mathematical operations with numeric columns.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void NumericDataTypesTest()
	{
	TEST2(TheDb.Create(KTestDbName), KErrNone);
	
	//Create a table with all possible numeric field types
	_LIT8(KSql1, "CREATE TABLE Tbl(A INTEGER, B SMALLINT, C REAL, D DOUBLE PRECISION, E FLOAT, \
					                    F DECIMAL, G BIGINT, H TINYINT, I BIT, J NUMERIC, K MONEY, \
					                    L SMALLMONEY)");
	TInt err = TheDb.Exec(KSql1);
	PrintLastMsgIfError(err);
	TEST(err >= 0);
	//Insert one record in to the created table
	_LIT8(KSql2, "INSERT INTO Tbl(A,B,C,D,E,F,G,H,I,J,K,L) VALUES(2000000000, 30000, 123.45, 0.912E+55,\
	                                    1.34E-14, 1234.5678, 32000000000, 100, 7, 23.123456, 2123678.56, 11.45)");
	err = TheDb.Exec(KSql2);
	PrintLastMsgIfError(err);
	TEST2(err, 1);
	//Get the inserted record data
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT * FROM TBL"));
	PrintLastMsgIfError(err);
	TEST2(err, KErrNone);
	err = stmt.Next();
	PrintLastMsgIfError(err);
	TEST2(err, KSqlAtRow);
	//Check column values
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("a"))) == 2000000000);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("b"))) == 30000);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("c"))) - 123.45) < 0.000001);
	//The next column value test is not working! The extracted column value is 0.9120000000000002E+55
	//I guess the reason is that SQLITE uses manifest typing and "DOUBLE PRECISION" is not in its keyword
	//list. Maybe it is interpreted and stored as 4-byte float value.
	//TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("d"))) - 0.912E+55) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("e"))) - 1.34E-14) < 0.000001);
	TEST(stmt.ColumnReal(stmt.ColumnIndex(_L("f"))) == 1234.5678);
	TEST(stmt.ColumnInt64(stmt.ColumnIndex(_L("g"))) == 32000000000LL);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("h"))) == 100);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("i"))) == 7);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("j"))) - 23.123456) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("k"))) - 2123678.56) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("l"))) - 11.45) < 0.000001);
	stmt.Close();
	//The statement object has to be closed before TheDb.Exec() call, 
	//otherwise the reported error is "database table is locked"
	//Insert second record in to the created table but inverse the column types!
	_LIT8(KSql3, "INSERT INTO Tbl(A,   B,  C, D, E, F,  G,  H,  I,  J,K,L) VALUES(\
										-2.5,1.1,12,23,45,111,5.6,7.9,1.1,2,6,7)");
	err = TheDb.Exec(KSql3);
	PrintLastMsgIfError(err);
	TEST2(err, 1);
	//Get the inserted record data
	err = stmt.Prepare(TheDb, _L("SELECT * FROM TBL"));
	PrintLastMsgIfError(err);
	TEST2(err, KErrNone);
	TEST2(stmt.Next(), KSqlAtRow);
	err = stmt.Next();
	PrintLastMsgIfError(err);
	TEST2(err, KSqlAtRow);
	//No column value checking because SQLITE inverted the column types!
	//Check column values
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("a"))) - (-2.5)) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("b"))) - 1.1) < 0.000001);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("c"))) == 12);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("d"))) == 23);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("e"))) == 45);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("f"))) == 111);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("g"))) - 5.6) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("h"))) - 7.9) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("i"))) - 1.1) < 0.000001);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("j"))) == 2);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("k"))) == 6);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("l"))) == 7);
	
	stmt.Close();
	//Insert third record in to the created table
	_LIT8(KSql4, "INSERT INTO Tbl(A,B,C,     D,  E,  F,   G,  H,  I,J,     K,     L) VALUES(\
	                                    2,3,123.45,1.5,2.5,1.56,320,100,7,23.123,212.56,11.45)");
	err = TheDb.Exec(KSql4);
	PrintLastMsgIfError(err);
	TEST2(err, 1);
	//Issue a "SELECT" statement doing there some arithmetic operations and comparisons on the column values.
	err = stmt.Prepare(TheDb, _L("SELECT A, A-C AS A2, K + L AS A3, H*I AS A4, E/D AS A5 FROM TBL WHERE A > 0 AND A < 10"));
	PrintLastMsgIfError(err);
	TEST2(err, KErrNone);
	TEST2(stmt.Next(), KSqlAtRow);
	//Check column values
	TEST2(stmt.ColumnInt(stmt.ColumnIndex(_L("a"))), 2);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("a2"))) - (2-123.45)) < 0.000001);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("a3"))) - (212.56+11.45)) < 0.000001);
	TEST(stmt.ColumnInt(stmt.ColumnIndex(_L("a4"))) == 700);
	TEST(Abs(stmt.ColumnReal(stmt.ColumnIndex(_L("a5"))) - (2.5/1.5)) < 0.000001);
	//There should be no more records
	TEST2(stmt.Next(), KSqlAtEnd);
	stmt.Close();

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1630
@SYMTestCaseDesc		Built-in functions test.
						abs(), coalesce(), ifnull(), last_insert_rowid(), length(), like(), lower(),
						max(), min(), nullif(), quote(), random(), round(), sqlite_version(), substr(),
						typeof(), upper(), avg(), count(), sun().
@SYMTestPriority		High
@SYMTestActions			Built-in functions test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void BuiltInFunctionsTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);

	_LIT(KCreateSql, "CREATE TABLE A(Id Integer, \
									 F1 Integer DEFAULT 0, \
									 F2 Integer Default NULL, \
									 F3 TEXT Default Null, \
									 F4 TEXT Default Null)");
	err = TheDb.Exec(KCreateSql);
	TEST(err >= 0);

	_LIT(KInsertSql1, "INSERT INTO A(Id, F1) VALUES(1, 100)");
	err = TheDb.Exec(KInsertSql1);
	TEST2(err, 1);
	
	_LIT(KInsertSql2, "INSERT INTO A(Id, F1) VALUES(2, 1)");
	err = TheDb.Exec(KInsertSql2);
	TEST2(err, 1);

	_LIT(KInsertSql3, "INSERT INTO A(Id, F3) VALUES(3, 'ABCD')");
	err = TheDb.Exec(KInsertSql3);
	TEST2(err, 1);

	_LIT(KInsertSql4, "INSERT INTO A(Id, F4) VALUES(4, 'DCBA')");
	err = TheDb.Exec(KInsertSql4);
	TEST2(err, 1);

	RSqlStatement stmt;
	
	//abs() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT * FROM A WHERE F1 > ABS(-10)"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(1) == 100);
	stmt.Close();

	//coalesce() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT COALESCE(F3, F4) AS F FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.IsNull(0));
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.IsNull(0));
	
	_LIT(KTextVal1, "ABCD");
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC colVal;
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	TEST(colVal == KTextVal1);
	
	_LIT(KTextVal2, "DCBA");
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	TEST(colVal == KTextVal2);

	stmt.Close();

	//ifnull() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT IFNULL(F3, F4) AS F FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.IsNull(0));
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.IsNull(0));
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	TEST(colVal == KTextVal1);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	TEST(colVal == KTextVal2);

	stmt.Close();

	//last_insert_rowid() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT last_insert_rowid() AS F"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt64 colVal64 = stmt.ColumnInt64(0);
	RDebug::Print(_L("Last insert row id=%d\r\n"), (TInt)colVal64);
	stmt.Close();

	//length() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT length(F4) AS L FROM A WHERE ID >= 3"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 0);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 4);
	
	stmt.Close();

	//like() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT Id FROM A WHERE F4 LIKE 'DC%'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 4);
	stmt.Close();

	_LIT(KInsertSql5, "INSERT INTO A(Id, F4) VALUES(5, 'ab%cd')");
	err = TheDb.Exec(KInsertSql5);
	TEST2(err, 1);
	
	err = stmt.Prepare(TheDb, _L("SELECT Id FROM A WHERE F4 LIKE 'ab/%cd' ESCAPE '/'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 5);
	stmt.Close();

	err = stmt.Prepare(TheDb, _L8("SELECT Id FROM A WHERE F4 LIKE 'ab/%cd' ESCAPE '/'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 5);
	stmt.Close();

	//lower() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT LOWER(F3) FROM A WHERE F3 = 'ABCD'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal3, "abcd");
	TEST(colVal == KTextVal3);
	stmt.Close();

	//max() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT MAX(F1) AS M FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 100);
	stmt.Close();
	
	//min() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT MIN(F1) AS M FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 0);
	stmt.Close();

	//nullif() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT NULLIF(3, 4) AS M"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 3);
	stmt.Close();
	
	err = stmt.Prepare(TheDb, _L("SELECT NULLIF(4, 4) AS M"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.IsNull(0));
	stmt.Close();
	
	//quote() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT QUOTE(F4) AS M FROM A WHERE Id = 5"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal4, "'ab%cd'");
	TEST(colVal == KTextVal4);
	stmt.Close();

	//random() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT * FROM A WHERE Id > RANDOM(*)"));
	TEST2(err, KErrNone);
	stmt.Close();

	//round() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT ROUND(5.4) AS D"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(Abs(stmt.ColumnReal(0) - 5.0) < 0.000001);
	stmt.Close();

	err = stmt.Prepare(TheDb, _L("SELECT ROUND(5.4321, 2) AS D"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(Abs(stmt.ColumnReal(0) - 5.43) < 0.000001);
	stmt.Close();

	//sqlite_version() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT sqlite_version(*) AS V"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	RDebug::Print(_L("Database engine version: \"%S\"\r\n"), &colVal);
	stmt.Close();

	//substr() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT SUBSTR('abcd', 2, 2) AS S"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal7, "bc");
	TEST(colVal == KTextVal7);
	stmt.Close();

	err = stmt.Prepare(TheDb, _L("SELECT SUBSTR('abcd', -3, 2) AS S"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	TEST(colVal == KTextVal7);
	stmt.Close();

	//typeof() test --------------------------------------------------------
	_LIT(KInsertSql6, "INSERT INTO A(Id, F2) VALUES(6, 2)");
	err = TheDb.Exec(KInsertSql6);
	TEST2(err, 1);

	err = stmt.Prepare(TheDb, _L("SELECT TYPEOF(F1 + F2) AS T FROM A WHERE ID > 4"));
	TEST2(err, KErrNone);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal8, "null");
	TEST(colVal == KTextVal8);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal9, "integer");
	TEST(colVal == KTextVal9);
	
	stmt.Close();

	//upper() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT UPPER('ghjk') AS U"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, colVal);
	TEST2(err, KErrNone);
	_LIT(KTextVal10, "GHJK");
	TEST(colVal == KTextVal10);
	stmt.Close();

	//avg() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT AVG(F2) AS F FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(Abs(stmt.ColumnReal(0) - 2) < 0.000001);
	stmt.Close();

	//count() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT COUNT(F2) AS F FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 1);
	stmt.Close();

	err = stmt.Prepare(TheDb, _L("SELECT COUNT(*) AS F FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(stmt.ColumnInt(0) == 6);
	stmt.Close();

	//sum() test --------------------------------------------------------
	err = stmt.Prepare(TheDb, _L("SELECT SUM(F2) AS S FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TEST(Abs(stmt.ColumnReal(0) - 2) < 0.000001);
	stmt.Close();

	// ----------------------------------
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1632
@SYMTestCaseDesc		"ATTACH DATABASE" test.
						The test creates two databases. The main database has 
						Account(Id Integer, PersonId Integer, Value Integer) table.
						The attached database has Person(Id Integer, Name TEXT) table.
						The test inserts some records with a valid relation between them in both tables.
						Then the test opens tha main database and attaches the second database to the first one.
						The test prepares and executes SQL statement which retrieves column values from both
						Account and Person tables. The test checks the column values.
@SYMTestPriority		High
@SYMTestActions			"ATTACH DATABASE" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void AttachDatabaseTest()
	{
	// ------------------------------------------------------------
	RSqlDatabase db2;
	TInt err = db2.Create(KTestDbName2);
	TEST2(err, KErrNone);
	
	err = db2.Exec(_L("CREATE TABLE Person(Id Integer, Name TEXT)"));
	TEST(err >= 0);

	err = db2.Exec(_L("INSERT INTO Person(Id, Name) VALUES(1, 'A')"));
	TEST2(err, 1);

	err = db2.Exec(_L("INSERT INTO Person(Id, Name) VALUES(2, 'B')"));
	TEST2(err, 1);

	err = db2.Exec(_L("INSERT INTO Person(Id, Name) VALUES(3, 'C')"));
	TEST2(err, 1);
	
	db2.Close();

	// ------------------------------------------------------------
	(void)RSqlDatabase::Delete(KTestDbName);
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);

	err = TheDb.Exec(_L("CREATE TABLE Account(Id Integer, PersonId Integer, Value Integer)"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("INSERT INTO ACCOUNT(Id, PersonId, Value) VALUES(1, 2, 20)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("INSERT INTO ACCOUNT(Id, PersonId, Value) VALUES(2, 1, 10)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("INSERT INTO ACCOUNT(Id, PersonId, Value) VALUES(3, 2, 25)"));
	TEST2(err, 1);

	err = TheDb.Exec(_L("INSERT INTO ACCOUNT(Id, PersonId, Value) VALUES(4, 3, 30)"));
	TEST2(err, 1);

	TheDb.Close();
	
	// ------------------------------------------------------------
	err = TheDb.Open(KTestDbName);	
	TEST2(err, KErrNone);
	
	TBuf<100> sql;
	sql.Copy(_L("ATTACH DATABASE '"));
	sql.Append(KTestDbName2);
	sql.Append(_L("' AS DB2"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT Account.Value, DB2.Person.Name FROM Account, DB2.Person \
						   		  WHERE Account.PersonId = DB2.Person.Id"));
	TEST2(err, KErrNone);

	TPtrC personName;
	_LIT(KName1, "A");
	_LIT(KName2, "B");
	_LIT(KName3, "C");

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(1, personName);
	TEST2(err, KErrNone);
	TEST(personName == KName2);
	TEST(stmt.ColumnInt(0) == 20);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(1, personName);
	TEST2(err, KErrNone);
	TEST(personName == KName1);
	TEST(stmt.ColumnInt(0) == 10);
		
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(1, personName);
	TEST2(err, KErrNone);
	TEST(personName == KName2);
	TEST(stmt.ColumnInt(0) == 25);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(1, personName);
	TEST2(err, KErrNone);
	TEST(personName == KName3);
	TEST(stmt.ColumnInt(0) == 30);
		
	stmt.Close();
	
	err = TheDb.Exec(_L("DETACH DATABASE DB2"));
	TEST(err >= 0);
		
	// ------------------------------------------------------------
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3502
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
						The test creates a test table with INT32, INT64, DOUBLE, TEXT, and BINARY columns
						and inserts couple of records there. Then the test prepares a SELECT statement,
						retrieves all table rows, and to retrieve each column value, calls RSqlStatement::ColumnInt().
						The column values are carefully chosen, so some of the ColumnInt() calls have to round
						or clamp the returned value. 
						Summary: the test checks the ColumnInt() behaviour when the column value type is not INT32 and
								 when the column value is too big or too small and cannot fit in 32 bits.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void ColumnIntTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I32 INTEGER,I64 INTEGER,D DOUBLE,T TEXT,B BINARY)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(-5,-5000000000,-10000000000.0,'AAA',x'1122FF')"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,5000000000,10000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,2000000000,1000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(NULL,NULL,1.1234567890123456E+317,NULL,NULL)"));
	TEST2(err, 1);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT I32,I64,D,T,B FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt val = stmt.ColumnInt(0);	
	TEST2(val, -5);					//"-5" is a 32-bit integer value
	val = stmt.ColumnInt(1);
	TEST2(val, KMinTInt);			//"-5000000000" is a 64-bit integer, will be clamped to KMinTInt
	val = stmt.ColumnInt(2);
	TEST2(val, KMinTInt);			//"-10000000000.0" is a 64-bit double, will be rounded to the nearest 32-bit integer
	val = stmt.ColumnInt(3);
	TEST2(val, 0);					//"AAA" is a text string, cannot be converted to a 32-bit integer
	val = stmt.ColumnInt(4);
	TEST2(val, 0);					//"1122FF" is a hex binary, cannot be converted to a 32-bit integer

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt(0);	
	TEST2(val, 5);					//"5" is a 32-bit integer value
	val = stmt.ColumnInt(1);
	TEST2(val, KMaxTInt);			//"5000000000" is a 64-bit integer, will be clamped to KMaxTInt
	val = stmt.ColumnInt(2);
	TEST2(val, KMaxTInt);			//"10000000000.0" is a 64-bit double, will be rounded to the nearest 32-bit integer
	val = stmt.ColumnInt(3);
	TEST2(val, 0);					//NULL column value
	val = stmt.ColumnInt(4);
	TEST2(val, 0);					//NULL column value

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt(0);	
	TEST2(val, 5);					
	val = stmt.ColumnInt(1);
	TEST2(val, 2000000000);					
	val = stmt.ColumnInt(2);
	TEST2(val, 1000000000);					
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt(2);
	TEST2(val, KMinTInt);			//"1.1234567890123456E+317" is too big and cannot fit in a 64-bit double
	
	stmt.Close();
	
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3503
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
						The test creates a test table with INT32, INT64, DOUBLE, TEXT, and BINARY columns
						and inserts couple of records there. Then the test prepares a SELECT statement,
						retrieves all table rows, and to retrieve each column value, calls RSqlStatement::ColumnInt64().
						The column values are carefully chosen, so some of the ColumnInt64() calls have to round
						or clamp the returned value. 
						Summary: the test checks the ColumnInt64() behaviour when the column value type is not INT64 and
								 when the column value is too big or too small and cannot be presented as 64-bits integer.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void ColumnInt64Test()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I32 INTEGER,I64 INTEGER,D DOUBLE,T TEXT,B BINARY)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(-5,-5000000000,-10000000000.0,'AAA',x'1122FF')"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,5000000000,10000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,2000000000,1000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(NULL,NULL,1.1234567890123456E+317,NULL,NULL)"));
	TEST2(err, 1);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT I32,I64,D,T,B FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt64 val = stmt.ColumnInt64(0);	
	TEST3(val, -5);					//"-5" is a 32-bit integer value
	val = stmt.ColumnInt64(1);
	TEST3(val, -5000000000LL);		//"-5000000000" is a 64-bit integer
	val = stmt.ColumnInt64(2);
	TEST3(val, -10000000000LL);		//"-10000000000.0" is a 64-bit double, will be rounded to the nearest 64-bit integer
	val = stmt.ColumnInt64(3);
	TEST3(val, 0);					//"AAA" is a text string, cannot be converted to a 64-bit integer
	val = stmt.ColumnInt64(4);
	TEST3(val, 0);					//"1122FF" is a hex binary, cannot be converted to a 64-bit integer

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt64(0);	
	TEST3(val, 5);					//"5" is a 32-bit integer value
	val = stmt.ColumnInt64(1);
	TEST3(val, 5000000000LL);		//"5000000000" is a 64-bit integer
	val = stmt.ColumnInt64(2);
	TEST3(val, 10000000000LL);		//"10000000000.0" is a 64-bit double, will be rounded to the nearest 64-bit integer
	val = stmt.ColumnInt64(3);
	TEST3(val, 0);					//NULL column value
	val = stmt.ColumnInt64(4);
	TEST3(val, 0);					//NULL column value

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt64(0);	
	TEST3(val, 5);					
	val = stmt.ColumnInt64(1);
	TEST3(val, 2000000000);					
	val = stmt.ColumnInt64(2);
	TEST3(val, 1000000000);					
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnInt64(2);
	TEST3(val, KMinTInt64);			//"1.1234567890123456E+317" is too big and cannot fit in a 64-bit double
	
	stmt.Close();
	
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3504
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
						The test creates a test table with INT32, INT64, DOUBLE, TEXT, and BINARY columns
						and inserts couple of records there. Then the test prepares a SELECT statement,
						retrieves all table rows, and to retrieve each column value, calls RSqlStatement::ColumnReal().
						Summary: the test checks the ColumnReal() behaviour when the column value type is not DOUBLE and
								 when the column value is too big or too small and cannot be presented as 64-bits double.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void ColumnRealTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I32 INTEGER,I64 INTEGER,D DOUBLE,T TEXT,B BINARY)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(-5,-5000000000,-10000000000.0,'AAA',x'1122FF')"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,5000000000,10000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(5,2000000000,1000000000.0,NULL,NULL)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(NULL,NULL,1.1234567890123456E+317,NULL,NULL)"));
	TEST2(err, 1);

	const TReal KEpsilon = 0.000001;

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT I32,I64,D,T,B FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TReal val = stmt.ColumnReal(0);	
	TEST(Abs(val - (-5)) < KEpsilon);		
	val = stmt.ColumnReal(1);
	TEST(Abs(val - (-5000000000LL)) < KEpsilon);
	val = stmt.ColumnReal(2);
	TEST(Abs(val - (-10000000000LL)) < KEpsilon);
	val = stmt.ColumnReal(3);
	TEST(Abs(val) < 0.0001);					//"AAA" is a text string, cannot be converted to a 64-bit double
	val = stmt.ColumnReal(4);
	TEST(Abs(val) < 0.0001);					//"1122FF" is a hex binary, cannot be converted to a 64-bit double

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnReal(0);	
	TEST(Abs(val - 5) < KEpsilon);
	val = stmt.ColumnReal(1);
	TEST(Abs(val - 5000000000LL) < KEpsilon);
	val = stmt.ColumnReal(2);
	TEST(Abs(val - 10000000000LL) < KEpsilon);
	val = stmt.ColumnReal(3);
	TEST(Abs(val) < KEpsilon);					//NULL column value
	val = stmt.ColumnReal(4);
	TEST(Abs(val) < KEpsilon);					//NULL column value

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnReal(0);	
	TEST(Abs(val - 5) < KEpsilon);		
	val = stmt.ColumnReal(1);
	TEST(Abs(val - 2000000000) < KEpsilon);
	val = stmt.ColumnReal(2);
	TEST(Abs(val - 1000000000) < KEpsilon);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	val = stmt.ColumnReal(2);
	TEST(Math::IsInfinite(val));				//"1.1234567890123456E+317" is too big and cannot fit in a 64-bit double
	
	stmt.Close();
	
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3505
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
						The test creates a test table with INT32, INT64, DOUBLE, TEXT, and BINARY columns
						and inserts a record there. Then the test prepares a SELECT statement,
						retrieves all table rows, and to retrieve each column value, calls RSqlStatement::ColumnText().
						Summary: the test checks the ColumnText() behaviour when the column value type is not TEXT.
								 (In all non-TEXT cases the ,ethod is expected to return KNullDesC8)
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void ColumnTextTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I32 INTEGER,I64 INTEGER,D DOUBLE,T TEXT,B BINARY)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(-5,-5000000000,-10000000000.0,'AAA',x'1122FF')"));
	TEST2(err, 1);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT I32,I64,D,T,B FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC val;
	err = stmt.ColumnText(0, val);	
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC);
	err = stmt.ColumnText(1, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC);
	err = stmt.ColumnText(2, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC);
	err = stmt.ColumnText(3, val);
	TEST2(err, KErrNone);		
	TEST2(val.Length(), 3);
	TEST(val == _L("AAA"));
	err = stmt.ColumnText(4, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC);
	
	stmt.Close();
	
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3506
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
						The test creates a test table with INT32, INT64, DOUBLE, TEXT, and BINARY columns
						and inserts a record there. Then the test prepares a SELECT statement,
						retrieves all table rows, and to retrieve each column value, calls RSqlStatement::ColumnBinary().
						Summary: the test checks the ColumnBinary() behaviour when the column value type is not BINARY.
								 (In all non-BINARY cases the method is expected to return KNullDesC8)
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void ColumnBinaryTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I32 INTEGER,I64 INTEGER,D DOUBLE,T TEXT,B BINARY)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A VALUES(-5,-5000000000,-10000000000.0,'AAA',x'1122FF')"));
	TEST2(err, 1);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT I32,I64,D,T,B FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC8 val;
	err = stmt.ColumnBinary(0, val);	
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC8);
	err = stmt.ColumnBinary(1, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC8);
	err = stmt.ColumnBinary(2, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC8);
	err = stmt.ColumnBinary(3, val);
	TEST2(err, KErrNone);		
	TEST(val == KNullDesC8);
	err = stmt.ColumnBinary(4, val);
	TEST2(err, KErrNone);		
	TEST2(val.Length(), 3);
	TEST(val[0] == 0x11);
	TEST(val[1] == 0x22);
	TEST(val[2] == 0xFF);
	
	stmt.Close();
	
	TheDb.Close();
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	}
	
void DoTestsL()
	{	
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1611 E011 numeric data types test "));		
	NumericDataTypesTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1630 Built-in functions test "));		
	BuiltInFunctionsTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1632 \"Attach database\" test "));		
	AttachDatabaseTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3502 \"RSqlStatement::ColumnInt()\" test "));		
	ColumnIntTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3503 \"RSqlStatement::ColumnInt64()\" test "));		
	ColumnInt64Test();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3504 \"RSqlStatement::ColumnReal()\" test "));		
	ColumnRealTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3505 \"RSqlStatement::ColumnText()\" test "));		
	ColumnTextTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3506 \"RSqlStatement::ColumnBinary()\" test "));		
	ColumnBinaryTest();
	}
	
TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestDir();
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
