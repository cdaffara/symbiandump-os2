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
// Secure shared databases failures test
// 
//

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include "t_dbfail.h"

LOCAL_D TBuf<64> TheFormat2;
_LIT(KTestDatabase2,"C:t_fail.db");
_LIT(KTestDatabase3,"C:t_fail2.db");
const TUid KPolicyUid = {0x11335579};

GLDEF_C void PrepareDbFmtString()
	{
	TheFormat2.Copy(_L("SECURE"));
	TheFormat2.Append(KPolicyUid.Name());
	}

LOCAL_C void DbCreate2L()
	{
	User::LeaveIfError(TheDatabase.Create(TheDbs, KTestDatabase2, TheFormat2));
	RDebug::Print(_L("Secure shared database created.\n"));
	}

LOCAL_C void DbDatabaseNamesL()
	{
	CDbDatabaseNames* dbNames = TheDbs.DatabaseNamesL(EDriveC, KPolicyUid);
	test(dbNames != NULL);
	TInt cnt = dbNames->Count();
	TInt i;
	for(i=0;i<cnt;++i)
		{
		const TDesC& name = (*dbNames)[i];
		RDebug::Print(_L("%02d. %S\n"), i + 1, &name);
		}
	delete dbNames;
	RDebug::Print(_L("Secure shared database - DatabaseNamesL().\n"));
	}

LOCAL_C void DbCopyDatabaseL()
	{
	User::LeaveIfError(TheDbs.CopyDatabase(KTestDatabase2, KTestDatabase3, KPolicyUid));
	RDebug::Print(_L("Secure shared database - CopyDatabase().\n"));
	}

LOCAL_C void DbDeleteDatabaseL()
	{
	User::LeaveIfError(TheDbs.DeleteDatabase(KTestDatabase2, KPolicyUid));
	RDebug::Print(_L("Secure shared database - DeleteDatabase().\n"));
	}

LOCAL_C void DbOpen2L()
	{
	User::LeaveIfError(TheDatabase.Open(TheDbs, KTestDatabase2, TheFormat2));
	CleanupClosePushL(TheDatabase);
	delete TheDatabase.TableNamesL();	// force a schema load
	CleanupStack::Pop();
	RDebug::Print(_L("Secure shared database - Open().\n"));
	}

GLDEF_C void TestOpen2()
	{
	_LIT(KFileNotFound2, "C:not a database");
	Connect();
	TInt r=TheDatabase.Open(TheDbs, KFileNotFound2, TheFormat2);
	Disconnect();
	test (r==KErrPathNotFound || r==KErrNotFound);//There is no secure shared database with name KFileNotFound2
//
	_LIT(KPathNotFound2, "Z:ddd.db");
	Connect();
	r=TheDatabase.Open(TheDbs, KPathNotFound2, TheFormat2);
	Disconnect();
	test (r==KErrPathNotFound || r==KErrNotFound);//There is no secure shared database with name KPathNotFound2
//
	_LIT(KDbName, "C:dddqweuqwe.db");
	_LIT(KDbInvalidFmt, "SECURE");
	Connect();
	r=TheDatabase.Open(TheDbs, KDbName, KDbInvalidFmt);
	Disconnect();
	test (r==KErrArgument);//The format string is invalid, if there is "SECURE" keyword, then a uid has to follw it.
//
	_LIT(KSecureSharedDbName, "C:xfgbdrgui.db");
	_LIT(KDbNonSecureValidFmt, "[123456768]");
	Connect();
	r=TheDatabase.Open(TheDbs, KSecureSharedDbName, KDbNonSecureValidFmt);
	Disconnect();
	test (r==KErrNotFound);//Non-secure, non-existent database
	}

class TFailCreateDatabase2 : public TFail
	{
	void RunL()
		{
		DbCreate2L();
		}
	void End()
		{
		TheDatabase.Close();
		(void)TheDbs.DeleteDatabase(KTestDatabase2, KPolicyUid);
		}
	};

class TFailOpenDatabase2 : public TFail
	{
	void RunL()
		{
		DbOpen2L();
		}
	void End()
		{
		TheDatabase.Close();
		}
	};

class TFailDatabaseNames : public TFail
	{
	void RunL()
		{
		DbDatabaseNamesL();
		}
	void End()
		{
		}
	};

class TFailCopyDatabase : public TFail
	{
	void RunL()
		{
		DbCopyDatabaseL();
		}
	void End()
		{
		(void)TheDbs.DeleteDatabase(KTestDatabase3, KPolicyUid);
		}
	};

class TFailDeleteDatabase : public TFail
	{
	void RunL()
		{
		DbDeleteDatabaseL();
		}
	void End()
		{
		}
	};

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0022
@SYMTestCaseDesc OOM test for the new "DBMS security" APIs.
@SYMTestPriority High
@SYMTestActions  Calling the new "DBMS security" APIs under OOM conditions.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
GLDEF_C void Origins2()
   	{
	Connect();
	(void)TheDbs.DeleteDatabase(KTestDatabase2, KPolicyUid);
	Disconnect();

	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0022 Allocation failures on creating a secure shared database "));
	TFailCreateDatabase2 t1;
	Connect();
	(void)TheDbs.DeleteDatabase(KTestDatabase2, KPolicyUid);
	t1.Test(KClientHeap);
	t1.Test(KServerHeap);
	Disconnect();

	Connect();
	test(TheDatabase.Create(TheDbs, KTestDatabase2, TheFormat2) == KErrNone);
	(void)TheDatabase.Close();
	Disconnect();

	Connect();

	test.Next(_L("Allocation failures on opening a secure shared database"));
	TFailOpenDatabase2 t2;
	t2.Test(KClientHeap);
	t2.Test(KServerHeap);

	test.Next(_L("Allocation failures on getting secure shared databases list"));
	TFailDatabaseNames t_dbNames;
	t_dbNames.Test(KClientHeap);
	t_dbNames.Test(KServerHeap);
	TheDatabase.Close();

	test.Next(_L("Allocation failures on copying secure shared databases"));
	TFailCopyDatabase t_copyDb;
	t_copyDb.Test(KClientHeap);
	t_copyDb.Test(KServerHeap);

	test.Next(_L("Allocation failures on deleting secure shared databases"));
	TFailDeleteDatabase t_deleteDb;
	t_deleteDb.Test(KClientHeap);
	test(TheDatabase.Create(TheDbs, KTestDatabase2, TheFormat2) == KErrNone);
	(void)TheDatabase.Close();
	t_deleteDb.Test(KServerHeap);

	Disconnect();

   	test.End();
   	}
   
