// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

LOCAL_D RTest test(_L("t_dbwindow: Testing the 'window' stage"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CFileStore* TheStore;
LOCAL_D RDbStoreDatabase TheDatabase;
LOCAL_D RDbView TheView;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDir=_S("C:\\DBMS-TST\\");
const TPtrC KTestFile=_S("T_WINDOW.DB");
const TPtrC KTableName=_S("TestTable");
const TPtrC KColumnID=_S("id");
const TPtrC KIndexName=_S("TestIndex");
const TInt KRecords=10;
const TPtrC KCreateTable=_L("CREATE TABLE TestTable (id INTEGER NOT NULL)");
const TPtrC KCreateIndex=_L("CREATE UNIQUE INDEX TestIndex ON TestTable (id)");
const TPtrC KEmptyQuery=_L("select * from TestTable where id is null");
const TPtrC KOrderQuery=_L("select * from TestTable order by id");
const TDbWindow KSingleSlotWindow(0,0);
const TDbWindow KSmallWindow(2,2);
const TDbWindow KLargeWindow(20,0);

//
// Create the database-in-a-store
//
LOCAL_C void CreateDatabaseL()
	{
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,KTestFile,EFileRead|EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId id;
		id=TheDatabase.CreateL(store);
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::Pop();
	TheStore=store;
	}

LOCAL_C void CloseDatabaseL()
	{
	// cannot evaluate size info for store database
	RDbDatabase::TSize size=TheDatabase.Size();
	test (size.iSize<0);
	test (size.iUsage<0);
	test (TheDatabase.UpdateStats()==KErrNone);
	size=TheDatabase.Size();
	test (size.iSize<0);
	test (size.iUsage<0);
	TheDatabase.Close();
	delete TheStore;
	}

LOCAL_C void CreateTableL()
	{
	TheDatabase.Begin();
	test(TheDatabase.Execute(KCreateTable)==KErrNone);
	RDbTable table;
	test(table.Open(TheDatabase,KTableName,table.EInsertOnly)==KErrNone);
	for (TInt ii=0;ii<KRecords;++ii)
		{
		table.InsertL();
		table.SetColL(1,ii);
		table.PutL();
		}
	table.Close();
	test(TheDatabase.Execute(KCreateIndex)==KErrNone);
	test (TheDatabase.Commit()==KErrNone);
	}

LOCAL_C void InitL()
	{
	CreateDatabaseL();
	CreateTableL();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0638
@SYMTestCaseDesc        Tests for navigation
@SYMTestPriority        Medium
@SYMTestActions         Tests for navigation in an empty window
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEmptyL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0638 Unevaluated "));
	test(TheView.Prepare(TheDatabase,KEmptyQuery,TDbWindow::EUnlimited)==KErrNone);
	test(TheView.CountL(TheView.EQuick)==0);
	test(!TheView.FirstL());
	test(TheView.AtEnd());
	test(!TheView.PreviousL());
	test(TheView.AtBeginning());
	test(!TheView.LastL());
	test(TheView.AtBeginning());
	test(!TheView.NextL());
	test(TheView.AtEnd());
	test.Next(_L("Evaluated"));
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==0);
	test(!TheView.FirstL());
	test(TheView.AtEnd());
	test(!TheView.PreviousL());
	test(TheView.AtBeginning());
	test(!TheView.LastL());
	test(TheView.AtBeginning());
	test(!TheView.NextL());
	test(TheView.AtEnd());
	test.End();
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0639
@SYMTestCaseDesc        Tests for navigation
@SYMTestPriority        Medium
@SYMTestActions         Tests for the unlimited (dynaset) window
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestUnlimitedL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0639 EvaluateAll "));
	test(TheView.Prepare(TheDatabase,KOrderQuery,TDbWindow::EUnlimited)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	test(!TheView.Unevaluated());
	test(TheView.CountL()==KRecords);
	TInt cc=0;
	for (TheView.BeginningL();TheView.NextL();)
		++cc;
	test(cc==KRecords);
	for (TheView.EndL();TheView.PreviousL();)
		--cc;
	test(cc==0);
	test(!TheView.Unevaluated());
	test.Next(_L("Incremental evaluate"));
	TheView.Reset();
	cc=TheView.CountL(TheView.EQuick);
	test(cc==0);
	test(!TheView.FirstL());
	while (TheView.Unevaluated())
		{
		test(TheView.Evaluate()>=KErrNone);
		TInt ii=TheView.CountL()-cc;
		test(ii>=0);
		cc+=ii;
		while (--ii>=0)
			{
			test(TheView.AtRow());
			TheView.NextL();
			}
		test(!TheView.AtRow());
		}
	test(cc==KRecords);
	test.Next(_L("Insert"));
// The new records should go on the end of the set
	TheView.InsertL();
	TheView.SetColL(1,TInt(-1));
	TheView.PutL();
	test(TheView.CountL()==KRecords+1);
	test(TheView.LastL());
	TheView.GetL();
	test(TheView.ColInt(1)==-1);
// because of order, it should appear at beginning on re-evaluation
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==KRecords+1);
	test(TheView.FirstL());
	TheView.GetL();
	test(TheView.ColInt(1)==-1);
	test.Next(_L("Update"));
// updated records should not move (or disappear) in the set
	TheView.UpdateL();
	TheView.SetColL(1,KRecords);
	TheView.PutL();
	test(TheView.CountL()==KRecords+1);
	test(TheView.FirstL());
	TheView.GetL();
	test(TheView.ColInt(1)==KRecords);
// because of order, it should appear at end on re-evaluation
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==KRecords+1);
	test(TheView.LastL());
	TheView.GetL();
	test(TheView.ColInt(1)==KRecords);
	test.Next(_L("Bookmarks"));
	TDbBookmark mark=TheView.Bookmark();
	TheView.BeginningL();
	TRAPD(r,TheView.GotoL(mark));
	test(r==KErrNone);
	TheView.GetL();
	test(TheView.ColInt(1)==KRecords);
	test(!TheView.NextL());
	test(TheView.PreviousL());
	test.Next(_L("Delete"));
// should disappear from the set
	TheView.DeleteL();
	test(TheView.CountL()==KRecords);
	for (TheView.BeginningL();TheView.NextL();)
		{
		TheView.GetL();
		test(TheView.ColInt(1)!=KRecords);
		}
	TRAP(r,TheView.GotoL(mark));
	test(r!=KErrNone);
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==KRecords);
	for (TheView.BeginningL();TheView.NextL();)
		{
		TheView.GetL();
		test(TheView.ColInt(1)!=KRecords);
		}
	TRAP(r,TheView.GotoL(mark));
	test(r!=KErrNone);
	test.End();
	TheView.Close();
	}

//
// do as much incremental evaluation as possible
//
LOCAL_C void Evaluate()
	{
	test(TheView.EvaluateAll()==KErrNone);
	}

//
// get to the true end of set
//
LOCAL_C void GotoEndL()
	{
	for (;;)
		{
		while (TheView.AtRow())
			TheView.NextL();
		if (!TheView.Unevaluated())
			break;
		Evaluate();
		}
	}

LOCAL_C void CheckWindowL(TInt forward,TInt back)
	{
	TInt ii;
	for (ii=0;ii<forward;++ii)
		test(TheView.NextL());
	test(!TheView.NextL());
	for (ii=0;ii<forward+back+1;++ii)
		test(TheView.PreviousL());
	test(!TheView.PreviousL());
	for (ii=0;ii<back+1;++ii)
		test(TheView.NextL());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0640
@SYMTestCaseDesc        Tests for RDbView
@SYMTestPriority        Medium
@SYMTestActions         Tests for a restricted sized window
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRestrictedL()
	{
	test.Start(_L("	@SYMTestCaseID:SYSLIB-DBMS-CT-0640 Behaviour at start of set "));
	test(TheView.Prepare(TheDatabase,KOrderQuery,KSingleSlotWindow)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==1);
	test(!TheView.Unevaluated());
	test(TheView.NextL());
	test(!TheView.Unevaluated());
	TheView.GetL();
	TInt id=TheView.ColInt(1);
	test(!TheView.NextL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtRow());
	test(!TheView.PreviousL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtRow());
	TheView.GetL();
	test(TheView.ColInt(1)==id);
	test(!TheView.PreviousL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtBeginning());
	test(TheView.NextL());
	TheView.GetL();
	test(TheView.ColInt(1)==id);
	test(!TheView.Unevaluated());
	test.Next(_L("Behaviour at end of set"));
	GotoEndL();
	test(TheView.LastL());
	test(!TheView.Unevaluated());
	TheView.GetL();
	id=TheView.ColInt(1);
	test(!TheView.PreviousL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtRow());
	test(!TheView.NextL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtRow());
	TheView.GetL();
	test(TheView.ColInt(1)==id);
	test(!TheView.NextL());
	test(TheView.Unevaluated());
	Evaluate();
	test(TheView.CountL()==1);
	test(TheView.AtEnd());
	test(TheView.PreviousL());
	TheView.GetL();
	test(TheView.ColInt(1)==id);
	test(!TheView.Unevaluated());
	TheView.Close();
//
	test.Next(_L("Forward and backwards slots"));
	test(TheView.Prepare(TheDatabase,KOrderQuery,KSmallWindow)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.CountL()==5);
	test(TheView.FirstL());
	CheckWindowL(4,0);
	test(TheView.NextL());
	test(TheView.NextL());		// now on preferred slot (id=2)
	test(!TheView.Unevaluated());
	test(TheView.NextL());
	test(TheView.Unevaluated());
	Evaluate();
	CheckWindowL(2,2);
	test(TheView.LastL());		// id=5
	Evaluate();
	test(TheView.LastL());		// id=7
	Evaluate();					// should now have last five rows
	CheckWindowL(2,2);
	test(TheView.LastL());		// id=9
	Evaluate();					// no more rows
	CheckWindowL(0,4);			// all behind us now
	TheView.GetL();
	test(TheView.ColInt(1)==9);
	TheView.FirstL();			// id=5
	Evaluate();
	TheView.FirstL();			// id=3
	Evaluate();
	TheView.FirstL();			// id=1
	Evaluate();
	CheckWindowL(3,1);
//
	test.Next(_L("Bookmarks"));
	TDbBookmark mark=TheView.Bookmark();
	test(TheView.NextL());
	test(TheView.NextL());
	test(TheView.NextL());
	Evaluate();
	TRAPD(r,TheView.GotoL(mark));
	test(r!=KErrNone);
	TheView.FirstL();
	Evaluate();
	TRAP(r,TheView.GotoL(mark));
	test(r==KErrNone);
	test.Next(_L("Delete"));
	test(TheView.FirstL());
	TheView.GetL();
	test(TheView.ColInt(1)==0);
	TheView.DeleteL();
	test(TheView.CountL()==4);
	TheView.FirstL();
	CheckWindowL(3,0);
	test.Next(_L("Insert"));
	TheView.InsertL();
	TheView.SetColL(1,TInt(0));
	TheView.PutL();
	test(TheView.CountL()==4);
	TheView.FirstL();
	TheView.GetL();
	test(TheView.ColInt(1)==1);
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
	test(TheView.FirstL());
	TheView.GetL();
	test(TheView.ColInt(1)==0);
//
	test.End();
	TheView.Close();
	}

LOCAL_C void TestL()
	{
	__UHEAP_MARK;
	test.Start(_L("Setting up test table"));
	TRAPD(r,InitL();)
	test(r==KErrNone);
	test.Next(_L("Empty Window"));
	TRAP(r,TestEmptyL();)
	test(r==KErrNone);
	test.Next(_L("Unlimited Window"));
	TRAP(r,TestUnlimitedL();)
	test(r==KErrNone);
	test.Next(_L("Sized window"));
	TRAP(r,TestRestrictedL();)
	test(r==KErrNone);
	test.Next(_L("Cleanup"));
	TRAP(r,CloseDatabaseL();)
	test(r==KErrNone);
	test.End();
	__UHEAP_MARKEND;
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	r=TheFs.MkDir(KTestDir);
	test(r==KErrNone || r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	test(r==KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
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
	setupTestDirectory();
	setupCleanup();
//
	test.Start(_L("TDbWindow - database test"));
	TRAPD(err, TestL());
	test(err == KErrNone);

	//deletion of data files must be done before call to end - DEF047652
	_LIT(KTestDbName, "C:\\DBMS-TST\\T_WINDOW.DB");
	::DeleteDataFile(KTestDbName);
	test.End();
//

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
    }
