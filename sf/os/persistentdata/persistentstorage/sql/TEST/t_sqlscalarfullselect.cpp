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
#include <bautils.h>
#include <s32strm.h>
#include <e32math.h>
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqlscalarfullselect test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDatabase1, "c:\\test\\t_sqlscalarfullselect.db");

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DeleteTestFiles()
	{
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt64 aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt64 aValue, TInt64 aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

void CreateTestDbLC(RSqlDatabase& aDb)
	{
	CleanupClosePushL(aDb);
	TInt rc = aDb.Create(KTestDatabase1);
	TEST2(rc, KErrNone);
	rc = aDb.Exec(_L("CREATE TABLE A(F1 INTEGER, F2 INTEGER, F3 FLOAT, F4 TEXT, F5 BLOB)"));
	TEST(rc >= 0);
	rc = aDb.Exec(_L("INSERT INTO A(F1, F2, F3, F4, F5) VALUES(1, 10000000000, 2.54, 'NAME1234567890', NULL)"));
	TEST2(rc, 1);
	rc = aDb.Exec(_L("INSERT INTO A(F1, F2, F3, F4) VALUES(2, 200, -1.11, 'ADDRESS')"));
	TEST2(rc, 1);
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	rc = stmt.Prepare(aDb, _L("UPDATE A SET F5=:P WHERE F1 = 2"));
	TEST2(rc, KErrNone);
	RSqlParamWriteStream strm;
	CleanupClosePushL(strm);
	rc = strm.BindBinary(stmt, 0);
	TEST2(rc, KErrNone);
	for(TInt i=0;i<100;++i)
		{
		strm << static_cast <TUint8> (i);	
		}
	strm.CommitL();	
	rc = stmt.Exec();	
	TEST2(rc, 1);
	CleanupStack::PopAndDestroy(&strm);
	CleanupStack::PopAndDestroy(&stmt);
	}

void DestroyTestDb(RSqlDatabase& aDb)
	{
	CleanupStack::PopAndDestroy(&aDb);
	TInt err = RSqlDatabase::Delete(KTestDatabase1);	
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1809
@SYMTestCaseDesc		A test database is created, test table created in the database with integer, 64-bit
						integer, float, text and blob fields.
						Inserted two records. 
						The test calls TSqlScalarFullSelectQuery functions in all possible 
						"requested value type:real column type" combinations and checks the returned value.
@SYMTestPriority		High
@SYMTestActions			SQL, Scalar fullselect test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
template <class BUF> void ScalarFullSelectTestL()
	{
	//Create test database.
	RSqlDatabase db;
	CreateTestDbLC(db);
	TSqlScalarFullSelectQuery fullSelectQuery(db);

	BUF sql;

	/////////////////// tests with F1 column (integer column) ///////////////////////////
	_LIT(KSql1, "SELECT F1 FROM A WHERE F2 = 10000000000");
	sql.Copy(KSql1);
	//Read F1 as integer. Expected value: 1.
	TInt valInt = fullSelectQuery.SelectIntL(sql);
	TEST2(valInt, 1);
	//Read F1 as 64-bit integer. Expected value: 1.
	TInt64 valInt64 = fullSelectQuery.SelectInt64L(sql);
	TEST2(valInt64, 1);
	//Read F1 as real. Expected value: 1.0.
	TReal valReal = fullSelectQuery.SelectRealL(sql);
	TEST(Abs(valReal - 1.0) < 0.0001);
	TBuf<10> valText;
	//Read F1 as text. Expected value: zero-length 16-bit descriptor.
	TInt err = fullSelectQuery.SelectTextL(sql, valText);
	TEST2(err, KErrNone);
	TEST2(valText.Length(), 0);
	//Read F1 as binary. Expected value: zero-length 8-bit descriptor.
	TBuf8<10> valBinary;
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	TEST2(err, KErrNone);
	TEST2(valBinary.Length(), 0);
	/////////////////// tests with F2 column (64-bit integer column) ///////////////////////////
	_LIT(KSql2, "SELECT F2 FROM A WHERE F1 = 1");
	sql.Copy(KSql2);
	//Read F2 as integer. Expected value: KMaxTInt.
	valInt = fullSelectQuery.SelectIntL(sql);
	TEST2(valInt, KMaxTInt);
	//Read F2 as 64-bit integer. Expected value: 10000000000
	valInt64 = fullSelectQuery.SelectInt64L(sql);
	TEST2(valInt64, 10000000000LL);
	//Read F2 as real. Expected value: 10000000000.0
	valReal = fullSelectQuery.SelectRealL(sql);
	TEST(Abs(valReal - 10000000000.0) < 0.0001);
	//Read F2 as text. Expected value: zero-length 16-bit descriptor.
	err = fullSelectQuery.SelectTextL(sql, valText);
	TEST2(err, KErrNone);
	TEST2(valText.Length(), 0);
	//Read F2 as binary. Expected value: zero-length 8-bit descriptor.
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	TEST2(err, KErrNone);
	TEST2(valBinary.Length(), 0);
	/////////////////// tests with F3 column (real column) ///////////////////////////
	_LIT(KSql3, "SELECT F3 FROM A WHERE F1 = 1");
	sql.Copy(KSql3);
	//Read F3 as integer. Expected value: 3.
	valInt = fullSelectQuery.SelectIntL(sql);
	TEST2(valInt, 3);
	//Read F3 as 64-bit integer. Expected value: 3.
	valInt64 = fullSelectQuery.SelectInt64L(sql);
	TEST2(valInt64, 3);
	//Read F3 as real. Expected value: 2.54.
	valReal = fullSelectQuery.SelectRealL(sql);
	TEST(Abs(valReal - 2.54) < 0.0001);
	//Read F3 as text. Expected value: zero-length 16-bit descriptor.
	err = fullSelectQuery.SelectTextL(sql, valText);
	TEST2(err, KErrNone);
	TEST2(valText.Length(), 0);
	//Read F3 as binary. Expected value: zero-length 8-bit descriptor.
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	TEST2(err, KErrNone);
	TEST2(valBinary.Length(), 0);
	/////////////////// tests with F4 column (text column) ///////////////////////////
	_LIT(KSql4, "SELECT F4 FROM A WHERE F1 = 1");
	sql.Copy(KSql4);
	//Read F4 as integer. Expected value: 0.
	valInt = fullSelectQuery.SelectIntL(sql);
	TEST2(valInt, 0);
	//Read F4 as 64-bit integer. Expected value: 0.
	valInt64 = fullSelectQuery.SelectInt64L(sql);
	TEST2(valInt64, 0);
	//Read F4 as real. Expected value: 0.0.
	valReal = fullSelectQuery.SelectRealL(sql);
	TEST(Abs(valReal - 0.0) < 0.0001);
	//Read F4 as text. Small buffer. Expected return code: positive value, which is the column length in characters.
	err = fullSelectQuery.SelectTextL(sql, valText);
	TEST(err > 0);
	TEST2(valText.Length(), 10);
	_LIT(KColText, "NAME123456");
	TEST(valText == KColText());
	//Read F4 as text. Big enough buffer. Expected error: KErrNone.
	TBuf<32> valText2;
	err = fullSelectQuery.SelectTextL(sql, valText2);
	TEST2(err, KErrNone);
	TEST2(valText2.Length(), 14);
	_LIT(KColText2, "NAME1234567890");
	TEST(valText2 == KColText2());
	//Read F4 as binary. Expected error: KErrNone. Zero-length 8-bit descriptor.
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	TEST2(err, KErrNone);
	TEST2(valBinary.Length(), 0);
	/////////////////// tests with F5 column (binary column) ///////////////////////////
	_LIT(KSql5, "SELECT F5 FROM A WHERE F1 = 2");
	sql.Copy(KSql5);
	//Read F5 as integer. Expected value: 0.
	valInt = fullSelectQuery.SelectIntL(sql);
	TEST2(valInt, 0);
	//Read F5 as 64-bit integer. Expected value: 0.
	valInt64 = fullSelectQuery.SelectInt64L(sql);
	TEST2(valInt64, 0);
	//Read F5 as real. Expected value: 0.0.
	valReal = fullSelectQuery.SelectRealL(sql);
	TEST(Abs(valReal - 0.0) < 0.0001);
	//Read F5 as text. Expected error: KErrNone. Zero-length 16-bit descriptor.
	err = fullSelectQuery.SelectTextL(sql, valText);
	TEST2(err, KErrNone);
	TEST2(valText.Length(), 0);
	//Read F5 as binary. Small buffer. Expected return code: positive value, which is the column length in bytes.
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	TEST(err > 0);
	TEST2(valBinary.Length(), 10);
	TInt i;
	for(i=0;i<10;++i)
		{
		TEST(valBinary[i] == i);	
		}
	//Read F5 as binary. Big enough buffer. Expected error: KErrNone.
	TBuf8<100> valBinary2;
	err = fullSelectQuery.SelectBinaryL(sql, valBinary2);
	TEST2(err, KErrNone);
	TEST2(valBinary2.Length(), 100);
	for(i=0;i<100;++i)
		{
		TEST(valBinary2[i] == i);	
		}
	///////////////////  Text column value, small buffer, reallocation /////////////////// 
	HBufC* hbuf = HBufC::NewLC(10);
	TPtr name = hbuf->Des();
	sql.Copy(KSql4);
	err = fullSelectQuery.SelectTextL(sql, name);
	TEST(err >= 0); //the function may return only non-negative values
	if(err > 0)
		{
		hbuf = hbuf->ReAllocL(err);
		CleanupStack::Pop();	
		CleanupStack::PushL(hbuf);
		name.Set(hbuf->Des());
		err = fullSelectQuery.SelectTextL(sql, name);
		TEST2(err, KErrNone);
		TEST(name == KColText2());
		}
	CleanupStack::PopAndDestroy();//hbuf, can't be put as parameter, because may be reallocated
	//Close database, delete database file.
	DestroyTestDb(db);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1810
@SYMTestCaseDesc		A test database is created, test table created in the database with integer, 64-bit
						integer, float, text and blob fields.
						Inserted two records. 
						The test calls TSqlScalarFullSelectQuery functions using inappropriate SQL statements:
						SELECT sql statement with parameters, INSERT sql statement, SELECT sql statement,
						which does not return records.
@SYMTestPriority		High
@SYMTestActions			SQL, Scalar fullselect test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
template <class BUF> void ScalarFullSelectNegativeTestL()
	{
	//Create test database.
	RSqlDatabase db;
	CreateTestDbLC(db);
	TSqlScalarFullSelectQuery fullSelectQuery(db);

	BUF sql;

	//An attempt to use inappropriate SQL - 1.
	_LIT(KSql1, "SELECT F1 FROM A WHERE F2 = :P");
	sql.Copy(KSql1);
	TRAPD(err, fullSelectQuery.SelectIntL(sql));
	TEST2(err, KErrArgument);

	//An attempt to use inappropriate SQL - 2.
	_LIT(KSql2, "INSERT INTO A(F1) VALUES(2)");
	sql.Copy(KSql2);
	TRAP(err, fullSelectQuery.SelectIntL(sql));
	TEST2(err, KErrArgument);

	//The SQL statement does not return any rows
	_LIT(KSql3, "SELECT F1 FROM A WHERE F2 = 456231");
	sql.Copy(KSql3);
	TRAP(err, fullSelectQuery.SelectIntL(sql));
	TEST2(err, KErrNotFound);

	//Close database, delete database file.
	DestroyTestDb(db);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4204
@SYMTestCaseDesc        TSqlScalarFullSelectQuery - border test.
@SYMTestPriority        High
@SYMTestActions         The test checks some border test cases such as:
						- retrieving NULL column as integer;
						- retrieving NULL column as 64-bit integer;
						- retrieving NULL column as TReal;
						- retrieving column value smaller than KMinTInt, as integer;
						- retrieving column value bigger than KMaxTInt, as integer;
@SYMTestExpectedResults Test must not fail
*/  
void ScalarFullSelectBorderTest()
	{
	(void)RSqlDatabase::Delete(KTestDatabase1);
	RSqlDatabase db;
	TInt rc = db.Create(KTestDatabase1);
	TEST2(rc, KErrNone);
	rc = db.Exec(_L("CREATE TABLE A(F1 INTEGER NULL, F2 INTEGER NULL, F3 FLOAT NULL, F4 TEXT NULL, F5 BLOB NULL)"));
	TEST(rc >= 0);
	
	TSqlScalarFullSelectQuery q(db);

	//Insert one record. Bigger than KMaxTInt F1 column value. Smaller than KMinTInt F2 column value.
	rc = db.Exec(_L("INSERT INTO A(F1,F2,F4) VALUES(5000000000,-5000000000,'aljhsfdlgefberveurfgvefkjgs;kjfgs;kjfsd')"));
	TEST2(rc, 1);
	//Select NULL column value as int.
	TInt res = -1;
	TRAP(rc, res = q.SelectIntL(_L("SELECT F5 FROM A")));
	TEST2(rc, KErrNone);
	TEST2(res, 0);
	//Select NULL column value as int64.
	res = -1;
	TRAP(rc, res = q.SelectInt64L(_L("SELECT F5 FROM A")));
	TEST2(rc, KErrNone);
	TEST2(res, 0);
	//Select NULL column value as TReal.
	TReal res2 = -1.0;
	TRAP(rc, res2 = q.SelectRealL(_L("SELECT F5 FROM A")));
	TEST2(rc, KErrNone);
	TEST(Abs(res2) < 0.000001);
	//Select NULL column value as text.
	TBuf<10> text;
	TRAP(rc, res = q.SelectTextL(_L("SELECT F5 FROM A"), text));
	TEST2(rc, KErrNone);
	TEST2(res, 0);
	TEST2(text.Length(), 0);
	//Select NULL column value as binary.
	TBuf8<10> data;
	TRAP(rc, res = q.SelectBinaryL(_L("SELECT F5 FROM A"), data));
	TEST2(rc, KErrNone);
	TEST2(res, 0);
	TEST2(data.Length(), 0);
	//Select column value bigger than KMaxTInt, as int.
	res = -1;
	TRAP(rc, res = q.SelectIntL(_L("SELECT F1 FROM A")));
	TEST2(rc, KErrNone);
	TEST2(res, KMaxTInt);
	//Select column value smaller than KMinTInt, as int.
	res = -1;
	TRAP(rc, res = q.SelectIntL(_L("SELECT F2 FROM A")));
	TEST2(rc, KErrNone);
	TEST2(res, KMinTInt);

	db.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}
	

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1809 Scalar fullselect test. 16-bit SQL "));
	ScalarFullSelectTestL< TBuf16<100> >();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1809 Scalar fullselect test. 8-bit SQL "));
	ScalarFullSelectTestL< TBuf8<100> >();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1810 Scalar fullselect - negative test. 16-bit SQL "));
	ScalarFullSelectNegativeTestL< TBuf16<100> >();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1810 Scalar fullselect - negative test. 8-bit SQL "));
	ScalarFullSelectNegativeTestL< TBuf8<100> >();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4204 Scalar fullselect - border cases "));
	ScalarFullSelectBorderTest();
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
	TheFs.Close();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
