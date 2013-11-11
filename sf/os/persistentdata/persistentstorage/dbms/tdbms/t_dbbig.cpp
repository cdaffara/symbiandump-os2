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

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <hal.h>

class TTimer
	{
public:
	void Start(const TDesC& aDes);
	void Stop();
private:
	TUint iTicks;
	};

LOCAL_D RTest test(_L("t_dbbig - Test Large DBMS objects"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CFileStore* TheStore;
LOCAL_D RDbStoreDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDir=_L("C:\\DBMS-TST\\");
const TPtrC KTestFile=_L("T_BIG.DB");
const TPtrC KTableName(_S("table"));
const TPtrC KIndexText=_S("text");
const TPtrC KIndexInt=_S("int");
const TPtrC KColumnText=_S("text");
const TPtrC KColumnInt=_S("int");
const TPtrC KIncFormat=_S("%5d\r");
const TInt KRecords=1000;
const TPtrC KOtherTable=_S("extra");

static TTimer TheTimer;

void TTimer::Start(const TDesC& aDes)
	{
	test.Printf(_L("  %S: "),&aDes);
	iTicks=User::FastCounter();
	}

void TTimer::Stop()
	{
	TUint ticks = User::FastCounter() - iTicks;
	TInt freq = 0;
	test(HAL::Get(HAL::EFastCounterFrequency, freq) == KErrNone);
	const TInt KMicroSecIn1Sec = 1000000;
	const TInt KMsIn1Sec = 1000;
	double v = ((double)ticks * KMicroSecIn1Sec) / (double)freq; TInt v2 = (TInt)v;
	test.Printf(_L("%d ms\r\n"),v2/KMsIn1Sec);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1309
@SYMTestCaseDesc        Create the database-in-a-store
@SYMTestPriority        Medium
@SYMTestActions        	Calls up RDbStoreDatabase::CreateL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDatabaseL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1309 "));
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,KTestFile,EFileRead|EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId id;
		id=TheDatabase.CreateL(store);
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::Pop();
	TheStore=store;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1310
@SYMTestCaseDesc        Open the database-in-a-store
@SYMTestPriority        Medium
@SYMTestActions        	Call up RDbStoreDatabase::OpenL()
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void OpenDatabaseL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1310 "));
	CFileStore* store=CPermanentFileStore::OpenLC(TheFs,KTestFile,EFileRead|EFileWrite);
		TheDatabase.OpenL(store,store->Root());
	CleanupStack::Pop();
	TheStore=store;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1311
@SYMTestCaseDesc        Close the database in store
@SYMTestPriority        Medium
@SYMTestActions        	Test for RDbStoreDatabase::Close() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void CloseDatabase()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1311 "));
	TheDatabase.Close();
	delete TheStore;
	}

LOCAL_C void CreateTableL()
	{
	CDbColSet *cs=CDbColSet::NewLC();
	TDbCol col1(KColumnInt,EDbColInt32);
	col1.iAttributes=TDbCol::ENotNull;
	cs->AddL(col1);
	TDbCol col2(KColumnText,EDbColText,200/sizeof(TText));
	col2.iAttributes=TDbCol::ENotNull;
	cs->AddL(col2);
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void WriteRecordsL(TInt aCount)
	{
	TBuf<10> text;
	TInt jj=0;
	for (TInt ii=0;ii<aCount;++ii)
		{
		TheTable.InsertL();
		jj=(jj+23);
		if (jj>=aCount)
			jj-=aCount;
		TheTable.SetColL(1,jj);
		text.Num(jj);
		TheTable.SetColL(2,text);
		TheTable.PutL();
		}
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1312
@SYMTestCaseDesc        Create a table in database
@SYMTestPriority        Medium
@SYMTestActions        	Build a table and write records into the table.Test for commiting the transactions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void BuildTableL(TInt aCount)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1312 "));
	TheTimer.Start(_L("build"));
	CreateTableL();
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	WriteRecordsL(aCount);
	test(TheDatabase.Commit()==KErrNone);
	TheTable.Close();
	TheTimer.Stop();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1313
@SYMTestCaseDesc        Tests for total rows in the rowset
@SYMTestPriority        Medium
@SYMTestActions        	Iterate through the table.Test for the total numbers of rows available
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void IterateL(RDbTable::TPosition aDirection)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1313 "));
	TheTimer.Start(_L("iterate"));
	TInt cc=0;
	while (TheTable.GotoL(aDirection))
		{
		++cc;
		TheTable.GetL();
		}
	TheTimer.Stop();
	test(cc=TheTable.CountL());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0580
@SYMTestCaseDesc        Tests the database definition and enquiry functions
@SYMTestPriority        Medium
@SYMTestActions        	Tests by setting an active index for the table.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestIndexL(const TDesC& aName,const CDbKey& aKey)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0580 "));
	TheTimer.Start(_L("build"));
	test(TheDatabase.CreateIndex(aName,KTableName,aKey)==KErrNone);
	TheTimer.Stop();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	test(TheTable.SetIndex(aName)==KErrNone);
	IterateL(TheTable.ENext);
	TheTable.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0581
@SYMTestCaseDesc        Tests the database definition and enquiry functions
@SYMTestPriority        Medium
@SYMTestActions        	Tests for bookmark which saves the current location of a rowset.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestBookmarkL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0581 creating alien bookmark "));
	CDbColSet* cs=CDbColSet::NewLC();
	TDbCol col(_L("column"),EDbColUint8);
	col.iAttributes=TDbCol::ENotNull+TDbCol::EAutoIncrement;
	cs->AddL(col);
	test (TheDatabase.CreateTable(KOtherTable,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	RDbTable extra;
	test (extra.Open(TheDatabase,KOtherTable)==KErrNone);
	extra.InsertL();
	extra.PutL();
	TDbBookmark alien=extra.Bookmark();
	extra.Close();
//
	test.Next(_L("Alien bookmark"));
	test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TRAPD(r,TheTable.GotoL(alien));
	test (r==KErrNotFound);
	test (TheTable.SetIndex(KIndexInt)==KErrNone);
	TRAP(r,TheTable.GotoL(alien));
	test (r==KErrNotFound);
	test (TheTable.SetIndex(KIndexText)==KErrNone);
	TRAP(r,TheTable.GotoL(alien));
	test (r==KErrNotFound);
//
	test.Next(_L("Cross-view bookmarks"));
	TheTable.LastL();	// indexed view
	TheTable.PreviousL();
	TDbBookmark mark=TheTable.Bookmark();
	test (extra.Open(TheDatabase,KTableName)==KErrNone);
	TRAP(r,extra.GotoL(mark));
	test (r==KErrNone);
	test (extra.PreviousL());
	TRAP(r,TheTable.GotoL(extra.Bookmark()));
	test (r==KErrNone);
	extra.Close();
//
	test.Next(_L("Bookmark persistence"));
	TheTable.Close();
	test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TRAP(r,TheTable.GotoL(mark));
	test (r==KErrNone);
	TheTable.Close();
//
	test.Next(_L("Delete alien record"));
	test (extra.Open(TheDatabase,KOtherTable)==KErrNone);
	TRAP(r, extra.GotoL(mark));
	test (r==KErrNotFound);
	TRAP(r,extra.GotoL(alien));
	test (r==KErrNone);
	extra.DeleteL();
	TRAP(r,extra.GotoL(alien));
	test (r==KErrNotFound);
	extra.Close();
//
	test.Next(_L("Delete extra table"));
	test (TheDatabase.DropTable(KOtherTable)==KErrNone);
	test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TRAP(r,TheTable.GotoL(alien));
	test (r==KErrNotFound);
	TheTable.Close();
//
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1314
@SYMTestCaseDesc        Discarding indexes belonging to the table on database
@SYMTestPriority        Medium
@SYMTestActions        	Tests for RDbIncremental::DropTable(),RDbIncremental::Next() function.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void BreakIndex()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1314 "));
	TheTimer.Start(_L("break"));
	TInt step;
	RDbIncremental drop;
	test(drop.DropTable(TheDatabase,KTableName,step)==KErrNone);
	test(drop.Next(step)==KErrNone);
	test(step>0);
	drop.Close();	// abort the drop
	test(TheDatabase.IsDamaged());
	TheTimer.Stop();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1315
@SYMTestCaseDesc        Database recovery test
@SYMTestPriority        Medium
@SYMTestActions        	Calls up RDbStoreDatabase::Recover() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Recover()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1315 "));
	TheTimer.Start(_L("recover"));
	test(TheDatabase.Recover()==KErrNone);
	TheTimer.Stop();
	test(!TheDatabase.IsDamaged());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1316
@SYMTestCaseDesc        Tests for dropping an index
@SYMTestPriority        Medium
@SYMTestActions        	Drop an integer and text index from the table. Test for damage of database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DropIndexes()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1316 "));
	TheTimer.Start(_L("drop Int[32]"));
	test(TheDatabase.DropIndex(KIndexInt,KTableName)==KErrNone);
	TheTimer.Stop();
	TheTimer.Start(_L("drop Text[200]"));
	test(TheDatabase.DropIndex(KIndexText,KTableName)==KErrNone);
	TheTimer.Stop();
	test(!TheDatabase.IsDamaged());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1317
@SYMTestCaseDesc        Deleting a table from the database
@SYMTestPriority        Medium
@SYMTestActions        	Delete the rows from the rowset.Check for empty rows in the rowset.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DeleteTableL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1317 "));
	const TInt KTenthRecords=KRecords/10;

	test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheDatabase.Begin();
	TInt ii;
	for (ii=0;ii<15;++ii)
		{
		TheTable.NextL();
		TheTable.DeleteL();
		}
	TheTable.NextL();
	TDbBookmark mark=TheTable.Bookmark();
	TheTable.Close();
	TheDatabase.Commit();
	CloseDatabase();
	OpenDatabaseL();
	TheTimer.Start(_L("delete table"));
	test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheDatabase.Begin();
	TheTable.GotoL(mark);
	TheTable.DeleteL();
	for (ii=0;ii<KTenthRecords*2-16;++ii)
		{
		TheTable.NextL();
		TheTable.DeleteL();
		}
	TheTable.EndL();
	for (ii=0;ii<KTenthRecords*2;++ii)
		{
		TheTable.PreviousL();
		TheTable.DeleteL();
		}
	TheTable.BeginningL();
	for (ii=0;ii<KTenthRecords*3;++ii)
		TheTable.NextL();
	for (ii=0;ii<KTenthRecords*2;++ii)
		{
		TheTable.NextL();
		TheTable.DeleteL();
		}
	for (ii=0;ii<KTenthRecords*2;++ii)
		{
		TheTable.PreviousL();
		TheTable.DeleteL();
		}
	for (ii=0;ii<KTenthRecords;++ii)
		{
		TheTable.NextL();
		TheTable.DeleteL();
		}
	for (ii=0;ii<KTenthRecords;++ii)
		{
		TheTable.PreviousL();
		TheTable.DeleteL();
		}
	test (TheTable.CountL()==0);
	test (!TheTable.NextL());
	test (!TheTable.PreviousL());
	test (TheDatabase.Commit()==KErrNone);
	TheTable.Close();
	TheTimer.Stop();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0579
@SYMTestCaseDesc        Tests the database definition and enquiry functions
@SYMTestPriority        Medium
@SYMTestActions        	Executes the index and bookmark tests
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void BigTestL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0579 Table "));
	CreateDatabaseL();
	BuildTableL(KRecords);
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.EndL();
	IterateL(TheTable.EPrevious);
	TheTable.BeginningL();
	IterateL(TheTable.ENext);
	TheTable.EndL();
	IterateL(TheTable.EPrevious);
	TheTable.Close();
	test.Next(_L("Int32 Index"));
	CDbKey *key=CDbKey::NewLC();
	key->AddL(KColumnInt);
	key->MakeUnique();
	TestIndexL(KIndexInt,*key);
	test.Next(_L("Text[200] Index"));
	key->Clear();
	key->AddL(KColumnText);
	key->MakeUnique();
	TestIndexL(KIndexText,*key);
	test.Next(_L("Bookmarks"));
	TestBookmarkL();
	test.Next(_L("Int32 Index"));
	TheTimer.Start(_L("drop"));
	test(TheDatabase.DropIndex(KIndexInt,KTableName)==KErrNone);
	TheTimer.Stop();
	key->Clear();
	key->AddL(KColumnInt);
	key->MakeUnique();
	TestIndexL(KIndexInt,*key);
	CleanupStack::PopAndDestroy();
	test.Next(_L("Break & Recover"));
	BreakIndex();
	Recover();
	test.Next(_L("Drop Indexes"));
	DropIndexes();
	test.Next(_L("Delete all records"));
	DeleteTableL();
	CloseDatabase();
	test.End();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
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
	__UHEAP_MARK;
//
	test.Start(_L("Standard database"));
	TRAPD(r,BigTestL();)
	test(r==KErrNone);

	// clean up data files used by this test - must be done before call to End() - DEF047652
	_LIT(KTestDbName, "C:\\DBMS-TST\\T_BIG.DB");
	::DeleteDataFile(KTestDbName);

	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;



	TheFs.Close();
	test.Close();
	return 0;
    }
