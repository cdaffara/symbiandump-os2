// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS security policy - testing new APIs.
// This test app has "None" capabilities,
// The UID policy file is 11335579.spd.
// The test uses C:TESTDB.DB secure shared database, which has tables A, B and C, each of them
// with at least one record.
// Please, ensure that t_dbenvcreate test is executed before t_dbplatsec<N>/t_dbplatsecperf tests!
// Please, ensure that t_dbenvdestroy test is executed after t_dbplatsec<N>/t_dbplatsecperf tests!
// 
//

#include <e32test.h>
#include <d32dbms.h>
#include "t_dbplatsecutl.h"

const TUid KSecureDbUid = {0x11335579};
_LIT(KSecure,	"SECURE");
_LIT(KDbName,	"C:TestDB.DB");
_LIT(KTblNameA,	"A");
_LIT(KTblNameB,	"B");
_LIT(KTblNameC,	"C");

static RTest 				TheTest(_L("t_dbplatsec5: DBMS platform security testing - 5"));
static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;
static RDbTable 			TheTbl;
static RDbView 				TheView;

TDBSCUtils 	TheDbscUtils(TheTest, NULL);

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0018
@SYMTestCaseDesc Open table test.
				 This test app has no capabilities and it is restricted to be able to
				 open tables B and C in read-only mode.
@SYMTestPriority High
@SYMTestActions  Open table test.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void Test1L()
	{
	TheTest.Printf(_L("An attempt to open tables in update/insert mode\n"));
	//The test must fail, because the test app cannot satisfy table A, B, C, policy W.
	TInt err = TheTbl.Open(TheDb, KTblNameA);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameC);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to open tables in read-only mode\n"));
	//The test must pass for table B & C, but the test app cannot satisfy table A, policy R.
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();

	TheTest.Printf(_L("An attempt to read tables\n"));
	//The test must pass for table B & C, but the test app cannot satisfy table A, policy R.
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM A")));
	TEST2(err, KErrPermissionDenied);
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM B")));
	TEST2(err, KErrNone);
	TInt cnt = TheView.CountL();
	TEST(cnt > 0);
	TheView.Close();
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM C")));
	TEST2(err, KErrNone);
	cnt = TheView.CountL();
	TEST(cnt > 0);
	TheView.Close();
	}

/**
@SYMTestCaseID			SYSLIB-DBMS-CT-3407
@SYMTestCaseDesc		Test for defect DEF103023 - DBMS requires ReadDeviceData and WriteDeviceData capability to read from the db.
 						The current test application has no capabilities at all.
 						"C:TestDB.DB" database is a secure shared database with:
 							- no "READ" polycy (no restrictions apply to the database read operations);
 							- "WRITE" policy with "WriteUserData" capability defined;
 							- "SCHEMA" policy with "NetworkServices" capability defined;
 							- table C has no defined securoty policy, so the database security policy will be used;
 						The current test application should be able to:
 							- begin/commit/rollback a "read-only" transaction;
 						But should fail if:
 							- begin a transaction and try to modify the database within the transaction;
 						This test function asserts the test cases described above.
@SYMTestPriority		High
@SYMTestActions			Test for defect DEF103023 - DBMS requires ReadDeviceData and WriteDeviceData capability to read from the db.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF103023
*/
void DEF103023L()
	{
	TheTest.Printf(_L("Begin a transaction. Read-only operations tested\n"));
	TInt err = TheDb.Begin();
	TEST2(err, KErrNone);
	TheTest.Printf(_L("Perform some read-only operations inside the transaction\n"));
	err = TheView.Prepare(TheDb, _L("SELECT * FROM C"));
	TEST2(err, KErrNone);
	err = TheView.EvaluateAll();
	TEST2(err, KErrNone);
	TInt cnt = TheView.CountL();
	TEST(cnt > 0);
	TBool rc = TheView.FirstL();
	TEST(rc);
	TheView.GetL();
	TInt val = TheView.ColInt32(1);
	rc = TheView.LastL();
	TEST(rc);
	rc = TheView.NextL();
	TEST(!rc);
	rc = TheView.PreviousL();
	TEST(rc);
	TheView.BeginningL();
	TheView.EndL();
	TheView.Close();
	TheTest.Printf(_L("Commit a transaction\n"));
	err = TheDb.Commit();
	TEST2(err, KErrNone);
	//
	TheTest.Printf(_L("Begin a transaction. Read-only operations tested\n"));
	err = TheDb.Begin();
	TEST2(err, KErrNone);
	err = TheView.Prepare(TheDb, _L("SELECT * FROM C"));
	TEST2(err, KErrNone);
	err = TheView.EvaluateAll();
	TEST2(err, KErrNone);
	cnt = TheView.CountL();
	TEST(cnt > 0);
	TheView.Close();
	TheTest.Printf(_L("Rollback a transaction\n"));
	TheDb.Rollback();
	//
	TheTest.Printf(_L("Begin a transaction. Tested operations violate the database security\n"));
	err = TheDb.Begin();
	TEST2(err, KErrNone);
	err = TheView.Prepare(TheDb, _L("SELECT * FROM C"));
	TEST2(err, KErrNone);
	err = TheView.EvaluateAll();
	TEST2(err, KErrNone);
	rc = TheView.FirstL();
	TEST(rc);
	TheView.GetL();
	TheTest.Printf(_L("An attempt to update a record within the transaction\n"));
	TRAP(err, TheView.UpdateL());
	TEST2(err, KErrPermissionDenied);
	TheTest.Printf(_L("An attempt to delete a record within the transaction\n"));
	TRAP(err, TheView.DeleteL());
	TEST2(err, KErrPermissionDenied);
	TheTest.Printf(_L("An attempt to insert a record within the transaction\n"));
	TRAP(err, TheView.InsertL());
	TEST2(err, KErrPermissionDenied);
	TheView.Close();
	TheTest.Printf(_L("An attempt to modify the database schema within the transaction\n"));
	err = TheDb.Execute(_L("CREATE TABLE C2(Id INTEGER, Z INTEGER)"));
	TEST2(err, KErrPermissionDenied);
	TheTest.Printf(_L("An attempt to execute an INSERT statement within the transaction\n"));
	err = TheDb.Execute(_L("INSERT INTO C VALUES(100)"));
	TEST2(err, KErrPermissionDenied);
	TheTest.Printf(_L("An attempt to modify the database within the transaction using RDbUpdate\n"));
	RDbUpdate update;
	err = update.Execute(TheDb, _L("INSERT INTO C VALUES(200)"));
	TEST2(err, KErrPermissionDenied);
	update.Close();
	TheTest.Printf(_L("Rollback a transaction\n"));
	TheDb.Rollback();
	}

void DoTestL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0018 An app with \"None\" capabilities set "));
	Test1L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-3407 DEF103023-DBMS requires ReadDeviceData and WriteDeviceData capability to read from the db "));
	DEF103023L();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	TBuf<32> format;
	TheTest.Printf(_L("Open database\n"));
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	err = TheDb.Open(TheDbs, KDbName, format);
	TEST2(err, KErrNone);

	TRAP(err, DoTestL());
	TEST2(err, KErrNone);

	TheView.Close();
	TheTbl.Close();
	TheDb.Close();
	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
