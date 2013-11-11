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
// Test code for bugs that have been fixed, to help prevent regression
// 
//

#include <d32dbms.h>
#include <f32file.h>
#include <e32test.h>
#include <s32mem.h>

LOCAL_D RTest test(_L("t_dbbug"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D RDbNamedDatabase TheDatabase;

const TInt KTestCleanupStack=0x40;

void Check(TInt aValue,TInt aExpected,TInt aLine)
	{
	if (aValue!=aExpected)
		{
		test.Printf(_L("*** Expected %d: got %d\r\n"),aExpected,aValue);
		test.operator()(EFalse,aLine);
		}
	}
#define test2(a,b) Check(a,b,__LINE__)

static void Print(const TText* aString)
	{
	test.Printf(_L("%s\n"),aString);
	}

////////////////////////////////////////////

_LIT(KTestDatabase,"c:\\dbms-tst\\bug.db");
_LIT(KTableA,"A");
_LIT(KTableB,"B");
_LIT(KTableC,"C");

class Defect_590829
	{
public:
	static void TestL();
	static const TDesC& Name();
	};


const TDesC& Defect_590829::Name()
	{
	_LIT(KName,"590829");
	return KName;
	}

/////////////////////////////////////////////////

// Length of text data used for each entry. This will be
// equivalent to 400 bytes for ansi characters. The number of
// bytes must not be less than 256 (or 128 for this const).
// If it is a stream will not be used for transfer of data.
const TInt KTextDataLength = 200;

// max column size
const TInt KMaxColLength = 1000;

// Buffer size to cause HDbsBuf::DoReadL() ipc check to be executed
const TInt KBufSizeDoReadL = 1000;

// Buffer size to cause HDbsBuf::UnderflowL() ipc check to be executed
const TInt KBufSizeUnderflowL = 500;

class Defect_071149
	{
public:
	static void TestL();
	static const TDesC& Name();
	};


const TDesC& Defect_071149::Name()
	{
	_LIT(KName,"071149");
	return KName;
	}

/**
HDbsBuf did not handle case when iIpc.iHandle is 0 causing IPC calls to panic.
The handle is 0 when opening a stream and all data is retrieved in this request.

@SYMTestCaseID			SYSLIB-DBMS-CT-1491
@SYMTestCaseDesc		Tests for defect number 590829
@SYMTestPriority			High
@SYMTestActions			Tests by setting up failure conditions.
@SYMTestExpectedResults	Test must not fail
@SYMDEF				INC071149
*/
void Defect_071149::TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1491 "));
	Print(_S("Creating test database"));

	// Connect to dbms and open db
	RDbs dbs;
	RDbNamedDatabase db;
	test2 (db.Replace(TheFs,KTestDatabase),KErrNone);
	db.Close();

	test2(dbs.Connect(), KErrNone);
	test2(db.Open(dbs,KTestDatabase), KErrNone);

	// creating column to hold LongText
	CDbColSet *colSet = CDbColSet::NewL();
	CleanupStack::PushL(colSet);
	colSet->AddL(TDbCol(_L("Id"), EDbColLongText, KMaxColLength));

	// create table
	test2(db.CreateTable(KTableA, *colSet), KErrNone);
	CleanupStack::PopAndDestroy(colSet);

	// create text data to add to table
	HBufC* testText = HBufC::New(KTextDataLength);
	test(testText !=NULL );
	TPtr ptr = testText->Des();
	for(TInt y=0;y<KTextDataLength;++y)
		{
		ptr.Append(TChar('A'));
		}

	// add data to table
	RDbTable newTable;
	test2 (newTable.Open(db,KTableA),KErrNone);
	db.Begin();
	newTable.InsertL();
	newTable.SetColL(1, ptr);
	newTable.PutL();

	test2 (db.Commit(),KErrNone);
	test2 (newTable.CountL(), 1);
	newTable.Close();

	// cleanup
	delete testText;

	// disconnect from db and dbms
	db.Close();
	dbs.Close();

//
	// Connect to dbms and open db
	test2(dbs.Connect(), KErrNone);
	test2(db.Open(dbs,KTestDatabase), KErrNone);

	// Test handle check in HDbsBuf::DoReadL() - See defect
	// If the handle check did not exist in the production code then
	// it would panic.

	// create test table
	RDbTable testTable;
	test2 (testTable.Open(db,KTableA),KErrNone);
	db.Begin();
	testTable.FirstL();
	testTable.GetL();

	// Open stream
	RDbColReadStream rs;
    	rs.OpenLC( testTable, 1);

	// Read data
	TBuf<KBufSizeDoReadL> buf;
	TRAPD(err, rs.ReadL( buf, buf.MaxLength()));
	if(err != KErrNone)
		{
		test2(err, KErrEof);
		}
	CleanupStack::PopAndDestroy();  // Close rs
	testTable.Close();

//
	// Test handle check in HDbsBuf::UnderflowL() - additional error not in defect
	// If the handle check did not exist in the production code then
	// it would panic.

	// create test table
	test2 (testTable.Open(db,KTableA),KErrNone);
	testTable.NextL();
	testTable.GetL();

	// Open stream
	RDbColReadStream rs2;
    	rs2.OpenLC( testTable, 1);

	// Read data
	TBuf<KBufSizeUnderflowL> buf2;
	TRAP(err, rs2.ReadL( buf2, buf2.MaxLength()));
	if(err != KErrNone)
		{
		test2(err, KErrEof);
		}
	CleanupStack::PopAndDestroy();  // Close rs

	// tidy up
	testTable.Close();
	db.Close();
	dbs.Close();
	}

/////////////////////////////////////////////////

/**
Cached, unused tables were breaking the iterator in CDbTableDatabase::CheckIdle
The latter function has been re-written to restart the iteration when tables are Idle()'d

@SYMTestCaseID          SYSLIB-DBMS-CT-0582
@SYMTestCaseDesc        Tests for defect number 590829
@SYMTestPriority        Medium
@SYMTestActions        	Tests by setting up failure conditions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Defect_590829::TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0582 "));
	Print(_S("Creating test database"));
	test2 (TheDatabase.Replace(TheFs,KTestDatabase),KErrNone);
	TheDatabase.Begin();
	test2 (TheDatabase.Execute(_L("create table A (id counter)")),KErrNone);
	test2 (TheDatabase.Execute(_L("create table B (id counter)")),KErrNone);
	test2 (TheDatabase.Execute(_L("create table C (id counter)")),KErrNone);
	test2 (TheDatabase.Commit(),KErrNone);
//
	Print(_S("Setting up failure"));
	RDbTable tA,tB,tC;
	test2 (tA.Open(TheDatabase,KTableA),KErrNone);
	test2 (tB.Open(TheDatabase,KTableB),KErrNone);
	tB.Close();
	test2 (tC.Open(TheDatabase,KTableC),KErrNone);
	tC.Close();
	TheDatabase.Begin();
	tA.Close();
//
	Print(_S("Testing fix"));
	test2 (TheDatabase.Commit(),KErrNone);
	TheDatabase.Destroy();
	}

/////////////////////////////////////////////////

class Defect_394751
	{
public:
	static void TestL();
	static const TDesC& Name();
private:
	static TInt Thread(TAny*);
	static void ThreadL();
	};


const TDesC& Defect_394751::Name()
	{
	_LIT(KName,"394751");
	return KName;
	}

void Defect_394751::ThreadL()
	{
	RDbs dbs;
	RDbNamedDatabase db;
	User::LeaveIfError(dbs.Connect());
	User::LeaveIfError(db.Open(dbs,KTestDatabase));
	db.Begin();
	db.Begin();	/// panic now
	User::Panic(_L("T_BUG failure"),0);
	}

TInt Defect_394751::Thread(TAny*)
	{
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (!cleanup)
		return KErrNoMemory;
	TRAPD(r,ThreadL());
	delete cleanup;
	return r;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0583
@SYMTestCaseDesc        Tests for defect number 394751
@SYMTestPriority        Medium
@SYMTestActions        	Tests for thread exit status.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Defect_394751::TestL()
//
//
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0583 "));
	Print(_S("Creating test database"));
	test2 (TheDatabase.Replace(TheFs,KTestDatabase),KErrNone);
	TheDatabase.Close();
//
	RDbs dbs;
	test2 (dbs.Connect(),KErrNone);
//
	Print(_S("Running test thread"));
	RThread t;
	_LIT(KTestThread,"Defect Fix 394751");
	test2 (t.Create(KTestThread,&Thread,0x2000,0x1000,0x10000,0,EOwnerThread),KErrNone);
	TRequestStatus s;
	t.Logon(s);
	test2 (s.Int(),KRequestPending);
	t.Resume();
	Print(_S("Awaiting completion"));
	User::WaitForRequest(s);
	_LIT(KCategory,"DBMS-Table");
	test2 (t.ExitType(),EExitPanic);
	test (t.ExitCategory()==KCategory);
	test2 (t.ExitReason(),11);		// begin nested transaction
	User::SetJustInTime(ETrue);	// enable debugger panic handling
	t.Close();
//
	test2 (dbs.ResourceCount(),0);
	dbs.Close();
	}

///////////////////////////////////////////////////////////

class Defect_COMBBAR_463J5D
	{
public:
	static void TestL();
	static const TDesC& Name();
private:
	static void WaitForServerExit();
	static void KillDbmsServer();
	static TInt Thread(TAny*);
	};

const TDesC& Defect_COMBBAR_463J5D::Name()
	{
	_LIT(KName,"COMBBAR_463J5D");
	return KName;
	}

void Defect_COMBBAR_463J5D::KillDbmsServer()
    {
    _LIT(KDbmsServer,"edbsrv.exe");
     TFullName name;
    //RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);
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

void Defect_COMBBAR_463J5D::WaitForServerExit()
	{
	_LIT(KDbmsServer,"*!DBMS server");
	TFullName n;
	TFindThread ft(KDbmsServer);
	if (ft.Next(n)==KErrNone)
		{
		RThread t;
		if (t.Open(ft)==KErrNone)
			{
			TRequestStatus s;
			t.Logon(s);
			User::WaitForRequest(s);
			t.Close();
			}
		}
	}

TInt Defect_COMBBAR_463J5D::Thread(TAny*)
//
// Just try to start the server
//
	{
	RDbs dbs;
	return dbs.Connect();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0584
@SYMTestCaseDesc        Tests for defect number COMBBAR_463J5D
@SYMTestPriority        Medium
@SYMTestActions        	Testing that defect COMBBAR_463J5D in ER5 Defects database has been fixed
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Defect_COMBBAR_463J5D::TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0584 "));
	Print(_S("Kill the server if it has started"));
	KillDbmsServer();
//
	Print(_S("Create the launching threads"));
	RThread t1,t2;
	TRequestStatus s1,s2;
	_LIT(KThread1,"t1");
	test2 (t1.Create(KThread1,&Thread,0x2000,0,0,EOwnerThread),KErrNone);
	t1.SetPriority(EPriorityLess);
	t1.Logon(s1);
	_LIT(KThread2,"t2");
	test2 (t2.Create(KThread2,&Thread,0x2000,0,0,EOwnerThread),KErrNone);
	t2.SetPriority(EPriorityLess);
	t2.Logon(s2);
//
	Print(_S("Run the threads and wait"));
	t1.Resume();
	t2.Resume();
	User::WaitForRequest(s1,s2);
	if (s1==KRequestPending)
		User::WaitForRequest(s1);
	else
		User::WaitForRequest(s2);
//
	test2 (t1.ExitType(),EExitKill);
	if (s1.Int()!=KErrNotFound)
		test2 (s1.Int(),KErrNone);
	test2 (t2.ExitType(),EExitKill);
	if (s2.Int()!=KErrNotFound)
		test2 (s2.Int(),KErrNone);
	t1.Close();
	t2.Close();
	}

/////////////////////////////////////////////////////////

class Defect_EDNATHE_48AEZW
	{
public:
	static void TestL();
	static const TDesC& Name();
	};

const TDesC& Defect_EDNATHE_48AEZW::Name()
	{
	_LIT(KName,"EDNATHE_48AEZW");
	return KName;
	}
/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0585
@SYMTestCaseDesc        Tests for defect number EDNATHE_48AEZW
@SYMTestPriority        Medium
@SYMTestActions        	Tests for navigation and deletion
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Defect_EDNATHE_48AEZW::TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0585 "));
	Print(_S("Set up database"));
	test2 (TheDatabase.Replace(TheFs,KTestDatabase),KErrNone);
	test2 (TheDatabase.Begin(),KErrNone);
	test2 (TheDatabase.Execute(_L("create table A (id counter)")),KErrNone);
	RDbView v1;
	test2 (v1.Prepare(TheDatabase,_L("select * from A"),v1.EInsertOnly),KErrNone);
	test2 (v1.EvaluateAll(),KErrNone);
	for (TInt ii=0;ii<4;++ii)
		{
		v1.InsertL();
		v1.PutL();
		}
	test2 (TheDatabase.Commit(),KErrNone);
	v1.Close();
//
	Print(_S("test navigation"));
	test2 (v1.Prepare(TheDatabase,_L("select * from A where id=0")),KErrNone);
	test2 (v1.EvaluateAll(),KErrNone);
	v1.FirstL();
	RDbView v2;
	test2 (v2.Prepare(TheDatabase,_L("select * from A where id=1")),KErrNone);
	test2 (v2.EvaluateAll(),KErrNone);
	v2.FirstL();
	v2.DeleteL();
	TRAPD(r, v1.NextL());
	test2 (r,KErrNone);
	test (v1.AtEnd());
	v2.Close();
//
	Print(_S("test deletion"));
	v1.FirstL();
	test2 (v2.Prepare(TheDatabase,_L("select * from A where id=2")),KErrNone);
	test2 (v2.EvaluateAll(),KErrNone);
	v2.FirstL();
	v2.DeleteL();
	TRAP(r,v1.DeleteL());
	test2 (r,KErrNone);
	TRAP(r, v1.NextL());
	test2 (r,KErrNone);
	test (v1.AtEnd());
//
	v1.Close();
	v2.Close();
	TheDatabase.Close();
	}

/////////////////////////////////////////////////////////

static void NextTest(const TDesC& aName)
	{
	TBuf<80> buf;
	buf=_S("Checking Defect ");
	buf+=aName;
	test.Next(buf);
	}

template <typename T>
struct RunTest
	{
	RunTest()
		{
		const TDesC& name = T::Name();
		NextTest(name);
		TRAPD(r,T::TestL());
		test2 (r,KErrNone);
		Print(_S("Defect fixed"));
		}
	};

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

GLDEF_C TInt E32Main()
//
// Test streaming conversions.
//
    {
	__UHEAP_MARK;
	test.Title();
	setupTestDirectory();
	setupCleanup();
//
	test.Start(_L("Verifying defect fixes"));
	RunTest<Defect_COMBBAR_463J5D>();
// The following short delay is needed for ccover builds only.
// Without the pause, the kernel scheduler would intermittently
// crash 0.3s after the last test ended.
	User::After(500000);
	test.Printf(_L("Resume test after delay.\n"));

	RunTest<Defect_394751>();
	User::After(500000);
	test.Printf(_L("Resume test after delay.\n"));

	RunTest<Defect_590829>();
	User::After(500000);
	test.Printf(_L("Resume test after delay.\n"));

	RunTest<Defect_071149>();
	User::After(500000);
	test.Printf(_L("Resume test after delay.\n"));

	RunTest<Defect_EDNATHE_48AEZW>();

	// clean up data files used by this test - must be done before call to End() - DEF047652
	::DeleteDataFile(KTestDatabase);

	test.End();
//
	delete TheTrapCleanup;

	TheFs.Close();
	test.Close();
	__UHEAP_MARKEND;
	return 0;
    }
