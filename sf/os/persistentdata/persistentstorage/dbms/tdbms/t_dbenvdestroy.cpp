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
// DBMS security policy - testing new APIs - cleanup operations
// Please, ensure that t_postdbsc test is executed after t_dbsc<N> tests!
// 
//

#include <d32dbms.h>
#include <e32test.h>

static RTest TheTest(_L("t_dbenvdestroy: DBMS platform security tetsing - cleanup"));
static RDbs TheDbs;
const TUid KSecureDbUid = {0x11335578};
const TUid KSecureDbUid2 = {0x11335579};
const TUid KSecureDbUid3 = {0x1133557A};
_LIT(KProtDb1CName, "c:A.DB");
_LIT(KProtDb2CName, "c:b.DB");
_LIT(KProtDb3CName, "c:C.DB");
_LIT(KProtDb4CName, "c:z.DB");
_LIT(KProtDb5CName, "C:TEstdB.dB");
_LIT(KProtDb6CName, "C:AbcD.db");
_LIT(KProtDb7CName, "c:A0123456789B0123456789C0123456789D0123456789E0123456789F0123.Db");
_LIT(KProtDb8CName, "c:ZADFS.Db");

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static void DeleteTestDatabase(TUid aUid, const TDesC& aDbName)
	{
	RDebug::Print(_L("Deleting %X \"%S\" database.\n"), aUid, &aDbName);
	TInt err = TheDbs.DeleteDatabase(aDbName, aUid);
	if(err != KErrNone && err != KErrNotFound)
		{
		RDebug::Print(_L("Error %d deleting \"%S\" database.\n"), err, &aDbName);
		}
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0024
@SYMTestCaseDesc This test app must be executed after all t_dbsc<N> tests.
                 It ensures that the test data removd from drive C:.
@SYMTestPriority High
@SYMTestActions  Remove used test databases from drive C:.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DoRun()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0024 Delete protected databases from C: "));
	::DeleteTestDatabase(KSecureDbUid2, KProtDb5CName);
	::DeleteTestDatabase(KSecureDbUid, KProtDb4CName);
	::DeleteTestDatabase(KSecureDbUid, KProtDb3CName);
	::DeleteTestDatabase(KSecureDbUid, KProtDb2CName);
	::DeleteTestDatabase(KSecureDbUid, KProtDb1CName);
	::DeleteTestDatabase(KSecureDbUid3, KProtDb6CName);
	::DeleteTestDatabase(KSecureDbUid3, KProtDb7CName);
	::DeleteTestDatabase(KSecureDbUid3, KProtDb8CName);
	}

TInt E32Main()
    {
	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	::DoRun();

	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	return KErrNone;
    }
