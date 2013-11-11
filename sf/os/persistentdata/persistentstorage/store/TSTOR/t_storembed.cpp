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
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_EMBED.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storembed"));
LOCAL_D RFs TheFs;
LOCAL_D TStreamId TheTempId;
LOCAL_D TBuf8<KTestLength+1> TheBuf;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1192
@SYMTestCaseDesc	    Writing to a store test
@SYMTestPriority 	    High
@SYMTestActions  	    Write a test data to a stream.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL(CPersistentStore& aStore)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1192 Writing... "));
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
@SYMTestCaseID          SYSLIB-STORE-CT-1193
@SYMTestCaseDesc	    Reading from a stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reading from stream for a specific length of data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1193 "));
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

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1194
@SYMTestCaseDesc	    Reading from a persistent store test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL(const CPersistentStore& aStore)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1194 Reading... "));
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

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1195
@SYMTestCaseDesc	    Copying from one stream to another stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for copying two streams
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL(RWriteStream& aWriteStream,RReadStream& aReadStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1195 Copying "));
	for (TInt i=KTestLength;i>=0;--i)
		{
		aWriteStream.WriteL(aReadStream,i);
		aReadStream.ReadL(aWriteStream,KTestLength-i);
		}
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1196
@SYMTestCaseDesc	    Writing to a write once file store created using CEmbeddedStore test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for writing to a newly created store and a temporary store.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1196 Replacing host file "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* file=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite);
	file->SetTypeL(file->Layout());
//
	test.Next(_L("Writing root store"));
	RStoreWriteStream stream;
	TStreamId root=stream.CreateLC(*file);
	stream.WriteL(_L8(" root"));
	CleanupStack::Pop();
	CEmbeddedStore* store=CEmbeddedStore::NewL(stream);
	CleanupStack::PushL(store);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	file->SetRootL(root);
//
	test.Next(_L("Writing temp store"));
	TheTempId=stream.CreateLC(*file);
	stream.WriteL(_L8(" temp"));
	CleanupStack::Pop();
	store=CEmbeddedStore::NewLC(stream);
	testWriteL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy();
//
	file->CommitL();
	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1197
@SYMTestCaseDesc	    Reading from a file buffer test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading from root and temporary store.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1197 Opening host file "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* file=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead);
//
	test.Next(_L("Reading from root store"));
	TStreamId root=file->Root();
	
	RStoreReadStream stream;
	stream.OpenLC(*file,root);
	TBuf8<5> b;
	stream.ReadL(b);
	test(b==_L8(" root"));
	CleanupStack::Pop(&stream);
	CEmbeddedStore* store=CEmbeddedStore::FromL(stream);
	CleanupStack::PushL(store);
	testReadL(*store);
	CleanupStack::PopAndDestroy(store);
	
	stream.OpenLC(*file,root);
	stream.ReadL(b);
	test(b==_L8(" root"));
	CleanupStack::Pop(&stream);
	store=CEmbeddedStore::FromLC(stream);
	testReadL(*store);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Reading from temp store"));
	stream.OpenLC(*file,TheTempId);
	stream.ReadL(b);
	test(b==_L8(" temp"));
	CleanupStack::Pop();
	store=CEmbeddedStore::FromLC(stream);
	testReadL(*store);
//
	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1198
@SYMTestCaseDesc	    Copying in a single file store test.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying using different buffer sizes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1198 Opening host file "));
	TParsePtrC parse(KFileLocationSpec);
	
	CFileStore* file=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
//
	test.Next(_L("Opening source (root) store"));
	TStreamId root=file->Root();
	RStoreReadStream src;
	src.OpenLC(*file,root);
	TBuf8<5> b;
	src.ReadL(b);
	test(b==_L8(" root"));
	CleanupStack::Pop();
	CEmbeddedStore* source=CEmbeddedStore::FromLC(src);
//
	test.Next(_L("Duplicating source store"));
	RStoreWriteStream trg;
	trg.CreateLC(*file);
	MStreamBuf* host=source->Host();
	TStreamPos pos=CEmbeddedStore::Position(source->Root());
	host->SeekL(host->ERead,EStreamBeginning);
	RReadStream stream(host);
	trg.WriteL(stream,pos.Offset());
	stream>>TheBuf;
	TStreamId id;
	stream>>id;
	test(host->TellL(host->ERead).Offset()==host->SizeL());
	trg<<TheBuf;
	trg<<id;
	trg.CommitL();
	source->Detach();
	host->Release();
	source->Reattach(trg.Sink());
	CleanupStack::Pop();
	testReadL(*source);
//
	test.Next(_L("Creating target store"));
	trg.CreateLC(*file);
	trg.WriteL(_L8(" target"));
	CleanupStack::Pop();
	CEmbeddedStore* target=CEmbeddedStore::NewLC(trg);
//
	test.Next(_L("Copying using small transfers"));
	RStoreReadStream in;
	in.OpenL(*source,source->Root());
	in>>TheBuf;
	TStreamId copyId;
	in>>copyId;
	in.Close();
	in.OpenL(*source,copyId);
	RStoreWriteStream out;
	id=out.CreateL(*target);
	testCopyL(out,in);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*target,id);
	testReadL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	in.OpenL(*source,copyId);
	id=out.CreateL(*target);
	in.ReadL(out,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*target,id);
	testReadL(in);
	in.Close();
	in.OpenL(*source,copyId);
	id=out.CreateL(*target);
	out.WriteL(in,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*target,id);
	testReadL(in);
	in.Close();
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

//
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
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
	TheFs.Close();
	test.Close();
	return 0;
    }

