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
// t_sqlsecurity3 application has capabilities allowing write-only access to the test database
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
RTest TheTest(_L("t_sqlsecurity3 test"));

_LIT(KTestDbName, "c:[21212125]t_ab.db");

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
		RestoreOriginalDb();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RestoreOriginalDb();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1645
@SYMTestCaseDesc		Testing database operations on a secure database.
						The test application's capabilities allow write-only access to the test secure database.
						Verify that any other kind of a database operation will fail with KErrPermissionDenied error.
@SYMTestPriority		High
@SYMTestActions			Testing database operations on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void WriteOnlyDatabaseTest()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	
	//Attempt to modify the database schema
	err = TheDb.Exec(_L("CREATE TABLE C(FFF TEXT)"));
	TEST2(err, KErrPermissionDenied);
    err = TheDb.Exec(_L("CREATE TRIGGER upd_a_b1 UPDATE OF B1 ON A BEGIN UPDATE B SET F3 = 'AAAA' WHERE F2 = A.F1; END;"));
    TEST2(err, KErrPermissionDenied);
    err = TheDb.Exec(_L("CREATE TEMP TRIGGER upd_a_b1 UPDATE OF B1 ON A BEGIN UPDATE B SET F3 = 'AAAA' WHERE F2 = A.F1; END;"));
    TEST2(err, KErrPermissionDenied);//Temp trigger which attempts to update one of the tables.
    err = TheDb.Exec(_L("CREATE VIEW V1 AS SELECT * FROM A"));
    TEST2(err, KErrPermissionDenied);
    err = TheDb.Exec(_L("CREATE TEMP VIEW V1 AS SELECT * FROM A"));
    TEST(err >= 0);
    err = TheDb.Exec(_L("DROP VIEW V1"));
    TEST(err >= 0);
	//Attempt to update the user data (but it includes a READ operation)
	err = TheDb.Exec(_L("UPDATE A SET F1 = 11 WHERE F1 = 1"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to update the user data (unconditional UPDATE, no READ operations)
	err = TheDb.Exec(_L("UPDATE A SET F1 = 11"));
	TEST(err >= 0);	
	//Attempt to delete the user data (but it includes a READ operation)
	err = TheDb.Exec(_L("DELETE FROM B WHERE F2 = 2"));
	TEST2(err, KErrPermissionDenied);
	//Attempt to delete the user data (unconditional DELETE, no READ operations)
	err = TheDb.Exec(_L("DELETE FROM A"));
	TEST(err >= 0);	
	//Restore the deleted table A
	err = TheDb.Exec(_L("INSERT INTO A(F1,B1) VALUES(1,x'41414141414141414141');INSERT INTO A(F1,B1) VALUES(2,x'42424242424242424242');INSERT INTO A(F1,B1) VALUES(3,x'43434343434343434343');INSERT INTO A(F1,B1) VALUES(4,x'44444444444444444444');"));
	TEST(err >= 0);	
	//Attempt to insert new user data
	err = TheDb.Exec(_L("INSERT INTO B(F2, F3, B2) VALUES(22, 'AAA', x'47474747474747474747')"));
	TEST2(err, 1);
	//Attempt to change the isolation level.
	err = TheDb.SetIsolationLevel(RSqlDatabase::ESerializable);	
	TEST2(err, KErrNone);
	err = TheDb.SetIsolationLevel(RSqlDatabase::EReadUncommitted);	
	TEST2(err, KErrNone);
	//Attempt to read the user data
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT A.F1 FROM B,A WHERE A.F1 = B.F2"));
	TEST2(err, KErrPermissionDenied);	
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
	
	TheDb.Close();
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4096
@SYMTestCaseDesc		Testing incremental blob writes on a secure database.
						The test application's capabilities allow write-only access to the blobs.
						Verify that any attempt to read a blob will fail with KErrPermissionDenied.
@SYMTestPriority		High
@SYMTestActions			Testing incremental blob writes on a secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5794
*/	
void WriteOnlyBlobTestL()
	{
	TInt err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
			
	// Attempt to write the blobs in tables A and B
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAP(err, wrStrm.OpenL(TheDb, _L("A"), _L("B1"), 2));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("YYYYYYY")));
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb, _L("B"), _L("B2"), 1));
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.WriteL(_L8("XXXXXXXXX")));
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);	

	TRAP(err, TSqlBlob::SetL(TheDb, _L("A"), _L("B1"), _L8("UUUUUUUU"), 4));
	TEST2(err, KErrNone);
	TRAP(err, TSqlBlob::SetL(TheDb, _L("B"), _L("B2"), _L8("SSS"), 2));
	TEST2(err, KErrNone);
	
	// Attempt to read from the blobs in tables A and B
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("A"), _L("B1"), 1));
	TEST2(err, KErrPermissionDenied);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb, _L("B"), _L("B2"), 1));
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(&rdStrm);	

	HBufC8* wholeBuf = NULL;
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb, _L("A"), _L("B1"), 1));
	TEST2(err, KErrPermissionDenied);
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb, _L("B"), _L("B2"), 1));
	TEST2(err, KErrPermissionDenied);

	HBufC8* buf = HBufC8::NewLC(10);	
	TPtr8 bufPtr(buf->Des());	  
	err = TSqlBlob::Get(TheDb, _L("A"), _L("B1"), bufPtr, 2);
	TEST2(err, KErrPermissionDenied); 
	err = TSqlBlob::Get(TheDb, _L("B"), _L("B2"), bufPtr, 1);
	TEST2(err, KErrPermissionDenied); 
	CleanupStack::PopAndDestroy(buf); 
	
	// SQLite and system tables
	
	// Attempt to read from and write to the SQLite master table -
	// reads should be permitted because write capability is enough for this, 
	// writes should not be permitted because schema capability is required for this
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb, _L("sqlite_master"), _L("tbl_name"), 1)); // TEXT column
	TEST2(err, KErrNone);
	TBuf8<20> data;
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
	
void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1645 Write-only database access test "));
	WriteOnlyDatabaseTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4096 Write-only blob access test"));
	WriteOnlyBlobTestL();
	
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
