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

#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <hal.h>

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_FRECL.DAT");
const TPtrC KTestText=_L("Reclamation testing in progress...");
const TInt 	KTestStreams=1000;
const TInt 	KTestReplicas=5000;

TFileName TheFileName; 

RTest 	TheTest(_L("t_storfrecl"));
RFs 	TheFs;
TInt64 	TheSeed;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteDataFile(const TDesC& aFullName)
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

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Boolean expression evaluated to false\r\n"));
		DeleteDataFile(TheFileName);
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteDataFile(TheFileName);
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static TInt TheCounterFreq = -10000000;
const TInt KMicroSecIn1Sec = 1000000;

TUint32 CalcTickDiff(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	return (TUint32)diffTicks;
	}

//Prints aFastCount parameter (converted to us) and the bytes count
void PrintFcDiffAsUs2(const TDesC& aFormatStr, TInt aBytes, TUint32 aFastCount)
	{
	if(TheCounterFreq <= 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
		TheTest.Printf(_L("Counter frequency=%d Hz\r\n"), TheCounterFreq);
		}
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, aBytes, v2);
	}

///////////////////////////////////////////////////////////////////////////////////////

TInt ReclaimL(CStreamStore& aStore)
	{
	TUint32 fc = User::FastCounter();
	TInt total = aStore.ReclaimL();
	TUint32 time = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs2(_L("### CStreamStore::ReclaimL(), %d bytes reclaimable, Time=%d us\r\n"), total, time);
	return total;
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1159
@SYMTestCaseDesc	    Tests CStreamStore::ReclaimL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reclaiming the space on the store.
                        Tests for an empty store,simple and a complex store.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void ReclaimTestL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1159 CPermanentFileStore::ReclaimL()"));
	
	TheTest.Printf(_L(" Empty store\r\n"));
	CFileStore* store = CPermanentFileStore::ReplaceLC(TheFs, TheFileName, EFileRead | EFileWrite);
	store->SetTypeL(store->Layout());
	TInt bytes = ReclaimL(*store);
	TEST2(bytes, 0);

	TheTest.Printf(_L(" Simple store\r\n"));
	RStoreWriteStream strm;
	TStreamId id=strm.CreateLC(*store);
	strm<<KTestText;
	strm.CommitL();
	strm.Release();
	store->CommitL();
	(void)ReclaimL(*store);

	TheTest.Printf(_L(" Complex store\r\n"));
	TInt ii;
	for (ii=0;ii<KTestStreams;++ii)
		{
		id=strm.CreateL(*store);
		TInt val=Math::Rand(TheSeed);
		TInt len=(val>>11)%(KTestText.Length()+1);
		if (len>0)
			strm<<KTestText.Left(len-1);
		strm.CommitL();
		strm.Release();
		if ((val>>7)&1)
			store->DeleteL(id);
		}
	store->CommitL();
	(void)ReclaimL(*store);

	TheTest.Printf(_L(" Large stream\r\n"));
	id=strm.CreateL(*store);
	for (ii=0;ii<KTestReplicas;++ii)
		strm<<KTestText;
	strm.CommitL();
	strm.Release();
	store->CommitL();
	(void)ReclaimL(*store);

	TheTest.Printf(_L(" Deleted large stream\r\n"));
	store->DeleteL(id);
	store->CommitL();
	(void)ReclaimL(*store);

	TheTest.Printf(_L(" Mixed reclaim and commit\r\n"));
	RStoreReclaim reclaim;
	TInt step;
	reclaim.OpenLC(*store,step);
	TRequestStatus status;
	TPckgBuf<TInt> pckgStep(step);
	TRAPD(r, reclaim.NextL(pckgStep,status));
	TEST2(r, KErrNone);
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	TEST(step>0);
	strm.CreateL(*store);
	strm<<KTestText;
	strm.CommitL();
	strm.Release();
	TRAP(r, reclaim.NextL(step));
	TEST(r!=KErrNone);
	reclaim.ResetL(step);
	r = reclaim.Next(step);
	TEST(r!=KErrNone);
	store->CommitL();
	TPckgBuf<TInt> pckgStep1(step);
	reclaim.Next(pckgStep1,status) ;
	User::WaitForRequest(status);
	TEST(status.Int() != KErrNone);
	reclaim.ResetL(step);
	while (step)
		{
		r = reclaim.Next(step);
		TEST2(r, KErrNone);
		}

	TInt total=reclaim.Available();
	CleanupStack::PopAndDestroy();
	TheTest.Printf(_L(" %d byte(s) reclaimable\n"),total);

	CleanupStack::Pop();	// strm
	CleanupStack::PopAndDestroy();	// store
	}

//Initialize TheFileName. RFs::Connect(). Prepare the test directory. 
void CreateTestEnv()
    {
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	TheFileName.Copy(parse.FullName());
	TheTest.Printf(_L("### Test file: %S\r\n"), &TheFileName);
    
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	err = TheFs.MkDirAll(TheFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

TInt E32Main()
    {
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	
	TRAPD(err, ReclaimTestL());
	::DeleteDataFile(TheFileName);
	TheFs.Close();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();

	delete tc;
	
	return 0;
    }

