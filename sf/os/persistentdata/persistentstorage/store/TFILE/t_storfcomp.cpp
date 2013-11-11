// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include <e32test.h>
#include <e32math.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_COMPACT.DAT");
_LIT(KResultsFile,"RESULTS_%d.TXT");

static TFileName TheResultsFile;
static TBool AllTests=ETrue;
static TInt Iterations=10000;
static TInt DataVolume=160;
static TInt CompactFrequency=40;

class RLog
	{
public:
	void CreateL(RFs& aFs, const TDesC& aFile);
	void Close();
	void PrintL(const char* aFormat, ...);
private:
	RFileWriteStream iLog;
	};

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storfcomp"));
LOCAL_D RFs TheFs;
LOCAL_D RLog TheLog;

class StopWatch
	{
public:
	void Start();
	TUint Stop();
private:
	TTime iTime;
	};

void StopWatch::Start()
	{
	iTime.UniversalTime();
	}

TUint StopWatch::Stop()
	{
 	TTime t;
 	t.UniversalTime();
 	TInt64 i = ((t.MicroSecondsFrom(iTime).Int64()) + 500)/1000;
 	return I64LOW(i);
	}

StopWatch CompactSW;

void CompactL(CFileStore& aStore)
//
// Compact the file and record the stats
//
	{
	aStore.CommitL();
	TInt startSize;
	User::LeaveIfError(aStore.File().Size(startSize));
	CompactSW.Start();
	TInt wasted = aStore.CompactL();
	aStore.CommitL();
	TUint ms = CompactSW.Stop();
	TInt endSize;
	User::LeaveIfError(aStore.File().Size(endSize));
	TheLog.PrintL("%d\t%d\t%d\t%u\n",startSize, endSize, wasted, ms);
	}

void ReclaimCompactL(CFileStore& aStore, TInt aTrigger)
//
// Reclaim the file and compact on trigger
//
	{
	TInt startSize;
	User::LeaveIfError(aStore.File().Size(startSize));
	CompactSW.Start();
	TBool compacted = EFalse;
	if (aTrigger == 0 || aStore.ReclaimL() * 100 > startSize * aTrigger)
		{
		aStore.CompactL();
		aStore.CommitL();
		compacted = ETrue;
		}
	TUint ms = CompactSW.Stop();
	TInt endSize;
	User::LeaveIfError(aStore.File().Size(endSize));
	TheLog.PrintL("%s\t%d\t%u\n",compacted ? "Compact" : "Reclaim", startSize - endSize, ms);
	}

void WriteBytesL(RWriteStream& s, TInt aCount)
	{
	const TInt KBufSize = 512;
	TUint8 buf[KBufSize];
	while (aCount > KBufSize)
		{
		s.WriteL(buf, KBufSize);
		aCount -= KBufSize;
		}
	s.WriteL(buf, aCount);
	s.CommitL();
	}

TStreamId CreateStreamL(CStreamStore& aStore, TInt aSize)
	{
	RStoreWriteStream s;
	TStreamId id = s.CreateLC(aStore);
	WriteBytesL(s, aSize);
	CleanupStack::PopAndDestroy(&s);
	return id;
	}

TInt Random(TUint aLimit)
	{
	return Math::Random() % aLimit;
	}


TBool AllocationFailure()
	{
	User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext,1);
	TAny* cell = User::Alloc(4);
	User::Free(cell);
	User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
	return cell == 0;
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1144
@SYMTestCaseDesc	    Tests for CFileStore::CompactL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for compaction process on the store.Tests for KErrNone flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void BasicCompactionTestsL()
	{
	const TInt KTestCount = 40;
	const TInt KTestSize = 50;
	TParsePtrC parse(KFileLocationSpec);
//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1144 "));
	CFileStore* store = CPermanentFileStore::ReplaceLC(TheFs, parse.NameAndExt(), EFileRead|EFileWrite);

	store->SetTypeL(store->Layout());
//
	TheLog.PrintL("Compact uncommitted empty store\n");
	TRAPD(r, store->CompactL());
	test (r == KErrNone);
//
	TheLog.PrintL("Compact committed empty store\n");
	r = store->Commit();
	test (r == KErrNone);
	TRAP(r, store->CompactL());
	test (r == KErrNone);
//
	TheLog.PrintL("Compact empty store with full TOC\n");
	TStreamId streams[KTestCount];
	TInt i;
	for (i = 0; i < KTestCount; ++i)
		streams[i] = CreateStreamL(*store, KTestSize);
	store->CommitL();
	for (i = 0; i < KTestCount; ++i)
		store->DeleteL(streams[i]);
	store->CommitL();
	TRAP(r, store->CompactL());
	test (r == KErrNone);
//
	TheLog.PrintL("Compact empty store with delta TOC\n");
	streams[0] = CreateStreamL(*store, KTestSize);
	store->CommitL();
	store->DeleteL(streams[0]);
	store->CommitL();
	TRAP(r, store->CompactL());
	test (r == KErrNone);
//
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1145
@SYMTestCaseDesc	    Testing fallback compaction algorithm
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for compaction process on the store,
                        If we have allocation failure we can test that the fallback algorithm is in place
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CompactionAlgorithmTestL()
	{
	const TInt KTestCount = 200;
	const TInt KTestSize = 50;
	const TInt KBeyondSuccess = 2;
	TInt stopat = KMaxTInt;
	TInt bestTime = -1;
	TParsePtrC parse(KFileLocationSpec);
//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1145 "));
	TheLog.PrintL("Testing fallback compaction algorithm\n");
//
	for (TInt failat = 1; failat <= stopat; ++failat)
		{
		TheLog.PrintL("Fail allocation #%d: ", failat);
//
		// prepare the store. Leave a single hole at the beginning
		CFileStore* store = CPermanentFileStore::ReplaceLC(TheFs, parse.NameAndExt(), EFileRead|EFileWrite);
		store->SetTypeL(store->Layout());
		TStreamId first = CreateStreamL(*store, KTestSize);
		for (TInt i = 1; i < KTestCount; ++i)
			CreateStreamL(*store, KTestSize);
		store->CommitL();
		store->DeleteL(first);
		store->CommitL();
		//
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext,failat);
		CompactSW.Start();
		TRAPD(r, store->CompactL();	store->CommitL();)
		TInt ms = CompactSW.Stop();
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		//
		if (r != KErrNone)
			{
			test (bestTime == -1);
			TheLog.PrintL("compaction failed\n");
			}
		else
			{
			TheLog.PrintL("compaction succeeded in %u ms\n", ms);
			if (bestTime == -1)
				{
				bestTime = ms;
				stopat = failat + KBeyondSuccess;		//stop after a few passes after sucsss
				}
			else if (ms < bestTime)
				{
				if (ms < bestTime/2)
					stopat = failat + KBeyondSuccess;	// new algorithm has kicked in
				bestTime = ms;
				}
			}

		CleanupStack::PopAndDestroy(store);
		}
	}


struct TTracker
	{
	TStreamId iId;
	TInt iSize;
	};

CFileStore* InitialiseStoreLC(RArray<TTracker>& aStreams, TInt aDataVolume, TInt aAverageSize)
	{
	TParsePtrC parse(KFileLocationSpec);	
	CFileStore* store = CPermanentFileStore::ReplaceLC(TheFs, parse.NameAndExt(), EFileRead|EFileWrite);
	store->SetTypeL(store->Layout());
	for (TInt count = Max(1,(aDataVolume + aAverageSize/2)/aAverageSize); count > 0; --count)
		{
		TInt size;
		if (count == 1)
			size = aDataVolume;
		else
			{
			size = aDataVolume / count;
			TInt spread = Min(aAverageSize, size);
			size += Random(spread) - spread/2;
			}
		TTracker e;
		e.iSize = size;
		e.iId = CreateStreamL(*store, size);
		User::LeaveIfError(aStreams.Append(e));
		aDataVolume -= size;
		}
	store->CommitL();
	return store;
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1146
@SYMTestCaseDesc	    Tests for compaction on store
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CFileStore::CompactL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CompactionTestL(const TInt aIterations, const TInt aDataVolume, const TInt aAverageSize, const TInt aCommitFrequency, const TInt aCompactionFrequency)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1146 "));
	TheLog.PrintL("CompactionTest : \n");
	TheLog.PrintL("\tIterations:\t%d\n",aIterations);
	TheLog.PrintL("\tDataVolume:\t%d\n",aDataVolume);
	TheLog.PrintL("\tAverageSize:\t%d\n",aAverageSize);
	TheLog.PrintL("\tCommitFrequency:\t%d\n",aCommitFrequency);
	TheLog.PrintL("\tCompactionFrequency:\t%d\n\n",aCompactionFrequency);

	RArray<TTracker> streams(8);
	CleanupClosePushL(streams);
	CFileStore* store = InitialiseStoreLC(streams, aDataVolume, aAverageSize);
	const TInt maxCount = streams.Count() + (streams.Count() / 5);
	TInt size = aDataVolume;
	const TInt span = aDataVolume / 10;		// +- 10%
	for (TInt i = aIterations ; --i >= 0; )
		{
		TInt from = Random(streams.Count());
		TInt to = Random(maxCount);
		TInt tfr = to == from ? 0 : Random(streams[from].iSize + 1);
		TInt adj = Random(span - 1) - span/2;
		if (size > aDataVolume + span/2)
			adj -= size - (aDataVolume + span/2);
		else if (size < aDataVolume - span/2)
			adj += (aDataVolume - span/2) - size;
		if (adj < 0)
			{
			if (adj < tfr - streams[from].iSize)
				adj = tfr - streams[from].iSize;
			}
		TInt fromSize = streams[from].iSize - tfr;
		if (adj < 0 || to == from)
			fromSize += adj;
		if (fromSize > 0)
			{
			RStoreWriteStream s;
			s.ReplaceLC(*store, streams[from].iId);
			WriteBytesL(s, fromSize);
			CleanupStack::PopAndDestroy(&s);
			streams[from].iSize = fromSize;
			}

		if (to != from)
			{
			if (to < streams.Count())
				{
				TInt toSize = streams[to].iSize + tfr;
				if (adj > 0)
					toSize += adj;
				RStoreWriteStream s;
				s.ReplaceLC(*store, streams[to].iId);
				WriteBytesL(s, toSize);
				CleanupStack::PopAndDestroy(&s);
				streams[to].iSize = toSize;
				}
			else
				{
				TInt toSize = tfr;
				if (adj > 0)
					toSize += adj;
				TTracker e;
				e.iSize = toSize;
				e.iId = CreateStreamL(*store, toSize);
				User::LeaveIfError(streams.Append(e));
				}
			}
		if (fromSize <= 0)
			{
			store->DeleteL(streams[from].iId);
			streams.Remove(from);
			}
		size += adj;
//
		if (Random(aCommitFrequency) == 0)
			{
			store->CommitL();
			if (aCompactionFrequency <= 0)
				ReclaimCompactL(*store, -aCompactionFrequency);
			}
		if (aCompactionFrequency > 0 && Random(aCompactionFrequency) == 0)
			CompactL(*store);
		}
	CleanupStack::PopAndDestroy(store);
	CleanupStack::PopAndDestroy(&streams);
	TheLog.PrintL("\nCompactionTestEnd\n\n");
	}

void RLog::CreateL(RFs& aFs, const TDesC& aFile)
	{
#ifdef __WINS__
	User::LeaveIfError(iLog.Replace(aFs, aFile, EFileWrite));
#endif
	}

void RLog::Close()
	{
	iLog.Close();
	}

void RLog::PrintL(const char* aFormat, ...)
	{
	VA_LIST list;
	VA_START(list,aFormat);
	TBuf8<256> b;
	b.FormatList(_L8(aFormat),list);
	VA_END(list);
#ifdef __WINS__
	iLog.WriteL(b);
#endif
	TBuf<256> b16;
	b16.Copy(b);
	test.Printf(_L("%S"),&b16);
	}

void GetOpt()
	{
	TBuf<256> options;
    User::CommandLine(options);
	if (options.Length() > 0)
		{
		AllTests = EFalse;
		TLex lex(options);
		lex.SkipSpace();
		if (lex.Val(DataVolume) == KErrNone)
			{
			lex.SkipSpace();
			if (lex.Val(Iterations) == KErrNone)
				{
				lex.SkipSpace();
				lex.Val(CompactFrequency);
				}
			}
		}
	TheResultsFile.Format(KResultsFile,DataVolume);
	}

void testCompactL()
	{
	GetOpt();
	TheLog.CreateL(TheFs, TheResultsFile);
	if (AllTests)
		{
		BasicCompactionTestsL();
		if (AllocationFailure())
			CompactionAlgorithmTestL();
		}
	CompactionTestL(Iterations, DataVolume<<10, 300, 5, CompactFrequency);
	TheLog.Close();
	}

LOCAL_C void setupTestDirectory()
//
// Prepare the test directory.
//
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	r=TheFs.MkDir(parse.DriveAndPath());
	test(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(parse.DriveAndPath());
	test(r==KErrNone);
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

class CTestStreamStore : public CStreamStore
	{
public:
	static CTestStreamStore* NewL();
	virtual ~CTestStreamStore();

private:
	CTestStreamStore();
	virtual MStreamBuf* DoReadL(TStreamId anId) const;
	virtual MStreamBuf* DoCreateL(TStreamId& anId);
	
	};

CTestStreamStore* CTestStreamStore::NewL()
	{
	return new (ELeave) CTestStreamStore; 
	}

CTestStreamStore::~CTestStreamStore()
	{
	}

CTestStreamStore::CTestStreamStore()
	{
	}

MStreamBuf* CTestStreamStore::DoReadL(TStreamId) const
	{
	return NULL;
	}

MStreamBuf* CTestStreamStore::DoCreateL(TStreamId&)
	{
	return NULL;
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4063
@SYMTestCaseDesc        CStreamStore tests.
@SYMTestActions         CStreamStore provides couple of virtual methods in its private section:
                        DoExtendL(), DoDeleteL(), DoReplaceL(), DoReclaimL().
                        They are no-ops and are expected to be overriden in the class derived from
                        CStreamStore. Their implementations leave with KErrNotsupported. 
                        The test uses a class derived from CStreamStore, which class does not implement
                        virtuals mentioned above. These virtuals should leave with KErrNotSupported when called.
@SYMTestPriority        High
@SYMTestExpectedResults Test must not fail
*/
void TestStreamStoreVirtualsL()
	{
	CTestStreamStore* store = CTestStreamStore::NewL();
	TRAPD(err, store->CommitL());
	test(err == KErrNone);
	TRAP(err, store->RevertL());
	test(err == KErrNotSupported);
	TRAP(err, store->ReclaimL());
	test(err == KErrNotSupported);
	TRAP(err, store->CompactL());
	test(err == KErrNotSupported);
	TRAP(err, store->DeleteL(TStreamId(1)));
	test(err == KErrNotSupported);
	delete store;
	}

//
// Test permanent file store.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test compaction"));
	TRAPD(r,testCompactL());
	test(r==KErrNone);
	test.Next(_L("@SYMTestCaseID:PDS-STORE-CT-4063: Test CStreamStore virtuals"));
	TRAP(r, TestStreamStoreVirtualsL())
	test(r==KErrNone);
	
	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::DeleteDataFile(parse.FullName());

	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
    }

