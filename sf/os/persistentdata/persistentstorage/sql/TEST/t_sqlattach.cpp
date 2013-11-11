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
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlattach test"));

RSqlDatabase TheDb;
RSqlDatabase TheDb2;

_LIT(KTestDir, "c:\\test\\");

_LIT(KTestDb1, "c:\\test\\t_sqlattach_1.db");
_LIT(KTestDb2, "c:\\test\\t_sqlattach_2.db");
_LIT(KTestDb3, "c:\\test\\t_sqlattach_3.db");
_LIT(KTestDb4, "c:\\test\\t_sqlattach_4.db");

_LIT(KSecureTestDb1, "c:[21212122]BBDb2.db");//Created outside this test app
_LIT(KSecureTestDb2, "c:[21212122]AADb2.db");//Created outside this test app
_LIT(KSecureTestDb3, "c:[21212123]t_sqlattach_3.db");
_LIT(KDbNameInjection, "c:\\test\\t_sqlattach_3.db' as db; delete from a;");

//const TUid KSecureUid = {0x21212122};//The UID of the secure test databases: KSecureTestDb1 and KSecureTestDb2

//The test uses two secure databases: KSecureTestDb1 and KSecureTestDb2.
//
//KSecureTestDb1 schema
//TABLE A1(F1 INTEGER , F2 INTEGER, B1 BLOB)
//
//KSecureTestDb1 security settings
//-Security UID  = KSecureUid
//-Schema policy = ECapabilityTrustedUI
//-Read policy   = ECapabilityReadDeviceData
//-Write policy  = ECapabilityWriteDeviceData
//The test application can read/write the database tables but cannot modify the database structure
//
//KSecureTestDb2 schema
//TABLE C(A1 INTEGER, B2 BLOB)
//
//KSecureTestDb2 security settings
//-Security UID  = KSecureUid
//-Schema policy = ECapabilityDiskAdmin
//-Read policy   = ECapabilityNetworkControl
//-Write policy  = ECapabilityWriteDeviceData
//The test application can write to the database tables but cannot modify the database structure or read from tables

///////////////////////////////////////////////////////////////////////////////////////

void DeleteDatabases()
	{
	TheDb2.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbNameInjection);
	(void)RSqlDatabase::Delete(KSecureTestDb3);
	(void)RSqlDatabase::Delete(KTestDb4);
	(void)RSqlDatabase::Delete(KTestDb3);
	(void)RSqlDatabase::Delete(KTestDb2);	
	(void)RSqlDatabase::Delete(KTestDb1);	
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteDatabases();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteDatabases();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

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

void CreateDatabases()
	{
	TBuf<100> sql;
	
	TInt err = TheDb.Create(KTestDb1);
	TEST2(err, KErrNone);
	sql.Copy(_L("CREATE TABLE A1(F1 INTEGER, F2 INTEGER)"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	sql.Copy(_L("CREATE TABLE A2(DDD INTEGER)"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	TheDb.Close();
	
	err = TheDb.Create(KTestDb2);
	TEST2(err, KErrNone);
	sql.Copy(_L("CREATE TABLE B(A1 INTEGER, A2 INTEGER)"));
	err = TheDb.Exec(sql);
	TEST(err >= 0);
	TheDb.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1641
@SYMTestCaseDesc		Attached database tests.
						Open non-secure database, attach secure database.
						The test application's security policy allows read/write operations on the attached
						database, but database schema modifications are not allowed. The test executes
						different kind of SQL statements to verify that the test application's security 
						policy is properly asserted by the SQL server.
@SYMTestPriority		High
@SYMTestActions			Execution SQL statements on attached database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void Test1()
	{
	TInt err = TheDb.Open(KTestDb1);
	TEST2(err, KErrNone);
	
	//Attach a secure database, the logical database name length is 0
	_LIT(KAttachDb0, "");
	err = TheDb.Attach(KSecureTestDb1, KAttachDb0);
	TEST2(err, KErrBadName);

	//Attach a secure database, the logical database name length is > than KMaxFileName
	TBuf<KMaxFileName + 1> longDbName;
	longDbName.SetLength(longDbName.MaxLength());
	longDbName.Fill(TChar('A'));
	err = TheDb.Attach(KSecureTestDb1, longDbName);
	TEST2(err, KErrBadName);
	
	//Attach a secure database
	//The test application can read/write the attached database tables but cannot modify the database structure
	_LIT(KAttachDb1, "Db1");
	err = TheDb.Attach(KSecureTestDb1, KAttachDb1);
	TEST2(err, KErrNone);
	
	//Attempt to read from the attached secure database
	err = TheDb.Exec(_L("SELECT * FROM db1.a1"));
	TEST(err >= 0);
	//Attempt to write to the attached secure database
	err = TheDb.Exec(_L("INSERT INTO dB1.a1(f1) valUES(10)"));
	TEST2(err, 1);
	//Attempt to modify the attached secure database schema
	err = TheDb.Exec(_L("CREATE TABLE db1.CCC(H REAL)"));
	TEST2(err, KErrPermissionDenied);
	err = TheDb.Exec(_L("ALTER TABLE db1.A1 ADD COLUMN a2 integer"));
	TEST2(err, KErrPermissionDenied);
	
	//Attempt to read from the main non-secure database
	err = TheDb.Exec(_L("SELECT * FROM main.a1"));
	TEST(err >= 0);
	//Attempt to write to the main non-secure database
	err = TheDb.Exec(_L("INSERT INTO a1(f1) valUES(10)"));
	TEST2(err, 1);
	//Attempt to modify the main non-secure database schema
	err = TheDb.Exec(_L("CREATE TABLE a3(H REAL)"));
	TEST(err >= 0);

	TheTest.Printf(_L("===Attach second, non-secure database"));
	//Attach a non-secure database
	//The test application should be able to do everything with the attached database
	_LIT(KAttachDb2, "db2");
	err = TheDb.Attach(KTestDb2, KAttachDb2);
	TEST2(err, KErrNone);

	//Attempt to read from the attached non-secure database
	err = TheDb.Exec(_L("SELECT * FROM db2.B"));
	TEST(err >= 0);
	//Attempt to write to the attached non-secure database
	err = TheDb.Exec(_L("INSERT INTO dB2.b(a2) ValUES(112)"));
	TEST2(err, 1);
	//Attempt to modify the attached non-secure database schema
	err = TheDb.Exec(_L("ALTER TABLE db2.b ADD COLUMN a3 text"));
	TEST(err >= 0);

	TheTest.Printf(_L("===Attach third, non-secure database (the main database)"));
	//Attach a non-secure database (the main database)
	//The test application should be able to do everything with the attached database
	_LIT(KAttachDb3, "db3");
	err = TheDb.Attach(KTestDb1, KAttachDb3);
	TEST2(err, KErrNone);
	
	//Attempt to read from the third, non-secure database
	err = TheDb.Exec(_L("SELECT * FROM db3.a1"));
	TEST(err >= 0);
	//Attempt to write to the third, non-secure database
	err = TheDb.Exec(_L("INSERT INTO db3.a1(f2) values(11)"));
	TEST2(err, 1);
	//Attempt to modify the third, non-secure database schema
	err = TheDb.Exec(_L("CREATE TABLE db3.a4(s blob)"));
	TEST(err < 0);//Cannot modify the main database from the atatched!?

	TheTest.Printf(_L("===Attach fourth, secure database"));
	//Attach a secure database
	//The test application can only write to the database, but cannot modify the schema or read from the database
	_LIT(KAttachDb4, "db4");
	err = TheDb.Attach(KSecureTestDb2, KAttachDb4);
	TEST2(err, KErrNone);

	//Attempt to read from the attached secure database
	err = TheDb.Exec(_L("SELECT * FROM db4.c"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to write to the attached secure database
	err = TheDb.Exec(_L("INSERT INTO Db4.c(a1) VALUES(1)"));
	TEST2(err, 1);
	//Attempt to write to a non-secure database using data from the attached secure database
	err = TheDb.Exec(_L("INSERT INTO a1(f1) select db4.c.a1 from db4.c"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to write to a secure database using data from a non-secure database
	err = TheDb.Exec(_L("INSERT INTO db4.c(a1) select f1 from a1"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("UPDATE db4.C SET a1 = 3 WHERE a1 = 1"));
	TEST2(err, KErrPermissionDenied);//!?!?!?
	err = TheDb.Exec(_L("DELETE FROM db4.C"));
	TEST(err >= 0);
	//Attempt to modify the attached secure database schema
	err = TheDb.Exec(_L("CREATE TABLE db4.CCC(z integer)"));
	TEST2(err, KErrPermissionDenied);
	err = TheDb.Exec(_L("DROP table db4.C"));
	TEST2(err, KErrPermissionDenied);
	
	err = TheDb.Detach(KAttachDb2);
	TEST2(err, KErrNone);	
	err = TheDb.Detach(KAttachDb1);
	TEST2(err, KErrNone);
	
	err = TheDb.Detach(KAttachDb4);
	TEST2(err, KErrNone);	
	err = TheDb.Exec(_L("SELECT * FROM db4.c"));
	TEST(err < 0);
		
	err = TheDb.Detach(KAttachDb2);
	TEST(err != KErrNone);	
	
	err = TheDb.Detach(KAttachDb3);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("INSERT INTO db3.a1(f2) values(11)"));
	TEST(err < 0);
	
	err = TheDb.Detach(KAttachDb4);
	TEST(err != KErrNone);	

    //Detach() with zero-length logical database name
    err = TheDb.Detach(_L(""));
    TEST2(err, KErrBadName);  
    
    //Detach() with logical database name containing "bad" unicode characters (cannot be converted to UTF8)
    TBuf<2> dbName3;
    dbName3.SetLength(2);
    dbName3[0] = TChar(0xD800); 
    dbName3[1] = TChar(0xFC00); 
    err = TheDb.Detach(dbName3);
    TEST2(err, KSqlErrGeneral);  
    
    //Attach a non-existing database
    _LIT(KAttachDbFile5, "c:\\test\\zxcvbnm987654321.db");
    _LIT(KAttachDb5, "zxcvbnm987654321");
    err = TheDb.Attach(KAttachDbFile5, KAttachDb5);
    TEST2(err, KErrNotFound);
        
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1642
@SYMTestCaseDesc		Attached database tests.
						Open secure database, attach secure database.
						The test application's security policy allows read/write operations on the main
						database, but database schema modifications are not allowed.  The test application
						is allowed to write to the attached database but can't read from or modify the schema.
						The test executes different kind of SQL statements to verify that the test application's security 
						policy is properly asserted by the SQL server.
@SYMTestPriority		High
@SYMTestActions			Execution SQL statements on attached database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void Test2()
	{
	//The test application can read/write the database tables but cannot modify the database structure
	TInt err = TheDb.Open(KSecureTestDb1);
	TEST2(err, KErrNone);
	_LIT(KAttachDb2, "Db2");
	//The test application can only write to the database, but cannot modify the schema or read from the database
	err = TheDb.Attach(KSecureTestDb2, KAttachDb2);
	TEST2(err, KErrNone);
	
	//Attempt to read from the main database and write to the attached database
	err = TheDb.Exec(_L("INSERT INTO db2.c(a1) SELECT f1 FROM a1"));
	TEST(err >= 0);
	
	//Attempt to read from the attached database and write to the main database
	err = TheDb.Exec(_L("INSERT INTO  a1(f2) SELECT a1 FROM db2.c"));
	TEST2(err, KErrPermissionDenied);

	//Attempt to detach database using DETACH sql statement directly.
	err = TheDb.Exec(_L("DETACH DATABASE DB2"));
	TEST2(err, KErrPermissionDenied);
		
	err = TheDb.Detach(KAttachDb2);
	TEST2(err, KErrNone);	

	//Attempt to attach a database using ATTACH sql statement directly.
	TBuf<100> sql;
	sql.Format(_L("ATTACH DATABASE '%S' AS Db3"), &KSecureTestDb2);
	err = TheDb.Exec(sql);
	TEST2(err, KErrPermissionDenied);
		
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1814
@SYMTestCaseDesc		Attached database tests. SQL injection.
						Create the following test databases:
						1) c:\test\inj.db
						2) c:\test\inj.db' as db; delete from a;
						3) c:[21212123]Injected.db
						Insert some records in database (3). Attach database (2) to database (3).
						Check the records count of table A. If the count is zero, then it means that the injection has been successful
						and a security hole exists when attaching/detaching databases.
@SYMTestPriority		High
@SYMTestActions			Attached database tests. SQL injection.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SqlInjectionTest()
	{
	//Create the database, which name is used for the attack. 
	//This is done just to ensure that the database, which name is used in the SQL injection, exists,
	//Otherwise the injection attack may fail with KErrNotFound error.
	TInt err = TheDb2.Create(KTestDb3);
	TEST2(err, KErrNone);
	TheDb2.Close();
	err = TheDb2.Create(KDbNameInjection);
	TEST2(err, KErrNone);
	TheDb2.Close();
	//Create a secure database, which will be impacted by the SQL injection
	TSecurityPolicy policy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy dbPolicy;
	err = dbPolicy.Create(policy);
	TEST2(err, KErrNone);
	err = TheDb.Create(KSecureTestDb3, dbPolicy);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id Integer)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(Id) VALUES(1)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(Id) VALUES(2)"));
	TEST(err >= 0);
	const TInt KInsertedRecCnt = 2;
	//Cleanup
	dbPolicy.Close();
	TheDb.Close();
	//Repopen the secure database and attach the secind database, which file name is actually a SQL injection
	err = TheDb.Open(KSecureTestDb3);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KDbNameInjection, _L("Db2"));
	TEST2(err, KErrNone);
	//Check table A contents. If the security hole still exists, table A content is gone.
	TSqlScalarFullSelectQuery query(TheDb);
	TInt recCnt = 0;
	TRAP(err, recCnt = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCnt, KInsertedRecCnt);//if zero records count - successfull SQL injection - the security hole exists!
	//Try to execute RSqlDatabase::Detach(), where instead of a logical database name, SQL statement is supplied.
	err = TheDb.Detach(_L("DB; INSERT INTO A(Id) VALUES(3)"));
	TEST(err != KErrNone);
	//Check table A contents. If the security hole still exists, table A will have one more record.
	TRAP(err, recCnt = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCnt, KInsertedRecCnt);//if one more record - successfull SQL injection - the security hole exists!
	TheDb.Close();
	//Cleanup
	(void)RSqlDatabase::Delete(KDbNameInjection);
	(void)RSqlDatabase::Delete(KTestDb3);
	(void)RSqlDatabase::Delete(KSecureTestDb3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3507
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlBufRIterator, TSqlAttachDbRefCounter is very low.
						The test opens two existing databases, and the attaches to them the same secure shared database.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlBufRIterator, TSqlAttachDbRefCounter is very low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/	
void TwoConnAttachTest()
	{
	//Connection 1
	TInt err = TheDb.Open(KTestDb1);	
	TEST2(err, KErrNone);
	//Connection 2
	err = TheDb2.Open(KTestDb2);	
	TEST2(err, KErrNone);
	//Attach KSecureTestDb1 to connection 1
	_LIT(KAttachDb1, "Db1");
	err = TheDb.Attach(KSecureTestDb1, KAttachDb1);
	TEST2(err, KErrNone);
	//Attach KSecureTestDb1 to connection 2
	err = TheDb2.Attach(KSecureTestDb1, KAttachDb1);
	TEST2(err, KErrNone);
	//Detach
	err = TheDb2.Detach(KAttachDb1);
	TEST2(err, KErrNone);
	err = TheDb.Detach(KAttachDb1);
	TEST2(err, KErrNone);
	//Cleanup
	TheDb2.Close();
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3515
@SYMTestCaseDesc		RSqlStatement::DeclaredColumnType() test
						The test creates 2 tables in two different databases. Then the test opens the first database and
						attaches the second one. After that a SELECT sql statement is prepared and the statement operates
						on both tables: from the main database and the attached one.
						DeclaredColumnType() is called after the statement preparation and column types checked.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::ColumnCount() test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ8035
*/	
void DeclaredColumnTypeTest()
	{
	//Preparation
	TInt err = TheDb.Open(KTestDb1);	
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE Y(Id INTEGER, Name TEXT)"));
	TEST(err >= 0);
	TheDb.Close();
	err = TheDb.Open(KTestDb2);	
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE Z(Id INTEGER, Data BLOB)"));
	TEST(err >= 0);
	TheDb.Close();
	//Open KTestDb1, attach KTestDb2
	err = TheDb.Open(KTestDb1);
	TEST2(err, KErrNone);
	_LIT(KAttachDb, "Db2");
	err = TheDb.Attach(KTestDb2, KAttachDb);
	TEST2(err, KErrNone);
	//SELECT from both db
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT Y.Id, Y.Name, DB2.Z.Data   FROM Y,DB2.Z   WHERE Y.Id = DB2.Z.Id"));
	TEST2(err, KErrNone);
	TInt colCnt = stmt.ColumnCount();
	TEST2(colCnt, 3);
	TSqlColumnType colType;
	err = stmt.DeclaredColumnType(0, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	err = stmt.DeclaredColumnType(1, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlText);
	err = stmt.DeclaredColumnType(2, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlBinary);
	stmt.Close();
	//Cleanup
	err = TheDb.Detach(KAttachDb);
	TEST2(err, KErrNone);
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4016
@SYMTestCaseDesc		Test for DEF116713 SQL: No redindexing occurs for an attached database.
 						The test does the following steps:
 						1) Sets the "CollationDllName" column value in the "symbian_settings" stable of the database to be used
 						   as an attached database (KTestDb2). The set column value is different than the default collation dll name.
 						2) Opens KTestDb1, attaches KTestDb2.
 						3) When KTestDb2 is attached to KTestDb1, the SQL server should detect that the "CollationDllName" column 
 						   value is different than the default collation dll name and should reindex the attached database and then 
 						   store the current collation dll name in the "CollationDllName" column.
 						4) The test checks that after attaching the KTestDb2 database, the "CollationDllName" column value 
 						   is not the previously used test collation dll name.
@SYMTestPriority		Low
@SYMTestActions			Test for DEF116713 SQL: No redindexing occurs for an attached database.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF116713
*/
void DEF116713()
 	{
 	//Set the "CollationDllName" column value in "symbian_settings" table of the database to be attached - 
 	//not to be the default collation dll name.
 	TInt err = TheDb.Open(KTestDb2);
 	TEST2(err, KErrNone);
 	err = TheDb.Exec(_L("UPDATE symbian_settings SET CollationDllName='ddkjrrm'"));
 	TEST2(err, 1);
 	TheDb.Close();
 	//Open the main database, attach the other one
 	err = TheDb.Open(KTestDb1);
 	TEST2(err, KErrNone);
 	err = TheDb.Attach(KTestDb2, _L("Db2"));
 	TEST2(err, KErrNone);
 	//The expectation is that the attached database is reindexed and the "CollationDllName" column value - set.
 	RSqlStatement stmt;
 	err = stmt.Prepare(TheDb, _L("SELECT CollationDllName FROM Db2.symbian_settings"));
 	TEST2(err, KErrNone);
 	err = stmt.Next();	
 	TEST2(err, KSqlAtRow);
 	TPtrC collationDllName;
 	err = stmt.ColumnText(0, collationDllName);
   	TEST2(err, KErrNone);
 	stmt.Close();
 	TheDb.Close();
 	
 	_LIT(KTestCollationDllName, "ddkjrrm");//The same as the used in the "UPDATE symbian_settings" sql.
 	TEST(collationDllName != KTestCollationDllName);
   	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4042
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) on attached database - injection test.
						The test creates a database and attempts to attach another database,
						passing a DELETE SQL statement in the attached database name.
						The attach operation is expected to fail, the database content should stay
						unchanged after the operation.						
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Size(TSize&) on attached database - injection test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void Size2InjectionTest()
	{
	TInt err = TheDb.Create(KTestDb4);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A VALUES(1)"));
	TEST2(err, 1);
	_LIT(KAttachDbName, "B");
	err = TheDb.Attach(KTestDb4, KAttachDbName);
	TEST2(err, KErrNone);
	RSqlDatabase::TSize	size;
	err = TheDb.Size(size, _L("B;DELETE FROM MAIN.A"));
	TEST2(err, KSqlErrGeneral);
	TPtrC msg = TheDb.LastErrorMessage();
	TheTest.Printf(_L("RSqlDatabase::Size(TSize&) injection, error message: %S\r\n"), &msg);
	TSqlScalarFullSelectQuery q(TheDb);
	TInt reccnt = 0;
	TRAP(err, reccnt = q.SelectIntL(_L("SELECT COUNT(*) FROM MAIN.A")));
	TEST2(err, KErrNone);
	TEST2(reccnt, 1);
	err = TheDb.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDb4);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4043
@SYMTestCaseDesc		RSqlDatabase::Compact() on attached database - injection test.
						The test creates a database and attaches another database.
						Then the test attempts to compact the attached database calling
						RSqlDatabase::Compact() passing DROP TABLE and DELETE statements
						as name of the attached database. The call is expected to fail,
						the database content should stay unchanged after the call.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Compact() on attached database - injection test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
*/
void CompactInjectionTest()
	{
	TInt err = TheDb.Create(KTestDb4);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A(I) VALUES(1)"));
	TEST(err >= 0);
	_LIT(KAttachDbName, "B");
	err = TheDb.Attach(KTestDb4, KAttachDbName);
	TEST2(err, KErrNone);
	err = TheDb.Compact(RSqlDatabase::EMaxCompaction, _L("B;DROP B.A"));
	TEST2(err, KSqlErrGeneral);
	TPtrC msg = TheDb.LastErrorMessage();
	TheTest.Printf(_L("RSqlDatabase::Compact() injection, error message: %S\r\n"), &msg);

	TSqlScalarFullSelectQuery query(TheDb);
	TInt recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);

	err = TheDb.Compact(8192, _L("B;DROP B.A;"));
	TEST2(err, KSqlErrGeneral);
	msg.Set(TheDb.LastErrorMessage());
	TheTest.Printf(_L("RSqlDatabase::Compact() injection, error message: %S\r\n"), &msg);

	recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);

	TRequestStatus stat;
	TheDb.Compact(8192, stat, _L("B;DELETE FROM B.A;"));
	User::WaitForRequest(stat);
	TEST2(stat.Int(), KSqlErrGeneral);
	msg.Set(TheDb.LastErrorMessage());
	TheTest.Printf(_L("RSqlDatabase::Compact() injection, error message: %S\r\n"), &msg);

	recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	
	err = TheDb.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDb4);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4094
@SYMTestCaseDesc		Incremental blob i/o tests on an attached database.
						Open secure database, attach secure database.
						The test application's security policy allows incremental blob read & write 
						operations on the main database, but only write operations on the attached database.
						The test attempts to read and write to a blob in the attached database to verify that 
						the test application's security policy is properly asserted by the Symbian SQL server.
@SYMTestPriority		High
@SYMTestActions			Execution of blob read and write operations on the attached database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/	
void BlobAttachedTestL()
	{
	// Open the main secure database - the test application can read & write blobs in it
	// Attach another secure database - the test application can only write blobs in it
	TInt err = TheDb.Open(KSecureTestDb1);
	TEST2(err, KErrNone);
	_LIT(KAttachDb1, "Db1");
	err = TheDb.Attach(KSecureTestDb2, KAttachDb1);
	TEST2(err, KErrNone);
	
	// Insert a new record into the attached database - the blob value is "AAAAAAAAAA"
	err = TheDb.Exec(_L("INSERT INTO Db1.C(A1, B2) VALUES(15, x'41414141414141414141')"));
	TEST2(err, 1);

	// Attempt to write to a blob in the attached database
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("C"), _L("B2"), KSqlLastInsertedRowId, KAttachDb1));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("ZZZ")));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("C"), _L("B2"), _L8("YYYYY"), KSqlLastInsertedRowId, KAttachDb1));
	TEST2(err, KErrNone);
	
	// Attempt to read a blob in the attached database
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("C"), _L("B2"), KSqlLastInsertedRowId, KAttachDb1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&rdStrm);	

	HBufC8* wholeBuf = NULL;
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb, _L("C"), _L("B2"), KSqlLastInsertedRowId, KAttachDb1));
	TEST2(err, KErrPermissionDenied);

	HBufC8* buf = HBufC8::NewLC(10);	
	TPtr8 bufPtr(buf->Des());	  
	err = TSqlBlob::Get(TheDb, _L("C"), _L("B2"), bufPtr, KSqlLastInsertedRowId, KAttachDb1);
	TEST2(err, KErrPermissionDenied); 
	CleanupStack::PopAndDestroy(buf); 
	
	// SQLite and system tables in the attached database
	
	// Attempt to read from and write to the SQLite master table -
	// reads should be permitted because write capability is enough for this, 
	// writes should not be permitted because schema capability is required for this
	TBuf8<20> data;
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("sqlite_master"), _L("tbl_name"), 1, KAttachDb1)); // TEXT column
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 1));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&rdStrm);	

	wholeBuf = TSqlBlob::GetLC(TheDb, _L("sqlite_master"), _L("tbl_name"), 1, KAttachDb1);
	TEST(wholeBuf->Length() > 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 	

	buf = HBufC8::NewLC(100);
	bufPtr.Set(buf->Des());	 	  
	err = TSqlBlob::Get(TheDb, _L("sqlite_master"), _L("tbl_name"), bufPtr, 1, KAttachDb1);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Length() > 0);	
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("sqlite_master"), _L("tbl_name"), 1, KAttachDb1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("sqlite_master"), _L("tbl_name"), _L8("VVVV"), 1, KAttachDb1));
	TEST2(err, KErrPermissionDenied);

	// Attempt to read from and write to the system tables in the attached database - neither reads nor writes should be permitted
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("symbian_security"), _L("PolicyData"), 1, KAttachDb1)); // BLOB column
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&rdStrm);	

	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb, _L("symbian_security"), _L("PolicyData"), 1, KAttachDb1));
	TEST2(err, KErrPermissionDenied);

	buf = HBufC8::NewLC(100);	
	bufPtr.Set(buf->Des());	  
	err = TSqlBlob::Get(TheDb, _L("symbian_security"), _L("PolicyData"), bufPtr, 1, KAttachDb1);
	TEST2(err, KErrPermissionDenied); 
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("symbian_security"), _L("PolicyData"), 1, KAttachDb1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("symbian_security"), _L("PolicyData"), _L8("VVVV"), 1, KAttachDb1));
	TEST2(err, KErrPermissionDenied);
		
	TheDb.Close();
	}

void DoTestsL()
	{
	CreateDatabases();

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1641 ===Open non-secure database, attach secure database "));
	Test1();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1642 ===Open secure database, attach secure database "));
	Test2();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1814 SQL injection test "));
	SqlInjectionTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3507 DEF109100 - SQL, code coverage for TSqlBufRIterator,TSqlAttachDbRefCounter is very low "));
	TwoConnAttachTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3515 RSqlStatement::DeclaredColumnType() and attached databases test "));
	DeclaredColumnTypeTest();
	
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4016 DEF116713 SQL: No redindexing occurs for an attached database "));
	DEF116713();

 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4042 RSqlDatabase::Size(TSize) - attached database, injection test"));
 	Size2InjectionTest();

 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4043 RSqlDatabase::Compact() - attached database, injection test"));
 	CompactInjectionTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4094 Incremental blob attached test"));
 	BlobAttachedTestL();
	}

TInt E32Main()
	{
	TheTest.Title();
		
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;

	CreateTestDir();
	DeleteDatabases();
	
	TRAPD(err, DoTestsL());
	DeleteDatabases();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
