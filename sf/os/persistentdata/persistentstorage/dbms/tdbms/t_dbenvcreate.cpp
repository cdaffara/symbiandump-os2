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
// DBMS security policy - testing new APIs - preparation operations
// Please, ensure that t_predbsc test is executed before t_dbsc<N> tests!
// 
//

#include <d32dbms.h>
#include <e32test.h>

static RTest TheTest(_L("t_dbenvcreate: DBMS platform security testing - preparation"));
static RDbs TheDbs;
const TUid KSecureDbUid = {0x11335578};
const TUid KSecureDbUid2 = {0x11335579};
const TUid KSecureDbUid3 = {0x1133557A};
_LIT(KProtDbZName, "z:z.db");
_LIT(KProtDbCName, "C:z.Db");
_LIT(KProtDbZName2, "z:TEstDB.dB");
_LIT(KProtDbCName2, "c:teSTDB.db");
_LIT(KProtDbCName3, "c:AbcD.Db");
_LIT(KProtDbCName4, "c:A0123456789B0123456789C0123456789D0123456789E0123456789F0123.Db");
_LIT(KProtDbCName5, "c:ZADFS.Db");
_LIT(KProtDbFormat3, "SECURE[1133557A]");
_LIT(KProtDbCPath, "C:\\Private\\100012a5\\Dbs_11335578_z.dB");
_LIT(KProtDbCPath2, "C:\\Private\\100012a5\\dBS_11335579_TesTDB.db");

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) 				 ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0023
@SYMTestCaseDesc This test app must be executed before all t_dbsc<N> tests.
                 It ensures that the test data is copied and prepared for the tests executed
				 after the current executable.
@SYMTestPriority High
@SYMTestActions  Copy test databases from Z: to C: drive in the DBMS server private data cage.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DoRun()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0023 Copy protected databases from Z: to C: "));
	TheDbs.DeleteDatabase(KProtDbCName, KSecureDbUid);
	TInt err = TheDbs.CopyDatabase(KProtDbZName, KProtDbCName, KSecureDbUid);
	TheTest.Printf(_L("copy file file from z:z.db to C:z.Db uid = 0x11335578 err = %d"),err);
	TEST2(err, KErrNone);
	TheDbs.DeleteDatabase(KProtDbCName2, KSecureDbUid2);
	err = TheDbs.CopyDatabase(KProtDbZName2, KProtDbCName2, KSecureDbUid2);
	TheTest.Printf(_L("copy file file from z:TEstDB.dB to c:teSTDB.db uid = 0x11335579 err = %d"),err);
	TEST2(err, KErrNone);

	TheDbs.DeleteDatabase(KProtDbCName3, KSecureDbUid3);
	TheTest.Next(_L("Create protected database on C:"));
	RDbNamedDatabase db;
	err = db.Create(TheDbs, KProtDbCName3, KProtDbFormat3);
	TEST2(err, KErrNone);
	db.Close();

	TheDbs.DeleteDatabase(KProtDbCName4, KSecureDbUid3);
	err = db.Create(TheDbs, KProtDbCName4, KProtDbFormat3);
	TEST2(err, KErrNone);
	db.Close();

	TheDbs.DeleteDatabase(KProtDbCName5, KSecureDbUid3);
	err = db.Create(TheDbs, KProtDbCName5, KProtDbFormat3);
	TEST2(err, KErrNone);
	db.Close();

	RFs fileSess;
	err = fileSess.Connect();
	TEST2(err, KErrNone);
	err = fileSess.SetAtt(KProtDbCPath,  0, KEntryAttReadOnly);
	TEST2(err, KErrNone);
	err = fileSess.SetAtt(KProtDbCPath2,  0, KEntryAttReadOnly);
	TEST2(err, KErrNone);
	fileSess.Close();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TheTest.Title();

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	::DoRun();

	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
