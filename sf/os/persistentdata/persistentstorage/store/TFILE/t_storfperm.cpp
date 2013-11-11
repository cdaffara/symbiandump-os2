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

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_FPERM.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storfperm"));
LOCAL_D RFs TheFs;
LOCAL_D TFileName TheTempFile;
LOCAL_D TBuf8<KTestLength+1> TheBuf;
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1152
@SYMTestCaseDesc	    Tests empty streams
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for empty stream buffers.Check for end of file error,overflow error flags
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testEmptyL(CStreamStore& aStore)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1152 Stream created using 'extend' "));
	TStreamId empty=aStore.ExtendL();
	RStoreReadStream in;
	in.OpenL(aStore,empty);
	TUint8 b;
	test(in.Source()->ReadL(&b,1)==0);
	in.Source()->SeekL(0,KStreamBeginning);
	test(in.Source()->SeekL(MStreamBuf::ERead,EStreamMark)==KStreamBeginning);
	test(in.Source()->SizeL()==0);
	TRAPD(r,in.Source()->SeekL(MStreamBuf::ERead,EStreamBeginning,1));
	test(r==KErrEof);
	TRAP(r,in.Source()->SeekL(MStreamBuf::ERead,EStreamEnd,-1));
	test(r==KErrEof);
	in.Close();
	RStoreWriteStream out;
	out.OpenL(aStore,empty);
	out.Sink()->SeekL(0,KStreamBeginning);
	test(out.Sink()->SeekL(MStreamBuf::EWrite,EStreamMark)==KStreamBeginning);
	test(out.Sink()->SizeL()==0);
	TRAP(r,out.Sink()->SeekL(MStreamBuf::EWrite,EStreamBeginning,1));
	test(r==KErrEof);
	TRAP(r,out.Sink()->SeekL(MStreamBuf::EWrite,EStreamEnd,-1));
	test(r==KErrEof);
	TRAP(r,out.WriteUint8L(0));
	test(r==KErrOverflow);
	out.Close();
//
	test.Next(_L("Replacing empty with empty"));
	out.ReplaceL(aStore,empty);
	out.CommitL();
	out.Release();
	in.OpenL(aStore,empty);
	test(in.Source()->ReadL(&b,1)==0);
	in.Source()->SeekL(0,KStreamBeginning);
	test(in.Source()->SeekL(MStreamBuf::ERead,EStreamMark)==KStreamBeginning);
	test(in.Source()->SizeL()==0);
	TRAP(r,in.Source()->SeekL(MStreamBuf::ERead,EStreamBeginning,1));
	test(r==KErrEof);
	TRAP(r,in.Source()->SeekL(MStreamBuf::ERead,EStreamEnd,-1));
	test(r==KErrEof);
	in.Close();
	out.OpenL(aStore,empty);
	out.Sink()->SeekL(0,KStreamBeginning);
	test(out.Sink()->SeekL(MStreamBuf::EWrite,EStreamMark)==KStreamBeginning);
	test(out.Sink()->SizeL()==0);
	TRAP(r,out.Sink()->SeekL(MStreamBuf::EWrite,EStreamBeginning,1));
	test(r==KErrEof);
	TRAP(r,out.Sink()->SeekL(MStreamBuf::EWrite,EStreamEnd,-1));
	test(r==KErrEof);
	TRAP(r,out.WriteUint8L(0));
	test(r==KErrOverflow);
	out.Close();
	}

//
// Test writing to a store
//
LOCAL_C void testWriteL(CPersistentStore& aStore)
	{
	test.Next(_L("Writing..."));
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(aStore);
	TStreamPos end=KStreamBeginning; //*
	for (TInt i=0;i<=KTestLength;++i)
		{
		test(out.Sink()->SeekL(MStreamBuf::EWrite,EStreamMark)==end); //*
		out.WriteL(KTestDes,i);
		test(out.Sink()->SeekL(0,EStreamEnd,-i)==end); //*
		out.WriteL(&KTestData[i],KTestLength-i);
		end+=KTestLength; //*
		}
//*	test(out.Sink()->SizeL()==end.Offset());
//*	out.WriteL(KTestDes,12);
//*	end+=12;
	test(out.Sink()->SizeL()==end.Offset()); //*
	out.CommitL();
	test(out.Sink()->SizeL()==end.Offset()); //*
	out.Close();
	aStore.SetRootL(out.CreateL(aStore));
	out<<KTestDes;
	out<<id;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Test reading from a store
//
LOCAL_C void testReadL(RReadStream& aStream)
	{
	for (TInt i=KTestLength;i>=0;--i)
		{
		aStream.ReadL(TheBuf,i);
		test(TheBuf.Length()==i);
		TheBuf.SetMax();
		aStream.ReadL(&TheBuf[i],KTestLength-i);
		TheBuf.SetLength(KTestLength);
		test(TheBuf==KTestDes);
		}
	}

//
// Test reading from a store
//
LOCAL_C void testReadL(const CPersistentStore& aStore)
	{
	test.Next(_L("Reading..."));
	RStoreReadStream in;
	in.OpenLC(aStore,aStore.Root());
	in>>TheBuf;
	TStreamId id;
	in>>id;
	in.Close();
	in.OpenL(aStore,id);
	testReadL(in);
	CleanupStack::PopAndDestroy();
	}

//
// Test copying from one stream to another.
//
LOCAL_C void testCopyL(RWriteStream& aWriteStream,RReadStream& aReadStream)
	{
	test.Next(_L("Copying"));
	for (TInt i=KTestLength;i>=0;--i)
		{
		aWriteStream.WriteL(aReadStream,i);
		aReadStream.ReadL(aWriteStream,KTestLength-i);
		}
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1153
@SYMTestCaseDesc	    Tests for writing using a permanent file store
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for memory and end of file error while creating the store.
                        Tests for writing to replaced,temporary,opened,created file.
						Tests for creating an already existing file.
						Tests for panic while deleting a file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1153 Creating and failing to open 'ghost' file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	TheFs.Delete(parse.NameAndExt());
	CFileStore* store=CPermanentFileStore::CreateLC(TheFs,parse.NameAndExt(),EFileWrite);
	CleanupStack::PopAndDestroy();
	store=NULL;
	TRAPD(r,store=CPermanentFileStore::OpenL(TheFs,parse.NameAndExt(),EFileRead|EFileWrite));
	test(store==NULL&&r==KErrEof);
//
	test.Next(_L("Empty tests on replaced file"));
	store=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite);
	store->SetTypeL(TUidType(KPermanentFileStoreLayoutUid,KPermanentFileStoreLayoutUid));
	testEmptyL(*store);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Writing to temp file"));
	store=CPermanentFileStore::TempLC(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite);
	store->SetTypeL(TUidType(store->Layout(),KNullUid,KPermanentFileStoreLayoutUid));
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	(void)TheFs.Delete(TheTempFile);
//
	test.Next(_L("Writing to temp file - 2"));
	store=CPermanentFileStore::TempL(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite);
	CleanupStack::PushL(store);
	store->SetTypeL(TUidType(store->Layout(),KNullUid,KPermanentFileStoreLayoutUid));
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Writing to opened file"));
	store=CPermanentFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Failing to create existing file"));
	store=NULL;
	TRAP(r,store=CPermanentFileStore::CreateL(TheFs,TheTempFile,EFileWrite));
	test(store==NULL&&r==KErrAlreadyExists);
	if (TheFs.Delete(parse.NameAndExt())!=KErrNone)
		test.Panic(_L("Deleting file"));
//
	test.Next(_L("Writing to created file"));
	RFile file;
	test(file.Create(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	store=CPermanentFileStore::NewLC(file);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1154
@SYMTestCaseDesc	    Tests for reading using a permanent file store.
@SYMTestPriority 	    High
@SYMTestActions  	    Read data from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1154 Reading from opened file "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFile file;
	test(file.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	CFileStore* store=CFileStore::FromLC(file);
	testReadL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Reading from temp file"));
	test(file.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	store=CPermanentFileStore::FromLC(file);
	testReadL(*store);
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void testRootL(CPersistentStore& aStore)
	{
	aStore.SetRootL(aStore.ExtendL());
	aStore.CommitL();
	}

LOCAL_C void testDummyL(CFileStore& aStore)
	{
	aStore.SetTypeL(aStore.Layout());
	aStore.CommitL();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1155
@SYMTestCaseDesc	    Tests for recovery from write failures
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for access denied error during writing,commit and update process
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testRecoveryL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1155 Recovering from write failures "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	testWriteL(*store);
	store->CommitL();
//
	store->File().Close();
	test(store->File().Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
// fail during writing
	TRAPD(r,store->SetTypeL(store->Type());testWriteL(*store));
	test(r==KErrAccessDenied);
	store->Revert();
	testReadL(*store);
// fail during commit
	TRAP(r,testRootL(*store));
	test(r==KErrAccessDenied);
	store->Revert();
	testReadL(*store);
// fail during dummy update
	TRAP(r,testDummyL(*store));
	test(r==KErrAccessDenied);
	store->Revert();
	testReadL(*store);
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1156
@SYMTestCaseDesc	    Tests copying in a single file store.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying using different buffer sizes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1156 Copying using small transfers "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* store=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	RStoreReadStream in;
	in.OpenLC(*store,store->Root());
	in>>TheBuf;
	TStreamId copyId;
	in>>copyId;
	in.Close();
	in.OpenL(*store,copyId);
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(*store);
	testCopyL(out,in);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*store,id);
	testReadL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	in.OpenL(*store,copyId);
	id=out.CreateL(*store);
	in.ReadL(out,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*store,id);
	testReadL(in);
	in.Close();
	in.OpenL(*store,copyId);
	id=out.CreateL(*store);
	out.WriteL(in,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*store,id);
	testReadL(in);
//
	CleanupStack::PopAndDestroy(3);
	}

//
// Test empty streams.
//
LOCAL_C void testEmptyL()
	{
	test.Next(_L("Empty tests on existing file"));
	TParsePtrC parse(KFileLocationSpec);
	CFileStore* store=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	testEmptyL(*store);
//
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1157
@SYMTestCaseDesc	    Tests for detaching file from store
@SYMTestPriority 	    High
@SYMTestActions  	    Detach the file and discard the store
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testDetachL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1157 Writing a file store "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	testWriteL(*store);
	store->CommitL();
//
	test.Next(_L("Detach the file and discard the store"));
	RFile file=store->File();
	store->Detach();
	store->Reattach(file);
	RFile& file2 = store->File();
	test(file2.SubSessionHandle() != KNullHandle);
	store->Detach();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Re-construct the store and check the contents"));
	store=CFileStore::FromLC(file);
	testReadL(*store);
	store->Reset();
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1158
@SYMTestCaseDesc	    Tests for defect No 039456
                        Permanent File Store allows code to open and read from deleted streams
@SYMTestPriority 	    High
@SYMTestActions  	    Create four streams,delete last three and close the store.
                        Open the store and test for reading the first stream.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testDef039456L()
{
	_LIT(KMsvDrivelessTestPath, ":\\t_storperm.dat");
	TFileName msvTestPath;
	msvTestPath.Append(RFs::GetSystemDriveChar());
	msvTestPath.Append(KMsvDrivelessTestPath);
	
	_LIT(KStringOne, "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
	_LIT(KStringTwo, "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222");

	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1158 "));
	TheFs.Delete(msvTestPath);
	CPermanentFileStore* testStore = CPermanentFileStore::CreateLC(TheFs, msvTestPath, EFileWrite|EFileShareAny);
	testStore->SetTypeL(KPermanentFileStoreLayoutUid);

	// create four streams,
	RStoreWriteStream wstream;
	TStreamId stream_one=wstream.CreateLC(*testStore);
	wstream << KStringOne;
	wstream.CommitL();
	CleanupStack::PopAndDestroy(&wstream);
	testStore->CommitL();
	TStreamId stream_two=wstream.CreateLC(*testStore);
	wstream << KStringTwo;
	wstream.CommitL();
	CleanupStack::PopAndDestroy(&wstream);
	testStore->CommitL();
	TStreamId stream_three=wstream.CreateLC(*testStore);
	wstream << KStringOne;
	wstream.CommitL();
	CleanupStack::PopAndDestroy(&wstream);
	testStore->CommitL();
	TStreamId stream_four=wstream.CreateLC(*testStore);
	wstream << KStringOne;
	wstream.CommitL();
	CleanupStack::PopAndDestroy(&wstream);
	testStore->CommitL();

	// delete last three streams added (not in the order added)
	testStore->DeleteL(stream_four);
	testStore->CommitL();
	testStore->DeleteL(stream_three);
	testStore->CommitL();
	testStore->DeleteL(stream_two);
	testStore->CommitL();

	// close the store
	CleanupStack::PopAndDestroy(testStore);

	// open store, try and read each of the streams, only stream_one should be present
	testStore = CPermanentFileStore::OpenLC(TheFs, msvTestPath, EFileWrite|EFileShareAny);
	RStoreReadStream rstream1, rstream2;

	// check stream_one ok
	TRAPD(error_stream_one,rstream1.OpenL(*testStore,stream_one));
	test(error_stream_one==KErrNone);
	rstream1.Close();

	// shouldn't be able to open this stream as we deleted it....
	TRAPD(error_stream_two,rstream2.OpenL(*testStore,stream_two));
	test(error_stream_two==KErrNotFound);

	CleanupStack::PopAndDestroy(testStore);
	
	(void)TheFs.Delete(msvTestPath);
	}

/**
@SYMTestCaseID          PDS-STORE-UT-4059
@SYMTestCaseDesc        Tests for defect No ou1cimx1#422232
                        The installed help topics are not organized to Application help topics.
@SYMTestPriority        High
@SYMTestActions         Tests that the EFileWriteDirectIO is appended only when necessary, also
                        test that any EFileWriteBuffered is unset (no error occurs when this is
                        passed in)
@SYMTestExpectedResults Test must not fail
@SYMDEF                 ou1cimx1#422232
*/
LOCAL_C void testOpenL()
    {    
    _LIT(KFileName,"C:\\t_storfperm.dat");
    
    test.Next(_L("@SYMTestCaseID:PDS-STORE-UT-4059: CPermanentFileStore::ReplaceL() test"));
    CPermanentFileStore* testStore = CPermanentFileStore::ReplaceL(TheFs, KFileName, EFileWrite|EFileWriteBuffered);
    delete testStore;
    
    (void)TheFs.Delete(KFileName);
    }

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
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
// Test permanent file store.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test permanent file store"));
	TRAPD(r,testWriteL());
	test(r==KErrNone);
	TRAP(r,testReadL());
	test(r==KErrNone);
	TRAP(r,testRecoveryL());
	test(r==KErrNone);
	TRAP(r,testCopyL());
	test(r==KErrNone);
	TRAP(r,testEmptyL());
	test(r==KErrNone);
	TRAP(r,testDetachL());
	test(r==KErrNone);
	TRAP(r,testDef039456L());
	test(r==KErrNone);
	TRAP(r,testOpenL());
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
	if (TheFs.Delete(TheTempFile)!=KErrNone)
		test.Panic(_L("Deleting temp file"));
	TheFs.Close();
	test.Close();
	return 0;
    }

