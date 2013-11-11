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
// DBMS security policy - performance tests
// Please, ensure that t_dbenvcreate test is executed before t_dbplatsec<N>/t_dbplatsecperf tests!
// Please, ensure that t_dbenvdestroy test is executed after t_dbplatsec<N>/t_dbplatsecperf tests!
// 
//

#include <e32test.h>
#include "t_dbplatsecutl.h"

static RTest TheTest(_L("t_dbplatsecperf: DBMS platform security - Performance test"));

static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;

const TUid KSecureDbUid = {0x11335579};
_LIT(KSecure,	"SECURE");
_LIT(KDbNameC,	"C:TestDB.DB");
_LIT(KTblNameA,	"A");
_LIT(KTblNameB,	"B");
_LIT(KTblNameC,	"C");
_LIT(KDbDirNSC,	"C:\\DBMS-TST\\");
_LIT(KDbNameNSC,"C:\\DBMS-TST\\TestDB.DB");

static void DeleteNsDb()
	{
	RFs fileSess;
	if(fileSess.Connect() == KErrNone)
		{
		fileSess.Delete(KDbNameNSC);
		}
	fileSess.Close();
	}

static void CleanupTest()
	{
	::DeleteNsDb();
	}

TDBSCUtils 	TheDbscUtils(TheTest, &CleanupTest);

const TInt KGetPolicyCallCnt = 10000;
const TInt KInsertRecCnt = 10000;

//Measures the performance of RDbs::GetDatabasePolicy() and RDbs::GetTablePolicy() calls.
//The DBMS server session kept alive during the test.
static void GetPolicyTest1()
	{
	TUint time = User::TickCount();
	RDbs::TPolicyType tblPolicyType[2] = {RDbs::EReadPolicy, RDbs::EWritePolicy};
	TSecurityPolicy dbPolicy;
	TSecurityPolicy tblPolicy;
	TInt i, k;
	for(i=0,k=0;i<KGetPolicyCallCnt;++i,k=(k+1)%2)
		{
		TInt err = TheDbs.GetDatabasePolicy(KSecureDbUid, tblPolicyType[k], dbPolicy);
		TEST2(err, KErrNone);
		err = TheDbs.GetTablePolicy(KSecureDbUid, KTblNameA, tblPolicyType[k], tblPolicy);
		TEST2(err, KErrNone);
		}
	time = User::TickCount() - time;
	TheTest.Printf(_L("GetDatabasePolicy(), GetTablePolicy(). Time=%d\r\n"), time);
	}

//Measures the performance of RDbs::GetTablePolicies() call.
//The DBMS server session connected before each GetTablePolicies() call and disconnected
//after the call.
static void GetPolicyTest2()
	{
	TheDbs.Close();
	TUint time = User::TickCount();
	RDbs::TPolicyType tblPolicyType[2] = {RDbs::EReadPolicy, RDbs::EWritePolicy};
	TSecurityPolicy dbPolicy;
	TSecurityPolicy tblPolicy;
	TInt i, k;
	for(i=0,k=0;i<KGetPolicyCallCnt;++i,k=(k+1)%2)
		{
		TInt err = TheDbs.Connect();
		TEST2(err, KErrNone);
		err = TheDbs.GetTablePolicies(KSecureDbUid, KTblNameA, tblPolicyType[k], dbPolicy, tblPolicy);
		TEST2(err, KErrNone);
		TheDbs.Close();
		}
	time = User::TickCount() - time;
	TheTest.Printf(_L("GetTablePolicies(). Time=%d\r\n"), time);
	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);
	}

//Measures the performance of RDbs::GetTablePolicies() call.
//The DBMS server session kept alive during the test.
static void GetPolicyTest3()
	{
	TUint time = User::TickCount();
	RDbs::TPolicyType tblPolicyType[2] = {RDbs::EReadPolicy, RDbs::EWritePolicy};
	TSecurityPolicy dbPolicy;
	TSecurityPolicy tblPolicy;
	TInt i, k;
	for(i=0,k=0;i<KGetPolicyCallCnt;++i,k=(k+1)%2)
		{
		TInt err = TheDbs.GetTablePolicies(KSecureDbUid, KTblNameA, tblPolicyType[k], dbPolicy, tblPolicy);
		TEST2(err, KErrNone);
		}
	time = User::TickCount() - time;
	TheTest.Printf(_L("GetTablePolicies(). Time=%d\r\n"), time);
	}

static void CreateNonsecureDbL()
	{
	TColDef const KColumns[]=
		{
		{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
		{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
		{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
		{0}
		};
	RFs fileSess;
	CleanupClosePushL(fileSess);
	TInt err = fileSess.Connect();
	TEST2(err, KErrNone);

	err = fileSess.MkDir(KDbDirNSC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	RDbNamedDatabase db;
	err = db.Create(fileSess, KDbNameNSC);
	TEST2(err, KErrNone);
	CleanupClosePushL(db);
	err = db.Open(fileSess, KDbNameNSC);

	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	err = db.CreateTable(KTblNameA, *colset);
	TEST2(err, KErrNone);
	err = db.CreateTable(KTblNameB, *colset);
	TEST2(err, KErrNone);
	err = db.CreateTable(KTblNameC, *colset);
	TEST2(err, KErrNone);

	CleanupStack::PopAndDestroy(colset);
	CleanupStack::PopAndDestroy(&db);
	CleanupStack::PopAndDestroy(&fileSess);
	}

static void DoInsertRecL(RDbNamedDatabase& aDb, RDbTable& aTbl)
	{
	TUint time = User::TickCount();
	TInt err = aDb.Begin();
	TEST2(err, KErrNone);
	for(TInt i=0;i<KInsertRecCnt;++i)
		{
		aTbl.InsertL();
		aTbl.SetColL(2, i);
		aTbl.SetColL(3, i);
		aTbl.PutL();
		}
	err = aDb.Commit();
	TEST2(err, KErrNone);
	time = User::TickCount() - time;
	TheTest.Printf(_L("Insert. Time=%d\r\n"), time);
	}

//Measures the performance of "Insert" operations in a non-secure database.
static void InsertRec1L()
	{
	RDbNamedDatabase db;
	CleanupClosePushL(db);
	TInt err = db.Open(TheDbs, KDbNameNSC);
	TEST2(err, KErrNone);

	RDbTable tblA;
	CleanupClosePushL(tblA);
	err = tblA.Open(db, KTblNameA);
	TEST2(err, KErrNone);

	::DoInsertRecL(db, tblA);

	CleanupStack::PopAndDestroy(&tblA);
	CleanupStack::PopAndDestroy(&db);
	}

//Measures the performance of "Insert" operations in a secure shared database.
static void InsertRec2L()
	{
	TBuf<32> format;
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());

	CleanupClosePushL(TheDb);
	TInt err = TheDb.Open(TheDbs, KDbNameC, format);
	TEST2(err, KErrNone);

	RDbTable tblA;
	CleanupClosePushL(tblA);
	err = tblA.Open(TheDb, KTblNameA);
	TEST2(err, KErrNone);

	::DoInsertRecL(TheDb, tblA);

	CleanupStack::PopAndDestroy(&tblA);
	CleanupStack::PopAndDestroy(&TheDb);
	}

//Measures the performance of "Update" SQL operations in a non-secure database.
static void UpdateRec1L()
	{
	RDbNamedDatabase db;
	CleanupClosePushL(db);
	TInt err = db.Open(TheDbs, KDbNameNSC);
	TEST2(err, KErrNone);

	TUint time = User::TickCount();

	err = db.Begin();
	TEST2(err, KErrNone);

	TInt cnt = db.Execute(_L("UPDATE A SET DATA1=10, DATA2=20 WHERE ID >= 0"));
	TEST(cnt > 0);

	err = db.Commit();
	TEST2(err, KErrNone);

	time = User::TickCount() - time;
	TheTest.Printf(_L("Update. Time=%d\r\n"), time);

	CleanupStack::PopAndDestroy(&db);
	}

//Measures the performance of "Update" SQL operations in a secure shared database.
static void UpdateRec2L()
	{
	TBuf<32> format;
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());

	CleanupClosePushL(TheDb);
	TInt err = TheDb.Open(TheDbs, KDbNameC, format);
	TEST2(err, KErrNone);

	TUint time = User::TickCount();

	err = TheDb.Begin();
	TEST2(err, KErrNone);

	TInt cnt = TheDb.Execute(_L("UPDATE A SET DATA1=10, DATA2=20 WHERE ID >= 0"));
	TEST(cnt > 0);

	err = TheDb.Commit();
	TEST2(err, KErrNone);

	time = User::TickCount() - time;
	TheTest.Printf(_L("Update. Time=%d\r\n"), time);

	CleanupStack::PopAndDestroy(&TheDb);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0020
@SYMTestCaseDesc DBMS security - performance tests. Insert/Update and Get<AAA>Policy
                 operations performance measured for non-secure and secure shared database.
@SYMTestPriority High
@SYMTestActions  RDbs::GetDatabasePolicy(), Rdbs::GetTablePolicy() and RDbs::GetTablePolicies()
				 calls measured.
                 RDBTable: insert operation measured, SQL: update operation measured.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DoRunL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0020 GetPolicy performance test "));
	::GetPolicyTest1();
	::GetPolicyTest2();
	::GetPolicyTest3();

	::CreateNonsecureDbL();

	TheTest.Next(_L("Insert records performance test"));
	InsertRec1L();
	InsertRec2L();

	TheTest.Next(_L("Update records performance test"));
	::UpdateRec1L();
	::UpdateRec2L();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	::DeleteNsDb();

	TRAP(err, ::DoRunL());
	TEST2(err, KErrNone);

	TheDb.Close();
	TheDbs.Close();
	::CleanupTest();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
