// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// t_sqlsecurity2 application has capabilities allowing read-only access to the test database
// 
//

#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////
//The test database has:
//  SCHEMA database policy: ECapabilityReadDeviceData, ECapabilityWriteUserData, ECapabilityReadUserData
//  WRITE database policy:  ECapabilityWriteUserData
//  READ database policy:   ECapabilityReadUserData
//
//Database tables:
//  TABLE A(F1 INTEGER, B1 BLOB)
//  TABLE B(F2 INTEGER, F3 TEXT, B2 BLOB)
//
//Database data:
//  TABLE A: {1, x'41414141414141414141'}, {2, x'42424242424242424242'}, {3, x'43434343434343434343'}, {4, x'44444444444444444444'}
//  TABLE B: {2, "ABC", x'45454545454545454545'}, {4, "DEF", x'46464646464646464646'}

///////////////////////////////////////////////////////////////////////////////////////

#define UNUSED_VAR(a) (a) = (a)

RTest TheTest(_L("t_sqlsecurity2 test"));
RSqlDatabase TheDb;

_LIT(KTestDbName, "c:[21212125]t_ab.db");
_LIT(KTestDbName2, "c:\\test\\t_sqlsecurity2_2.db");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestDb()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestDb();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestDb();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1644
@SYMTestCaseDesc		Testing database operations on a secure database.
						The test application's capabilities allow read-only access to the test secure database.
						Verify that any other kind of a database operation will fail with KErrPermissionDenied error.
@SYMTestPriority		High
@SYMTestActions			Testing database operations on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void ReadOnlyDatabaseTest()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	
	//Attempt to modify the database schema
	err = TheDb.Exec(_L("CREATE TABLE C(FFF TEXT)"));
	TEST2(err, KErrPermissionDenied);
    err = TheDb.Exec(_L("CREATE TEMP TABLE TBL100(COL1 INTEGER)"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("CREATE INDEX IDX100 ON TBL100(COL1)"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("DROP INDEX IDX100"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("DROP TABLE TBL100"));
    TEST(err >= 0);
	//Attempt to update the user data
	err = TheDb.Exec(_L("UPDATE A SET F1 = 11 WHERE F1 = 1"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to delete the user data
	err = TheDb.Exec(_L("DELETE FROM B WHERE F2 = 2"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to insert new user data
	err = TheDb.Exec(_L("INSERT INTO B(F2, F3) VALUES(22, 'AAA')"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to read the user data
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT A.F1 FROM B,A WHERE A.F1 = B.F2"));
	TEST2(err, KErrNone);
	//ColumnCount() has no capabilities assigned
	TInt colCnt = stmt.ColumnCount();
	TEST2(colCnt, 1);
	//DeclaredColumnType() has no capabilities assigned
	TSqlColumnType colType;
	err = stmt.DeclaredColumnType(0, colType);
	TEST2(err, KErrNone);
	TEST2(colType, ESqlInt);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	RDebug::Print(_L("Value=%d\r\n"), stmt.ColumnInt(0));
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	RDebug::Print(_L("Value=%d\r\n"), stmt.ColumnInt(0));
	stmt.Close();
	//Attempt to read the system data
	err = stmt.Prepare(TheDb, _L("SELECT * FROM SQLITE_MASTER"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC p;
	err = stmt.ColumnText(0, p);
	TEST2(err, KErrNone);
	RDebug::Print(_L("Value=%S\r\n"), &p);
	stmt.Close();

	//Attempt to execute PRAGMA statement directly
	err = TheDb.Exec(_L("PRAGMA encoding = \"UTF-8\""));
	TEST2(err, KErrPermissionDenied);
	
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4009
@SYMTestCaseDesc		PlatSec warnings can occur even if an SQL database is successfully opened.
						This test application has a "ReadUserData" capability, and that should allow the
						test database ("c:[21212125]t_ab.db") to be opened successfully, because the "read" 
						database policy consists of a "ReadUserData" capability only.
						No platsec warnings should be seen in the log file ("epocwind.out" file).
@SYMTestPriority		High
@SYMTestActions			PlatSec warnings can occur even if an SQL database is successfully opened.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF115811
*/	
void DEF115811()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4095
@SYMTestCaseDesc		Testing incremental blob reads on a secure database.
						The test application's capabilities allow read-only access to the blobs.
						Verify that any attempt to write to a blob will fail with KErrPermissionDenied.
@SYMTestPriority		High
@SYMTestActions			Testing incremental blob reads on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/
void ReadOnlyBlobTestL()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
		
	// Attempt to read the blobs in tables A and B
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TBuf8<20> data;
	TRAP(err, rdStrm.OpenL(TheDb, _L("A"), _L("B1"), 1));
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 3));
	TEST2(err, KErrNone);
	TEST(data.Compare(_L8("AAA")) == 0);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb, _L("B"), _L("B2"), 2));
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 10));
	TEST2(err, KErrNone);
	TEST(data.Compare(_L8("FFFFFFFFFF")) == 0);
	CleanupStack::PopAndDestroy(&rdStrm); 	
	
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb, _L("A"), _L("B1"), 4);
	TEST(wholeBuf->Des().Compare(_L8("DDDDDDDDDD")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb, _L("B"), _L("B2"), 1);
	TEST(wholeBuf->Des().Compare(_L8("EEEEEEEEEE")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 

	HBufC8* buf = HBufC8::NewLC(10);	
	TPtr8 bufPtr(buf->Des());	  
	err = TSqlBlob::Get(TheDb, _L("A"), _L("B1"), bufPtr, 2);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("BBBBBBBBBB")) == 0);	
	err = TSqlBlob::Get(TheDb, _L("B"), _L("B2"), bufPtr, 2);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("FFFFFFFFFF")) == 0);
	CleanupStack::PopAndDestroy(buf); 
	
	// Attempt to write to the blobs in tables A and B
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("A"), _L("B1"), 1));
	TEST2(err, KErrPermissionDenied);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb, _L("B"), _L("B2"), 1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("A"), _L("B1"), _L8("VVVV"), 1));
	TEST2(err, KErrPermissionDenied);
	TRAP(err, TSqlBlob::SetL(TheDb, _L("B"), _L("B2"), _L8("VVVV"), 1));
	TEST2(err, KErrPermissionDenied);
	
	// SQLite and system tables
	
	// Attempt to read from and write to the SQLite master table - only reads should be permitted
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("sqlite_master"), _L("tbl_name"), 1)); // TEXT column
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 1));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&rdStrm);	

	wholeBuf = TSqlBlob::GetLC(TheDb, _L("sqlite_master"), _L("tbl_name"), 1);
	TEST(wholeBuf->Length() > 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 	

	buf = HBufC8::NewLC(100);
	bufPtr.Set(buf->Des());	 	  
	err = TSqlBlob::Get(TheDb, _L("sqlite_master"), _L("tbl_name"), bufPtr, 1);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Length() > 0);	
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("sqlite_master"), _L("tbl_name"), 1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("sqlite_master"), _L("tbl_name"), _L8("VVVV"), 1));
	TEST2(err, KErrPermissionDenied);

	// Attempt to read from and write to the system tables - neither reads nor writes should be permitted
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("symbian_security"), _L("PolicyData"), 1)); // BLOB column
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&rdStrm);	

	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb, _L("symbian_security"), _L("PolicyData"), 1));
	TEST2(err, KErrPermissionDenied);

	buf = HBufC8::NewLC(100);	
	bufPtr.Set(buf->Des());	  
	err = TSqlBlob::Get(TheDb, _L("symbian_security"), _L("PolicyData"), bufPtr, 1);
	TEST2(err, KErrPermissionDenied); 
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("symbian_security"), _L("PolicyData"), 1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("symbian_security"), _L("PolicyData"), _L8("VVVV"), 1));
	TEST2(err, KErrPermissionDenied);
	
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4078
@SYMTestCaseDesc		RSqlDatabase::Compact(), platsec test.
						The test verifies that RSqlDatabase::Compact() can be called
						on the main or on an attached database no matter what the client capabilities are.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Compact(), platsec test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
*/
void CompactTest()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	
	err = TheDb.Compact(RSqlDatabase::EMaxCompaction);
	TEST(err >= 0);
	
	TRequestStatus stat;
	TheDb.Compact(RSqlDatabase::EMaxCompaction, stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() >= 0);

	TheDb.Close();
	
	err = TheDb.Create(KTestDbName2);
	TEST2(err, KErrNone);
	_LIT(KDbName, "Db");
	err = TheDb.Attach(KTestDbName, KDbName);
	TEST2(err, KErrNone);

	err = TheDb.Compact(RSqlDatabase::EMaxCompaction, KDbName);
	TEST(err >= 0);

	TheDb.Compact(RSqlDatabase::EMaxCompaction, stat, KDbName);
	User::WaitForRequest(stat);
	TEST(stat.Int() >= 0);
	
	err = TheDb.Detach(KDbName);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1644 Read-only database access test "));
	ReadOnlyDatabaseTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4009 DEF115811 - PlatSec warnings can occur even if an SQL database is successfully opened "));
	DEF115811();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4095 - Read-only blob access test"));
	ReadOnlyBlobTestL();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4078 - RSqlDatabase::Compact() test"));
	CompactTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;

	TRAPD(err, DoTestsL());
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
