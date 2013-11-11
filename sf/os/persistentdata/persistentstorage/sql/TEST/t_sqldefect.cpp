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
// NTT DOCOMO, INC - Fix for Bug 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#include <e32test.h>
#include <bautils.h>
#include <s32stor.h>
#include <sqldb.h>
#include "SqlResourceProfiler.h"
#include "SqlResourceTester.h"

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqldefect test"));
RSqlDatabase TheDb;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDatabase1, "c:\\test\\t_sqldefect_1.db");
_LIT(KTestDatabase2, "c:\\test\\t_sqldefect_2.db");
_LIT(KCorruptDb, "c:\\test\\t_SqlShortNonDb.db");
_LIT(KCorruptDbZ, "z:\\test\\t_SqlShortNonDb.db");	//Created outside this test app
_LIT(KSecureTestDb1, "c:[21212122]BBDb2.db");		//Created outside this test app
_LIT(KTestDatabase3, "c:\\test\\t_sqldefect_3.db");
_LIT(KTestDatabase4, "z:\\test\\t_inc095412.db");	//Created outside this test app
_LIT(KTestDatabase5, "c:\\test\\t_sqldefect_5.db");
_LIT(KTestDatabase6, "c:\\test\\t_def120237.db");
_LIT(KTestDatabase7, "c:\\test\\t_def144027.db");
_LIT(KTestDatabase7Journal, "c:\\test\\t_def144027.db-journal");

// This value has been found by performing the OOM test
// with an allocation limit of 2000 and then taking a value
// which is just above the allocation failure rate.
const TInt KDEF115954MaxAllocLimit = 1300;

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DeleteTestFiles()
	{
	TheDb.Close();
	(void)TheFs.Delete(KTestDatabase7Journal);
	(void)RSqlDatabase::Delete(KTestDatabase7);
	(void)RSqlDatabase::Delete(KTestDatabase6);
	(void)RSqlDatabase::Delete(KTestDatabase5);
	(void)RSqlDatabase::Delete(KTestDatabase3);
	(void)RSqlDatabase::Delete(KTestDatabase2);
	(void)RSqlDatabase::Delete(KTestDatabase1);
	(void)RSqlDatabase::Delete(KCorruptDb);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
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

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1763
@SYMTestCaseDesc		The test creates database 1 and attaches database 2.
					    Then the test prepares a SELECT sql statement which is supposed to
					    retrieve records from the attached database 2. Then the test tries to detach
					    database 2. The expectation is that the detaching operation must fail.
					    The database can be detached only when there are no alive sql statements prepared on it.
@SYMTestPriority		High
@SYMTestActions			SQL, "Detach database" test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SqlDetachedDbTest()
	{
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));	
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));	
	TEST2(err, 1);
	TheDb.Close();	

	err = TheDb.Create(KTestDatabase2);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));	
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));	
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));	
	TEST2(err, 1);
	TheDb.Close();	

	err = TheDb.Open(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestDatabase2, _L("Db2"));
	TEST2(err, KErrNone);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM B"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	
	err = TheDb.Detach(_L("Db2"));
	TEST(err != KErrNone);
	TPtrC errMsg = TheDb.LastErrorMessage();
	RDebug::Print(_L("Detach err: %S\r\n"), &errMsg);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	stmt.Close();

	err = TheDb.Detach(_L("Db2"));
	TEST2(err, KErrNone);
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase2);
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4034
@SYMTestCaseDesc		Corrupted database test.
						The test tries to open a corrupted database file which length is too short.
						The 'database open' operation is expected to fail with KSqlErrNotDb error.
@SYMTestPriority		High
@SYMTestActions			Corrupted database test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
*/
void CorruptDbFileTest()
	{
	TInt err = TheDb.Open(KCorruptDb);
	TEST2(err, KSqlErrNotDb);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4035
@SYMTestCaseDesc		Attach database with bad name.
						Attempt to attach a file which name cannot be parsed.
						The 'attach database' operation is expected to fail with KErrBadName error.
@SYMTestPriority		High
@SYMTestActions			Attach database with bad name.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
*/
void AttachBadDbFileNameTest()
	{
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(_L("\"c:\\test\\d12345678.db\""), _L("Db10"));
	TEST2(err, KErrBadName);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4036
@SYMTestCaseDesc		Attach secure database. The client cannot pass the security checks.
						Attempt to attach a secure database. The client cannot pass the security checks.
						The 'attach database' operation is expected to fail with KErrPermissionDenied error.
@SYMTestPriority		High
@SYMTestActions			Attach secure database. The client cannot pass the security checks.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/
void AttachSecureDbTest()
	{
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KSecureTestDb1, _L("Db10"));
	TEST2(err, KErrPermissionDenied);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4032
@SYMTestCaseDesc		Test for defect INC091579 - SQL Panic 7 when streaming BLOB fields.
						The test creates a database with a table with a BLOB column. Then inserts
						a record. Then the test makes an attempt to read the BLOB column using a stream - 
						RSqlColumnReadStream. If the defect is not fixed, the code will panic.
@SYMTestPriority		High
@SYMTestActions			Test for defect INC091579 - SQL Panic 7 when streaming BLOB fields.
@SYMTestExpectedResults Test must not fail
@SYMDEF					INC091579
*/
void INC091579L()
	{
	//Create test database
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	_LIT8(KCreateStmt, "CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB)"); 
	err = TheDb.Exec(KCreateStmt);
	TEST(err >= 0);
	//Insert 1 row, using a binary stream
	_LIT8(KInsertStmt, "INSERT INTO A(Fld1,Fld2) VALUES(:p1,:p2)"); 
	RSqlStatement stmt; 
	err = stmt.Prepare(TheDb, KInsertStmt);
	TEST2(err, KErrNone);
	err = stmt.BindInt(0, 1);
	TEST2(err, KErrNone);

	RSqlParamWriteStream out;
	err = out.BindBinary(stmt, 1);
	TEST2(err, KErrNone);
	
	const TInt KDataSize = 100;
	TUint16 columnData[KDataSize];
	for(TInt i=0;i<KDataSize;i++)
		{
		columnData[i] = (TUint16)i;
		}
		
	out.WriteL(columnData, KDataSize);
	out.CommitL();
	err = stmt.Exec();
	TEST2(err, 1);
	
	out.Close();
	stmt.Close();

	//Try to read the inserted row, using a binary stream
	_LIT8(KSelectStmt, "SELECT * FROM A WHERE Fld1=1"); 
	err = stmt.Prepare(TheDb, KSelectStmt);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	RSqlColumnReadStream in;
	TInt idx = stmt.ColumnIndex(_L("Fld2"));
	TInt type = stmt.ColumnType(idx);
	TInt size = stmt.ColumnSize(idx);
	err = in.ColumnBinary(stmt,idx); // panic occurs here, if the defect is not fixed
	TEST2(err, KErrNone);
	in.Close();
	
	//Cleanup
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4033
@SYMTestCaseDesc		Test for defect INC091580 - SQL returns bogus pointer when too much text in field.
						The test creates a database with a table with 2 TEXT columns. Then inserts
						a record using a prepared statement with parameters. 
						The second text column value is set twice calling RSqlStatement::BindText().
						Then the test makes an attempt to read the TEXT columns using a SELECT statement - 
						RSqlStatement. If the defect is not fixed, the RSqlStatement::ColumnText() returns a 
						bad value for the first TEXT column.
@SYMTestPriority		High
@SYMTestActions			Test for defect INC091580 - SQL returns bogus pointer when too much text in field.
@SYMTestExpectedResults Test must not fail
@SYMDEF					INC091580
*/
void INC091580L()
	{
	_LIT8(KCreateStmt, "CREATE TABLE A(Fld1 INTEGER, Fld2 TEXT, Fld3 TEXT)"); 
	TInt err = TheDb.Create(KTestDatabase3); 
	TEST2(err, KErrNone);
	err = TheDb.Exec(KCreateStmt);
	TEST(err >= 0);

	_LIT8(KInsertStmt,"INSERT INTO A(Fld1,Fld2,Fld3) VALUES(1,:p1,:p2)"); 
	RSqlStatement stmt; 
	err = stmt.Prepare(TheDb, KInsertStmt); 
	TEST2(err, KErrNone);
	err = stmt.BindText(0, _L("AAA"));// "AAA" assigned to p1 parameter
	TEST2(err, KErrNone);

	err = stmt.BindText(1,_L("123456789ABCD"));//"123456789ABCD" assigned to p2 parameter (Fld2 column)
	TEST2(err, KErrNone);
	err = stmt.BindText(1, _L("123456789ABCDE"));//"123456789ABCDE" assigned to p2 parameter (Fld3 column)
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST(err >= 0);
	
	stmt.Close();

	_LIT8(KSelectStmt,"SELECT * FROM A WHERE Fld1=1"); 
	err = stmt.Prepare(TheDb, KSelectStmt); 
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	TPtrC fld2 = stmt.ColumnTextL(1); // returns bad value if the defect is not fixed
	TPtrC fld3 = stmt.ColumnTextL(2);

	TEST(fld2 == _L("AAA"));
	TEST(fld3 == _L("123456789ABCDE"));
	
	//Cleanup
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1815
@SYMTestCaseDesc		KSqlErrFull test.
						Create a test database with a table, which first column is declared as
						"INTEGER PRIMARY KEY AUTOINCREMENT".
						Insert one record into the table, initializing the ROWID with 0x7FFFFFFFFFFFFFFF
						(KMaxTInt64). Try to insert one more record into the table. The operation must
						fails with KSqlErrDiskFull.
@SYMTestPriority		High
@SYMTestActions			Verifying that SQL server returns KSqlErrFull when there are no more available row ids.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void SqlErrFullTest()
	{
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER PRIMARY KEY AUTOINCREMENT, T TEXT)"));
	TEST(err >= 0);
	
	TBuf8<100> sql;
	
	sql.Copy(_L8("INSERT INTO A(ROWID,T) VALUES("));
	sql.AppendNum(KMaxTInt64);
	sql.Append(_L8(", 'TTT')"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	
	err = TheDb.Exec(_L8("INSERT INTO A(T) VALUES('UUU')"));
	TEST2(err, KSqlErrFull);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT ROWID FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt64 val = stmt.ColumnInt64(0);
	TEST(val == KMaxTInt64);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1816-0001
@SYMTestCaseDesc		Test for defect INC094870 - Database became corrupted and cannot be opened.
						Case 1: Create a test database and set the database encoding to be UTF-8. Create a table.
						Close and open again the database. "Database open" operation should not panic.
						Case 2: Create a test database and set the database encoding to be UTF-8. Open two 
						connections to the database and check the encoding. It should be UTF-8.
@SYMTestPriority		High
@SYMTestActions			Test for defect INC094870 - Database became corrupted and cannot be opened.
@SYMTestExpectedResults Test must not fail
@SYMDEF					INC094870
*/
void INC094870L()
	{
	(void)RSqlDatabase::Delete(KTestDatabase3);
	//Test 1 - with a single database
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("PRAGMA cache_size=1000"));
	TEST(err >= 0);
    err = TheDb.SetIsolationLevel(RSqlDatabase::EReadUncommitted);
	TEST(err >= 0);
    err = TheDb.Exec(_L8("PRAGMA encoding = \"UTF-8\";"));
	TEST(err >= 0);
    err = TheDb.Exec(_L8("PRAGMA case_sensitive_like = 0"));
	TEST(err >= 0);
    err = TheDb.Exec(_L8("CREATE TABLE RDFS_Namespace(NamespaceId        INTEGER NOT NULL,Uri                TEXT NOT NULL,ReadOnly           INTEGER NOT NULL,VID                INTEGER NOT NULL,PRIMARY KEY(NamespaceId),UNIQUE(Uri));"));
	TEST(err >= 0);
	//Close and reopen the database. The Open() call should not fail.
    TheDb.Close();
    err  = TheDb.Open(KTestDatabase3); // This call fails if the defect is not fixed
	TEST2(err, KErrNone);
    TheDb.Close();
	//Test 2 - with two database connections
	(void)RSqlDatabase::Delete(KTestDatabase3);
	_LIT8(KEncoding, "encoding=UTF-8");
	_LIT(KUtf8, "UTF-8");
	RSqlDatabase db1, db2;
	//Connection 1: Create a database with default encoding UTF-8.
	err = db1.Create(KTestDatabase3, &KEncoding);
	TEST2(err, KErrNone);
	TBuf<100> buf;
	//Connection 1: Check the database encoding
	TSqlScalarFullSelectQuery query(db1);
	err = query.SelectTextL(_L8("PRAGMA encoding"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Find(KUtf8) >= 0);
	//Connection 1: Create a table
	err = db1.Exec(_L8("CREATE TABLE Tbl(Id INTEGER, T BLOB)"));
	TEST(err >= 0);
	//Connection 1: Check the database encoding
	err = query.SelectTextL(_L8("PRAGMA encoding"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Find(KUtf8) >= 0);
	//Connection 2: open the same database
	err = db2.Open(KTestDatabase3);
	TEST2(err, KErrNone);
	//Connection 1: Check the database encoding
	err = query.SelectTextL(_L8("PRAGMA encoding"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Find(KUtf8) >= 0);
	//Connection 2: Check the database encoding
	query.SetDatabase(db2);
	err = query.SelectTextL(_L8("PRAGMA encoding"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Find(KUtf8) >= 0);
	//Cleanup
	db2.Close();
	db1.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1817-0001
@SYMTestCaseDesc		Test for defect INC095412 - Retrieving query results may corrupt heap.
						Open t_inc095412.db test database and prepare SQL query (the SQL statement is in the test code). 
						Execute RSqlStatement::Next() and then try to retrieve the text value of column number 1.
						If the defect is not fixed RSqlStatement::ColumnText() will panic.
@SYMTestPriority		High
@SYMTestActions			Test for defect INC095412 - Retrieving query results may corrupt heap.
@SYMTestExpectedResults Test must not fail
@SYMDEF					INC095412
*/
void INC095412()
	{
    TInt err = TheDb.Open(KTestDatabase4);
    TEST2(err, KErrNone);
    RSqlStatement stmt;
    err = stmt.Prepare(TheDb, _L("SELECT ObjId, Uri, InstanceOf, Flags, p1.PropertyId, p1.PropertyTypeId, p1.Value, p1.Source, p1.Confidence FROM Objects JOIN Properties p1 ON ObjId = p1.HostObjId WHERE  (  ( (NOT Flags%3) = ? AND (NOT Flags%11) = ? AND InstanceOf IN  ( ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?  )  ) OR  ( (NOT Flags%3) = ? AND (NOT Flags%11) = ? AND InstanceOf IN  ( ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?  )  ) OR  ( (NOT Flags%3) = ? AND (NOT Flags%11) = ? AND InstanceOf IN  ( ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?  )  ) OR  ( (NOT Flags%3) = ? AND (NOT Flags%11) = ? AND InstanceOf IN  ( ? , ? , ? , ?  )  )  );"));
    TEST2(err, KErrNone);
    const TInt KPrmValues[] = {0, 0, 33, 40, 47, 45, 43, 42, 50, 39, 51, 44, 41, 46, 38, 48, 49, 0, 0, 26, 40, 53, 54, 57, 52, 39, 58, 56, 41, 38, 55, 0, 0, 26, 40, 53, 54, 57, 52, 39, 58, 56, 41, 38, 55, 0, 0, 38, 40, 39, 41};
    for(TInt i=0;i<sizeof(KPrmValues)/sizeof(KPrmValues[0]);++i)
    	{
    	err = stmt.BindInt(i, KPrmValues[i]);
    	TEST2(err, KErrNone);
    	}
    err = stmt.Next();
    TEST2(err, KSqlAtRow);
    TPtrC ptr;
    stmt.ColumnText(1, ptr); // May corrupt heap and panic the test application if the defect is not fixed!
    stmt.Close();
    TheDb.Close();
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3427
@SYMTestCaseDesc		Test for DEF104242 - The SQL server fails to open database with default security policy only.
						The test plays with C:[21212125]T_OneDefPolicy.db secure database, which was created by
						an external tool and the database security policy table contains just a single record
						with the default security policy. The default security policy is set to be 
						TSecurityPolicy::EAlwaysPass. The test attempts to read from the database, modify the database
						content, modify the database schema.
@SYMTestPriority		High
@SYMTestActions			Test for DEF104242 - The SQL server fails to open database with default security policy only.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF104242
*/
void DEF104242()
	{
	TInt err = TheDb.Open(_L("C:[21212125]T_OneDefPolicy.db"));
	TEST2(err, KErrNone);
	//Read
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT t1key, data from t1"));
	TEST2(err, KErrNone);
	while((err = stmt.Next()) == KSqlAtRow)
		{
		TheTest.Printf(_L("t1key=%d\r\n"), stmt.ColumnInt(0));
		}
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Write
	err = TheDb.Exec(_L("INSERT INTO t1(t1key) VALUES(12)"));
	TEST2(err, 1);
	//Schema
	err = TheDb.Exec(_L("CREATE TABLE AAA(Id INTEGER)"));
	TEST(err >= 0);
	//Cleanup
	TheDb.Close();
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3430
@SYMTestCaseDesc		Test for DEF104437 - RSqlStatement::Next() panics the SQL server with KERN-EXEC 3.
						The test attempts to execute 2 SELECT queries ("IN" and "IN + JOIN"). The table, on
						which queries operate, has multi-column primary key. The right-hand side of the 
						"IN" operator contains NULL.
@SYMTestPriority		High
@SYMTestActions			Test for DEF104437 - RSqlStatement::Next() panics the SQL server with KERN-EXEC 3.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF104437
*/
void DEF104437()
	{
	//Test case 1 - "IN" + "JOIN"
	(void)RSqlDatabase::Delete(KTestDatabase5);
	TInt err = TheDb.Create(KTestDatabase5);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE inmk(cls TEXT,sec INTEGER,inst INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO inmk VALUES ('ORD', 2751, 2750)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("CREATE TABLE clss(hrar TEXT,cls TEXT,PRIMARY KEY (hrar, cls))"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TABLE rels(prnt_inst INTEGER,chld_inst INTEGER)"));
	TEST(err >= 0);
	RSqlStatement stmt;
	_LIT(KSelectSql,
	"SELECT I.sec\
	  FROM inmk I\
	   LEFT JOIN\
	     rels R ON R.prnt_inst = I.inst\
	   LEFT JOIN\
	      inmk UI ON UI.inst = R.chld_inst\
	   LEFT JOIN\
	     clss C1U ON C1U.cls = UI.cls AND C1U.hrar = 'STH'\
	   LEFT JOIN\
	     clss C10U ON C10U.hrar = c1u.hrar AND C10U.cls IN (C1U.cls)\
	  WHERE I.sec = 2751;");
	err = stmt.Prepare(TheDb, KSelectSql);
	TEST2(err, KErrNone);
	while((err = stmt.Next()) == KSqlAtRow)
		{
		TInt val = stmt.ColumnInt(0);
		TEST2(val, 2751);
		TheTest.Printf(_L("column value=%d\r\n"), val);
		}
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	TheDb.Close();
	//////////////////////////////////////////////////////////////
	//Test case 2 - "IN"
	(void)RSqlDatabase::Delete(KTestDatabase5);
	err = TheDb.Create(KTestDatabase5);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE b(y,z,PRIMARY KEY(y, z))"));
	TEST(err >= 0);
	err = stmt.Prepare(TheDb, _L("SELECT * FROM b WHERE y = NULL AND z IN ('hello')"));
	TEST2(err, KErrNone);
	while((err = stmt.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase5);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3442
@SYMTestCaseDesc		Test for DEF105259 - SQL, RSqlColumnReadStream's internal buffer may become invalid.
						The test does the following steps:
						1) Create a table: CREATE TABLE A(Name TEXT, Data BLOB);
						2) Insert only one record in A, such that, the "Name" column length is less than 
						   8 characters, the "Data" column length is 1K bytes.
						3) Create RSqlStatement object with the following SQL statement: SELECT * FROM A
						4) Call RSqlStatement::Next() to get the record
						5) Create RSqlColumnReadStream for column 0 (the "Name" column)
						6) Try to access the "Data" column value, without using a stream
						7) Try to read the "Name" column using the stream
						8) Compare the read "Name" column value vs. the original column value (at the moment 
						   when the table record was stored)
@SYMTestPriority		High
@SYMTestActions			Test for DEF105259 - SQL, RSqlColumnReadStream's internal buffer may become invalid.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF105259
*/
void DEF105259L()
	{
	(void)RSqlDatabase::Delete(KTestDatabase3);
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("CREATE TABLE A(Name TEXT, Data BLOB)"));
	TEST(err >= 0);
	
	const TInt KBlobDataSize = 1024;
	HBufC* recBuf = HBufC::New(KBlobDataSize * 2 + 100);
	TEST(recBuf != NULL);
	TPtr sql = recBuf->Des();
	sql.Copy(_L("INSERT INTO A(Name,Data) VALUES('A12',x'"));
	for(TInt i=0;i<KBlobDataSize;++i)
		{
		TBuf<2> tmp;
		tmp.AppendFormat(_L("%02X"), i % 256);
		sql.Append(tmp);
		}
	sql.Append(_L("')"));
	
	err = TheDb.Exec(sql);
	TEST2(err, 1);

	delete recBuf;
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	
	RSqlColumnReadStream strm;
	err = strm.ColumnText(stmt, 0);
	TEST2(err, KErrNone);
	
	TPtrC8 data;
	err = stmt.ColumnBinary(1, data);
	TEST2(err, KErrNone);

	TBuf<10> name;
	strm.ReadL(name, 3);
	TEST(name == _L("A12"));

	strm.Close();	
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3470
@SYMTestCaseDesc		Test for DEF105681 - SQL, HReadOnlyBuf::ConstructL() sets a pointer to a local TPtr8 variable.
						The test creates a database with a table: A(Name TEXT, Data BLOB). One record is inserted
						in the table. Then the test creates a statement object with "SELECT * FROM A" sql statement.
						The test moves the statement cursor on the record and attempts to access the BLOB column
						using a stream. When the stream object is created, the test attempts to create an embedded
						store object from the stream, using CEmbeddedStore::FromLC(strm) call. If the defect
						is not fixed, the call will panic.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105681 - SQL, HReadOnlyBuf::ConstructL() sets a pointer to a local TPtr8 variable.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF105681
*/
void DEF105681L()
	{	
	(void)RSqlDatabase::Delete(KTestDatabase3);
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("CREATE TABLE A(Name TEXT, Data BLOB)"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("INSERT INTO A(Name,Data) VALUES('A12',x'0400000000')"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	RSqlColumnReadStream strm;
	err = strm.ColumnBinary(stmt, 1);
	TEST2(err, KErrNone);
	CEmbeddedStore* store = CEmbeddedStore::FromLC(strm);
	CleanupStack::PopAndDestroy(store);

	strm.Close();	
	stmt.Close();

	//Testing with a NULL binary column value
	err = TheDb.Exec(_L("INSERT INTO A(Name,Data) VALUES('BBB',NULL)"));
	TEST2(err, 1);

	err = stmt.Prepare(TheDb, _L("SELECT * FROM A WHERE Name='BBB'"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	err = strm.ColumnBinary(stmt, 1);
	TEST2(err, KErrNone);
	store = NULL;
	TRAP(err, store = CEmbeddedStore::FromL(strm));
	TEST2(err, KErrEof);
	delete store;

	strm.Close();	
	stmt.Close();
	
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3476
@SYMTestCaseDesc		Test for DEF106391 - SQL server does not deallocate the already allocated memory.
						The test is executed only on the Emulator, because its execution depends on the amount
						of the available memory and the amount of free disk space - factors which cannot be easily 
						resolved on target hardware.
						The test creates a database with a table: T(Id INTEGER, Data BLOB). 
						One record with a BLOB (either 0.79Mb or 0.9Mb) is inserted using RSqlDatabase::Exec().
						Another record with a BLOB (either 1.58 or 1.8Mb) is inserted using RSqlStatement and BLOB parameter.
						If the defect is not fixed, after the first INSERT the SQL server will not free occupied by
						the statement memory. The available heap memory won't be enough for the execution of the second INSERT statement.
						The second INSERT will fail with KErrNoMemory.
@SYMTestPriority		High
@SYMTestActions			Test for DEF106391 - SQL server does not deallocate the already allocated memory.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF106391
*/
void DEF106391()
	{
#if defined __WINS__ ||	defined __WINSCW__
#ifndef SYMBIAN_USE_SQLITE_VERSION_3_6_4
    const TInt KBlobSize = 900 * 1024;
#else
    const TInt KBlobSize = 790 * 1024;
#endif    

	_LIT8(KConfigStr, "encoding=UTF-8");

	HBufC8* sqlBuf = HBufC8::New(KBlobSize * 2 + 200);//"+ 200" - for the SQL INSERT statement
	TEST(sqlBuf != NULL);

	(void)RSqlDatabase::Delete(KTestDatabase5);
	
	//Step 1: insert a record with a very large BLOB column (using RSqlDatabase::Exec())
	//        (the operation is rolled back because there may not be enough disk space)
	TVolumeInfo volInfo;
	TInt err = TheFs.Volume(volInfo);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("INSERT#1, Volume size: %ldK  Free space: %ldK\r\n"), volInfo.iSize / 1024, volInfo.iFree / 1024);
	TheTest.Printf(_L("Test BLOB size: %dK\r\n"), KBlobSize / 1024);
	
	err = TheDb.Create(KTestDatabase5, &KConfigStr);
	TEST2(err, KErrNone);
	
	TSqlResourceProfiler profiler(TheDb);
	(void)profiler.Start(TSqlResourceProfiler::ESqlCounterMaxAlloc);
	
	err = TheDb.Exec(_L8("CREATE TABLE T(Id INTEGER, Data BLOB)"));
	TEST(err >= 0);
	TPtr8 sql = sqlBuf->Des();
	sql.Copy(_L8("BEGIN TRANSACTION;INSERT INTO T(Id,Data) VALUES(1, x'"));
	for(TInt i=0;i<KBlobSize;++i)
		{
		sql.Append(_L8("A5"));
		}
	sql.Append(_L8("');"));	
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterMaxAlloc);
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	err = TheDb.Exec(_L("ROLLBACK TRANSACTION"));
	TEST(err >= 0);
	TBuf8<32> profilerRes8;
	TBuf<32> profilerRes;
	if(profiler.Query(TSqlResourceProfiler::ESqlCounterMaxAlloc, profilerRes8) == KErrNone)
		{
		profilerRes.Copy(profilerRes8);
		TheTest.Printf(_L("RSqlDatabase::Exec(): <SQL server max alloc>;<SQLite max alloc>=%S\r\n"), &profilerRes);
		}

	//Step 2: insert a record with a very large BLOB column (using RSqlStatement::Exec())
	//        (the operation is rolled back because there may not be enough disk space)
	err = TheFs.Volume(volInfo);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("INSERT#2, Volume size: %ldK  Free space: %ldK\r\n"), volInfo.iSize / 1024, volInfo.iFree / 1024);
	TheTest.Printf(_L("Test BLOB size: %dK\r\n"), sql.Length() / 1024);
	
	(void)profiler.Reset(TSqlResourceProfiler::ESqlCounterMaxAlloc);
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO T(Id, Data) VALUES(2, :V)"));
	TEST2(err, KErrNone);
	err = stmt.BindBinary(0, sql);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	err = stmt.Exec();
	TEST2(err, 1);
	err = TheDb.Exec(_L("ROLLBACK TRANSACTION"));
	TEST(err >= 0);
	stmt.Close();
	if(profiler.Query(TSqlResourceProfiler::ESqlCounterMaxAlloc, profilerRes8) == KErrNone)
		{
		profilerRes.Copy(profilerRes8);
		TheTest.Printf(_L("RSqlStatement::Bind/Exec(): <SQL server max alloc>;<SQLite max alloc>=%S\r\n"), &profilerRes);
		}
	
	delete sqlBuf;

	(void)profiler.Stop(TSqlResourceProfiler::ESqlCounterMaxAlloc);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase5);
#endif// defined __WINS__ || defined __WINSCW__	
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3501
@SYMTestCaseDesc		Test for DEF109025 - SQL, dangling long binary/text column value pointer
						The test does the following steps:
						1) Create a table: CREATE TABLE A(Name TEXT, Data BLOB);
						2) Insert only one record in A, such that, the "Name" column length is less than 
						   8 characters, the "Data" column length is 1K bytes.
						3) Create RSqlStatement object with the following SQL statement: SELECT * FROM A
						4) Call RSqlStatement::Next() to get the record
						5) Allocate a 1024 bytes buffer (so, when the row buffer has to be reallocated, the system will be forced to 
						   search another block of memory, because the current one is capped by the allocated 1024 bytes)
						6) Get a pointer to the "name" column value
						7) Get a pointer to the "data" column value
						8) Check the "name" column value. If the defect still exists, the "name" pointer will point to a deleted 
						   block of memory.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109025 - SQL, dangling long binary/text column value pointer.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109025
*/
void DEF109025()
	{
	(void)RSqlDatabase::Delete(KTestDatabase3);
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("CREATE TABLE A(Name TEXT, Data BLOB)"));
	TEST(err >= 0);
	
	const TInt KBlobDataSize = 1024;
	HBufC* recBuf = HBufC::New(KBlobDataSize * 2 + 100);
	TEST(recBuf != NULL);
	TPtr sql = recBuf->Des();
	sql.Copy(_L("INSERT INTO A(Name,Data) VALUES('A12',x'"));
	for(TInt i=0;i<KBlobDataSize;++i)
		{
		TBuf<2> tmp;
		tmp.AppendFormat(_L("%02X"), i % 256);
		sql.Append(tmp);
		}
	sql.Append(_L("')"));
	
	err = TheDb.Exec(sql);
	TEST2(err, 1);

	delete recBuf;
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	TUint8* mem = new TUint8[1024];	//This memory block will be allocated right after the row buffer,
	TEST(mem != NULL);				//so the ColumnBinary() call will reallocate the row buffer somewhere else, not at the same address.

	TPtrC name;
	err = stmt.ColumnText(0, name);
	TEST2(err, KErrNone);
	
	TPtrC8 data;
	err = stmt.ColumnBinary(1, data);
	TEST2(err, KErrNone);

	TEST(name == _L("A12"));

	delete [] mem;
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3546
@SYMTestCaseDesc		Test for DEF109843 - SQL, RSQLStatement::BindBinary() is causing panic if empty descriptor is passed.
						The test does the following steps:
						1) Create a table: CREATE TABLE A(Id INTEGER, Data BLOB);
						2) Create a RSqlStatement object stmt;
						3) Prepare stmt with the following SQL statement:
						INSERT INTO A(Id,Data) VALUES(:Val1,:Val2);
						4) Set the Val2 field with an empty descriptor("") by using RSqlStatement::BindBinary();
						5) Execute the statement;
						6) If the defect still exist, stmt.BindBinary() will cause a panic;
@SYMTestPriority		High
@SYMTestActions			Test for DEF109843 - SQL, RSQLStatement::BindBinary() is causing panic if empty descriptor is passed.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109843
*/
void DEF109843()
	{
	(void)RSqlDatabase::Delete(KTestDatabase5);
	TInt err = TheDb.Create(KTestDatabase5);
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, Data BLOB)"));
	TEST(err >= 0);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("INSERT INTO A(Id,Data) VALUES(:Val1,:Val2)"));
	TEST2(err, KErrNone);
	
	TInt paramIndex;
	paramIndex = stmt.ParameterIndex(_L(":Val1"));
	TEST(paramIndex >= 0);
	
	err = stmt.BindInt(paramIndex, 1);
	TEST2(err, KErrNone);
	
	paramIndex = stmt.ParameterIndex(_L(":Val2"));
	TEST(paramIndex >= 0);
	
	TPtrC8 emptyEntry (_L8(""));
	err = stmt.BindBinary(paramIndex, emptyEntry);
	TEST2(err, KErrNone);
	
	err = stmt.Exec();
	TEST(err >= 0);
	
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase5);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4005
@SYMTestCaseDesc		Test for DEF114698 - SqlSrv.EXE::!SQL Server Insert/Update Error.
						The test does the following steps:
						1) DB3: 
							CREATE TABLE A(Id INTEGER, Id1 INTEGER)
							INSERT INTO A(Id,Id1) VALUES(2,3)
						2) DB2:
							CREATE TABLE B(Id INTEGER, Id1 INTEGER)
							INSERT INTO B(Id,Id1) VALUES(2,3)
						3) DB:
							CREATE TABLE MAIN(Id INTEGER, Id1 INTEGER)
							INSERT INTO MAIN(Id,Id1) VALUES(2,3)
						4) Attach DB2 and DB3 to DB.
						5) Execute:
							INSERT INTO B SELECT * FROM B UNION ALL SELECT * FROM MAIN WHERE exists (select * FROM B WHERE B.Id = MAIN.Id);
							UPDATE A SET Id= (SELECT Id1 FROM B WHERE Id=1) WHERE EXISTS ( SELECT MAIN.Id1 FROM MAIN WHERE MAIN.Id = A.Id1);
@SYMTestPriority		High
@SYMTestActions			Test for DEF114698 - SqlSrv.EXE::!SQL Server Insert/Update Error.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF114698
*/
void DEF114698()
	{
	(void)RSqlDatabase::Delete(KTestDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase2);
	(void)RSqlDatabase::Delete(KTestDatabase5);
	
	TInt err = TheDb.Create(KTestDatabase5);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, Id1 INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(Id,Id1) VALUES(2,3)"));
	TEST2(err, 1);
	TheDb.Close();
	
	err = TheDb.Create(KTestDatabase2);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE B(Id INTEGER, Id1 INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO B(Id,Id1) VALUES(2,3)"));
	TEST2(err, 1);
	TheDb.Close();
	
	err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE MAIN(Id INTEGER, Id1 INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO MAIN(Id,Id1) VALUES(2,3)"));
	TEST2(err, 1);
	
	err = TheDb.Attach(KTestDatabase2, _L("db2"));
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestDatabase5, _L("db3"));
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("INSERT INTO B SELECT * FROM B UNION ALL SELECT * FROM MAIN WHERE exists (select * FROM B WHERE B.Id = MAIN.Id)"));
	TEST2(err, 2);
	err = TheDb.Exec(_L("UPDATE A SET Id= (SELECT Id1 FROM B WHERE Id=1) WHERE EXISTS ( SELECT MAIN.Id1 FROM MAIN WHERE MAIN.Id = A.Id1)"));
	TEST2(err, 0);

	err = TheDb.Detach(_L("db3"));
	TEST2(err, KErrNone);
	err = TheDb.Detach(_L("db2"));
	TEST2(err, KErrNone);
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase5);
	(void)RSqlDatabase::Delete(KTestDatabase2);
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4009-0001
@SYMTestCaseDesc		Test for DEF115556 SqlSrv.EXE::!SQL Server when preparing complex sql query.
						The test does the following steps:
						1) Create a database with two tables
							CREATE TABLE A(Id INTEGER, Id1 INTEGER, F2 BLOB)
							CREATE TABLE B(Id INTEGER, Id1 INTEGER, F2 BLOB)
						2) Prepare the following statement
							DELETE FROM A WHERE Id1 IN (SELECT Id1 FROM B WHERE Id IN (1,11) UNION SELECT * FROM B WHERE Id1=2 ORDER BY Id ASC LIMIT 1)
						If the defect is not fixed, step (2) asserts in SQLITE.
@SYMTestPriority		High
@SYMTestActions			Test for DEF115556 SqlSrv.EXE::!SQL Server when preparing complex sql query.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115556
*/
void DEF115556()
	{
	(void)RSqlDatabase::Delete(KTestDatabase1);
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);

	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TABLE B(Id INTEGER, Id1 INTEGER, F2 BLOB)"));
	TEST(err >= 0);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("DELETE FROM A WHERE Id1 IN (SELECT Id1 FROM B WHERE Id IN (1,11) UNION SELECT * FROM B WHERE Id1=2 ORDER BY Id ASC LIMIT 1)"));
	TEST(err != KErrDied);
	TPtrC errDescr = TheDb.LastErrorMessage();
	RDebug::Print(_L("\"Stmt prepare\" %d error. Message:\"%S\"\r\n"), err, &errDescr);
	stmt.Close();
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}


/**
@SYMTestCaseID			SYSLIB-SQL-UT-4012
@SYMTestCaseDesc		Test for DEF115954: CSession Code = 2 executing SQL stmt in OOM loop.
						It is an OOM test. In the loop, the test does the following steps:
						1) Create a database. 
						2) Set heap failure.
						3) Open the database
						4) Execute the statement which caused the panic before this defect is fixed.
						5) Close the database.

						If the defect is not fixed, step (5) will panic with CSession Code = 2.
						
						Note: It's possible for database operations to be performed even after memory
 						allocation has failed. This is because SQLITE reuses some pages of the page
 						cache which have been allocated but are curently not in use. This means it is 
 						necessary to undo any operations on the database and continue checking for
 						memory and resource leaks even after an operation has been completed successfully
@SYMTestPriority		High
@SYMTestActions			Test for DEF115954: CSession Code = 2 executing SQL stmt in OOM loop.
@SYMTestExpectedResults Test program must not panic.
@SYMDEF					DEF115954
*/
void DEF115954()
	{
	TInt err = KErrNone;
	TInt failingAllocationNo = 0;
	TInt allocationNo = 0;
	TheTest.Printf(_L("\r\n"));
	while(allocationNo < KDEF115954MaxAllocLimit)
 		{
		TheTest.Printf(_L("%d    \r"), allocationNo);
  		RSqlDatabase::Delete(KTestDatabase1);
  		err = TheDb.Create(KTestDatabase1);
  		TEST(err == KErrNone);
  		TheDb.Close(); 
  		
  		const TInt KDelayedDbHeapFailureMask = 0x1000;
  		TSqlResourceTester::SetDbHeapFailure(KDelayedDbHeapFailureMask, ++allocationNo);

  		err = TheDb.Open(KTestDatabase1);
  		TEST(err == KErrNone);

  		err = TheDb.Exec(_L("CREATE TABLE node(id INTEGER PRIMARY KEY,name TEXT);CREATE INDEX node_idx ON node(name);CREATE TABLE edge(orig INTEGER REFERENCES node,dest INTEGER REFERENCES node,PRIMARY KEY(orig, dest));CREATE INDEX edge_idx ON edge(dest,orig)"));
  		TheDb.Close();
  		if(err != KErrNoMemory)
			{
			TEST2(err, KErrNone);	
			}
		else
			{
			failingAllocationNo = allocationNo;
			}
  
  		TSqlResourceTester::SetDbHeapFailure(RHeap::ENone, 0);
  		}
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	TEST(err == KErrNone);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4008
@SYMTestCaseDesc		Test for DEF115567 - Critical SQLite defect that can cause database corruption during UPDATE/DELETE.
						The test creates 2 tables with couple of records and a "BEFORE DELETE" trigger on the first table.
						Then the test deletes 1 record from the first table and checks the record count in both tables.
						In both cases the record count should not be 0.
@SYMTestPriority		Critical
@SYMTestActions			Test for DEF115567 - Critical SQLite defect that can cause database corruption during UPDATE/DELETE.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115567
*/
void DEF115567L()
	{
	(void)RSqlDatabase::Delete(KTestDatabase1);
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE t3(a INTEGER, b INTEGER);CREATE TABLE t4(a INTEGER, b INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TRIGGER t3_t BEFORE DELETE ON t3 BEGIN DELETE FROM t4 WHERE t4.a = old.a;DELETE FROM t3 WHERE a = 2;END"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO t3 VALUES(1,1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO t3 VALUES(2,2)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO t3 VALUES(3,3)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO t4 VALUES(1,1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("INSERT INTO t4 VALUES(3,3)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L("DELETE FROM t3 WHERE a=1 OR a=2"));
	TEST2(err, 1);
	TSqlScalarFullSelectQuery query(TheDb);
	TInt rowcnt = query.SelectIntL(_L("SELECT COUNT(*) FROM t3"));
	TEST2(rowcnt, 1);
	rowcnt = query.SelectIntL(_L("SELECT COUNT(*) FROM t4"));
	TEST2(rowcnt, 1);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}	

/**
@SYMTestCaseID			SYSLIB-SQL-CT-4019
@SYMTestCaseDesc		Test for DEF116397 Attaching a non-db file should return KSqlErrNotDb rather than KErrEof
@SYMTestPriority		Medium
@SYMTestActions			Test for DEF116397 - SQL, Attaching database returns KErrEof(-25).
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF116397
*/
void DEF116397()
	{
	//Test error code opening a corrupt db file
	TInt err = TheDb.Open(KCorruptDb);
	TEST2(err, KSqlErrNotDb);
	TheDb.Close();
	
	//create a sql db file
	(void)RSqlDatabase::Delete(KTestDatabase1);
	err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE t3(a INTEGER, b INTEGER);CREATE TABLE t4(a INTEGER, b INTEGER)"));
	TEST(err >= 0);
	TheDb.Close();
	
	//open the db file
	err=TheDb.Open(KTestDatabase1);
	TEST2(err,KErrNone);
	
	//try to attach a non db file
	err=TheDb.Attach(KCorruptDb,_L("db2"));
	TEST2(err,KSqlErrNotDb);
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);			
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4029
@SYMTestCaseDesc		Test for DEF119403 - creates a databse, retrieves its Collation Dll Name and makes 
						sure the Collation Dll Name doesn't contain the path 
@SYMTestPriority		Medium
@SYMTestActions			Test for DEF119403 Database re-index unnecessarily during open operation 
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF119403
*/
void DEF119403L()
	{
	//create a sql db file
	(void)RSqlDatabase::Delete(KTestDatabase1);
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	
	TFileName buf;
	TSqlScalarFullSelectQuery query(TheDb);
	//Get Collation Dll Name from SYMBIAN_SETTINGS
	err = query.SelectTextL(_L8("SELECT CollationDllName FROM SYMBIAN_SETTINGS"), buf);
	TEST2(err, KErrNone);
	TParse parse;
	parse.Set(buf, NULL, NULL);
	
	//Check that the Collation Dll Name doesn't include the path
	TEST(!parse.PathPresent());
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);

	}

// Helper function for DEF120237L()
TInt ExecuteSelect(RSqlDatabase& aDatabase, const TDesC8& aSelectStatement)
	{
	RSqlStatement statement;

	TInt err = statement.Prepare(aDatabase, aSelectStatement);
	TEST2(err, KErrNone);
	
	TInt ret;
	TInt count = 0;
	TheTest.Printf(_L("Results:\n"));
	while((ret = statement.Next()) == KSqlAtRow)
	    {  
	    TPtrC coltext;
	    err = statement.ColumnText(0, coltext);
	    TEST2(err, KErrNone);
	    
	    RDebug::RawPrint(coltext);
	    TheTest.Printf(_L("\n"));
	    count++;
	    }
	TEST2(ret, KSqlAtEnd);
	
	statement.Close();	
	return count;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-4031
@SYMTestCaseDesc		Test for DEF120237 - Checks that When using SELECT together with 
                        the LIKE operator the right number of results are returned.
@SYMTestPriority		Medium
@SYMTestActions			1) Create a database and fill it with test data.
                        2) Execute a SELECT statement with the LIKE operator and the % wildcard
                        3) Check that the expected number of results are returned.
@SYMTestExpectedResults The SELECT statements should return the expected number of results.
@SYMDEF					DEF120237
*/
void DEF120237L()
	{
	const TInt KSelectTest1ExpectedResults = 3;
	const TInt KSelectTest2ExpectedResults = 3;
	
	_LIT8(KCfgStr, "encoding=UTF-8");
	_LIT8(KCreate, "CREATE TABLE A(Id INTEGER,Name TEXT collate nocase)"); //Adding "collate nocase" allows us to see defect
	_LIT8(KIndex, "CREATE INDEX name_index on A (name)"); //Adding index allows us to see defect

	_LIT8(KRecord1, "INSERT INTO A VALUES(1, '\"AAA')");
	_LIT8(KRecord2, "INSERT INTO A VALUES(2, '\"AAB')");
	_LIT8(KRecord3, "INSERT INTO A VALUES(3, '\"AAC')");
	_LIT8(KRecord4, "INSERT INTO A VALUES(4, '&BAA')");
	_LIT8(KRecord5, "INSERT INTO A VALUES(5, '%BAA')");
	_LIT8(KRecord6, "INSERT INTO A VALUES(6, '''BAA')");
	_LIT8(KRecord7, "INSERT INTO A VALUES(7, '''BAB')");
	_LIT8(KRecord8, "INSERT INTO A VALUES(8, '''BAC')");

	//Create database
	RSqlDatabase database;
	CleanupClosePushL(database);
	
	(void*)database.Delete(KTestDatabase6);
	User::LeaveIfError(database.Create(KTestDatabase6, &KCfgStr));
	TInt err = database.Exec(KCreate);
	TEST(err >= KErrNone);
	
	err = database.Exec(KIndex);
	TEST(err >= KErrNone);
	
	//Insert Neccessary Data
	err = database.Exec(KRecord1);
	TEST2(err, 1);
	err = database.Exec(KRecord2);
	TEST2(err, 1);
	err = database.Exec(KRecord3);
	TEST2(err, 1);
	err = database.Exec(KRecord4);
	TEST2(err, 1);
	err = database.Exec(KRecord5);
	TEST2(err, 1);
	err = database.Exec(KRecord6);
	TEST2(err, 1);
	err = database.Exec(KRecord7);
	TEST2(err, 1);
	err = database.Exec(KRecord8);
	TEST2(err, 1);

	//Case 1 when search criteria is "
	//Defect: Select does not return result
	_LIT8(KSelectTest1, "select name from A where name like '\"%'");
	TInt numResults = ExecuteSelect(database, KSelectTest1);
	TheTest.Printf(_L("Case 1 Results: %d\n"), numResults);
	TEST2(numResults, KSelectTest1ExpectedResults);
	
	//Case 2 when search criteria is '
	//Defect: Select returns data beginning with & and % as well
	_LIT8(KSelectTest2,"select name from A where name like '''%'");
	numResults = ExecuteSelect(database, KSelectTest2);
	TheTest.Printf(_L("Case 2 Results: %d\n"), numResults);
	TEST2(numResults, KSelectTest2ExpectedResults);

	CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4120
@SYMTestCaseDesc		Test for DEF125881 - Checks that when a INSERT statement is executed
						under a I/O failure simulation, the correct error code will be returned
@SYMTestPriority		Medium
@SYMTestActions			1) Create a database and fill it with test data.
                        2) Execute a INSERT statement under a I/O failure simulation
                        3) Check that the expected return error code is retruned
@SYMTestExpectedResults The INSERT statement should return the correct error code under I/O failure
@SYMDEF					DEF125881
*/
void DEF125881L()
	{
	_LIT(KDbPath, "c:\\t_sqldefect-def125881.db");
	_LIT(KSchema, "CREATE TABLE test(t TEXT);");
	_LIT(KInsert, "INSERT INTO test (t) VALUES (\'mkldfmklmklmkldfmkldfmklm\
												  klcdmklmkldsdklfjwoierthj\
												  iofnkjwefniwenfwenfjiowen\
												  mkldfmklmklmkldfmkldfmklm\
												  klcdmklmkldsdklfjwoierthj\
												  mkldfmklmklmkldfmkldfmklm\
												  klcdmklmkldsdklfjwoierthj\
												  iofnkjwefniwenfwenfjiowen\
												  mkldfmklmklmkldfmkldfmklm\
												  klcdmklmkldsdklfjwoierthj\
												  iofnkjwefniwenfwenfjiowen\
												  mkldfmklmklmkldfmkldfmklm\
												  klcdmklmkldsdklfjwoierthj\
												  iofnkjwefniwenfwenfjiowen\
												  iofnkjwefniwenfwenfjiowen\
												  fiwenfwejnfwinsdf2sdf4sdf\');");
	
	_LIT(KLogFormat, "After %d operations: %d returned\n");
	
	// Create file server session
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect());
	
	// Open a SQL DB, setup basic schema
	RSqlDatabase sqlDb;
	CleanupClosePushL(sqlDb);
	
	TRAPD(createErr, sqlDb.OpenL(KDbPath));
	if (createErr != KErrNone)
		{
		sqlDb.CreateL(KDbPath);
		User::LeaveIfError(sqlDb.Exec(KSchema));
		}	
	
	// Create a SQL statement
	RSqlStatement stmnt;
	TInt err = stmnt.Prepare(sqlDb, KInsert);
	TEST2(err,KErrNone);
	
	// Begin test
	TInt fsError = KErrGeneral;
	TInt count = 0;
	
	const TInt KMaxOps = 300;

	TSqlResourceProfiler pr(sqlDb);
	pr.Start(TSqlResourceProfiler::ESqlCounterOsCallDetails);
	pr.Reset(TSqlResourceProfiler::ESqlCounterOsCallDetails);
	
	while (fsError != 1 && count <= KMaxOps)
		{
		// Setup for KErrGeneral failure
		fsSession.SetErrorCondition(KErrGeneral, count);
		
		// Database operation
		fsError = stmnt.Exec();
		stmnt.Reset();
		
		// Test for KErrGeneral
		TheTest.Printf(KLogFormat, count, fsError);
		TEST( (fsError == KErrGeneral) || (fsError == 1) || (fsError == KSqlErrIO));
		
		// Increment fail-after count
		++count;
		}
	fsSession.SetErrorCondition(KErrNone);
	pr.Stop(TSqlResourceProfiler::ESqlCounterOsCallDetails);
	stmnt.Close();
	CleanupStack::PopAndDestroy(2); // fsSession, sqlDb
	(void)RSqlDatabase::Delete(KDbPath);
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4128
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						When executing a pragma which deosn't return any results (e.g performing "PRAGMA index_list"
						on a table with no index. The client panics when RSqlStatement::Next() is called. 
						This test checks the client does not panic in this case
@SYMTestPriority		High
@SYMTestActions			DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void DEF129581()
	{
	_LIT8(KPragma, "Pragma index_list(T)");
	_LIT8(KCreateTable, "CREATE TABLE T (A INTEGER)");
	(void)RSqlDatabase::Delete(KTestDatabase1);
	
	//create a sql db file and make sure no index is added to it
	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);

	//create a table
	err = TheDb.Exec(KCreateTable);
	TEST(err >= KErrNone);
	
	RSqlStatement stmt;
	
	//Executes a "Pragam index_list.." statement
	err = stmt.Prepare(TheDb, KPragma);
	TEST2(err, KErrNone);
	
	//Calls RSqlStatement::Next() to make sure the client does not panic.
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	
	stmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4153
@SYMTestCaseDesc        Test for DEF143047: SQL, default "max parameter count" value, compatibility problem.
                        This test case proves that an SQL statement with more than 1000 parameters can be prepared 
                        successfully. The default value of the SQLITE_MAX_VARIABLE_NUMBER macro is 999. 
                        Changed to 32767 with this defect fix.
@SYMTestPriority        High
@SYMTestActions         DEF143047: SQL, default "max parameter count" value, compatibility problem
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143047
*/
void DEF143047()
    {
    (void)RSqlDatabase::Delete(KTestDatabase1);
    
    TInt err = TheDb.Create(KTestDatabase1);
    TEST2(err, KErrNone);

    _LIT8(KCreateTable, "CREATE TABLE T(A INTEGER)");
    err = TheDb.Exec(KCreateTable);
    TEST(err >= KErrNone);
    
    const TInt KPrmCount = 1200;
    HBufC8* buf = HBufC8::New(KPrmCount * 2 + 200);
    TEST(buf != NULL);
    TPtr8 sql = buf->Des();
    sql.Copy(_L8("SELECT * FROM T WHERE A IN(?"));
    for(TInt i=0;i<KPrmCount;++i)
        {
        sql.Append(_L8(",?"));
        }
    sql.Append(_L8(")"));
    
    RSqlStatement stmt;
    err = stmt.Prepare(TheDb,sql);
    if(err != KErrNone)
        {
        TPtrC errdes = TheDb.LastErrorMessage();
        TheTest.Printf(_L("RSqlStatement::Prepare() failed. Err %d, ErrMsg: \"%S\".\r\n"), err, &errdes);
        }
    TEST2(err, KErrNone);
    stmt.Close();
    
    delete buf;
    TheDb.Close();
    (void)RSqlDatabase::Delete(KTestDatabase1);
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4157
@SYMTestCaseDesc        Test for PDEF143461  Calling CSqlSrvDatabase::LastErrorMessage() does not panic with the descriptor alignment error
@SYMTestPriority        Normal
@SYMTestActions         Test for PDEF143461  - CSqlSrvDatabase::LastErrorMessage() alignment problem.
                        This tests the following SQLite Error messages to make sure it doesn't panic:
                        1)library routine called out of sequence
                        2)out of memory
@SYMTestExpectedResults Test must not fail
@SYMDEF                 PDEF143461 
*/
void PDEF143461L()
    {
    (void) RSqlDatabase::Delete(KTestDatabase6);
    
    //Create and setup the database file
    RSqlDatabase db;
    TInt err =0;
    err = db.Create(KTestDatabase6);
    TEST2(err, KErrNone);
    err = db.Exec(_L("CREATE TABLE t(num INTEGER)"));
    TEST2(err, 1);
    err = db.Exec(_L("INSERT INTO t VALUES(1)"));
    TEST2(err, 1);
    err = db.Exec(_L("INSERT INTO t VALUES(2)"));
    TEST2(err, 1);

    
    //Purposely commit an error so LastErrorMessage can be called 
    RSqlStatement stmt;
    err = stmt.Prepare(db, _L("DELETE FROM t WHERE ROWID=?"));
    TEST2(err, KErrNone);
    err = stmt.BindInt(0, 1);
    TEST2(err, KErrNone);        
    err = stmt.Exec();
    TEST2(err, 1);
    
    //Should have reset stmt here
    err = stmt.BindInt(0, 2);
    TEST2(err, KErrNone); 
    err = stmt.Exec();
    TEST2(err, KSqlErrMisuse);
    
    //Test "library routine called out of sequence" error message 
    //If the defect is not fixed then it will panic here   
    TPtrC errMsg = db.LastErrorMessage();
    RDebug::Print(_L("errMsg=%S\r\n"), &errMsg);

    stmt.Close();
    db.Close();
    
    TInt allocationNo = 0;
    //The mask allows the out of memory simulation of the SQL Server to be delayed until the database is opened
    const TInt KDelayedDbHeapFailureMask = 0x1000;
    
    do
        {
        TSqlResourceTester::SetDbHeapFailure(RHeap::EDeterministic |KDelayedDbHeapFailureMask, ++allocationNo);
        err = db.Open(KTestDatabase6);
        TEST2(err, KErrNone);
        err = db.Exec(_L("INSERT INTO t VALUES(3)"));
        TSqlResourceTester::SetDbHeapFailure(RHeap::ENone, 0);
        
        TheTest.Printf(_L("%d    \r"), allocationNo);
        //Test "out of memory" error message, if the defect is not fixed then it will panic here   
        TPtrC errMsg = db.LastErrorMessage();
        RDebug::Print(_L("errMsg=%S\r\n"), &errMsg);
        db.Close();
        }
    while (err == KErrNoMemory);
    TEST2(err, 1);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4166
@SYMTestCaseDesc        Tests for DEF144027: SQL Open returns error if the reported and actual file size are different
@SYMTestPriority        Medium
@SYMTestActions         1) Create a simple database and close it (this will automatically delete the journal file
                        2) Create a 15 bytes garbage journal file which is just less than the minimum file size allowed.
                        3) Reopen the database and checks that the open operation does not fail even thou we've used a 
                        garbage journal file which is too small
@SYMTestExpectedResults The RSqlDatabase::Open operation should not fail
@SYMDEF                 DEF144027
                        DEF144238
*/
void DEF144027()
    {
    (void) RSqlDatabase::Delete(KTestDatabase7);
    (void) TheFs.Delete(KTestDatabase7Journal);
    
    TInt err = TheDb.Create(KTestDatabase7);
    TEST2(err, KErrNone);
    
    err = TheDb.Exec(_L("CREATE TABLE t1(NUM INTEGER)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t1(NUM) VALUES (1)"));
    TEST2(err, 1);
    
    TheDb.Close();
    
    //Created a garbage 15 bytes journal file 
    RFile file;
    err = file.Create(TheFs, KTestDatabase7Journal, EFileWrite);
    TEST2(err, KErrNone);
    
    _LIT8(KJournalJunkData, "A123456789B1234");//15 bytes
    err = file.Write(0, KJournalJunkData);
    TEST2(err, KErrNone);
    
    file.Flush();
    file.Close();
    
    //Here we check the open operation does not return an error, 
    //even though there is a journal file less than 16 bytes
    err = TheDb.Open(KTestDatabase7);
    TEST2(err, KErrNone);
    TheDb.Close();
    }

/**
Test defect where calling RSQLStatement::DeclaredColumnType() on a table which contains long (> 20 characters) column type 
names results in a USER 11 panic.
This test should pass because these are valid SQL column types 
*/
void LongColumnTypeTest()
	{
	(void)RSqlDatabase::Delete(KTestDatabase3);
	TInt err = TheDb.Create(KTestDatabase3);
	TEST2(err, KErrNone);
	
	_LIT8(KCreateStmt, "CREATE TABLE t(a CHARACTER VARYING(100000), b NCHAR VARYING(100000), c NATIONAL CHARACTER(100000), d NATIONAL CHARACTER VARYING(100000))");
	err = TheDb.Exec(KCreateStmt);
	TEST(err >= 0);
	
	//Select all columns (SELECT *)
	_LIT(KSelectStmt, "SELECT * FROM t");
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, KSelectStmt);
	TEST2(err, KErrNone);
	
	TSqlColumnType colType;
	err = stmt.DeclaredColumnType(0, colType);
	TEST2(err,KErrNone);
	TEST2(colType, ESqlText);
	
	err = stmt.DeclaredColumnType(1, colType);
	TEST2(err,KErrNone);
	TEST2(colType, ESqlText);
	
	err = stmt.DeclaredColumnType(2, colType);
	TEST2(err,KErrNone);
	TEST2(colType, ESqlText);
	
	err = stmt.DeclaredColumnType(3, colType);
	TEST2(err,KErrNone);
	TEST2(colType, ESqlText);
	
	stmt.Close();

	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDatabase3); 
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1763 \"SQL against a detached db\" test "));
	SqlDetachedDbTest();	

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4034 Corrupted db file (file length too short)"));
	CorruptDbFileTest();	
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4035 Attempt to attach a file which name cannot be parsed"));
	AttachBadDbFileNameTest();	

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4036 Attempt to attach a secure database. The client cannot pass the security checks"));
	AttachSecureDbTest();	

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4032 INC091579 - SQL Panic 7 when streaming BLOB fields"));
	INC091579L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4033 INC091580 - SQL returns bogus pointer when too much text in field..."));
	INC091580L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1815 Testing KSqlErrFull error code "));
	SqlErrFullTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1816-0001 INC094870 - [SQL] Database became corrupted and cannot be opened "));
	INC094870L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1817-0001 INC095412: [SQL] Retrieving query results may corrupt heap "));
 	INC095412();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3427 DEF104242 - The SQL server fails to open database with default security policy only "));
	DEF104242();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3430 DEF104437 - RSqlStatement::Next() panics the SQL server with KERN-EXEC 3 "));
	DEF104437();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3442 DEF105259 - SQL, RSqlColumnReadStream's internal buffer may become invalid "));
	DEF105259L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3470 DEF105681 - SQL, HReadOnlyBuf::ConstructL() sets a pointer to a local TPtr8 variable "));
	DEF105681L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3476 DEF106391 SQL server does not deallocate the already allocated memory "));
	DEF106391();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3501 DEF109025 SQL, dangling long binary/text column value pointer "));
	DEF109025();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3546 DEF109843 SQL, RSQLStatement::BindBinary() is causing panic if empty descriptor is passed "));
	DEF109843();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4005 DEF114698: SqlSrv.EXE::!SQL Server Insert/Update Error "));
	DEF114698();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4008 DEF115567 Critical SQLite defect that can cause database corruption during UPDATE/DELETE "));
	DEF115567L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4009-0001 DEF115556: SqlSrv.EXE::!SQL Server when preparing complex sql query "));
	DEF115556();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4012 DEF115954: CSession Code = 2 executing SQL stmt in OOM loop ")); 
	DEF115954();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4019 DEF116397: SQL, Attaching database returns KErrEof(-25) "));
	DEF116397();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4029 DEF119403: Database re-index unnecessarily during open operation"));
	DEF119403L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4031 DEF120237: SQL, SQLITE3.3.17, \"collate nocase\", wrong results."));
	DEF120237L();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4120 DEF125881: RSqlDatabase::Exec() returns KErrAlreadyExists in I/O failure use cases."));
	DEF125881L();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4128 DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases."));
    DEF129581();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4153 DEF143047: SQL, default \"max parameter count\" value, compatibility problem."));
    DEF143047();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4157 PDEF143461 : CSqlSrvDatabase::LastErrorMessage() alignment problem"));
    PDEF143461L();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4166 DEF144027: SQL Open returns error if the reported and actual file size are different"));
    DEF144027();
    
    TheTest.Next(_L("RSQLStatement::DeclaredColumnType() causes USER 11 panic when table contains long column type strings"));
    LongColumnTypeTest();
 
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DeleteTestFiles();
	TInt err = RSqlDatabase::Copy(KCorruptDbZ, KCorruptDb);	
	TEST2(err, KErrNone);
	TRAP(err, DoTestsL());
	DeleteTestFiles();
	TheFs.SetErrorCondition(KErrNone);
	TheFs.Close();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
