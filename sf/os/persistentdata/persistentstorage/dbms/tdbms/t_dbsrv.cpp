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
#include <e32math.h>
#include <hal.h>
#include "D32DBAS.H"

LOCAL_D RTest test(_L("t_dbsrv: Test DBMS server"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbView TheView;

const TInt KTestCleanupStack=0x40;
const TPtrC KTestDatabase(_S("c:\\dbms-tst\\share.db"));
const TPtrC KTestFormat(_S(""));
const TPtrC KTable1(_S("Table1"));
const TPtrC KTable2(_S("Table2"));
const TInt KRowCount=1000;
const TUint KExitDelay=6*0x100000;	// ~6 seconds

const TPtrC KCreateTable1=_S("CREATE TABLE Table1 (Id INTEGER,Name CHAR(200),Memo LONG VARCHAR,Memo2 LONG VARBINARY)");
const TPtrC KCreateTable2=_S("CREATE TABLE Table2 (Id INTEGER,Name CHAR(200),Memo LONG VARCHAR)");
const TPtrC KDropTable1=_S("DROP TABLE Table1");
const TPtrC KDropTable2=_S("DROP TABLE Table2");

_LIT(KTestSpdFile1, "c:\\dbms-tst\\11335577.spd");
_LIT(KTestSpdFile2, "c:\\dbms-tst\\11335578.spd");
_LIT(KTestSpdFile3, "c:\\dbms-tst\\11335579.spd");
_LIT(KTestSpdFile4, "c:\\dbms-tst\\1133557A.spd");

const TInt KReturnValueForCompare = 10;
const TInt KReturnValueForFind = 20;

class TTimer
	{
public:
	void Start(const TDesC& aDes);
	void Stop();
private:
	TUint iTicks;
	};

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

void DeleteTestFiles()
	{
	(void)TheFs.Delete(KTestDatabase);
	(void)TheFs.Delete(KTestSpdFile1); 
	(void)TheFs.Delete(KTestSpdFile2); 
	(void)TheFs.Delete(KTestSpdFile3); 
	(void)TheFs.Delete(KTestSpdFile4); 
	}

void Check(TInt aValue,TInt aExpected,TInt aLine)
	{
	if (aValue!=aExpected)
		{
		DeleteTestFiles();
		test.Printf(_L("*** Expected %d: got %d\r\n"),aExpected,aValue);
		test.operator()(EFalse,aLine);
		}
	}
#define test2(a,b) Check(a,b,__LINE__)

LOCAL_C void Connect()
	{
	test2(TheDbs.Connect(),KErrNone);
	TheDbs.ResourceMark();
	}

LOCAL_C void Disconnect()
	{
	TheDbs.ResourceCheck();
	TheDbs.Close();
	}

LOCAL_C void Execute(const TDesC& aSql)
	{
	test2 (TheDatabase.Execute(aSql),KErrNone);
	}

LOCAL_C void WriteRecordsL(TInt aCount)
	{
	TheTimer.Start(_L("writing"));
	TheDatabase.Begin();
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
	test2 (TheDatabase.Commit(),KErrNone);
	TheTimer.Stop();
	}

LOCAL_C void Ipc(TInt aCount)
	{
	TheTimer.Start(_L("ipc"));
	while (--aCount>=0)
		TheDatabase.InTransaction();
	TheTimer.Stop();
	}

LOCAL_C void LoopL(RDbTable::TPosition aDirection)
	{
	TheTimer.Start(_L("loop"));
	TInt cc=0;
	while (TheTable.GotoL(aDirection))
		++cc;
	test2 (cc,TheTable.CountL());
	TheTimer.Stop();
	}

LOCAL_C void IterateL(RDbTable::TPosition aDirection)
	{
	TheTimer.Start(_L("iterate"));
	TInt cc=0;
	while (TheTable.GotoL(aDirection))
		{
		++cc;
		TheTable.GetL();
		TBuf<10> text;
		text.Num(TheTable.ColInt(1));
		test (text==TheTable.ColDes(2));
		}
	test2 (cc,TheTable.CountL());
	TheTimer.Stop();
	}

LOCAL_C void LocateL(const TDesC& aSql)
	{
	test2 (TheView.Prepare(TheDatabase,aSql),KErrNone);
	test2 (TheView.EvaluateAll(),KErrNone);
	TheTimer.Start(_L("locate"));
	test (!TheView.NextL());
	TheTimer.Stop();
	TheView.Close();
	}

LOCAL_C void Locate1L()
	{
	LocateL(_L("select * from Table1 where id is null"));
	}

LOCAL_C void Locate2L()
	{
	LocateL(_L("select * from Table1 where id = -12345"));
	}

LOCAL_C TInt DeleteL()
	{
	TheTimer.Start(_L("deleting"));
	TheDatabase.Begin();
	TInt cc=0;
	TheTable.BeginningL();
	while (TheTable.NextL())
		{
		++cc;
		TheTable.DeleteL();
		}
	test (TheTable.CountL()==0);
	test (TheDatabase.Commit()==KErrNone);
	TheTimer.Stop();
	return cc;
	}

LOCAL_C TInt DeleteSQL()
	{
	TheTimer.Start(_L("deleting [SQL]"));
	TInt cc=TheDatabase.Execute(_L("delete from Table1"));
	test (cc>=KErrNone);
	test (TheTable.CountL()==0);
	TheTimer.Stop();
	return cc;
	}

LOCAL_C void BenchmarkL()
	{
	TInt r=TheTable.Open(TheDatabase,KTable1);
	TheTable.LastL();
	test (r==KErrNone);
	Ipc(KRowCount);
	TheTable.Reset();
	LoopL(TheTable.ENext);
	TheTable.Reset();
	IterateL(TheTable.ENext);
	Locate1L();
	Locate2L();
	TheTable.Close();
	}

const TPtrC KSemaphore(_S("Connect-test"));

LOCAL_C TInt ConnectThread(TAny*)
	{
	RSemaphore s;
	s.OpenGlobal(KSemaphore,EOwnerThread);
	RDbs dbs;
	dbs.Connect();
	s.Signal();
	User::After(0x100000);
	s.Wait();
	dbs.Close();
	s.Signal();
	s.Close();
	return KErrNone;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0605
@SYMTestCaseDesc        Single and multiple database connection tests
@SYMTestPriority        Medium
@SYMTestActions         Setup a DBMS server session Search the server for pattern matching
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestConnect()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0605 Single connection "));
	test (TheDbs.Connect()==KErrNone);
	TheDbs.Close();
	test.Next(_L("Deferred exit"));
	TFindServer find(_L("!DBMS server"));
	TFullName result;
	test2 (find.Next(result),KErrNone);
	test.Next(_L("Multiple connection"));
	Connect();
	RDbs dbs;
	test2 (dbs.Connect(),KErrNone);
	Disconnect();
	dbs.Close();
	test.Next(_L("Deferred exit"));
	find.Find(_L("!DBMS server"));
	test2 (find.Next(result),KErrNone);
	User::After(KExitDelay);
	find.Find(_L("!DBMS server"));
	test2 (find.Next(result),KErrNotFound);
	test.Next(_L("multiple thread connections"));
	RSemaphore s;
	test2 (s.CreateGlobal(KSemaphore,0,EOwnerThread),KErrNone);
	RThread t;
	test2 (t.Create(KSemaphore,ConnectThread,0x2000,0x1000,0x10000,0,EOwnerThread),KErrNone);
	t.Resume();
	TRequestStatus st;
	t.Logon(st);
	t.Close();
	s.Wait();
	Connect();
	s.Signal();
	s.Close();
	User::WaitForRequest(st);
	test2 (TheDbs.ResourceCount(),0);
	Disconnect();
	test.Next(_L("Deferred exit"));
	find.Find(_L("!DBMS server"));
	test2 (find.Next(result),KErrNone);
	User::After(KExitDelay);
	find.Find(_L("!DBMS server"));
	test2 (find.Next(result),KErrNotFound);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0606
@SYMTestCaseDesc        Tests for benchmark
@SYMTestPriority        Medium
@SYMTestActions         Tests for creating database and writing a table.Test for no error conditions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestBenchL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0606 Create database "));
	TInt r=TheDatabase.Replace(TheFs,KTestDatabase,KTestFormat);
	test (r==KErrNone);
	Execute(KCreateTable1);
	test.Next(_L("write table"));
	r=TheTable.Open(TheDatabase,KTable1);
	test (r==KErrNone);
	WriteRecordsL(KRowCount);
	TheTable.Close();
	test.Next(_L("BenchmarkL"));
	TRAPD(errCode, BenchmarkL());
	test(errCode == KErrNone);
	test.Next(_L("Open server-side"));
	TheDatabase.Close();
	Connect();
	r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	test.Next(_L("BenchmarkL"));
	TRAP(errCode, BenchmarkL());
	test(errCode == KErrNone);
	test.Next(_L("Wrap up"));
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0607
@SYMTestCaseDesc        Tests for opening a shared database
@SYMTestPriority        Medium
@SYMTestActions         Tests for database open,column type,write and drop table
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOpenL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0607 Create database "));
	test2 (TheDatabase.Replace(TheFs,KTestDatabase,KTestFormat),KErrNone);
	TheDatabase.Close();
	test.Next(_L("Open as shared"));
	Connect();
	test2 (TheDatabase.Open(TheDbs,KTestDatabase),KErrNone);
	Execute(KCreateTable1);
	test2 (TheTable.Open(TheDatabase,KTable1),KErrNone);
	test2 (TheTable.ColCount(),4);
	test2 (TheTable.ColType(1),EDbColInt32);
	test2 (TheTable.ColType(2),EDbColText);
	test2 (TheTable.ColType(3),EDbColLongText);
	test2 (TheTable.ColType(4),EDbColLongBinary);
	delete TheTable.ColSetL();
	test.Next(_L("write table"));
	WriteRecordsL(KRowCount);
	test.Next(_L("Check table"));
	TheTable.Reset();
	test (TheTable.AtBeginning());
	test (!TheTable.AtRow());
	test (!TheTable.AtEnd());
	test2 (TheTable.CountL(),KRowCount);
	IterateL(TheTable.ENext);
	test (!TheTable.AtBeginning());
	test (!TheTable.AtRow());
	test (TheTable.AtEnd());
	test.Next(_L("Delete rows"));
	test2 (DeleteL(),KRowCount);
	TheTable.Close();
	test.Next(_L("Drop table"));
	Execute(KDropTable1);
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0608
@SYMTestCaseDesc        Tests for database locking
@SYMTestPriority        Medium
@SYMTestActions         Tests for shared,exclusive locks on database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestLocking()
	{
	TInt r;
	RDbNamedDatabase db;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0608 Open databases "));
	Connect();
	test2 (TheDatabase.Open(TheDbs,KTestDatabase),KErrNone);
	test2 (db.Open(TheDbs,KTestDatabase,KTestFormat),KErrNone);
	RDbNotifier ob;
	test2 (ob.Open(TheDatabase),KErrNone);
//
	test.Next(_L("Shared locks"));
	TRequestStatus stat;
	ob.NotifyUnlock(stat);
	test2 (TheDatabase.Begin(),KErrNone);
	test2 (db.Begin(),KErrNone);
	test2 (TheDatabase.Commit(),KErrNone);
	User::After(1);
	test2 (stat.Int(),KRequestPending);		// should not report yet
	test2 (db.Commit(),KErrNone);
	User::WaitForRequest(stat);
	test2 (stat.Int(),ob.EUnlock);
//
	test.Next(_L("Exclusive locks"));
	ob.NotifyUnlock(stat);
	test2 (TheDatabase.Begin(),KErrNone);
	test2 (db.Begin(),KErrNone);
	test2 (TheDatabase.Execute(KCreateTable1),KErrLocked);	// cannot acquire
	test2 (db.Commit(),KErrNone);	// release lock
	test2 (TheDatabase.Execute(KCreateTable1),KErrNone);	// upgrade to X-lock
	test2 (db.Begin(),KErrLocked);	// cannot share
	test2 (TheTable.Open(db,KTable1),KErrNone);
	TRAP(r,TheTable.InsertL());		// cannot lock
	test2 (r,KErrLocked);
	TheTable.Close();
	test2 (db.Execute(KDropTable1),KErrLocked);	// cannot lock
	test2 (TheTable.Open(TheDatabase,KTable1),KErrNone);
	TRAP(r,TheTable.InsertL());		// we own the lock
	test2 (r,KErrNone);
	TheTable.Cancel();
	TheTable.Close();
	test2 (TheDatabase.Execute(KDropTable1),KErrNone);
	test2 (stat.Int(),KRequestPending);	// no NotifyUnlock yet
	test2 (TheDatabase.Commit(),KErrNone);
	User::WaitForRequest(stat);
	test2 (stat.Int(),ob.ECommit);
//
	ob.Close();
	db.Close();
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0609
@SYMTestCaseDesc        Tests for sharing a database
@SYMTestPriority        Medium
@SYMTestActions         Tests for opening a database more than once.
                        Tests the integrity of the table data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestShareL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0609 Open database "));
	Connect();
	TInt r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	Execute(KCreateTable1);
	test.Next(_L("Open it again"));
	RDbNamedDatabase db;
	test (db.Open(TheDbs,KTestDatabase,KTestFormat)==KErrNone);
	test (TheTable.Open(db,KTable1)==KErrNone);
	test2 (TheTable.ColCount(),4);
	test2 (TheTable.ColType(1),EDbColInt32);
	test2 (TheTable.ColType(2),EDbColText);
	test2 (TheTable.ColType(3),EDbColLongText);
	test2 (TheTable.ColType(4),EDbColLongBinary);
	delete TheTable.ColSetL();
	TheTable.Close();
//
	test.Next(_L("write table"));
	test (TheTable.Open(TheDatabase,KTable1)==KErrNone);
	WriteRecordsL(KRowCount);
	TheTable.Close();
	test.Next(_L("Check table"));
	TheDatabase.Close();
	TheDatabase=db;
	test (TheTable.Open(TheDatabase,KTable1)==KErrNone);
	test (TheTable.AtBeginning());
	test (!TheTable.AtRow());
	test (!TheTable.AtEnd());
	test (TheTable.CountL()==KRowCount);
	IterateL(TheTable.ENext);
	test (!TheTable.AtBeginning());
	test (!TheTable.AtRow());
	test (TheTable.AtEnd());
	test.Next(_L("Delete rows"));
	test (DeleteSQL()==KRowCount);
	TheTable.Close();
	test.Next(_L("Drop table"));
	Execute(KDropTable1);
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

LOCAL_C void TestScenario1L()
	{
	test.Start(_L("Take stream-lock on client 2"));
	test (TheView.FirstL());
	TheView.GetL();
	RDbColReadStream rcol;
	rcol.OpenLC(TheView,3);
	test2 (rcol.ReadUint16L(),0);
	test.Next(_L("Take a [shared] transaction-lock on client 1"));
	test2 (TheDatabase.Begin(),KErrNone);
	test.Next(_L("Attempt a write-lock on client 1"));
	TRAPD(r,TheTable.InsertL());
	test2 (r,KErrLocked);
	test2 (rcol.ReadUint16L(),1);
	test.Next(_L("Release locks on client 1 and then 2"));
	TheDatabase.Rollback();
	CleanupStack::PopAndDestroy();	// rcol
	test.End();
	}

LOCAL_C void TestScenario2L()
	{
	test.Start(_L("Take stream-lock on client 2"));
	test (TheView.FirstL());
	TheView.GetL();
	RDbColReadStream rcol;
	rcol.OpenLC(TheView,3);
	test2 (rcol.ReadUint16L(),0);
	test.Next(_L("Take a [shared] transaction-lock on client 1"));
	test2 (TheDatabase.Begin(),KErrNone);
	test.Next(_L("Attempt a write-lock on client 1"));
	TRAPD(r,TheTable.InsertL());
	test2 (r,KErrLocked);
	test2 (rcol.ReadUint16L(),1);
	test.Next(_L("Release client 2 stream-lock"));
	CleanupStack::PopAndDestroy();	// rcol
	test.Next(_L("Take write-lock on client 1"));
	TheTable.InsertL();				// read-lock removed
	test.Next(_L("release client 1 locks"));
	TheTable.Cancel();
	TheDatabase.Rollback();
	test.End();
	}

LOCAL_C void TestScenario3L(RDbDatabase& aClient2)
	{
	test.Start(_L("Take transaction-lock on client 1"));
	test2 (TheDatabase.Begin(),KErrNone);
	test.Next(_L("Take stream-lock on client 2"));
	test (TheView.FirstL());
	TheView.GetL();
	RDbColReadStream rcol;
	rcol.OpenLC(TheView,3);
	test2 (rcol.ReadUint16L(),0);
	test.Next(_L("Release client 1 lock"));
	TheDatabase.Rollback();
	test.Next(_L("Take write-lock on client 2"));
	RDbTable table2;			// need a second cursor (cannot do insert with open streams)
	test2 (table2.Open(aClient2,KTable1,table2.EInsertOnly),KErrNone);
	TRAPD(r,table2.InsertL());
	test2 (r,KErrNone);
	test.Next(_L("Release stream-lock"));
	CleanupStack::PopAndDestroy();
	test.Next(_L("Release write-lock"));
	table2.PutL();
	table2.Close();
	test.Next(_L("check the system is unlocked"));
	test2 (TheDatabase.Begin(),KErrNone);
	test2 (TheDatabase.Commit(),KErrNone);
	test.End();
	}

LOCAL_C void TestScenario4L()
	{
	test.Start(_L("Take transaction-lock on client 1"));
	test2 (TheDatabase.Begin(),KErrNone);
	test.Next(_L("Take stream-lock on client 1"));
	test (TheTable.FirstL());
	TheTable.GetL();
	RDbColReadStream rcol;
	rcol.OpenLC(TheTable,3);
	test2 (rcol.ReadUint16L(),0);
	test.Next(_L("Take write-lock on client 1"));
	RDbTable table2;			// need a second cursor (cannot do insert with open streams)
	test2 (table2.Open(TheDatabase,KTable1,table2.EInsertOnly),KErrNone);
	TRAPD(r,table2.InsertL());
	test2 (r,KErrNone);
	test.Next(_L("Release write-lock"));
	table2.PutL();
	table2.Close();
	test.Next(_L("Release stream-lock"));
	CleanupStack::PopAndDestroy();
	test.Next(_L("release transaction-lock"));
	test2 (TheDatabase.Commit(),KErrNone);
	test.End();
	}

LOCAL_C void TestScenario5L()
	{
	test.Start(_L("Begin compaction on client 1"));
	RDbIncremental inc;
	TInt s;
	test2 (inc.Compact(TheDatabase,s),KErrNone);
	test.Next(_L("Attempt a stream-lock on client 2"));
	test (TheView.FirstL());
	TheView.GetL();
	RDbColReadStream rcol;
	TRAPD(r,rcol.OpenL(TheView,3));
	test (r==KErrLocked);
	test.Next(_L("Attempt a stream-lock on client 1"));
	TRAP(r,rcol.OpenL(TheTable,3));
	test (r==KErrAccessDenied);
	test.Next(_L("Release compaction lock"));
	inc.Close();
	test.End();
	}

LOCAL_C void TestScenario6L()
	{
	test.Start(_L("Begin compaction on client 1 - open and read columns"));
	TheTable.Close();

	RDbIncremental inc;
	TInt s;
	test2 (inc.Compact(TheDatabase,s),KErrNone);

	test.Next(_L("Attempt to open a table on client 1"));
	TInt r=TheTable.Open(TheDatabase,KTable1);
	test (r==KErrNone);

	//read short column data
	TheTable.FirstL();
	TheTable.GetL();
	TheTable.ColInt32(1);

	// We cant write to tables
	TRAP(r,TheTable.InsertL());
	test (r==KErrAccessDenied);

	RDbColReadStream rcol;
	// We can't read long columns
	TheTable.FirstL();
	TheTable.GetL();
	TRAP(r, rcol.OpenL(TheTable,3));
	test2 (r,KErrAccessDenied);

	// can read other columns
	TRAP(r, rcol.OpenL(TheTable,4));
	test2 (r,KErrNone);
	rcol.Close();

	test.Next(_L("Release compaction lock"));
	inc.Close();
	test.End();
	}

LOCAL_C void TestScenario7L()
	{
	test.Start(_L("Open tables on client 1 - Begin compaction on client 1"));

	TheTable.Close();
	test.Next(_L("Attempt to open a table on client 1"));
	TInt r=TheTable.Open(TheDatabase,KTable1);
	test (r==KErrNone);

	test.Next(_L("Begin compaction on client 1"));
	RDbIncremental inc;
	TInt s;
	test2 (inc.Compact(TheDatabase,s),KErrNone);

	//read short column data
	TheTable.FirstL();
	TheTable.GetL();
	TheTable.ColInt32(1);

	// We cant write to tables
	TRAP(r,TheTable.InsertL());
	test (r==KErrAccessDenied);

	RDbColReadStream rcol;
	// cant read 4K text
	TheTable.FirstL();
	TheTable.GetL();
	TRAP(r, rcol.OpenL(TheTable,3));
	test2 (r,KErrAccessDenied);

	// can read 1K blob
	TRAP(r, rcol.OpenL(TheTable,4));
	test2 (r,KErrNone);
	rcol.Close();

	test.Next(_L("Release compaction lock"));
	inc.Close();
	test.End();
	}

LOCAL_C void TestScenario8L()
	{
	test.Start(_L("Take update-lock on client 1"));
	test2 (TheDatabase.Begin(),KErrNone);
	TheTable.InsertL();
	test.Next(_L("Read blob without locking on client 2"));
	test (TheView.FirstL());
	TheView.GetL();
	RDbColReadStream rcol;
	rcol.OpenLC(TheView,4);			// 1K blob
	test2 (rcol.ReadInt32L(),0);
	CleanupStack::PopAndDestroy();
	TRAPD(r,rcol.OpenL(TheView,3));			// 4K text blob
	test2 (r,KErrLocked);
	test.Next(_L("Release client 1 lock"));
	TheTable.Cancel();
	TheDatabase.Rollback();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0610
@SYMTestCaseDesc        Tests for a defect on database
@SYMTestPriority        Medium
@SYMTestActions         Tests for database connection
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDefectL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0610 Build test database "));
	Connect();
	TInt r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	Execute(KCreateTable1);
	test2 (TheTable.Open(TheDatabase,KTable1),KErrNone);
	TheTable.InsertL();
	RDbColWriteStream wcol;
	wcol.OpenLC(TheTable,3);
	TInt ii;
	for (ii=0;ii<2048;++ii)
		wcol.WriteUint16L(ii);
	wcol.CommitL();
	CleanupStack::PopAndDestroy();
	wcol.OpenLC(TheTable,4);
	for (ii=0;ii<256;++ii)
		wcol.WriteInt32L(ii);
	wcol.CommitL();
	CleanupStack::PopAndDestroy();
	TheTable.PutL();
//
	test.Next(_L("open client 2 connection"));
	RDbNamedDatabase db;
	test2 (db.Open(TheDbs,KTestDatabase,KTestFormat),KErrNone);
	test2 (TheView.Prepare(db,_L("select * from Table1")),KErrNone);
	test2 (TheView.EvaluateAll(),KErrNone);
//
	test.Next(_L("Scenario 1"));
	TestScenario1L();
//
	test.Next(_L("Scenario 2"));
	TestScenario2L();
//
	test.Next(_L("Scenario 3"));
	TestScenario3L(db);
//
	test.Next(_L("Scenario 4"));
	TestScenario4L();
//
	test.Next(_L("Scenario 5"));
	TestScenario5L();
//
	test.Next(_L("Scenario 6"));
	TestScenario6L();
//
	test.Next(_L("Scenario 7"));
	TestScenario7L();
//
	test.Next(_L("Scenario 8"));
	TestScenario8L();
//
	
	test.Next(_L("tidy up"));
	test2 (TheDatabase.Begin(),KErrNone);
	TheView.Close();
	TheTable.Close();
	db.Close();
	Execute(KDropTable1);
	test2 (TheDatabase.Commit(),KErrNone);
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

/**
@SYMTestCaseID          PDS-DBMS-CT-4002
@SYMTestCaseDesc	    Testing RDbIncremental API
@SYMTestPriority 	    High
@SYMTestActions  	    Executing SQL using RDbIncremental, Altering tables, creating and droping indexes
@SYMTestExpectedResults After execution Table should exist in DB, after altering, table colums should change,
						creation of index should create index column in table, dropping index should remove
						it from table
@SYMDEF                 DEF135710
*/
LOCAL_C void TestRDbIncrementalAPIL()
	{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4002 Testing RDbIncremental API"));
	
	Connect();
	TInt err = TheDatabase.Open(TheDbs,KTestDatabase);
	test2(err, KErrNone);
	
	test.Next(_L("RDbIncremental API"));
	RDbIncremental inc;
	TInt step;
	
	test2(TheDatabase.Begin(), KErrNone);
	err = inc.Execute(TheDatabase, KCreateTable2, step);
	test2(err, KErrNone);	
	while(step > 0)
		{
		err = inc.Next(step);
		test2(err, KErrNone);
		}
	inc.Close();
	
	test.Next(_L("AlterTable"));
	CDbColSet* colSet = CDbColSet::NewLC();
	colSet->AddL(TDbCol(_L("Id2"), EDbColUint32));
	colSet->AddL(TDbCol(_L("Memo2"), EDbColText));
	test2(colSet->Count(), 2);
	err = inc.AlterTable(TheDatabase, KTable2, *colSet, step);
	test2(err, KErrNone);
	while(step > 0)
		{
		err = inc.Next(step);
		test2(err, KErrNone);
		}
	inc.Close();
	err = TheDatabase.Commit();
	test2(err, KErrNone);
	
	test.Next(_L("CreateIndex"));
	TDbKeyCol kcol(_L("Id2"), TDbKeyCol::EAsc);
	CDbKey* key = CDbKey::NewLC();
	test2(TheDatabase.Begin(), KErrNone);
	key->AddL(kcol);
	err = inc.CreateIndex(TheDatabase, _L("Id2"), KTable2, *key, step);
	test2(err, KErrNone);
	while(step > 0)
		{
		err = inc.Next(step);
		test2(err, KErrNone);
		}
	inc.Close();
	
	test.Next(_L("DropIndex"));
	err = inc.DropIndex(TheDatabase, _L("Id2"), KTable2, step);
	test2(err, KErrNone);
	while(step > 0)
		{
		err = inc.Next(step);
		test2(err, KErrNone);
		} 
	inc.Close();
	err = inc.Execute(TheDatabase, KDropTable2, step);
	test2(err, KErrNone);
	while(step > 0)
		{
		err = inc.Next(step);
		test2(err, KErrNone);
		}
	err = TheDatabase.Commit();
	test2(err, KErrNone);
	inc.Close();
	
	CleanupStack::PopAndDestroy(2);
	
	TheDatabase.Close();
	Disconnect();
	test.End();
	}

/**
@SYMTestCaseID          PDS-DBMS-CT-4003
@SYMTestCaseDesc	    Testing RDbUpdate API
@SYMTestPriority 	    High
@SYMTestActions  	    Executing DML command using RDbUpdate, checking if rowCount working properly
@SYMTestExpectedResults After DML execution, rowCount should return proper number of afected rows. 
@SYMDEF                 DEF135710
*/
LOCAL_C void TestRDbUpdate()
{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4003 Testing RDbUpdate API"));
	_LIT(DMLinsert, "INSERT INTO Table2 VALUES (2,'Mietek', 'Mietek ma kota')");
	
	Connect();
	TInt err = TheDatabase.Open(TheDbs,KTestDatabase);
	test2(err, KErrNone);
	
	err = TheDatabase.Begin();
	test2(err, KErrNone);
	
	err = TheDatabase.Execute(KCreateTable2, EDbCompareNormal);
	test(err >= KErrNone);
	
	RDbUpdate update;
	err = update.Execute(TheDatabase, DMLinsert, EDbCompareNormal );
	if(err != KErrNone)
		RDebug::Printf("Error on Execute %d",err);
	test2(err, KErrNone);	
	
	TInt rows = update.RowCount();
	RDebug::Printf("Afected rows %d",rows);
	test2(rows, 1);	
	update.Close();
	
	err = TheDatabase.Commit();
	test2(err, KErrNone);
	
	TheDatabase.Close();
	Disconnect();
	test.End();
}

/**
@SYMTestCaseID          PDS-DBMS-CT-4004 
@SYMTestCaseDesc	    Testing RDbRow API
@SYMTestPriority 	    High
@SYMTestActions  	    Test for construction functions for RDbRow
@SYMTestExpectedResults After Creation/Opening row should be a valid object.
@SYMDEF                 DEF135710
*/
LOCAL_C void TestRDbRowL()
{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4004 Testing RDbRow API"));
	const TInt KRowSize = 300;
	const TInt KRealRowSize = 512;

	RDbRow row;
	row.CreateL(KRowSize);
	RDebug::Printf("Rows %d %d",row.Size(), row.MaxSize() );
	test2(row.MaxSize(), KRealRowSize);
	TAny* rptr = row.First();
	
	RDbRow row2;
	row2.Open(rptr, KRowSize, 2*KRowSize);
	RDebug::Printf("Row2s %d %d", KRowSize, row2.MaxSize());
	test2(row2.Size(), KRowSize);
	test2(row2.MaxSize(), 2*KRowSize);
	
	row.Close();
	row2.Close();
	test.End();
}
/*
 * Helper function for SYSLIB-DBMS-CT-4005 Testing TTextOps API
 */
LOCAL_C TInt HelperCompareForTestTTextOps(const TText8*,TInt,const TText8*,TInt)
	{
	return KReturnValueForCompare;
	}
/*
 * Helper function for SYSLIB-DBMS-CT-4005 Testing TTextOps API
 */
LOCAL_C TInt HelperFindForTestTTextOps(const TDesC8&,const TText8*,TInt)
	{
	return KReturnValueForFind;
	}
/**
@SYMTestCaseID          PDS-DBMS-CT-4005
@SYMTestCaseDesc	    Testing TTextOps API
@SYMTestPriority 	    High
@SYMTestActions  	    Test for Compare and Find functions for TTextOps
@SYMTestExpectedResults Test if those functions really calling proper targets.
@SYMDEF                 DEF135710
*/
LOCAL_C void TestTTextOps()
{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4005 Testing TTextOps API"));
	TTextOps text;
	
	text.iCompare8 = HelperCompareForTestTTextOps;
	text.iFind8 = HelperFindForTestTTextOps;
	TInt ret = text.Compare(_L8("Ala ma kota"), _L8("Ala ma konia"));
	test2(ret, KReturnValueForCompare);
	ret = text.Find(_L8("Ala ma kota"), _L8("ma ko"));
	test2(ret, KReturnValueForFind);
	
	test.End();
}

#if defined __WINS__ ||	defined __WINSCW__

LOCAL_C TInt ExecuteRemoteL(const TDesC& aCommand, const TDesC& aCommandLineArg)
	{
	RProcess process;
	User::LeaveIfError(process.Create(aCommand, aCommandLineArg));
	
	TRequestStatus status;
	process.Logon(status);
	process.Resume();

	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	
	process.Close();
	User::LeaveIfError(exitReason);
	
	return exitReason;
	}

#endif //defined __WINS__ ||	defined __WINSCW__

/**
@SYMTestCaseID          PDS-DBMS-CT-4007
@SYMTestCaseDesc	    Testing SPConv - This test case uses the Edbsconv tool 
						so therefore only runs on WINSCW builds
@SYMTestPriority 	    High
@SYMTestActions  	    Launch the external tool to generate SPD files
@SYMTestExpectedResults Should produce SPD files and report no errors
@SYMDEF                 DEF135710
*/
LOCAL_C void TestSPConvL()
	{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4007 Testing SPConv"));
	
#if defined __WINS__ ||	defined __WINSCW__
	
	_LIT(KRomTxtFile1, "z:\\test\\11335577.txt");
	_LIT(KRomTxtFile2, "z:\\test\\11335578.txt");
	_LIT(KRomTxtFile3, "z:\\test\\11335579.txt");
	_LIT(KRomTxtFile4, "z:\\test\\1133557A.txt");
	_LIT(KCommand,"Edbspconv");
	_LIT(KCommandParameter, " /f=%S /b=%S /s");
	
	TBuf<200> commandParameter;
	
	test.Next(_L("Create SPD File from \"11335577.txt\""));
	commandParameter.Format(KCommandParameter, &KRomTxtFile1, &KTestSpdFile1);
	ExecuteRemoteL(KCommand, commandParameter);
		
	test.Next(_L("Create SPD File from \"11335578.txt\""));
	commandParameter.Format(KCommandParameter, &KRomTxtFile2, &KTestSpdFile2);
	ExecuteRemoteL(KCommand, commandParameter);
	
	test.Next(_L("Create SPD File from \"11335579.txt\""));
	commandParameter.Format(KCommandParameter, &KRomTxtFile3, &KTestSpdFile3);
	ExecuteRemoteL(KCommand, commandParameter);
	
	test.Next(_L("Create SPD File from \"1133557A.txt\""));
	commandParameter.Format(KCommandParameter, &KRomTxtFile4, &KTestSpdFile4);
	ExecuteRemoteL(KCommand, commandParameter);
	
#else
	RDebug::Print(_L("Testing SPConv - This test case cannot run on hardware"));
#endif	
	
	test.Next(_L("End"));
	test.End();
	}


/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0611
@SYMTestCaseDesc        Tests the behaviour of observers
@SYMTestPriority        Medium
@SYMTestActions         Tests for DDL,DML
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestObserverL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0611 Single connection/observer "));
	Connect();
	TInt r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	RDbNotifier ob;
	r=ob.Open(TheDatabase);
	test (r==KErrNone);
//
	test.Next(_L("Cancel"));
	TRequestStatus stat;
	ob.NotifyUnlock(stat);
	test (stat==KRequestPending);
	ob.Cancel();
	User::WaitForRequest(stat);
	test (stat==KErrCancel);
//
	test.Next(_L("Close"));
	ob.NotifyUnlock(stat);
	test (stat==KRequestPending);
	ob.Close();
	User::WaitForRequest(stat);
	test (stat==KErrCancel);
//
	test.Next(_L("DDL"));
	r=ob.Open(TheDatabase);
	test (r==KErrNone);
	ob.NotifyUnlock(stat);
	test (stat==KRequestPending);
	Execute(KCreateTable1);
	User::WaitForRequest(stat);
	test (stat==ob.ECommit);
//
	test.Next(_L("DML"));
	ob.NotifyUnlock(stat);
	test (stat==KRequestPending);
	r=TheTable.Open(TheDatabase,KTable1);
	test (r==KErrNone);
	WriteRecordsL(50);
	User::WaitForRequest(stat);
	test (stat==ob.ECommit);
//
	test.Next(_L("Locked read"));
	ob.NotifyUnlock(stat);
	TheDatabase.Begin();
	test (stat==KRequestPending);
	TheDatabase.Commit();
	User::WaitForRequest(stat);
	test (stat==ob.EUnlock);
//
	test.Next(_L("Database closed"));
	ob.NotifyUnlock(stat);
	TheTable.Close();
	TheDatabase.Close();
	User::WaitForRequest(stat);
	test (stat==ob.EClose);
	ob.NotifyUnlock(stat);
	User::WaitForRequest(stat);
	test (stat==ob.EClose);
//
	test.Next(_L("Re-open database"));
	r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	ob.NotifyUnlock(stat);
	User::WaitForRequest(stat);
	test (stat==ob.EClose);
	ob.Close();
//
	test.Next(_L("Multiple connections and observers"));
	r=ob.Open(TheDatabase);
	test (r==KErrNone);
	RDbNamedDatabase db2;
	r=db2.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	RDbNotifier ob2;
	r=ob2.Open(db2);
	test (r==KErrNone);
//
	test.Next(_L("Cancel"));
	ob.NotifyUnlock(stat);
	TRequestStatus stat2;
	ob2.NotifyUnlock(stat2);
	test (stat==KRequestPending);
	ob.Cancel();
	User::WaitForRequest(stat);
	test (stat==KErrCancel);
	test (stat2==KRequestPending);
	ob2.Cancel();
	User::WaitForRequest(stat2);
	test (stat2==KErrCancel);
//
	test.Next(_L("Close"));
	ob.NotifyUnlock(stat);
	ob2.NotifyUnlock(stat2);
	test (stat2==KRequestPending);
	ob2.Close();
	User::WaitForRequest(stat2);
	test (stat2==KErrCancel);
	test (stat==KRequestPending);
	ob.Close();
	User::WaitForRequest(stat);
	test (stat==KErrCancel);
//
	test.Next(_L("NotifyChange"));
	r=ob.Open(TheDatabase);
	test (r==KErrNone);
	r=ob2.Open(db2);
	test (r==KErrNone);
	ob.NotifyUnlock(stat);
	ob2.NotifyChange(stat2);
	TheDatabase.Begin();
	r=TheDatabase.Commit();
	test (r==KErrNone);
	User::WaitForRequest(stat);
	test (stat==ob.EUnlock);
	test (stat2==KRequestPending);
	ob.NotifyUnlock(stat);
	db2.Begin();
	r=db2.Commit();
	test (r==KErrNone);
	User::WaitForRequest(stat);
	test (stat==ob.EUnlock);
	test (stat2==KRequestPending);
	Execute(KDropTable1);
	User::WaitForRequest(stat2);
	test (stat2==ob.ECommit);
//
	test.Next(_L("missed event"));
	ob.NotifyUnlock(stat);
	User::WaitForRequest(stat);
	test (stat==ob.ECommit);
//
	test.Next(_L("database close"));
	ob.NotifyUnlock(stat);
	test (stat==KRequestPending);
	ob2.NotifyUnlock(stat2);
	test (stat2==KRequestPending);
	TheDatabase.Close();
	User::After(0x20000);	// ~.1s
	test (stat==KRequestPending);
	test (stat2==KRequestPending);
	db2.Close();
	User::WaitForRequest(stat);
	test (stat==ob.EClose);
	User::WaitForRequest(stat2);
	test (stat2==ob.EClose);
	ob.NotifyUnlock(stat);
	User::WaitForRequest(stat);
	test (stat==ob.EClose);
	ob.Cancel();
	ob.Close();
	ob2.NotifyUnlock(stat2);
	User::WaitForRequest(stat2);
	test (stat2==ob.EClose);
	ob2.Cancel();
	ob2.Close();
//
	Disconnect();
	test.End();
	}

LOCAL_C void setupTestDirectory()
//
// Prepare the test directory.
//
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	r=TheFs.MkDir(KTestDatabase);
	test(r==KErrNone || r==KErrAlreadyExists);
	}

LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
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

LOCAL_C void  KillDbmsServer()
    {
    _LIT(KDbmsServer,"edbsrv.exe");
     TFullName name;
    TBuf<64> pattern(KDbmsServer);
    TInt length = pattern.Length();
    pattern += _L("*");
    TFindProcess procFinder(pattern);

    while (procFinder.Next(name) == KErrNone)
        {
        if (name.Length() > length)
            {//If found name is a string containing aProcessName string.
            TChar c(name[length]);
            if (c.IsAlphaDigit() ||
                c == TChar('_') ||
                c == TChar('-'))
                {
                // If the found name is other valid application name
                // starting with aProcessName string.
                //RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
                continue;
                }
            }
        RProcess proc;
        if (proc.Open(name) == KErrNone)
            {
            proc.Kill(0);
            //RDebug::Print(_L("\"%S\" process killed.\n"), &name);
            }
        proc.Close();
        }
    }
void DoTests()
	{
	TVersionName n=RDbs::Version().Name();
	test.Printf(_L("DBMS server v%S\n"),&n);
	TInt r;
	test.Start(_L("Connection"));
	TestConnect();
	
	test.Next(_L("Open Database"));
	TRAP(r,TestOpenL());
	test2 (r,KErrNone);
	
	test.Next(_L("test defect"));
	TRAP(r,TestDefectL());
	test2 (r,KErrNone);
	
	test.Next(_L("Share Database"));
	TRAP(r,TestShareL());
	test2 (r,KErrNone);
	
	test.Next(_L("Locking"));
	TRAP(r,TestLocking());
	test2 (r,KErrNone);
	
	test.Next(_L("Observers"));
	TRAP(r,TestObserverL());
	test2 (r,KErrNone);
	
	test.Next(_L("Benchmarks"));
	TRAP(r,TestBenchL());
	test2 (r,KErrNone);
	
	test.Next(_L("RDbIncremental"));
	TRAP(r, TestRDbIncrementalAPIL() );
	test2 (r,KErrNone);
	
	test.Next(_L("RDbUpdate"));
	TestRDbUpdate();

	test.Next(_L("RDbRow"));
	TRAP(r, TestRDbRowL() );
	test2 (r,KErrNone);
	
	test.Next(_L("TTextOpsL"));
	TestTTextOps();
	
	test.Next(_L("TestSPConvL"));
	TRAP(r, TestSPConvL() );
	test2 (r, KErrNone);
	
	test.Next(_L("Waiting for server exit"));
	}


GLDEF_C TInt E32Main()
//
// Test streaming conversions.
//
    {
	__UHEAP_MARK;
	
	test.Title();
	setupTestDirectory();
	DeleteTestFiles();
	setupCleanup();
	KillDbmsServer();
	DoTests();
	delete TheTrapCleanup;
	DeleteTestFiles();
	
	test.End();
	TheFs.Close();
	test.Close();
	__UHEAP_MARKEND;
	return 0;
    }
