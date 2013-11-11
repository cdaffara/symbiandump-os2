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
// This test app has "WriteDeviceData" (TABLE B: WRITE) capability, although it does not allows
// writing data to table B, because (UID: WRITE) policy is not satisfied.
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

static RTest 				TheTest(_L("t_dbplatsec4: DBMS platform security testing - 4"));
static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;
static RDbTable 			TheTbl;
static RDbView 				TheView;

TDBSCUtils 	TheDbscUtils(TheTest, NULL);

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0017
@SYMTestCaseDesc Open table test.
				 This test app has "WriteDeviceData" (TABLE B: WRITE) capability,
				 although it does not allows writing data to table B, because
				 (UID: WRITE) policy is not satisfied. The attempts to open database
				 tables in insert/update mode must fail.
@SYMTestPriority High
@SYMTestActions  Open table test.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void TblTestL()
	{
	TheTest.Printf(_L("An attempt to open table A\n"));
	//The test must fail, because the test app cannot satisfy table A, policy W.
	TInt err = TheTbl.Open(TheDb, KTblNameA);
	TEST2(err, KErrPermissionDenied);
	//The test must fail, because the test app cannot satisfy table A, policy R.
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to open table B\n"));
	//The test must fail, because the test app cannot satisfy table B, policy W.
	err = TheTbl.Open(TheDb, KTblNameB);
	TEST2(err, KErrPermissionDenied);
	//The test must pass, because table B has no R policy.
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();

	TheTest.Printf(_L("An attempt to write to table B\n"));
	//The test must fail, because the test app cannot satisfy table B, policy W.
	err = TheDb.Execute(_L("UPDATE B SET DATA1 = 400 WHERE ID < 10"));
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("An attempt to open table C\n"));
	//The test must fail, because the test app cannot satisfy table C, policy W.
	err = TheTbl.Open(TheDb, KTblNameC);
	TEST2(err, KErrPermissionDenied);
	//The test must pass, because table C has no R policy.
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	}

static void DoRunL()
	{
	TheTest.Start(_L("An app with \"TABLE B:WRITE\" capabilities set"));

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0017 Tble tests "));
	::TblTestL();
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
