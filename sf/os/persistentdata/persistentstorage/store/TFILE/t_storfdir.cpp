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
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_FDIR.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storfdir"));
LOCAL_D RFs TheFs;
LOCAL_D TFileName TheTempFile;
LOCAL_D TBuf8<KTestLength+1> TheBuf;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1147
@SYMTestCaseDesc	    Writing to a store test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing to a store
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL(CPersistentStore& aStore)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1147 Writing... "));
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(aStore);
	for (TInt i=0;i<=KTestLength;++i)
		{
		out.WriteL(KTestDes,i);
		out.WriteL(&KTestData[i],KTestLength-i);
		}
	out.CommitL();
	out.Close();
	aStore.SetRootL(out.CreateL(aStore));
	out<<KTestDes;
	out<<id;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1148
@SYMTestCaseDesc	    Reading from a stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reading data from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1148 "));
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
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1148 Reading... "));
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
@SYMTestCaseID          SYSLIB-STORE-CT-1149
@SYMTestCaseDesc	    Tests writing using a direct file store
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
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1149 Creating and failing to open 'ghost' file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	TheFs.Delete(parse.NameAndExt());
	CFileStore* store=CDirectFileStore::CreateLC(TheFs,parse.NameAndExt(),EFileWrite);
	CleanupStack::PopAndDestroy();
	store=NULL;
	TRAPD(r,store=CDirectFileStore::OpenL(TheFs,parse.NameAndExt(),EFileRead|EFileWrite));
	test(store==NULL&&r==KErrEof);
//
	test.Next(_L("Writing to replaced file"));
	store=CDirectFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KDirectFileStoreLayoutUid));
	testWriteL(*store);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Writing to replaced file - 2"));
	store=CDirectFileStore::ReplaceL(TheFs,parse.NameAndExt(),EFileWrite);
	CleanupStack::PushL(store);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KDirectFileStoreLayoutUid));
	testWriteL(*store);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Writing to temp file"));
	store=CDirectFileStore::TempLC(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite);
	store->SetTypeL(TUidType(store->Layout(),KNullUid,KDirectFileStoreLayoutUid));
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	(void)TheFs.Delete(TheTempFile);
//	
	test.Next(_L("Writing to temp file - 2"));
	store=CDirectFileStore::TempL(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite);
	CleanupStack::PushL(store);
	store->SetTypeL(TUidType(store->Layout(),KNullUid,KDirectFileStoreLayoutUid));
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Writing to opened file"));
	store=CDirectFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Failing to create existing file"));
	store=NULL;
	TRAP(r,store=CDirectFileStore::CreateL(TheFs,TheTempFile,EFileWrite));
	test(store==NULL&&r==KErrAlreadyExists);
	if (TheFs.Delete(parse.NameAndExt())!=KErrNone)
		test.Panic(_L("Deleting file"));
//
	test.Next(_L("Writing to created file"));
	RFile file;
	test(file.Create(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	store=CDirectFileStore::NewLC(file);
	CleanupStack::PopAndDestroy();
	test(file.Open(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	store=CDirectFileStore::NewL(file);
	CleanupStack::PushL(store);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1150
@SYMTestCaseDesc	    Tests reading from an opened file
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading using a direct file store
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1150 Reading from opened file "));
	TParsePtrC parse(KFileLocationSpec);
	
	RFile file;
	test(file.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	CFileStore* store=CFileStore::FromL(file);
	CleanupStack::PushL(store);
	testReadL(*store);
	store->CommitL();
	TRAPD(r, store->RevertL());
	test(r== KErrNotSupported);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Reading from temp file"));
	test(file.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	store=CDirectFileStore::FromLC(file);
	testReadL(*store);
	CleanupStack::PopAndDestroy();

	test.Next(_L("Reading from temp file - 2"));
	test(file.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	store=CDirectFileStore::FromL(file);
	CleanupStack::PushL(store);
	testReadL(*store);
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1151
@SYMTestCaseDesc	    Copying to a single file store test.
@SYMTestPriority 	    High
@SYMTestActions  	    Test for copying using different buffer sizes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1151 Copying using small transfers "));
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

GLDEF_C TInt E32Main()
//
// Test direct file store.
//
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test direct file store"));
	TRAPD(r,testWriteL());
	test(r==KErrNone);
	TRAP(r,testReadL());
	test(r==KErrNone);
	TRAP(r,testCopyL());
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

