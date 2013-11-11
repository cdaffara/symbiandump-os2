// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

RTest TheTest(_L("t_storetoc test"));
RFs TheFs;

TBuf<KMaxFileName> TheFileName;

CPermanentFileStore* TheStore = NULL;

void DeleteTestFiles()
	{
	delete TheStore;
	TheStore = NULL;
	(void)TheFs.Delete(TheFileName);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestEnv(TInt aDriveNo)
    {
    TChar ch;
	TInt err = TheFs.DriveToChar(aDriveNo, ch);
	TEST2(err, KErrNone);
    TBuf<2> drvName;
    drvName.SetLength(2);
    drvName[0] = ch;
    drvName[1] = TChar(':');

	_LIT(KTestFile, "c:\\stor-tst\\t_storetoc.dat");

    TParse parse;
    parse.Set(drvName, &KTestFile, 0);
	const TDesC& dbFilePath = parse.FullName();
	TheFileName.Copy(dbFilePath);

	err = TheFs.MkDir(parse.DriveAndPath());
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////              TTocTestBase        ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Base TOC test class.
//It offers the folowing functions: Prepare(), Test() and Verify().
//The test algorithm is:
//  =================================
//  void TestFunc(TTocTestBase& aTestObj)
//  {
//	  aTestObj.Prepare();
//    [simulate file system error];
//	  TInt err = aTestObj.Test();
//    [clear file system error];
//	  aTestObj.Verify(err);
//  }
//  =================================
//The class has two private pure virtual methods, which have to be overriden in derived classes:
//  DoPrepareL() - instance specific test preparation;
//  DoVerify()   - instance specific test verification;
class TTocTestBase
	{
public:
	TTocTestBase()
		{
		TheStore = NULL;
		}
	void Prepare()
		{
		TEST(TheStore == NULL);
		TRAPD(err, DoPrepareL());
		TEST2(err, KErrNone);
		}
	TInt Test()
		{
		TEST(TheStore != NULL);
		TRAPD(err, {TheStore->CompactL(); TheStore->CommitL();});
		delete TheStore;
		TheStore = NULL;
		return err;
		}
	void Verify(TInt aTestErr)
		{
		TEST(TheStore == NULL);
		TRAPD(err, DoVerifyL(aTestErr));
		TEST2(err, KErrNone);
		}

protected:
	TStreamId WriteNewStreamL(const TDesC8& aStreamData)
		{
		TEST(TheStore != NULL);
		RStoreWriteStream stream;
		TStreamId streamId = stream.CreateLC(*TheStore);
		stream.WriteL(aStreamData);
		stream.CommitL();
		TheStore->CommitL();
		CleanupStack::PopAndDestroy(&stream);
		return streamId;
		}

private:
	virtual void DoPrepareL() = 0;
	virtual void DoVerifyL(TInt aTestErr) = 0;

	};

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////                TBaseTocEntryTest        ////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//Do not change/shift the next lines, because "new line/space" characters will be inserted in the string!
_LIT8(KStreamData1, "-01-ABCD-02-ABCD-03-ABCD-04-ABCD-05-ABCD-06-ABCD-07-ABCD-08-ABCD");
_LIT8(KStreamData1_2, "ABCDEFGH-41-ABCD-42-ABCD-43-A\
-33-ABCD-34-ABCD-35-ABCD-36-ABCD-37-ABCD-38-ABCD-39-ABCD-40-ABCD\
-25-ABCD-26-ABCD-27-ABCD-28-ABCD-29-ABCD-30-ABCD-31-ABCD-32-ABCD\
-17-ABCD-18-ABCD-19-ABCD-20-ABCD-21-ABCD-22-ABCD-23-ABCD-24-ABCD\
-09-ABCD-10-ABCD-11-ABCD-12-ABCD-13-ABCD-14-ABCD-15-ABCD-16-ABCD");
_LIT8(KStreamData1_3, "0123456789");

//"Base TOC entry" test.
//When a stream is relocated, its base TOC entry needs 2 "file write" (for "atomic block write" media) operations, because
//the TOC entry is split over a block boundary.
//This test class uses a set of test data, which puts the TOC entry of the relocated stream at position
//510 in the file (TOC entry length - 5 bytes).
class TBaseTocEntryTest : public TTocTestBase
	{
public:
	TBaseTocEntryTest() :
		iStreamId1(KNullStreamIdValue),
		iStreamId2(KNullStreamIdValue),
		iStreamId3(KNullStreamIdValue)
		{
		}

private:
	void DoPrepareL()
		{
		TheStore = CPermanentFileStore::ReplaceLC(TheFs, TheFileName, EFileWrite);
		TheStore->SetTypeL(TheStore->Layout());
		TheStore->CommitL();

		iStreamId1 = WriteNewStreamL(KStreamData1);
		TheStore->SetRootL(iStreamId1);
		TheStore->CommitL();
		iStreamId2 = WriteNewStreamL(KStreamData1_2);
		iStreamId3 = WriteNewStreamL(KStreamData1_3);

		TheStore->DeleteL(iStreamId2);
		TheStore->CommitL();

		CleanupStack::Pop(TheStore);
		}

	virtual void DoVerifyL(TInt)
		{
		TEST(TheStore == NULL);
		TheStore = CPermanentFileStore::OpenLC(TheFs, TheFileName, EFileWrite);
		RPermanentFileStoreIter iter;
		iter.ResetLC(*TheStore);
		//Stream 1
		TStreamId streamId = iter.NextL();
		TEST(streamId == iStreamId1);
		RStoreReadStream strm;
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData1().Length());
		TBuf8<sizeof(KStreamData1)> buf;
		strm.ReadL(buf, KStreamData1().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData1().Ptr(), KStreamData1().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 2
		streamId = iter.NextL();
		TEST(streamId == iStreamId3);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData1_3().Length());
		strm.ReadL(buf, KStreamData1_3().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData1_3().Ptr(), KStreamData1_3().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//No more streams
		streamId = iter.NextL();
		TEST(streamId == KNullStreamIdValue);
		//Cleanup
		CleanupStack::PopAndDestroy(&iter);
		CleanupStack::PopAndDestroy(TheStore);
		TheStore = NULL;
		}

private:
	TStreamId iStreamId1;
	TStreamId iStreamId2;
	TStreamId iStreamId3;

	};

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////              TDeltaTocEntryTest        /////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//Do not change/shift the next lines, because "new line/space" characters will be inserted in the string!
_LIT8(KStreamData2, "\
-01-ABCD-02-ABCD-03-ABCD-04-ABCD-05-ABCD-06-ABCD-07-ABCD-08-ABCD\
-09-ABCD-10-ABCD-11-ABCD-12");
_LIT8(KStreamData2_2, "ABC");
_LIT8(KStreamData2_3, "012345678");

//"Delta TOC entry" test
//When a stream is relocated, its delta TOC entry needs 2 "file write" (for "atomic block write" media) operations, because
//the TOC entry is split over a block boundary.
//This test class uses a set of test data, which puts the TOC entry of the relocated stream at position
//507 in the file (TOC entry length - 8 bytes), so the last 4 bytes (stream offset) are split.
class TDeltaTocEntryTest : public TTocTestBase
	{
public:
	TDeltaTocEntryTest() :
		iStreamId1(KNullStreamIdValue),
		iStreamId2(KNullStreamIdValue),
		iStreamId3(KNullStreamIdValue),
		iStreamId4(KNullStreamIdValue),
		iStreamId5(KNullStreamIdValue)
		{
		}

protected:
	virtual void DoPrepareL()
		{
		TheStore = CPermanentFileStore::ReplaceLC(TheFs, TheFileName, EFileWrite);
		TheStore->SetTypeL(TheStore->Layout());
		TheStore->CommitL();

		iStreamId1 = WriteNewStreamL(KStreamData2);
		TheStore->SetRootL(iStreamId1);
		TheStore->CommitL();
		iStreamId2 = WriteNewStreamL(KStreamData2);
		iStreamId3 = WriteNewStreamL(KStreamData2().Left(KStreamData2().Length() - 1));
		iStreamId4 = WriteNewStreamL(KStreamData2_2);
		iStreamId5 = WriteNewStreamL(KStreamData2_3);

		TheStore->DeleteL(iStreamId4);
		TheStore->CommitL();

		CleanupStack::Pop(TheStore);
		}

	virtual void DoVerifyL(TInt)
		{
		TEST(TheStore == NULL);
		TheStore = CPermanentFileStore::OpenLC(TheFs, TheFileName, EFileWrite);
		RPermanentFileStoreIter iter;
		iter.ResetLC(*TheStore);
		//Stream 1
		TStreamId streamId = iter.NextL();
		TEST(streamId == iStreamId1);
		RStoreReadStream strm;
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData2().Length());
		TBuf8<sizeof(KStreamData2)> buf;
		strm.ReadL(buf, KStreamData2().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData2().Ptr(), KStreamData2().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 2
		streamId = iter.NextL();
		TEST(streamId == iStreamId2);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData2().Length());
		strm.ReadL(buf, KStreamData2().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData2().Ptr(), KStreamData2().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 3
		streamId = iter.NextL();
		TEST(streamId == iStreamId3);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData2().Length() - 1);
		strm.ReadL(buf, KStreamData2().Length() - 1);
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData2().Ptr(), KStreamData2().Length() - 1), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 4
		streamId = iter.NextL();
		TEST(streamId == iStreamId5);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData2_3().Length());
		strm.ReadL(buf, KStreamData2_3().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData2_3().Ptr(), KStreamData2_3().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//No more streams
		streamId = iter.NextL();
		TEST(streamId == KNullStreamIdValue);
		//Cleanup
		CleanupStack::PopAndDestroy(&iter);
		CleanupStack::PopAndDestroy(TheStore);
		TheStore = NULL;
		}

private:
	TStreamId iStreamId1;
	TStreamId iStreamId2;
	TStreamId iStreamId3;
	TStreamId iStreamId4;
	TStreamId iStreamId5;

	};

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////              TBaseTocReferenceTest        //////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//Do not change/shift the next lines, because "new line/space" characters will be inserted in the string!
_LIT8(KStreamData3, "\
-01-ABCD-02-ABCD-03-ABCD-04-ABCD-05-ABCD-06-ABCD-07-ABCD-08-ABCD\
-09-ABCD-10-ABCD-11-ABCD-12-ABCD");
_LIT8(KStreamData3_2, "ABCDEFGH");
_LIT8(KStreamData3_3, "0123456789");

//"Base TOC reference" test.
//When the base TOC is relocated, the delta TOC reference to the base TOC has to be updated with
// 2 "file write" operations (for "atomic block write" media), because the reference is split over a block boundary.
//This test class uses a set of test data, which puts the delta TOC reference to the base TOC
//at position 511 in the file (reference length - 4 bytes)
class TBaseTocReferenceTest : public TTocTestBase
	{
public:
	TBaseTocReferenceTest() :
		iStreamId1(KNullStreamIdValue),
		iStreamId2(KNullStreamIdValue),
		iStreamId3(KNullStreamIdValue),
		iStreamId4(KNullStreamIdValue),
		iStreamId5(KNullStreamIdValue)
		{
		}

protected:
	virtual void DoPrepareL()
		{
		TheStore = CPermanentFileStore::ReplaceLC(TheFs, TheFileName, EFileWrite);
		TheStore->SetTypeL(TheStore->Layout());
		TheStore->CommitL();

		iStreamId1 = WriteNewStreamL(KStreamData3);
		TheStore->SetRootL(iStreamId1);
		TheStore->CommitL();
		iStreamId2 = WriteNewStreamL(KStreamData3);
		iStreamId3 = WriteNewStreamL(KStreamData3().Left(KStreamData3().Length() - 3));
		iStreamId4 = WriteNewStreamL(KStreamData3_2);
		iStreamId5 = WriteNewStreamL(KStreamData3_3);

		TheStore->DeleteL(iStreamId4);
		TheStore->CommitL();

		CleanupStack::Pop(TheStore);
		}

	virtual void DoVerifyL(TInt)
		{
		TEST(TheStore == NULL);
		TheStore = CPermanentFileStore::OpenLC(TheFs, TheFileName, EFileWrite);
		RPermanentFileStoreIter iter;
		iter.ResetLC(*TheStore);
		//Stream 1
		TStreamId streamId = iter.NextL();
		TEST(streamId == iStreamId1);
		RStoreReadStream strm;
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData3().Length());
		TBuf8<sizeof(KStreamData3)> buf;
		strm.ReadL(buf, KStreamData3().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData3().Ptr(), KStreamData3().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 2
		streamId = iter.NextL();
		TEST(streamId == iStreamId2);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData3().Length());
		strm.ReadL(buf, KStreamData3().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData3().Ptr(), KStreamData3().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 3
		streamId = iter.NextL();
		TEST(streamId == iStreamId3);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData3().Length() - 3);
		strm.ReadL(buf, KStreamData3().Length() - 3);
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData3().Ptr(), KStreamData3().Length() - 3), 0);
		CleanupStack::PopAndDestroy(&strm);
		//Stream 4
		streamId = iter.NextL();
		TEST(streamId == iStreamId5);
		strm.OpenLC(*TheStore, streamId);
		TEST2(strm.Source()->SizeL(), KStreamData3_3().Length());
		strm.ReadL(buf, KStreamData3_3().Length());
		TEST2(Mem::Compare(buf.Ptr(), buf.Length(), KStreamData3_3().Ptr(), KStreamData3_3().Length()), 0);
		CleanupStack::PopAndDestroy(&strm);
		//No more streams
		streamId = iter.NextL();
		TEST(streamId == KNullStreamIdValue);
		//Cleanup
		CleanupStack::PopAndDestroy(&iter);
		CleanupStack::PopAndDestroy(TheStore);
		TheStore = NULL;
		}

private:
	TStreamId iStreamId1;
	TStreamId iStreamId2;
	TStreamId iStreamId3;
	TStreamId iStreamId4;
	TStreamId iStreamId5;

	};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void TocTest(TTocTestBase& aTestObj)
	{
	TInt err = KErrGeneral;
	for(TInt cnt=1;err!=KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d\r"), cnt);
		aTestObj.Prepare();
		(void)TheFs.SetErrorCondition(KErrGeneral, cnt);
		err = aTestObj.Test();
		(void)TheFs.SetErrorCondition(KErrNone);
		aTestObj.Verify(err);
		}
	TheTest.Printf(_L("\n"));
	TEST2(err, KErrNone);
	aTestObj.Verify(err);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3481
@SYMTestCaseDesc        The test is performed on media, where atomic "block write" operations are guaranteed.
						In a "file I/O error" simulation loop:
						 - Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						   such that a base TOC entry is split over the block boundary;
						 - Compacts the store;
						 - Commits;
@SYMTestPriority        High
@SYMTestActions         Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						such that a base TOC entry is split over the block boundary. When the store is compacted
						and the used media supports atomic "block write" operations, the STORE will update
						the base TOC entry, which requires a "file write" operation crossing the block boundary.
@SYMTestExpectedResults The test should not fail or panic.
@SYMREQ                 REQ0000
*/
void BaseTocEntryTest()
	{
	TBaseTocEntryTest baseTocEntryTestObj;
	TocTest(baseTocEntryTestObj);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3482
@SYMTestCaseDesc        The test is performed on media, where atomic "block write" operations are guaranteed.
						In a "file I/O error" simulation loop:
						 - Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						   such that a delta TOC entry is split over the block boundary;
						 - Compacts the store;
						 - Commits;
@SYMTestPriority        High
@SYMTestActions         Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						such that a delta TOC entry is split over the block boundary. When the store is compacted
						and the used media supports atomic "block write" operations, the STORE will update
						the delta TOC entry, which requires a "file write" operation crossing the block boundary.
@SYMTestExpectedResults The test should not fail or panic.
@SYMREQ                 REQ0000
*/
void DeltaTocEntryTest()
	{
	TDeltaTocEntryTest deltaTocEntryTestObj;
	TocTest(deltaTocEntryTestObj);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3483
@SYMTestCaseDesc        The test is performed on media, where atomic "block write" operations are guaranteed.
						In a "file I/O error" simulation loop:
						 - Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						   such that a base TOC reference is split over the block boundary;
						 - Compacts the store;
						 - Commits;
@SYMTestPriority        High
@SYMTestActions         Creates a CPermanentStoreFile object, stores there carefully selected set of test data,
						such that a base TOC reference is split over the block boundary. When the store is compacted
						and the used media supports atomic "block write" operations, the STORE will update
						the base TOC reference, which requires a "file write" operation crossing the block boundary.
@SYMTestExpectedResults The test should not fail or panic.
@SYMREQ                 REQ0000
*/
void BaseTocReferenceTest()
	{
	TBaseTocReferenceTest baseTocReferenceTestObj;
	TocTest(baseTocReferenceTestObj);
	}

///////////////////////////////////////////////////////////////////////////////////////

void DoTests()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3481 Base TOC entry test "));
	BaseTocEntryTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3482 Delta TOC entry test "));
	DeltaTocEntryTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3483 Base TOC reference test "));
	BaseTocReferenceTest();
	}

//The function returns true, if the file system guarantees atomic "block write" operations on aDriveNo.
TBool IsBlockAtomic(TInt aDriveNo)
	{
	__ASSERT_DEBUG(aDriveNo >= EDriveA && aDriveNo <= EDriveZ, User::Invariant());

	TVolumeIOParamInfo volInfo;
	TInt err = TheFs.VolumeIOParam(aDriveNo, volInfo);
	//If VolumeIOParam() succeeds, the media block size is >= 512 bytes and the media block size is power of two - report
	//that the media supports atomic "block write" operations.
	const TInt KDefaultMediaBlockSize = 512;
	return err == KErrNone && volInfo.iBlockSize >= KDefaultMediaBlockSize && (volInfo.iBlockSize & (volInfo.iBlockSize - 1)) == 0;
	}

//Returns the number of the drive on which atomic "block write" operations are supported.
TInt GetBlockAtomicDriveNo()
	{
	//Uncomment, if you want to do the test on drive C:
	//(but you will need also to uncomment 2 lines in CPermanentStoreCoord::FileQoSL())
	//return EDriveC;
	//
	for(TInt driveNo=EDriveA;driveNo<=EDriveZ;++driveNo)
		{
		TDriveInfo driveInfo;
		TInt err = TheFs.Drive(driveInfo, driveNo);
		if(err == KErrNone)
			{
			if(!(driveInfo.iDriveAtt & KDriveAttTransaction))
				{
#ifdef __X86GCC__
			TFSName thefsname;
			err = TheFs.FileSystemName (thefsname, driveNo);

			if ((err == KErrNone) && (!(thefsname.MatchF(_L("NTFS"))))) // X86GCC doesn't (yet) support write operations on NTFS partitions
#endif
				if(IsBlockAtomic(driveNo))
					{
					return driveNo;
					}
				}
			}
		}
	return -1;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// If you want to check that the atomic "block write" optimisation works and the code change is robust, then you
// have to:
// 1) Prove that if the the "file write" operations over a sector boundary are not done properly, the current test will crash
// 2) Prove that if the the "file write" operations over a sector boundary are done properly, the current test won't crash
// How this could be done:
// For (1) - Simulate that the file system for drive C: on the Emulator is transactional.
//    In order to do that:
//		- uncomment the "return EDriveC;" statement in the function above (GetBlockAtomicDriveNo());
//		- go to CPermanentStoreCoord::FileQoSL() function and uncomment the first two lines:
//			"iFileQos = ETransactional;"
//			"return iFileQos;"
//	  So, the result of your changes will be: STORE will try to optimize the "file write" operations on drive C:,
//	  because drive C: will be reported as a "transactional" drive. Since the drive C: does not really support "transactional"
//	  operations, the optimization will cause some failures in the current test, if the data block which has to be written,
//	  is split across a sector boundary. In order to make sure that the split will hapen - do step (2).
// 2) Declare SYSLIBS_TEST macro in the related variant.hrh file. This change will cause RFileBuf to split the "file write"
//	  operations in two separate operations, if the data block to be written has to be split over a sector boundary.
// After the changes, build STORE and run the current test. You must see that the test crashes.
//
// Then, for (2) (a) restore the original code in GetBlockAtomicDriveNo() and (b) CPermanentStoreCoord::FileQoSL() and rebuild STORE.
// Rerun the current test and the test must pass.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	TheTest.Start(_L("Check for \"block atomic write\" drives"));
	TInt driveNo = GetBlockAtomicDriveNo();
	if(driveNo != -1)
		{
		TheTest.Printf(_L("Test drive - %c:\r\n"), 'A' + driveNo);
		TheTest.Printf(_L("The test expects that STORE component is built with SYSLIBS_TEST macro defined\r\n"));
		CreateTestEnv(driveNo);
		DoTests();
		DeleteTestFiles();
		}
	else
		{
		TheTest.Printf(_L("!!! No \"block atomic write\" drive found!\r\n"));
		}
	TheFs.Close();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
