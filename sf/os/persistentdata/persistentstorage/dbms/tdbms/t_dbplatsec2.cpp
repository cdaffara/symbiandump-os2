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
// This test app has "WriteUserData" (UID: WRITE) capability, which allows it to
// write data in some of the tables.
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
_LIT(KTblNameCC,"CC");

static RTest 				TheTest(_L("t_dbplatsec2: DBMS platform security testing - 2"));
static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;
static RDbTable 			TheTbl;
static RDbView 				TheView;

TDBSCUtils 	TheDbscUtils(TheTest, NULL);

static TColDef const KColumns[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
	{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
	{0}
	};

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0012
@SYMTestCaseDesc Database operations test.
				 This test app has "WriteUserData" (UID: WRITE) capability, which allows it to
				 write data in some of the tables. Almost all database calls must fail, the caller
				 cannot satisfy the database's schema security policy.
@SYMTestPriority High
@SYMTestActions  Database calls.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DbTestL()
	{
	TheTest.Printf(_L("An attempt to delete the database\n"));
	TInt err = TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDbName);
	TEST2(err, KErrPermissionDenied);

	TBuf<32> format;

	TheTest.Printf(_L("An attempt to create the database\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	err = TheDb.Create(TheDbs, KDbName, format);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("Open database\n"));
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	err = TheDb.Open(TheDbs, KDbName, format);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("An attempt to create a table\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	err = TheDb.CreateTable(KTblNameCC, *colset);
	TEST2(err, KErrPermissionDenied);

	CleanupStack::PopAndDestroy(colset);

	TheTest.Printf(_L("An attempt to create a table using SQL\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	err = TheDb.Execute(_L("create table AAAA (id counter)"));
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to alter a table using SQL\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	err = TheDb.Execute(_L("alter table A add DATA11 CHAR(20)"));
	TEST2(err, KErrPermissionDenied);

	_LIT(KColName, "DATA2");
	TheTest.Printf(_L("An attempt to create an index\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	CDbKey* key = TDBSCUtils::CreateKeyLC(KColName);
	err = TheDb.CreateIndex(KColName, KTblNameA, *key);
	TEST2(err, KErrPermissionDenied);
	CleanupStack::PopAndDestroy(key);

	TheTest.Printf(_L("An attempt to drop an index\n"));
	//The test must fail, because the test app cannot satisfy KSecureDbUid uid, policy S.
	err = TheDb.DropIndex(KColName, KTblNameA);
	TEST2(err, KErrPermissionDenied);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0013
@SYMTestCaseDesc Open table test.
				 This test app has "WriteUserData" (UID: WRITE) capability, which allows it to
				 write data in some of the tables. Some of the calls must fail because the caller has no
				 enough rights for the requested operation (for example - the attempts to open some
				 of the tables in ready-only mode)
@SYMTestPriority High
@SYMTestActions  RDBTable::Open() called fon different tables from the test database.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblOpenL()
	{
	TheTest.Printf(_L("An attempt to open table A\n"));
	//The test must pass, because the test app can satisfy table A, policy W.
	TInt err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EUpdatable);
	TEST2(err, KErrNone);
	TheTbl.Close();
	//The test must pass, because the test app can satisfy table A, policy W.
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EInsertOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	//The test must fail, because the test app cannot satisfy table A, policy R.
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to open table B\n"));
    if(PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) &&
       PlatSec::IsCapabilityEnforced(ECapabilityWriteDeviceData))
        {
	    //The test must fail, because the test app cannot satisfy table B, policy W.
	    err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EUpdatable);
	    TEST2(err, KErrPermissionDenied);
	    //The test must fail, because the test app cannot satisfy table B, policy W.
	    err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EInsertOnly);
	    TEST2(err, KErrPermissionDenied);
        }
	//The test must pass, because the test app can satisfy table B, policy R.
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();

	TheTest.Printf(_L("An attempt to open table C\n"));
	//The test must pass, because the test app can satisfy table C, policy W.
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EUpdatable);
	TEST2(err, KErrNone);
	TheTbl.Close();
	//The test must pass, because the test app can satisfy table C, policy W.
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EInsertOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	//The test must pass, because the test app can satisfy table C, policy R.
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0014
@SYMTestCaseDesc R/W operations at a table level.
				 This test app has "WriteUserData" (UID: WRITE) capability, which allows it to
				 write data in some of the tables. Some of the calls must fail because the caller has no
				 enough rights for the requested operation.
@SYMTestPriority High
@SYMTestActions  R/W operations at a table level.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblRWL()
	{
	TheTest.Printf(_L("Table A - Write\n"));
	TInt err = TheTbl.Open(TheDb, KTblNameA);
	TEST2(err, KErrNone);
	//The test must pass, because the test app can satisfy table A, policy W.
	TRAP(err, TheTbl.InsertL());
	TEST2(err, KErrNone);
	TheTbl.SetColL(2, 100);
	TheTbl.SetColL(3, 200);
	TRAP(err, TheTbl.PutL());
	TEST2(err, KErrNone);
	TInt cnt = TheDb.Execute(_L("UPDATE A SET DATA1 = 400 WHERE ID < 10"));
	TEST(cnt > 0);

	TheTest.Printf(_L("Table A - Read\n"));
	//The test must fail, because the test app cannot satisfy table A, policy R.
	TBool res = EFalse;
	TRAP(err, res = TheTbl.FirstL());
	TEST2(err, KErrPermissionDenied);
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM A")));
	TEST2(err, KErrPermissionDenied);
	TheView.Close();

	TheTbl.Close();

	TheTest.Printf(_L("Table B - Write\n"));
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	if(PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) &&
	   PlatSec::IsCapabilityEnforced(ECapabilityWriteDeviceData))
		{
		//The test must fail, because the test app cannot satisfy table B, policy W.
		TRAP(err, TheTbl.InsertL());
		TEST2(err, KErrPermissionDenied);
		err = TheDb.Execute(_L("INSERT INTO B (DATA2) VALUES (45)"));
		TEST2(err, KErrPermissionDenied);
		}

	TheTest.Printf(_L("Table B - Read\n"));
	//The test must pass, because table B has no R policy.
	TRAP(err, res = TheTbl.FirstL());
	TEST2(err, KErrNone);
	TEST(res);
	cnt = TheTbl.CountL();
	TEST(cnt > 0);
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM B")));
	TEST2(err, KErrNone);
	cnt = TheView.CountL();
	TEST(cnt > 0);
	TheView.Close();

	TheTbl.Close();

	TheTest.Printf(_L("Table C - Write\n"));
	err = TheTbl.Open(TheDb, KTblNameC);
	TEST2(err, KErrNone);
	//The test must pass, because the test app can satisfy table C, policy W.
	TRAP(err, TheTbl.InsertL());
	TEST2(err, KErrNone);
	TheTbl.SetColL(2, 100);
	TheTbl.SetColL(3, 200);
	TRAP(err, TheTbl.PutL());
	TEST2(err, KErrNone);
	cnt = TheDb.Execute(_L("UPDATE C SET DATA1 = 400 WHERE ID < 10"));
	TEST(cnt > 0);

	TheTest.Printf(_L("Table C - Read\n"));
	//The test must pass, because table C has no R policy.
	TRAP(err, res = TheTbl.FirstL());
	TEST2(err, KErrNone);
	TEST(res);
	cnt = TheTbl.CountL();
	TEST(cnt > 0);
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM C")));
	TEST2(err, KErrNone);
	cnt = TheView.CountL();
	TEST(cnt > 0);
	TheView.Close();

	TheTbl.Close();
	}

static void DoRunL()
	{
	TheTest.Start(_L("An app with \"UID:WRITE\" capabilities set"));

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0012 Database tests "));
	::DbTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0013 Open table tests "));
	::TblOpenL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0014 Table R/W tests "));
	::TblRWL();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	TRAP(err, ::DoRunL());
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
