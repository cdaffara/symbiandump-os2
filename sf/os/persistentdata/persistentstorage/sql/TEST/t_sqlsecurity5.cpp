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
// t_sqlsecurity5 application has capabilities allowing schema access to the test database
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

RSqlDatabase TheDb;
RTest TheTest(_L("t_sqlsecurity5 test"));

_LIT(KTestDbName, "c:[21212125]t_ab.db");
_LIT(KTestDbName2, "c:\\test\\t_sqlsecurity5_2.db");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestDb2()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	}

///////////////////////////////////////////////////////////////////////////////////////
//Restore original test database function
void RestoreOriginalDb()
	{
	TheDb.Close();
	TheDb.Open(KTestDbName);
	
	// Delete and restore the content of table A (unconditional DELETE, no READ operations)
	TheDb.Exec(_L("DELETE FROM A"));
	TheDb.Exec(_L("INSERT INTO A(F1,B1) VALUES(1,x'41414141414141414141');INSERT INTO A(F1,B1) VALUES(2,x'42424242424242424242');INSERT INTO A(F1,B1) VALUES(3,x'43434343434343434343');INSERT INTO A(F1,B1) VALUES(4,x'44444444444444444444');"));

	// Delete and restore the content of table B (unconditional DELETE, no READ operations)
	TheDb.Exec(_L("DELETE FROM B"));
	TheDb.Exec(_L("INSERT INTO B(F2,F3,B2) VALUES(2, 'ABC',x'45454545454545454545');INSERT INTO B(F2,F3,B2) VALUES(4,'DEF',x'46464646464646464646');"));

	TheDb.Close();	
	}
	
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestDb2();
		RestoreOriginalDb();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestDb2();
		RestoreOriginalDb();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1647
@SYMTestCaseDesc		Testing database operations on a secure database.
						The test application's capabilities allow schema access to the test secure database.
						Verify that any other kind of a database operation will pass.
@SYMTestPriority		High
@SYMTestActions			Testing database operations on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SchemaSecurityTest()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	
	//Attempt to modify the database schema
	err = TheDb.Exec(_L("CREATE TABLE IF NOT EXISTS C(FFF TEXT)"));
	TEST(err >= 0);	
	//Index operations
    err = TheDb.Exec(_L("CREATE INDEX Cidx ON C(FFF)"));
    TEST(err >= 0);     
    err = TheDb.Exec(_L("ANALYZE C"));
    TEST(err >= 0);     
    err = TheDb.Exec(_L("DROP INDEX Cidx"));
    TEST(err >= 0);     
    //Trigger operations
    err = TheDb.Exec(_L("CREATE TRIGGER T1 AFTER INSERT ON C BEGIN INSERT INTO B VALUES(1, 2); END;"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("DROP TRIGGER T1"));
    TEST(err >= 0);
    //View operations
    err = TheDb.Exec(_L("CREATE VIEW V1 AS SELECT * FROM C"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("DROP VIEW V1"));
    TEST(err >= 0);
	//Attempt to update the user data (but it includes a READ operation)
	err = TheDb.Exec(_L("UPDATE A SET F1 = 11 WHERE F1 = 1"));
	TEST(err >= 0);	
	//Attempt to update the user data (unconditional UPDATE, no READ operations)
	err = TheDb.Exec(_L("UPDATE A SET F1 = 11"));
	TEST(err >= 0);	
	//Attempt to delete the user data (but it includes a READ operation)
	err = TheDb.Exec(_L("DELETE FROM B WHERE F2 = 2"));
	TEST(err >= 0);	
	//Attempt to delete the user data (unconditional DELETE, no READ operations)
	err = TheDb.Exec(_L("DELETE FROM A"));
	TEST(err >= 0);	
	//Restore the deleted table A
	err = TheDb.Exec(_L("INSERT INTO A(F1,B1) VALUES(1,x'41414141414141414141');INSERT INTO A(F1,B1) VALUES(2,x'42424242424242424242');INSERT INTO A(F1,B1) VALUES(3,x'43434343434343434343');INSERT INTO A(F1,B1) VALUES(4,x'44444444444444444444');"));
	TEST(err >= 0);	
	//Restore the deleted record in table B
	err = TheDb.Exec(_L("INSERT INTO B(F2, F3, B2) VALUES(2, 'ABC', x'45454545454545454545');"));
	TEST2(err, 1);
	//Attempt to insert new user data
	err = TheDb.Exec(_L("INSERT INTO B(F2, F3, B2) VALUES(6, 'GHI', x'47474747474747474747');"));
	TEST2(err, 1);
	//Attempt to read the user data
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT A.F1 FROM B,A WHERE A.F1 = B.F2"));
	TEST2(err, KErrNone);
	//ColumnCount() has no capabilities assigned
	TInt colCnt = stmt.ColumnCount();
	TEST2(colCnt, 1);
	//
	stmt.Close();
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4037
@SYMTestCaseDesc		RSqlStatement::DeclaredColumnType() - security test.
						The test calls RSqlStatement::DeclaredColumnType() on a secure database.
						It should be possible to retrieve the declared column type without problems.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::DeclaredColumnType() - security test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/
void DeclaredColumnTypeTest()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT A.F1 FROM B,A WHERE A.F1 = B.F2"));
	TEST2(err, KErrNone);
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
	//
	stmt.Close();
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4046
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&), platsec test.
						The test verifies that RSqlDatabase::Size(TSize&) can be called
						on the main or on an attached database no matter what the client capabilities are.
@SYMTestPriority		Medium
@SYMTestActions			RSqlDatabase::Size(TSize&), platsec test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void Size2Test()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	//Size(TSize&) has no capabilities assigned
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	//Attach and repeat the test again
	_LIT(KAttachDbName, "Db");
	err = TheDb.Attach(KTestDbName, KAttachDbName);
	TEST2(err, KErrNone);
	TEST(size.iSize > 0);
	TEST(size.iFree >= 0);
	err = TheDb.Size(size, KAttachDbName);
	TEST2(err, KErrNone);
	TEST(size.iSize > 0);
	TEST(size.iFree >= 0);
	err = TheDb.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4047
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
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4098
@SYMTestCaseDesc		Testing incremental blob reads and writes on a secure database.
						The test application's schema capabilities allow read and write access to the blobs.
						Verify that both reads and writes are allowed and also that database operations 
						that require schema capability are allowed.
@SYMTestPriority		Medium
@SYMTestActions			Testing incremental blob reads and writes and schema operations on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/	
void SchemaBlobTestL()
	{
	// Current database data:
	// TABLE A: {1, x'41414141414141414141'}, {2, x'42424242424242424242'}, {3, x'43434343434343434343'}, {4, x'44444444444444444444'}
	// TABLE B: {4, "DEF", x'46464646464646464646'} <- ROWID = 2, {2, "ABC", x'45454545454545454545'} <- ROWID = 3, {6, "GHI", x'47474747474747474747'} <- ROWID = 4

	RSqlDatabase db;
	TInt err = db.Open(KTestDbName);
	TEST2(err, KErrNone);
			
	// Attempt to read the blobs in tables A and B
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TBuf8<20> data;
	TRAP(err, rdStrm.OpenL(db, _L("A"), _L("B1"), 2));
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 7));
	TEST2(err, KErrNone);
	TEST(data.Compare(_L8("BBBBBBB")) == 0);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(db, _L("B"), _L("B2"), 2));
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 9));
	TEST2(err, KErrNone);
	TEST(data.Compare(_L8("FFFFFFFFF")) == 0);
	CleanupStack::PopAndDestroy(&rdStrm);	

	HBufC8* wholeBuf = TSqlBlob::GetLC(db, _L("A"), _L("B1"), 1);
	TEST(wholeBuf->Des().Compare(_L8("AAAAAAAAAA")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf);  	
	wholeBuf = TSqlBlob::GetLC(db, _L("B"), _L("B2"), 4);
	TEST(wholeBuf->Des().Compare(_L8("GGGGGGGGGG")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 

	HBufC8* buf = HBufC8::NewLC(10);	
	TPtr8 bufPtr(buf->Des());	  
	err = TSqlBlob::Get(db, _L("A"), _L("B1"), bufPtr, 3);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("CCCCCCCCCC")) == 0);	
	err = TSqlBlob::Get(db, _L("B"), _L("B2"), bufPtr, 2);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("FFFFFFFFFF")) == 0);
	CleanupStack::PopAndDestroy(buf); 
	
	// Attempt to write the blobs in tables A and B
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(db, _L("A"), _L("B1"), 1));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("ZZZ")));
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(db, _L("B"), _L("B2"), 3));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("WWWWWWWWWW")));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(db, _L("A"), _L("B1"), _L8("UUUUUUUU"), 4));
	TEST2(err, KErrNone);
	TRAP(err, TSqlBlob::SetL(db, _L("B"), _L("B2"), _L8("SSS"), 4));
	TEST2(err, KErrNone);
	
	// SQLite and system tables
	
	// Attempt to read from and write to the SQLite master table -
	// reads should be permitted because schema capability is enough for this, 
	// writes should be permitted because schema capability is enough for this
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(db, _L("sqlite_master"), _L("tbl_name"), 1)); // TEXT column
	TEST2(err, KErrNone);
	TRAP(err, rdStrm.ReadL(data, 1));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&rdStrm); 	

	wholeBuf = TSqlBlob::GetLC(db, _L("sqlite_master"), _L("tbl_name"), 1);
	TEST(wholeBuf->Length() > 0);	
	CleanupStack::PopAndDestroy(wholeBuf);  	

	buf = HBufC8::NewLC(100);
	bufPtr.Set(buf->Des());	 	  
	err = TSqlBlob::Get(db, _L("sqlite_master"), _L("tbl_name"), bufPtr, 1);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Length() > 0);	
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(db, _L("sqlite_master"), _L("tbl_name"), 1));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("myTableName")));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(db, _L("sqlite_master"), _L("tbl_name"), _L8("myTableName"), 1));
	TEST2(err, KErrNone);

	// Attempt to read from and write to the system tables - neither reads nor writes should be permitted
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(db, _L("symbian_security"), _L("PolicyData"), 1)); // BLOB column
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&rdStrm);	

	TRAP(err, wholeBuf = TSqlBlob::GetLC(db, _L("symbian_security"), _L("PolicyData"), 1));
	TEST2(err, KErrPermissionDenied);

	buf = HBufC8::NewLC(100);	
	bufPtr.Set(buf->Des());	  
	err = TSqlBlob::Get(db, _L("symbian_security"), _L("PolicyData"), bufPtr, 1);
	TEST2(err, KErrPermissionDenied); 
	CleanupStack::PopAndDestroy(buf); 
	
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(db, _L("symbian_security"), _L("PolicyData"), 1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(db, _L("symbian_security"), _L("PolicyData"), _L8("VVVV"), 1));
	TEST2(err, KErrPermissionDenied);
	
	db.Close();
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1647 Schema database access test "));
	SchemaSecurityTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4037 Declared column type test"));
	DeclaredColumnTypeTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4046 Size(TSize&) test"));
	Size2Test();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4047 Compact() test"));
	CompactTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4098 Schema blob access test"));
	SchemaBlobTestL();
	
	RestoreOriginalDb(); // the same db is used by the other t_security test exe's
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
