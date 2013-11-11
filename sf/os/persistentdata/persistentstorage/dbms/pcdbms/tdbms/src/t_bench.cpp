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
#ifndef __TOOLS2__	// we aren't interested in timings for tools2
#include <e32svr.h>
#include <hal.h>
#endif

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_BENCH.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_BENCH.CRC");
#endif
#endif

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

#ifndef __TOOLS2__
class TTimer
	{
public:
	void Start();
	TReal Stop() const;
private:
	TUint iTicks;
	};
#endif

LOCAL_D RTest test(_L("T_BENCH"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbView TheView;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;
//T_BENCH file shall not be deleted at the end of the test! It will be used by T_COMP test.

#ifdef __TOOLS2__
const TPtrC KTestDatabase=_L(".\\dbms-tst\\T_BENCH.DB");
#else
const TPtrC KTestDatabase=_L("C:\\dbms-tst\\T_BENCH.DB");
#endif
const TPtrC KTableName=_S("Test");
const TPtrC KColCluster=_S("Cluster");
const TPtrC KColXcluster=_S("xCluster");
const TPtrC KColRandom=_S("Random");
const TPtrC KColXrandom=_S("xRandom");
const TInt KRecords=2000;

#ifndef __TOOLS2__
static TTimer TheTimer;

void TTimer::Start()
	{
	iTicks=User::FastCounter();
	}

TReal TTimer::Stop() const
	{
	TUint ticks = User::FastCounter() - iTicks;
	TInt freq = 0;
	test(HAL::Get(HAL::EFastCounterFrequency, freq) == KErrNone);
	const TInt KMicroSecIn1Sec = 1000000;
	const TInt KMsIn1Sec = 1000;
	double v = ((double)ticks * KMicroSecIn1Sec) / (double)freq; TInt v2 = (TInt)v;
	return v2 / KMsIn1Sec;
	}
#endif

LOCAL_C void CloseDatabase()
	{
	TheDatabase.Close();
	TheCrcChecker.GenerateCrcL(KTestDatabase);
	}

/**
Create the database: keep the code 050 compatible

@SYMTestCaseID          SYSLIB-DBMS-CT-0577
@SYMTestCaseDesc        Benchmark Tests. Creation of a local Database test
@SYMTestPriority        Medium
@SYMTestActions        	Attempt to test RDbNamedDatabase::CreateTable(),RDbNamedDatabase::CreateIndex(),
						RDbNamedDatabase::Compact(),RDbView::Prepare() functions 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDatabase()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0577 "));
	User::LeaveIfError(TheDatabase.Replace(TheFs,KTestDatabase));
	CDbColSet& set=*CDbColSet::NewLC();
	TDbCol col(KColCluster,EDbColInt32);
	col.iAttributes=col.ENotNull;
	set.AddL(col);
	col.iName=KColXcluster;
	set.AddL(col);
	col.iName=KColRandom;
	set.AddL(col);
	col.iName=KColXrandom;
	set.AddL(col);
	TInt r=TheDatabase.CreateTable(KTableName,set);
	test (r==KErrNone);
	CleanupStack::PopAndDestroy();
#ifndef __TOOLS2__
	TheTimer.Start();
#endif
	r=TheView.Prepare(TheDatabase,_L("select * from test"),TheView.EInsertOnly);
	TheDatabase.Begin();
	test (r==KErrNone);
	TInt jj=0;
	for (TInt ii=0;ii<KRecords;++ii)
		{
		TheView.InsertL();
		jj=(jj+23);
		if (jj>=KRecords)
			jj-=KRecords;
		TheView.SetColL(1,ii);
		TheView.SetColL(2,ii);
		TheView.SetColL(3,jj);
		TheView.SetColL(4,jj);
		TheView.PutL();
		}
	r=TheDatabase.Commit();
	test (r==KErrNone);
	TheView.Close();
#ifndef __TOOLS2__
	test.Printf(_L("Build table: %7.1f ms\n"),TheTimer.Stop());
	TheTimer.Start();
#endif
	CDbKey& key=*CDbKey::NewLC();
	key.AddL(KColXcluster);
	key.MakeUnique();
	r=TheDatabase.CreateIndex(KColXcluster,KTableName,key);
	test (r==KErrNone);
#ifndef __TOOLS2__
	test.Printf(_L("Cluster index: %7.1f ms\n"),TheTimer.Stop());
	TheTimer.Start();
#endif
	key.Clear();
	key.AddL(KColXrandom);
	r=TheDatabase.CreateIndex(KColXrandom,KTableName,key);
	test (r==KErrNone);
	CleanupStack::PopAndDestroy();
#ifndef __TOOLS2__
	test.Printf(_L("Random index: %7.1f ms\n"),TheTimer.Stop());
	TheTimer.Start();
#endif
	r = TheDatabase.Compact();
#ifndef __TOOLS2__
	test.Printf(_L("Compact: %7.1f ms\n"),TheTimer.Stop());
#endif
	test (r == KErrNone);
	}

LOCAL_C TReal Evaluate(const TDesC& aSql)
	{
	TInt m=1;
	for (;;)
		{
#ifndef __TOOLS2__
		TheTimer.Start();
#endif
		for (TInt ii=0;ii<m;++ii)
			{
			TInt r=TheView.Prepare(TheDatabase,aSql,KDbUnlimitedWindow,TheView.EReadOnly);
			if (r<0)
				return r;
			r=TheView.EvaluateAll();
			test (r==KErrNone);
			TheView.Close();
			}
#ifndef __TOOLS2__
		TReal t=TheTimer.Stop();
		if (t>=100.0)
			return t/m;
		m*=4;
#else
		// We aren't interested in timings for tools2.
		return(1.0/m); // is this right?
#endif
		}
	}

struct TTest
	{
	const TText* iName;
	const TText* iQuery;
	};
const TTest KQuery[]=
	{
	{_S("project"),_S("select cluster,xcluster,random,xrandom from test")},
	{_S("restrict 1"),_S("select * from test where cluster=0")},
	{_S("restrict 2"),_S("select * from test where xrandom=0")},
	{_S("restrict 3"),_S("select * from test where xcluster<500 and xrandom <500")},
	{_S("order 1"),_S("select * from test order by xrandom")},
	{_S("order 2"),_S("select * from test order by cluster")},
	{_S("all 1"),_S("select * from test where random<500 order by xrandom")},
	{_S("all 2"),_S("select * from test where xcluster<500 order by xrandom")},
	{_S("all 3"),_S("select * from test where xcluster<500 order by xcluster")},
	{_S("all 4"),_S("select * from test where xcluster<500 and xrandom<200 order by xcluster")}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0578
@SYMTestCaseDesc        Benchmark Test.Querying a local Database Test
@SYMTestPriority        Medium
@SYMTestActions        	Evaluate SELECT queries on the created database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Queries()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0578 "));
	for (TUint ii=0;ii<sizeof(KQuery)/sizeof(KQuery[0]);++ii)
		{
		test.Printf(_L("%15s: "),KQuery[ii].iName);
		TReal t=Evaluate(TPtrC(KQuery[ii].iQuery));
		if (t<0.0)
			test.Printf(_L("-\n"));
		else
			test.Printf(_L("%7.1f ms\n"),t);
		}
	}

//
// Benchmark tests
//
LOCAL_C void BenchTest()
	{
	CreateDatabase();
	Queries();
	CloseDatabase();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
#if 0
	TDriveList drives;
	TheFs.DriveList(drives);
	if (drives[EDriveK] == KDriveAbsent)
		{
		TInt r = TheFs.AddFileSystem(_L("ELFFS"));
		test (r == KErrNone);
		r = TheFs.MountFileSystem(_L("Lffs"),EDriveK);
		if (r == KErrCorrupt || r == KErrNotReady) 
			{
			RFormat format;
			TInt    count;
			r = format.Open(TheFs, _L("K:\\"), EHighDensity, count);
			test (r == KErrNone);
			while (count)
				format.Next(count);
			format.Close();
    		}
		else
			test (r == KErrNone);
		}
#endif
//
	r=TheFs.MkDir(KTestDatabase);
	test(r==KErrNone || r==KErrAlreadyExists);
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

//
// entry point
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;

	test.Start(_L("Benchmarking..."));
	TRAPD(r,BenchTest();)
	test(r==KErrNone);

#ifndef __linux__
	TInt err;
#ifndef __TOOLS2__
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	test(err==KErrNone);
	test(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	test(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif // TOOLS2
#endif // linux

	test.End();

	__UHEAP_MARKEND;
	delete TheTrapCleanup;

	//T_BENCH.DB cannot be deleted here, because it is used by T_COMP test!

	TheFs.Close();
	test.Close();
	return 0;
    }
