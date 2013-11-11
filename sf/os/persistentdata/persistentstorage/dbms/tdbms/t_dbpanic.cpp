// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <s32mem.h>

LOCAL_D RTest test(_L("t_dbpanic - Panic test when cancelling two blobs transactions"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbTable TheTables[2];
LOCAL_D RFs TheFs;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;

const TPtrC KTestDatabase=_L("\\DBMS-TST\\T_PANIC.DB");

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0641
@SYMTestCaseDesc        Tests for creating the database and tables
@SYMTestPriority        Medium
@SYMTestActions         Tests for creating the tables.Leave on error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void PreTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0641 "));
	// Create the database:
	User::LeaveIfError(TheDatabase.Replace(TheFs,KTestDatabase));
	CleanupClosePushL(TheDatabase);

	// Create tables in the database:
	User::LeaveIfError(TheDatabase.Execute(_L("create table ta (a integer, b Long Varbinary)")));
	User::LeaveIfError(TheDatabase.Execute(_L("create table tb (a integer, b Long Varbinary)")));

	// Open the tables:
	User::LeaveIfError(TheTables[0].Open(TheDatabase, _L("ta")));
	CleanupClosePushL(TheTables[0]);
	User::LeaveIfError(TheTables[1].Open(TheDatabase, _L("tb")));
	CleanupClosePushL(TheTables[1]);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0642
@SYMTestCaseDesc        Tests for transaction of large data
@SYMTestPriority        Medium
@SYMTestActions         Tests for streaming of blob data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0642 "));
	// Start a transaction:
	TheDatabase.Begin();

	// Create a new row on each table:
	TheTables[0].InsertL();
	TheTables[1].InsertL();

	for(TInt i = 0; i < 2; ++i)
		{
		// Setting to null sets the dirty flag:
		TheTables[i].SetColNullL(1);

		// Create a blob of data:
		_LIT8(blobdata, "abcdefghijklmnopqrstuvwxyz");
		CBufFlat * blobbuff = CBufFlat::NewL(32);
		CleanupStack::PushL(blobbuff);
		blobbuff->InsertL(0, blobdata());

		// Open a read stream on the blob:
		RBufReadStream blobstream;
		blobstream.Open(*blobbuff, 0);
		CleanupClosePushL(blobstream);

		// Open a write stream on the table:
		RDbColWriteStream blobwrite;
		blobwrite.OpenLC(TheTables[i], 2);

		// Stream data from the read stream to the write stream:
		blobwrite.WriteL(blobstream);
		blobwrite.CommitL();

		// Close the write stream:
		CleanupStack::PopAndDestroy();
		// Close the read stream:
		CleanupStack::PopAndDestroy();
		// Delete the blob of data:
		CleanupStack::PopAndDestroy(blobbuff);
		}

	TheTables[0].Cancel();
	TheTables[1].Cancel(); //This call to cancel panics.


	// End the transaction:
	TheDatabase.Commit();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0643
@SYMTestCaseDesc        Tests for closing of tables
@SYMTestPriority        Medium
@SYMTestActions         Tests for closing of tables and database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void PostTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0643 "));
	// Close the tables:
	TheTables[0].Close();
	CleanupStack::Pop(); // Table close
	TheTables[1].Close();
	CleanupStack::Pop(); // Table close

	// Close the database:
	CleanupStack::PopAndDestroy();
	}

void doTestL()
	{
	test.Start(_L("bang"));

	// Open a connection to the DBMS server:
	User::LeaveIfError(TheDbs.Connect());
	CleanupClosePushL(TheDbs);

	PreTestL();
	TestL();
	PostTestL();

	CleanupStack::PopAndDestroy(); // TheDbs close
	}

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

//
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	TheTrapCleanup=CTrapCleanup::New();

	TInt r=TheFs.Connect();
	test(r==KErrNone);
	r=TheFs.MkDir(KTestDatabase);
	test(r==KErrNone || r==KErrAlreadyExists);

	TRAP(r, doTestL());
	test(r == KErrNone);

	::DeleteDataFile(KTestDatabase);

	test.End();

	TheFs.Close();
	delete TheTrapCleanup;
	test.Close();
	return r;
    }
