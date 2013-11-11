// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <s32file.h>
#include <s32fileiter.h>
#include <s32mem.h>
#include <hal.h>

RTest TheTest(_L("t_storstreamperf"));
RFs TheFs;

TFileName TheTestFile;
TFileName TheTestDir;
TFileName TheTestDictFile;

const TUid KDictFileUid = {19445};
const TUid KDictStrmUid1 = {19446};
const TUid KDictStrmUid2 = {19447};

const TInt KBufSize = 30000;
const TInt KIterCnt = 1000;
const TInt KTestDataLen = KBufSize / KIterCnt;
const TUint8 KTestData[KTestDataLen] = 
	{
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3'
	};
const TPtrC8  KTestDes(KTestData, KTestDataLen);

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	(void)TheFs.Delete(TheTestDictFile);
	(void)TheFs.Delete(TheTestFile);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Boolean expression evaluated to false\r\n"));
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);
	
	err = TheFs.MkDirAll(TheTestFile);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

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

//Prints aFastCount parameter (converted to us)
void PrintFcDiffAsUs(const TDesC& aFormatStr, TUint32 aFastCount)
	{
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, v2);
	}

///////////////////////////////////////////////////////////////////////////////////////

//Testing RWriteStream performance.
void StreamWriteTestL(RWriteStream& aStream, TUint32& aWriteFc, TUint32& aCommitFc)
	{
	TUint32 fc = User::FastCounter();
	for(TInt i=0;i<KIterCnt;++i)
		{
		aStream.WriteL(KTestDes);
		}
	TUint32 fc2 = User::FastCounter();
	aStream.CommitL();
	TUint32 fc3 = User::FastCounter();
	
	aWriteFc = CalcTickDiff(fc, fc2);
	aCommitFc = CalcTickDiff(fc2, fc3);
	}

void DoStreamWriteTestL(RWriteStream& aStream)
	{
	TUint32 writeFc = 0, commitFc = 0;
	StreamWriteTestL(aStream, writeFc, commitFc);
	PrintFcDiffAsUs(_L("###     RWriteStream::WriteL(),  Time=%d us\r\n"), writeFc);
	PrintFcDiffAsUs(_L("###     RWriteStream::CommitL(), Time=%d us\r\n"), commitFc);
	}

//Testing RReadStream performance.
void StreamReadTestL(RReadStream& aStream, TUint32& aReadFc)
	{
	TBuf8<KTestDataLen> buf;
	TUint32 fc = User::FastCounter();
	for(TInt i=0;i<KIterCnt;++i)
		{
		aStream.ReadL(buf);
		}
	TUint32 fc2 = User::FastCounter();
	TEST(buf == KTestDes);

	aReadFc = CalcTickDiff(fc, fc2);
	}

void DoStreamReadTestL(RReadStream& aStream)
	{
	TUint32 readFc = 0;
	StreamReadTestL(aStream, readFc);
	PrintFcDiffAsUs(_L("###     RReadStream::ReadL(),   Time=%d us\r\n"), readFc);
	}

///////////////////////////////////////////////////////////////////////////////////////

void GetFastCounterFrequency()
	{
	TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
	TheTest.Printf(_L("Counter frequency=%d Hz\r\n"), TheCounterFreq);
	}

/**
@SYMTestCaseID			PDS-STORE-UT-4053
@SYMTestCaseDesc		Test for DEF141471 - STORE, new stream performance tests.
						PREQ2505 Insturmentation of PDS.
						RDictionaryWriteStream & RDictionaryReadStream performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141471 - STORE, new stream performance tests.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141471
*/
void DictionaryStreamTestL()
	{
	(void)TheFs.Delete(TheTestDictFile);
	CDictionaryFileStore* store = CDictionaryFileStore::OpenLC(TheFs, TheTestDictFile, KDictFileUid);
	
	//RDictionaryWriteStream::AssignL()
	RDictionaryWriteStream strm1;
	CleanupClosePushL(strm1);
	TUint32 fc = User::FastCounter();
	strm1.AssignL(*store, KDictStrmUid1);
	TUint32 assignFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RDictionaryWriteStream::AssignL(), Time=%d us\r\n"), assignFc);
	DoStreamWriteTestL(strm1);
	CleanupStack::PopAndDestroy(&strm1);
	
	//RDictionaryWriteStream::AssignLC()
	RDictionaryWriteStream strm2;
	fc = User::FastCounter();
	strm2.AssignLC(*store, KDictStrmUid2);
	assignFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RDictionaryWriteStream::AssignLC(), Time=%d us\r\n"), assignFc);
	DoStreamWriteTestL(strm2);
	CleanupStack::PopAndDestroy(&strm2);

	//RDictionaryReadStream::OpenL()
	RDictionaryReadStream strm3;
	CleanupClosePushL(strm3);
	fc = User::FastCounter();
	strm3.OpenL(*store, KDictStrmUid1);
	TUint32 openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RDictionaryReadStream::OpenL(), Time=%d us\r\n"), openFc);
	DoStreamReadTestL(strm3);
	CleanupStack::PopAndDestroy(&strm3);

	//RDictionaryReadStream::OpenLC()
	RDictionaryReadStream strm4;
	fc = User::FastCounter();
	strm4.OpenLC(*store, KDictStrmUid2);
	openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RDictionaryReadStream::OpenLC(), Time=%d us\r\n"), openFc);
	DoStreamReadTestL(strm4);
	CleanupStack::PopAndDestroy(&strm4);
	
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID			PDS-STORE-UT-4054
@SYMTestCaseDesc		Test for DEF141471 - STORE, new stream performance tests.
						PREQ2505 Insturmentation of PDS.
						RFileWriteStream & RFileReadStream performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141471 - STORE, new stream performance tests.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141471
*/
void FileStreamTestL()
	{
	(void)TheFs.Delete(TheTestFile);
	
	//RFileWriteStream::Create()
	RFileWriteStream strm1;
	TUint32 fc = User::FastCounter();
	TInt err = strm1.Create(TheFs, TheTestFile, EFileWrite | EFileRead);
	TUint32 createFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###  RFileWriteStream::Create(), Time=%d us\r\n"), createFc);
	strm1.Close();
	
	//RFileWriteStream::Replace()
	RFileWriteStream strm2;
	fc = User::FastCounter();
	err = strm2.Replace(TheFs, TheTestFile, EFileWrite | EFileRead);
	TUint32 replaceFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###  RFileWriteStream::Replace(), Time=%d us\r\n"), replaceFc);
	strm2.Close();
	
	//RFileWriteStream::Open()
	RFileWriteStream strm3;
	fc = User::FastCounter();
	err = strm3.Open(TheFs, TheTestFile, EFileWrite | EFileRead);
	TUint32 openFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###  RFileWriteStream::Open(), Time=%d us\r\n"), openFc);
	strm3.Close();
	
	//RFileWriteStream::Attach()
	RFile file;
	err = file.Open(TheFs, TheTestFile, EFileWrite | EFileRead);
	TEST2(err, KErrNone);
	RFileWriteStream strm4;
	CleanupClosePushL(strm4);
	fc = User::FastCounter();
	strm4.Attach(file);
	TUint32 attachFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RFileWriteStream::Attach(), Time=%d us\r\n"), attachFc);
	DoStreamWriteTestL(strm4);
	CleanupStack::PopAndDestroy(&strm4);
	
	//RFileWriteStream::Temp()
	RFileWriteStream strm5;
	CleanupClosePushL(strm5);
	TFileName fname;
	fc = User::FastCounter();
	err = strm5.Temp(TheFs, TheTestDir, fname, EFileWrite | EFileRead);
	TUint32 tempFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###  RFileWriteStream::Temp(), Time=%d us\r\n"), tempFc);
	DoStreamWriteTestL(strm5);
	CleanupStack::PopAndDestroy(&strm5);
	err = TheFs.Delete(fname);
	TEST2(err, KErrNone);
	
	//RFileReadStream::Open()
	RFileReadStream strm6;
	fc = User::FastCounter();
	err = strm6.Open(TheFs, TheTestFile, EFileRead);
	openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RFileReadStream::Open(), Time=%d us\r\n"), openFc);
	strm6.Close();

	//RFileReadStream::Attach()
	err = file.Open(TheFs, TheTestFile, EFileRead);
	TEST2(err, KErrNone);
	RFileReadStream strm7;
	fc = User::FastCounter();
	strm7.Attach(file);
	attachFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RFileReadStream::Attach(), Time=%d us\r\n"), attachFc);
	strm7.Close();

	//RFileReadStream::RFileReadStream(RFile&)
	err = file.Open(TheFs, TheTestFile, EFileRead);
	TEST2(err, KErrNone);
	fc = User::FastCounter();
	RFileReadStream strm8(file);
	TUint32 constrFc = CalcTickDiff(fc, User::FastCounter());
	CleanupClosePushL(strm8);
	PrintFcDiffAsUs(_L("###  RFileReadStream::RFileReadStream(RFile&), Time=%d us\r\n"), constrFc);
	DoStreamReadTestL(strm8);
	CleanupStack::PopAndDestroy(&strm8);
	}

/**
@SYMTestCaseID			PDS-STORE-UT-4055
@SYMTestCaseDesc		Test for DEF141471 - STORE, new stream performance tests.
						PREQ2505 Insturmentation of PDS.
						RMemWriteStream & RMemReadStream performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141471 - STORE, new stream performance tests.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141471
*/
void MemStreamTestL()
	{
	HBufC8* buf = HBufC8::NewLC(KBufSize);
	TPtr8 bufPtr = buf->Des();

	//RMemWriteStream::Open()	
	RMemWriteStream strm1;
	TUint32 fc = User::FastCounter();
	strm1.Open(const_cast <TUint8*> (bufPtr.Ptr()), KBufSize);
	TUint32 openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RMemWriteStream::Open(), Time=%d us\r\n"), openFc);
	strm1.Close();
	
	//RMemWriteStream::RMemWriteStream(TAny*,...)	
	fc = User::FastCounter();
	RMemWriteStream strm2(const_cast <TUint8*> (bufPtr.Ptr()), KBufSize);
	TUint32 constrFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RMemWriteStream::RMemWriteStream(TAny*,...), Time=%d us\r\n"), constrFc);
	CleanupClosePushL(strm2);
	DoStreamWriteTestL(strm2);
	CleanupStack::PopAndDestroy(&strm2);
	
	//RMemReadStream::Open()
	RMemReadStream strm3;
	fc = User::FastCounter();
	strm3.Open(bufPtr.Ptr(), KBufSize);
	openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RMemReadStream::Open(), Time=%d us\r\n"), openFc);
	strm3.Close();

	//RMemReadStream::RMemReadStream(TAny*,...)	
	fc = User::FastCounter();
	RMemReadStream strm4(bufPtr.Ptr(), KBufSize);
	constrFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RMemReadStream::RMemReadStream(TAny*,...), Time=%d us\r\n"), openFc);
	CleanupClosePushL(strm4);
	DoStreamReadTestL(strm4);
	CleanupStack::PopAndDestroy(&strm4);
	
	CleanupStack::PopAndDestroy(buf);
	}

/**
@SYMTestCaseID			PDS-STORE-UT-4056
@SYMTestCaseDesc		Test for DEF141471 - STORE, new stream performance tests.
						PREQ2505 Insturmentation of PDS.
						RBufWriteStream & RBufReadStream performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141471 - STORE, new stream performance tests.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141471
*/
void BufStreamTestL()
	{
	CBufFlat* bufFlat = CBufFlat::NewL(KTestDataLen);
	CleanupStack::PushL(bufFlat);
	
	//RBufWriteStream::Open()
	RBufWriteStream strm1;
	CleanupClosePushL(strm1);
	TUint32 fc = User::FastCounter();
	strm1.Open(*bufFlat);
	TUint32 openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RBufWriteStream::Open(), Time=%d us\r\n"), openFc);
	DoStreamWriteTestL(strm1);
	CleanupStack::PopAndDestroy(&strm1);
	TheTest.Printf(_L("       Buffer size=%d\r\n"), bufFlat->Size());

	//RBufWriteStream::Append()
	RBufWriteStream strm2;
	CleanupClosePushL(strm2);
	fc = User::FastCounter();
	strm2.Append(*bufFlat);
	TUint32 appendFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RBufWriteStream::Append(), Time=%d us\r\n"), appendFc);
	DoStreamWriteTestL(strm2);
	CleanupStack::PopAndDestroy(&strm2);
	TheTest.Printf(_L("       Buffer size=%d\r\n"), bufFlat->Size());
	
	//RBufWriteStream::Insert()
	RBufWriteStream strm3;
	CleanupClosePushL(strm3);
	fc = User::FastCounter();
	strm3.Insert(*bufFlat, KBufSize);
	TUint32 insertFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RBufWriteStream::Insert(), Time=%d us\r\n"), insertFc);
	DoStreamWriteTestL(strm3);
	CleanupStack::PopAndDestroy(&strm3);
	TheTest.Printf(_L("       Buffer size=%d\r\n"), bufFlat->Size());
	
	//RBufWriteStream::Truncate()
	RBufWriteStream strm4;
	CleanupClosePushL(strm4);
	fc = User::FastCounter();
	strm4.Truncate(*bufFlat);
	TUint32 truncateFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RBufWriteStream::Truncate(), Time=%d us\r\n"), truncateFc);
	DoStreamWriteTestL(strm4);
	CleanupStack::PopAndDestroy(&strm4);
	TheTest.Printf(_L("       Buffer size=%d\r\n"), bufFlat->Size());
	
	RBufReadStream strm5;
	CleanupClosePushL(strm5);
	fc = User::FastCounter();
	strm5.Open(*bufFlat);
	openFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  RBufReadStream::Open(), Time=%d us\r\n"), openFc);
	DoStreamReadTestL(strm5);
	CleanupStack::PopAndDestroy(&strm5);
	
	CleanupStack::PopAndDestroy(bufFlat);
	}

/** 
@SYMTestCaseID			PDS-STORE-UT-4057
@SYMTestCaseDesc		Test for DEF141471 - STORE, new stream performance tests.
						PREQ2505 Insturmentation of PDS.
						RPermanentFileStoreIter performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141471 - STORE, new stream performance tests.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141471
*/
void PermanentFileStoreIterTestL()
	{
	(void)TheFs.Delete(TheTestFile);

	CPermanentFileStore* store = CPermanentFileStore::ReplaceLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	
	//Create streams
	const TInt KStreamCnt = 10;
	TStreamId streamId[KStreamCnt] = {0}; 	
	TheTest.Printf(_L("###  CPermanentFileStore, create %d streams\r\n"), KStreamCnt);
	for(TInt i=0;i<KStreamCnt;i++)
		{
		RStoreWriteStream out;
		streamId[i] = out.CreateLC(*store);
		TheTest.Printf(_L("###    Stream##%02d, streamId=%08X\r\n"), i + 1, streamId[i].Value());
		DoStreamWriteTestL(out);
		CleanupStack::PopAndDestroy(&out);
		}
	TUint32 fc = User::FastCounter();
	store->CommitL();
	TUint32 commitFc = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(store);
	PrintFcDiffAsUs(_L("###  RPermanentFileStoreIter::CommitL(), Time=%d us\r\n"), commitFc);
	
	//RPermanentFileStoreIter::ResetL()
	store = CPermanentFileStore::OpenLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	RPermanentFileStoreIter it1;
	CleanupClosePushL(it1);
	fc = User::FastCounter();
	it1.ResetL(*store);
	TUint32 resetFc = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(&it1);
	CleanupStack::PopAndDestroy(store);
	PrintFcDiffAsUs(_L("###  RPermanentFileStoreIter::ResetL(), Time=%d us\r\n"), resetFc);

	//RPermanentFileStoreIter::ResetLC()
	store = CPermanentFileStore::OpenLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	RPermanentFileStoreIter it2;
	fc = User::FastCounter();
	it2.ResetLC(*store);
	resetFc = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(&it2);
	CleanupStack::PopAndDestroy(store);
	PrintFcDiffAsUs(_L("###  RPermanentFileStoreIter::ResetLC(), Time=%d us\r\n"), resetFc);

	//RPermanentFileStoreIter - construction & destruction
	store = CPermanentFileStore::OpenLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	fc = User::FastCounter();
	RPermanentFileStoreIter it3;
	CleanupClosePushL(it3);
	it3.ResetL(*store);
	CleanupStack::PopAndDestroy(&it3);
	TUint32 fc2 = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(store);
	PrintFcDiffAsUs(_L("###  RPermanentFileStoreIter - construction & destruction, Time=%d us\r\n"), fc2);

	//RPermanentFileStoreIter::NextL()
	store = CPermanentFileStore::OpenLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	RPermanentFileStoreIter it4;
	it4.ResetLC(*store);
	TStreamId id(KNullStreamIdValue);
	TInt cnt = 0;
	fc = User::FastCounter();
	while((id = it4.NextL()) != KNullStreamIdValue)
		{
		++cnt;
		}
	TUint32 nextFc = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(&it4);
	CleanupStack::PopAndDestroy(store);
	TEST2(cnt, KStreamCnt);
	PrintFcDiffAsUs(_L("###  RPermanentFileStoreIter::NextL(), Time=%d us\r\n"), nextFc);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-4007
@SYMTestCaseDesc	    CDirectFileStore::ReplaceLC() and SetTypeL() performance test
@SYMTestPriority 	    High
@SYMTestActions  	    Creates new CDirectFileStore object and measures the time of the operation. 
@SYMTestExpectedResults Tests set type time 
@SYMCR                  ATHE-7CQP8H
*/
void CreateDirectFileStoreTestL()
	{	
	(void)TheFs.Delete(TheTestFile);
	    
    TUint32 fc = User::FastCounter();
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	// Must say what kind of file store
	// SetTypeL() calls RFileBuf::EndL() which used to call RFile::Size()
	store->SetTypeL(KDirectFileStoreLayoutUid);
	TUint32 time = CalcTickDiff(fc, User::FastCounter());
	CleanupStack::PopAndDestroy(store);
	PrintFcDiffAsUs(_L("###  CDirectFileStore::ReplaceLC() & SetTypeL(), Time=%d us\r\n"), time);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-4008
@SYMTestCaseDesc	    Tests performance when calling CFileStore::Destruct()
@SYMTestPriority 	    High
@SYMTestActions  	    Creates and deletes a File Store 
@SYMTestExpectedResults Tests deletion time 
@SYMCR                  ATHE-7CQP8H
*/
void DirectFileStoreTestL()
	{	
	(void)TheFs.Delete(TheTestFile);
	
    TUint32 fc = User::FastCounter();
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFs, TheTestFile, EFileWrite | EFileRead);
	//then delete it. this calls CFileStore::Destruct(), which used to call Revert()
	CleanupStack::PopAndDestroy(store);
	TUint32 time = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###  CDirectFileStore construction & destruction, Time=%d us\r\n"), time);
	}

void DoTestsL()
	{
	TheTest.Start(_L("Get fast counter frequency"));
	GetFastCounterFrequency();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-STORE-UT-4053 RDictionaryWriteStream & RDictionaryReadStream - performance test"));
	DictionaryStreamTestL();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-STORE-UT-4054 RFileWriteStream & RFileReadStream - performance test"));
	FileStreamTestL();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-STORE-UT-4055 RMemWriteStream & RMemReadStream - performance test"));
	MemStreamTestL();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-STORE-UT-4056 RBufWriteStream & RBufReadStream - performance test"));
	BufStreamTestL();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-STORE-UT-4057 RPermanentFileStoreIter - performance test"));
	PermanentFileStoreIterTestL();
	
	TheTest.Next(_L(" @SYMTestCaseID SYSLIB-STORE-CT-4007 CDirectFileStore::ReplaceLC() & SetTypeL() performance test"));
	CreateDirectFileStoreTestL();
	
	TheTest.Next(_L(" @SYMTestCaseID SYSLIB-STORE-CT-4008 CDirectFileStore construction & destruction performance test"));
	DirectFileStoreTestL();
	}

//Usage: "t_streamperf [<drive letter>:]]"
TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
		
	TBuf<256> cmdline;
	User::CommandLine(cmdline);

	TParse parse;

	_LIT(KTestFile, "c:\\stor-tst\\t_streamperf.dat");
	parse.Set(cmdline, &KTestFile, 0);
	TheTestFile.Copy(parse.FullName());

	_LIT(KTestPath, "c:\\stor-tst\\");
	parse.Set(cmdline, &KTestPath, 0);
	TheTestDir.Copy(parse.FullName());
	
	_LIT(KDictFile, "c:\\stor-tst\\dicfile.ini");
	parse.Set(cmdline, &KDictFile, 0);
	TheTestDictFile.Copy(parse.FullName());

	__UHEAP_MARK;

	CreateTestEnv();
	TRAPD(err, DoTestsL());
	TEST2(err, KErrNone);
	DestroyTestEnv();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
