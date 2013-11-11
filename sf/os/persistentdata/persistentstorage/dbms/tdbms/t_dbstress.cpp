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

#include "t_dbstress.h"

//#define __DUMP_STATE
#if defined(__WINS__)
//#define _INSTALL_FILE_SYSTEM
#endif

GLDEF_D RTest TheTest(_L("t_dbstress: Stress testing DBMS"));

GLDEF_D TPtrC KTestDir=_S("\\DBMS-TST\\");
GLDEF_D TPtrC KLogFile=_L("T_STRESS.LOG");
GLDEF_D TPtrC KTestDatabase=_S("T_STRESS.DB");
GLDEF_D TInt NewCount,OldCount;
GLDEF_D TInt TransId;
GLDEF_D Timer RunTimer;


LOCAL_D RFs TheFs;
LOCAL_D RThread TheThread;
LOCAL_D TRequestStatus TheStatus;
LOCAL_D RDbStoreDatabase TheDatabase;
LOCAL_D RDbView Accs;
LOCAL_D RDbView Trans;
LOCAL_D RDbTable TheTable;
LOCAL_D TInt Shot,ShotDuringCommit;
LOCAL_D TInt64 RunningTime(1);
LOCAL_D Timer Stopwatch;

#ifdef __DUMP_STATE
const TPtrC KDumpFile=_S("T_STRESS.DMP");
#endif
const TInt KTestCleanupStack=0x20;

void Timer::Start()
	{
	iTime.UniversalTime();
	}

TInt64 Timer::Stop()
	{
	TTime t;
	t.UniversalTime();
	return ((t.MicroSecondsFrom(iTime).Int64()) + 500)/1000;
	}

void Timer::Print()
	{
	TInt64 milli=Stop();
	TheTest.Printf(_L("  %u milliseconds\n"), I64LOW(milli) );
	}

class Set
	{
public:
	struct SColDef
		{
		const TDesC* iName;
		TDbColType iType;
		TInt iAttributes;
		};
public:
	static CDbColSet* CreateL(const SColDef* aDef);
	};
CDbColSet* Set::CreateL(const SColDef* aDef)
	{
	CDbColSet *set=CDbColSet::NewLC();
	for (;aDef->iName!=NULL;++aDef)
		{
		TDbCol col(*aDef->iName,aDef->iType);
		col.iAttributes=aDef->iAttributes;
		set->AddL(col);
		}
	CleanupStack::Pop();
	return set;
	}

// Accounts table
const TPtrC KAccounts=_S("ACCOUNTS");
const TPtrC KAccountsID=_S("ID");
const TPtrC KAccountsBalance=_S("BALANCE");
const TPtrC KAccountsStatement=_S("STATEMENT_BALANCE");
Set::SColDef const AccountsDef[]=
	{
	{&KAccountsID,EDbColInt32,TDbCol::ENotNull},
	{&KAccountsBalance,EDbColInt32,TDbCol::ENotNull},
	{&KAccountsStatement,EDbColInt32,TDbCol::ENotNull},
	{0}
	};
const TInt KInitialCash=100000;
const TInt KInitialBalance=1000;

// Transaction table
const TPtrC KTransactions=_S("TRANSACTIONS");
const TPtrC KTransactionDate=_S("T_DATE");
const TPtrC KTransactionFrom=_S("FROM_ID");
const TPtrC KTransactionTo=_S("TO_ID");
const TPtrC KTransactionAmount=_S("AMOUNT");
Set::SColDef const TransactionsDef[]=
	{
//	{&KTransactionDate,EDbColDateTime,TDbCol::ENotNull},
	{&KTransactionDate,EDbColInt32,TDbCol::ENotNull},
	{&KTransactionFrom,EDbColInt32,TDbCol::ENotNull},
	{&KTransactionTo,EDbColInt32,TDbCol::ENotNull},
	{&KTransactionAmount,EDbColInt32,TDbCol::ENotNull},
	{0}
	};

LOCAL_D TInt32 TotalMonies;
LOCAL_D TBuf<100> Buf;

GLDEF_C TInt Random(TInt aRange)
	{
	return (Math::Random()>>11)%aRange;
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

TPtrC FileName(const TText* aFile)
    {
    TPtrC p(aFile);
    TInt ix=p.LocateReverse('\\');
    if (ix<0)
        ix=p.LocateReverse('/');
    if (ix>=0)
        p.Set(p.Mid(1+ix));
    return p;
    }

//Test macros and functions
void Check1(TInt aValue, const TText* aFile, TInt aLine)
    {
    if(!aValue)
        {
        TPtrC fname(FileName(aFile));
        TheTest.Printf(_L("*** Expression evaluated to false. %S-%d\r\n"), &fname, aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check2(TInt aValue, TInt aExpected, const TText* aFile, TInt aLine)
    {
    if(aValue != aExpected)
        {
        TPtrC fname(FileName(aFile));
        TheTest.Printf(_L("*** Expected %d, got %d. %S-%d\r\n"), aExpected, aValue, &fname, aLine);
        TheTest(EFalse, aLine);
        }
    }

///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void CreateIndexL(RDbDatabase& aDatabase,const TDesC& aTable,const TDesC& aColumn,TBool aUnique)
	{
	CDbKey* key=CDbKey::NewLC();
	key->AddL(aColumn);
	if (aUnique)
		key->MakeUnique();
	TEST2(aDatabase.CreateIndex(aColumn,aTable,*key),KErrNone);
	CleanupStack::PopAndDestroy();
	}

//
// Create the database
//
LOCAL_C void CreateDatabaseL()
	{
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,KTestDatabase,EFileRead|EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	store->SetRootL(TheDatabase.CreateL(store));
//	create the tables
	TheDatabase.Begin();
	CDbColSet* set=Set::CreateL(AccountsDef);
	TEST2(TheDatabase.CreateTable(KAccounts,*set),KErrNone);
	delete set;
	CreateIndexL(TheDatabase,KAccounts,KAccountsID,ETrue);
	CreateIndexL(TheDatabase,KAccounts,KAccountsBalance,EFalse);
	set=Set::CreateL(TransactionsDef);
	TEST2(TheDatabase.CreateTable(KTransactions,*set),KErrNone);
	delete set;
	CreateIndexL(TheDatabase,KTransactions,KTransactionDate,EFalse);
	TEST2(TheDatabase.Commit(),KErrNone);
	OldCount=NewCount=0;
// prepare Accs table
	TheDatabase.Begin();
	TEST2(Accs.Prepare(TheDatabase,_L("select * from accounts"),Accs.EInsertOnly),KErrNone);
	Accs.InsertL();
	Accs.SetColL(1,TInt32(ECash));
	Accs.SetColL(2,KInitialCash);
	Accs.SetColL(3,KInitialCash);
	Accs.PutL();
	TotalMonies=KInitialCash;
	for (TInt ii=EJohn;ii<=EPenny;++ii)
		{
		Accs.InsertL();
		Accs.SetColL(1,ii);
		Accs.SetColL(2,KInitialBalance);
		Accs.SetColL(3,KInitialBalance);
		Accs.PutL();
		TotalMonies+=KInitialBalance;
		}
	TEST2(TheDatabase.Commit(),KErrNone);
	Accs.Close();
	TheDatabase.Close();
	CleanupStack::PopAndDestroy();	// store
	}


#ifdef __DUMP_STATE
LOCAL_C void DumpStateL()
	{
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Replace(TheFs,KLogFile,EFileWrite|EFileStreamText));
	RDbRowSet::RConstraint match;
	CleanupClosePushL(match);
	for (TInt id=ECash;id<=EPenny;++id)
		{
		Buf.Format(_L("id=%d"),id);
		Accs.FirstL();
		TEST(Accs.FindL(Accs.EForwards,Buf)>=0);
		Accs.GetL();
		TInt balance=Accs.ColInt(2);
		Buf.Format(_L("\nStatement for account %d: Previous balance %d\n"),id,balance);
		User::LeaveIfError(file.Write(Buf));
		Buf.Format(_L("from_id=%d or to_id=%d"),id,id);
		User::LeaveIfError(match.Open(Trans,Buf));
		for (Trans.BeginningL();Trans.NextL();)
			{
			if (Trans.MatchL(match))
				{
				Trans.GetL();
				TInt from=Trans.ColInt(2);
				TInt amount=Trans.ColInt(4);
				Buf.Format(_L("%04d: %6s %5d\n"),Trans.ColInt(1),from==id?_S("debit"):_S("credit"),amount);
				User::LeaveIfError(file.Write(Buf));
				if (from==id)
					balance-=amount;
				else
					balance+=amount;
				}
			}
		Buf.Format(_L("Closing balance %d\n"),balance);
		User::LeaveIfError(file.Write(Buf));
		Buf.Format(_L("Account balance %d\n"),Accs.ColInt(3));
		User::LeaveIfError(file.Write(Buf));
		}
	CleanupStack::PopAndDestroy(2);
	TEST(0);
	}
#endif

//
// Check that the database structure is fully intact
//
LOCAL_C void VerifyDatabaseL(CPersistentStore& aStore)
	{
	TheDatabase.OpenL(&aStore,aStore.Root());
// check any indexes
	TEST2(TheTable.Open(TheDatabase,KAccounts,TheTable.EReadOnly),KErrNone);
	TEST2(TheTable.CountL(),KAccountIDs);
	TInt r=TheTable.SetIndex(KAccountsID);
	if (r!=KErrCorrupt)
		{
		TEST2(r,KErrNone);
		TEST2(TheTable.CountL(),KAccountIDs);
		for (TInt id=ECash;id<=EPenny;++id)
			{
			TEST(TheTable.NextL());
			TheTable.GetL();
			TEST2(TheTable.ColInt(1),id);
			}
		TEST(!TheTable.NextL());
		}
	r=TheTable.SetIndex(KAccountsBalance);
	if (r!=KErrCorrupt)
		{
		TEST2(r,KErrNone);
		TEST2(TheTable.CountL(),KAccountIDs);
		TEST(TheTable.FirstL());
		TheTable.GetL();
		TInt last=TheTable.ColInt(2);
		for (TInt ii=1;ii<KAccountIDs;++ii)
			{
			TEST(TheTable.NextL());
			TheTable.GetL();
			TInt bal=TheTable.ColInt(2);
			TEST(bal>=last);
			last=bal;
			}
		TEST(!TheTable.NextL());
		}
	TheTable.Close();
	TEST2(TheTable.Open(TheDatabase,KTransactions,TheTable.EReadOnly),KErrNone);
	TInt count=TheTable.CountL();
	r=TheTable.SetIndex(KTransactionDate);
	if (r!=KErrCorrupt)
		{
		TEST2(r,KErrNone);
		TEST2(TheTable.CountL(),count);
		if (count)
			{
			TEST(TheTable.FirstL());
			TheTable.GetL();
			TInt last=TheTable.ColInt(1);
			while (--count!=0)
				{
				TEST(TheTable.NextL());
				TheTable.GetL();
				TInt date=TheTable.ColInt(1);
				TEST(date>last);
				last=date;
				}
			TEST(!TheTable.NextL());
			}
		else
			TEST(!TheTable.FirstL());
		}
	TheTable.Close();
// verify database integrity
	TInt balance[KAccountIDs];
	TEST2(Accs.Prepare(TheDatabase,_L("select id,statement_balance,balance from accounts"),Accs.EReadOnly),KErrNone);
	TEST2(Accs.CountL(),KAccountIDs);
	while (Accs.NextL())
		{
		Accs.GetL();
		TInt id=Accs.ColInt(1);
		balance[id]=Accs.ColInt(2);
		}
	TEST2(Trans.Prepare(TheDatabase,_L("select t_date,from_id,to_id,amount from Transactions"),Trans.EReadOnly),KErrNone);
	TInt transact=0;
	while (Trans.NextL())
		{
		++transact;
		Trans.GetL();
		TInt from=Trans.ColInt(2);
		TInt to=Trans.ColInt(3);
		TInt amount=Trans.ColInt(4);
		balance[from]-=amount;
		balance[to]+=amount;
		}
	TEST2(transact,Trans.CountL());
	if (NewCount!=-1 && transact!=NewCount)
		{
		TEST2(transact,OldCount);
		++ShotDuringCommit;
		}
	OldCount=NewCount=transact;
	TInt total=0;
	for (Accs.BeginningL();Accs.NextL();)
		{
		Accs.GetL();
		TInt id=Accs.ColInt(1);
#ifdef __DUMP_STATE
		if (balance[id]!=Accs.ColInt(3))
			DumpStateL();
#else
		TEST(balance[id]==Accs.ColInt(3));
#endif
		total+=balance[id];
		}
	TEST2(total,TotalMonies);
	Trans.Close();
	Accs.Close();
	TheDatabase.Close();
	}

LOCAL_C TInt Verify(CPersistentStore& aStore)
	{
	TRAPD(r,VerifyDatabaseL(aStore));
	return r;
	}

LOCAL_C TInt Recover(CPersistentStore& aStore)
	{
	TRAPD(r,TheDatabase.OpenL(&aStore,aStore.Root()));
	if (r==KErrNone)
		{
		r=TheDatabase.Recover();
		TheDatabase.Close();
		}
	return r;
	}

LOCAL_C void CompactL(CStreamStore& aStore)
	{
	TInt t=aStore.ReclaimL();
	Stopwatch.Start();
	t-=aStore.CompactL();
	TheTest.Printf(_L("  compacted %d byte(s) in"),t);
	Stopwatch.Print();
	aStore.CommitL();
	}

LOCAL_C TInt Compact(CStreamStore& aStore)
	{
	TRAPD(r,CompactL(aStore));
	return r;
	}

LOCAL_C TInt EndThread()
	{
	RunningTime+=RunTimer.Stop();
	if (TheStatus==KRequestPending)
		TheThread.Kill(1);
	User::WaitForRequest(TheStatus);
	TInt r;
	if (TheThread.ExitType()==EExitKill)
		r=TheThread.ExitReason();
	else
		r=TheStatus.Int();
	TheThread.Close();
	return r;
	}

//aTestExecutionTime - desired test execution time in minutes
LOCAL_C void RunTestL(TInt aTestExecutionTime = 0)
	{
	__ASSERT_ALWAYS(aTestExecutionTime >= 0, User::Invariant());

	RThread().SetPriority(EPriorityMore);
	TheTest.Start(_L("Create the database"));
	CreateDatabaseL();

	TTimeIntervalMinutes timeInterval(aTestExecutionTime);

	TTime timeCurrent;
	timeCurrent.UniversalTime();
	TTime timeEnd(timeCurrent);
	timeEnd += timeInterval;

	for (TBool condition=ETrue; condition; condition = aTestExecutionTime > 0 ? (timeCurrent < timeEnd) : ETrue)
		{
		TheTest.Next(_L("Main loop"));
		TheTest.Start(_L("Kick off the thread"));
		TEST2 (StartThread(TheThread,TheStatus),KErrNone);
		// random delay
		for (;;)
			{
			User::After(95000);
			if (TheStatus!=KRequestPending)
				break;
			if (Random(1000)<30)
				break;
			}
		TheTest.Next(_L("End the thread"));
		TInt exit=EndThread();
		if (exit!=1)
		    TheTest.Printf(_L("  thread failed with error %d\n"),exit);
//
		++Shot;
		CFileStore* store=NULL;
		for (TInt ii=0;;++ii)
			{
			TheTest.Printf(_L("Opening %d\r"),ii);
			TRAPD(r,store=CFileStore::OpenL(TheFs,KTestDatabase,EFileRead|EFileWrite|EFileWriteDirectIO));
			if (r==KErrNone)
				break;
			TEST2(r, KErrInUse);
			User::After(100000);
			}
		TheTest.Next(_L("Verify & Recover"));
		TEST2 (Verify(*store),KErrNone);
		TInt64 tps(TransId);
		tps*=1000u;
		tps/=RunningTime;
		TheTest.Printf(_L("    Iteration %d, TPS %d, during commit %d%%\n"),Shot,I64LOW(tps),(100*ShotDuringCommit)/Shot);
		TInt r=Recover(*store);
		if (r==KErrNoMemory || r==KErrDiskFull)
			{	// need to compact before completing recovery
			TheTest.Next(_L("No space, compacting"));
			TEST2 (Compact(*store),KErrNone);
			TheTest.Next(_L("Verify & Recover again"));
			TEST2 (Verify(*store),KErrNone);
			r=Recover(*store);
			}
		TEST2 (r,KErrNone);
		TheTest.Next(_L("Verify & Compact"));
//		TEST2 (Verify(*store),KErrNone);
		TEST2 (Compact(*store),KErrNone);
		TheTest.Next(_L("Verify"));
		TEST2 (Verify(*store),KErrNone);
//
		delete store;
		TheTest.End();

		timeCurrent.UniversalTime();
		}
	TheTest.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0636
@SYMTestCaseDesc        DBMS stess testing.
@SYMTestPriority        Medium
@SYMTestActions         Tests for verifying the database integrity.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void RunVerify()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0636 Open store "));
	CFileStore* store=NULL;
	TRAPD(r,store=CFileStore::OpenL(TheFs,KTestDatabase,EFileRead|EFileWrite|EFileWriteDirectIO));
	TEST2 (r,KErrNone);
	TheTest.Next(_L("Verify"));
	NewCount=-1;
	TotalMonies=KInitialCash + (EPenny-EJohn+1)*KInitialBalance;
	TEST2 (Verify(*store),KErrNone);
	TheTest.Next(_L("Recover"));
	TEST2 (Recover(*store),KErrNone);
	TheTest.Next(_L("Verify"));
	TEST2 (Verify(*store),KErrNone);
	delete store;
	TheTest.End();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	TEST2(r,KErrNone);
//
	r=TheFs.MkDir(KTestDir);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	TEST2(r,KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C CTrapCleanup* setupCleanup()
    {
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TEST(cleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(r,KErrNone);
	return cleanup;
	}

//
// entry point
//
// Parameters usage:
//	t_stress [-v]|[0]|[<positive number>]
// Where:
//	-v - a verification test will be run;
//	0  - a stress test will be run for indefinite time;
//	<positive number>  - a stress test will be run for <positive number> minutes;
// If the test is run without arguments, the test execution time will be 10 minutes
// (KDefaultTestExecutionTime constant bellow).
GLDEF_C TInt E32Main()
    {
    TheTest.Title();
	setupTestDirectory();
	CTrapCleanup* cleanup=setupCleanup();
	__UHEAP_MARK;
//
	TBuf<100> cmd;
    User::CommandLine(cmd);
	TLex lex(cmd);
	TInt err = KErrNone;
	for(;;)
		{
		TPtrC arg(lex.NextToken());
		if(arg.Length() == 0)
			{
			const TInt KDefaultTestExecutionTime = 10;//default test execution time - minutes
			TRAP(err, RunTestL(KDefaultTestExecutionTime));
			break;
			}
		else if(arg.CompareF(_L("-v")) == 0)
			{
			RunVerify();
			break;
			}
		else
			{
			TInt32 testExecutionTime = 0;
			lex.Assign(arg);
			(void)lex.Val(testExecutionTime);
			TRAP(err, RunTestL(testExecutionTime));
			break;
			}
		}
	TInt err2 = TheFs.Delete(KTestDatabase);
	if(err2 != KErrNone)
		{
		RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err2, &KTestDatabase);
		}
	err2 = TheFs.Delete(KLogFile);
	if(err2 != KErrNone)
		{
		RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err2, &KLogFile);
		}
	TEST2(err, KErrNone);
//
	__UHEAP_MARKEND;
	delete cleanup;
	TheFs.Close();
	TheTest.Close();
	return 0;
    }
