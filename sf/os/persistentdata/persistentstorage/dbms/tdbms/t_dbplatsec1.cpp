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
// This test app has "NetworkServices" (SCHEMA) capability, which allows it to
// modify the DBMS structure but not to write any data in the tables.
// The UID policy file is 11335579.spd.
// Please, ensure that t_dbenvcreate test is executed before t_dbplatsec<N>/t_dbplatsecperf tests!
// Please, ensure that t_dbenvdestroy test is executed after t_dbplatsec<N>/t_dbplatsecperf tests!
// 
//

#include <e32test.h>
#include <d32dbms.h>
#include "t_dbplatsecutl.h"

const TUid KSecureDbUid = {0x11335579};
const TUid KProtSecureDbUid = {0x11335578};
_LIT(KSecure,	"SECURE");
_LIT(KDbName,	"C:TestDB2.dB");
_LIT(KTblNameA,	"A");
_LIT(KTblNameB,	"B");
_LIT(KTblNameC,	"C");

static RTest 				TheTest(_L("t_dbplatsec1: DBMS platform security testing - 1"));
static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;
static RDbTable 			TheTbl;
static RDbView 				TheView;

static void CleanupTest()
	{
	TheView.Close();
	TheTbl.Close();
	TheDb.Close();
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDbName);
	TheDbs.Close();
	}

TDBSCUtils 	TheDbscUtils(TheTest, &CleanupTest);

static TColDef const KColumns[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
	{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
	{0}
	};

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0008
@SYMTestCaseDesc Database tests. Some of the calls must fail because the caller has no enough rights
                 for the requested operation.
@SYMTestPriority High
@SYMTestActions  RDbNamedDatabase::Open()/RDbNamedDatabase::DatabaseNamesL()/RDbNamedDatabase::Create().
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DbTestL()
	{
	TBuf<32> format;

	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDbName);
	TheTest.Printf(_L("Create database\n"));
	//The test must pass, because the test app has "SCHEMA" capability
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	TInt err = TheDb.Create(TheDbs, KDbName, format);
	TEST2(err, KErrNone);

	//The test must pass, because "DatabaseNamesL" is a DBMS operation available for everyone.
	TheTest.Printf(_L("Database list\n"));
	CDbDatabaseNames* dbNames = TheDbs.DatabaseNamesL(EDriveC, KSecureDbUid);
	TEST(dbNames->Count() > 0);
	TBool casePreserved = EFalse;
	for(TInt i=0;i<dbNames->Count();++i)
		{
		const TDesC& dbName = (*dbNames)[i];
		RDebug::Print(_L("--Database: %S\n"), &dbName);
		TBuf<128> dbName2;
		dbName2.Append(TChar('A' + EDriveC));
		dbName2.Append(TChar(':'));
		dbName2.Append(dbName);
		if(dbName2 == KDbName())
			{
			casePreserved = ETrue;
			}
		}
	//if casePreserved is non-zero that means the DBMS server does not change the database names to
	//upper or lower case - that's what we want to check
	TEST(casePreserved);
	delete dbNames;

	TheDb.Close();

	TheTest.Printf(_L("An attempt to create database - existing, but protected UID\n"));
	//The test must fail, because the test app does not have capabilities to satisfy
	//KProtSecureDbUid "SCHEMA" policy.
	format.Copy(KSecure);
	format.Append(KProtSecureDbUid.Name());
	err = TheDb.Create(TheDbs, KDbName, format);
	TEST2(err, KErrPermissionDenied);

	//The test must pass, because the test app has "SCHEMA" capability (it must have capabilities,
	//satisfying at least one of the UID's R/W/S policies)
	TheTest.Printf(_L("Open database\n"));
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	err = TheDb.Open(TheDbs, KDbName, format);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0009
@SYMTestCaseDesc Opening table test. The caller has a set of capabilities which satisfy database's
                 schema security policy only. The test checks that the capapbility checking
				 on the DBMS server side works properly. Some of the initiated open table
				 operations won't executed and the returned error will be KErrPermisssionDenied.
@SYMTestPriority High
@SYMTestActions  Attempts to execute RDbTable::Open() on different tables from the test database.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblOpenL()
	{
	TheTest.Printf(_L("Create tables\n"));
	//The test must pass, because the test app has "SCHEMA" capability
	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	TInt err = TheDb.CreateTable(KTblNameA, *colset);//R: PowerMgmt, W: WriteUserData
	TEST2(err, KErrNone);
	err = TheDb.CreateTable(KTblNameB, *colset);//R: None, W: WriteUserData WriteDeviceData
	TEST2(err, KErrNone);
	err = TheDb.CreateTable(KTblNameC, *colset);//R: None, W: WriteUserData
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(colset);

	TheTest.Printf(_L("An attempt to open table A\n"));
	//The test must fail, because the test app has no capabilities to satisfy
	//R/W policies of table A
	err = TheTbl.Open(TheDb, KTblNameA);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EUpdatable);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EInsertOnly);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to open table B\n"));
	//Open table B in insert/update mode - the test must fail, because the test app has no
	//capabilities to satisfy table B, policy W.
	//Open table B in read-only mode - the test must pass, because table B has no R policy.
	err = TheTbl.Open(TheDb, KTblNameB);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EUpdatable);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EInsertOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();

	TheTest.Printf(_L("An attempt to open table C\n"));
	//Open table C in insert/update mode - the test must fail, because the test app has no
	//capabilities to satisfy table C, policy W.
	//Open table C in read-only mode - the test must pass, because table C has no R policy.
	err = TheTbl.Open(TheDb, KTblNameC);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EUpdatable);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EInsertOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0010
@SYMTestCaseDesc Table R/w operations. The caller has a set of capabilities which satisfy database's
                 schema security policy only. The test checks that the capapbility checking
				 on the DBMS server side works properly. Some of the R/W table operations won't be
				 executed and the returned error will be KErrPermisssionDenied.
@SYMTestPriority High
@SYMTestActions  Attempts to execute RDbTable::Insert()/RDbTable::Update()/RDbTable::FirstL()
                 on different tables from the test database.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblRWL()
	{
	TheTest.Printf(_L("An attempt to write in table B\n"));
	TInt err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	//"Write table B" test must fail, because the test app has no capabilities
	//to satisfy table B, policy W.
	TRAP(err, TheTbl.InsertL());
	TEST2(err, KErrPermissionDenied);
	TRAP(err, TheTbl.UpdateL());
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to read from table B\n"));
	//"Read table B" test must pass, because table B has no R policy
	TBool res = TheTbl.FirstL();
	TEST(!res);

	TheTbl.Close();

	TheTest.Printf(_L("An attempt to write in table C\n"));
	//"Write table C" test must fail, because the test app has no capabilities
	//to satisfy table C, policy W.
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TRAP(err, TheTbl.InsertL());
	TEST2(err, KErrPermissionDenied);
	TRAP(err, TheTbl.UpdateL());
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to read from table C\n"));
	//"Read table C" test must pass, because table C has no R policy
	res = TheTbl.FirstL();
	TEST(!res);

	TheTbl.Close();
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0011
@SYMTestCaseDesc SQL tests. The caller has a set of capabilities which satisfy database's
                 schema security policy only. The test checks that the capapbility checking
				 on the DBMS server side works properly. Some of the SQL statements won't be
				 executed and the returned error will be KErrPermisssionDenied.
@SYMTestPriority High
@SYMTestActions  Attempts to execute various INSERT/UPDATE/SELECT SQL statements.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblSqlL()
	{
	TheTest.Printf(_L("SELECT SQL\n"));
	//The test must fail, because the test app cannot satisfy table A, policy R.
	TInt err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM A")));
	TEST2(err, KErrPermissionDenied);
	//The test must pass, because table B has no R policy.
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM B")));
	TEST2(err, KErrNone);
	TheView.Close();
	//The test must pass, because table C has no R policy.
	err = TheView.Prepare(TheDb, TDbQuery(_L("SELECT * FROM C")));
	TEST2(err, KErrNone);
	TheView.Close();

	TheTest.Printf(_L("INSERT/UPDATE SQL\n"));
	//The test must fail, because the test app cannot satisfy table A, policy W.
	err = TheDb.Execute(_L("INSERT INTO A (DATA2) VALUES(45)"));
	TEST2(err, KErrPermissionDenied);
	//The test must fail, because the test app cannot satisfy table B, policy W.
	err = TheDb.Execute(_L("INSERT INTO B (DATA2) VALUES(45)"));
	TEST2(err, KErrPermissionDenied);
	//The test must fail, because the test app cannot satisfy table C, policy W.
	err = TheDb.Execute(_L("INSERT INTO C (DATA2) VALUES(45)"));
	TEST2(err, KErrPermissionDenied);

	//The test must fail, because the test app cannot satisfy table A, policy W.
	err = TheDb.Execute(_L("UPDATE A SET DATA2=56 WHERE ID = 0"));
	TEST2(err, KErrPermissionDenied);
	//The test must fail, because the test app cannot satisfy table B, policy W.
	err = TheDb.Execute(_L("UPDATE B SET DATA2=56 WHERE ID = 0"));
	TEST2(err, KErrPermissionDenied);
	//The test must fail, because the test app cannot satisfy table C, policy W.
	err = TheDb.Execute(_L("UPDATE C SET DATA2=56 WHERE ID = 0"));
	TEST2(err, KErrPermissionDenied);
	}

static void DoRunL()
	{
	TheTest.Start(_L("An app with \"SCHEMA\" capabilities set"));

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0008 Database test "));
	::DbTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0009 Open table test "));
	::TblOpenL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0010 Read/Write table test "));
	::TblRWL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0011 SQL table test "));
	::TblSqlL();
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

	::CleanupTest();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
