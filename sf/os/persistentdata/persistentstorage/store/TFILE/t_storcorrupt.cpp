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

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDir=_L("\\STOR-TST\\T_CORUPT\\");

#ifdef _DEBUG
const TPtrC alphabet(_S("abcdefghijklmnopqrstuvwxyz"),26);
LOCAL_D CFileStore* store;
LOCAL_D CFileStore* storeCopy;
LOCAL_D RFile file;
RStoreWriteStream out;
RStoreReadStream in;
_LIT(KFileName, "t_storcorrupt2.dat");
#endif

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest TheTest(_L("t_storcorrupt"));
LOCAL_D RFs TheFs;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Boolean expression evaluated to false!\r\n"));
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void setupTestDirectory()
    {// Prepare the test directory.
	TInt r=TheFs.Connect();
	TEST2(r, KErrNone);
//
	r=TheFs.MkDirAll(KTestDir);
	TEST(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	TEST2(r, KErrNone);
	}

LOCAL_C void setupCleanup()
    {// Initialise the cleanup stack
	TheTrapCleanup=CTrapCleanup::New();
	TEST(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(r, KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1132
@SYMTestCaseDesc	    Setting a larger file size test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for opening a large size file.Check for KErrNone flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
#ifdef _DEBUG
LOCAL_D void SetFileSizeLargerL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1132 "));
	(void)TheFs.Delete(KFileName);
	store=CPermanentFileStore::CreateLC(TheFs,KFileName,EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootId = store->ExtendL();
	store->SetRootL(rootId);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	RFile file;
	file.Open(TheFs,KFileName,EFileWrite|EFileRead);
	file.SetSize(100000);
	file.Close();

	TRAPD(r,store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrNone);

	(void)TheFs.Delete(KFileName);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1133
@SYMTestCaseDesc	    Reducing the file size test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reducing the file size gradually and checking for KErrEof flag on every attempt.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void SetFileSizeSmallerL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1133 "));
	(void)TheFs.Delete(KFileName);
	store=CPermanentFileStore::CreateLC(TheFs,KFileName,EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootId = store->ExtendL();
	store->SetRootL(rootId);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
	TStreamId id2 = out.CreateLC(*store);
	out << alphabet;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	out.ReplaceLC(*store,rootId);
	out << id2;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	store->CommitL();
	CleanupStack::PopAndDestroy();

	store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
	CleanupStack::PopAndDestroy();

	RFile file;
	file.Open(TheFs,KFileName,EFileWrite|EFileRead);
	file.SetSize(16);
	file.Close();

	TRAPD(r,store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrEof);

	file.Open(TheFs,KFileName,EFileWrite|EFileRead);
	file.SetSize(3);
	file.Close();

	TRAP(r,store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrEof);

	file.Open(TheFs,KFileName,EFileWrite|EFileRead);
	file.SetSize(1);
	file.Close();

	TRAP(r,store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrEof);

	file.Open(TheFs,KFileName,EFileWrite|EFileRead);
	file.SetSize(0);
	file.Close();

	TRAP(r,store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrEof);

	(void)TheFs.Delete(KFileName);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1134
@SYMTestCaseDesc	    Opening a corrupt file test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for writing garbage text to file and opening the corrupt file.Check for KErrCorrupt flag.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void CorruptFileL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1134 "));
	(void)TheFs.Delete(KFileName);
	store=CPermanentFileStore::CreateLC(TheFs,KFileName,EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootId = store->ExtendL();
	store->SetRootL(rootId);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
	TStreamId id2 = out.CreateLC(*store);
	out << alphabet;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	out.ReplaceLC(*store,rootId);
	out << id2;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	store->CommitL();
	CleanupStack::PopAndDestroy();

	storeCopy=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
	CleanupStack::PopAndDestroy();
//
	storeCopy=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
	_LIT8(KGarbage,"garbagegarbagegarbage");
	TInt pos;
	storeCopy->File().Size(pos);
	storeCopy->File().Write(pos-KGarbage().Length(),KGarbage);
	CleanupStack::PopAndDestroy(storeCopy);
//
	TRAPD(r,storeCopy=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);CleanupStack::PopAndDestroy();)
	TEST2(r, KErrCorrupt);

	(void)TheFs.Delete(KFileName);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1135
@SYMTestCaseDesc	    Corrupting the file to various sizes test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for clipping the file to various sizes.Check for KErrCorrupt,KErrNone,KErrEof error flags
                        while opening the corrupt file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void CorruptFileToVariousSizesL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1135 "));
	_LIT(KFileName, "t_storcorrupt1.dat");
	store=CPermanentFileStore::ReplaceLC(TheFs,KFileName,EFileWrite|EFileRead);

	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootIdent = store->ExtendL();
	store->SetRootL(rootIdent);
	store->CommitL();

	TStreamId id5 = out.CreateLC(*store);
	out << alphabet;
	out << id5;
	out.CommitL();
	CleanupStack::PopAndDestroy();//out
	store->CommitL();
	CleanupStack::PopAndDestroy();//store

	TEST2(file.Open(TheFs,KFileName,EFileWrite|EFileRead), KErrNone);
	TInt size;
	TEST2(file.Size(size), KErrNone);
	file.Close();
	const TInt KErrorChange=23;
	for (TInt clip=0;clip<=size;++clip)
		{
		TEST2(file.Open(TheFs,KFileName,EFileWrite|EFileRead) ,KErrNone);
		TEST2(file.SetSize(size-clip), KErrNone);
		file.Close();

		TRAPD(r,
			store=CPermanentFileStore::OpenLC(TheFs,KFileName,EFileWrite|EFileRead);
			CleanupStack::PopAndDestroy();
			)
		if (clip==0)
			TEST2(r, KErrNone);
		else if (clip<KErrorChange)
			TEST2(r, KErrCorrupt);
		else if (clip>=KErrorChange)
			TEST2(r, KErrEof);
		}
	TInt err = TheFs.Delete(KFileName);
	if(err != KErrNone)
		{
		RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &KFileName);
		}
	}

// Is it impossible to CorruptFileWhileWritingStreamsL
/*
LOCAL_D void CorruptFileWhileWritingStreamsL()
	{
	TheFs.Delete(_L("swiss"));
	store=CPermanentFileStore::CreateLC(TheFs,_L("swiss"),EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootId = store->ExtendL();
	store->SetRootL(rootId);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	store=CPermanentFileStore::OpenLC(TheFs,_L("swiss"),EFileWrite|EFileRead);CleanupStack::PopAndDestroy();
	store->File().SetSize(3);
	store->File().SetSize(100000);

	TStreamId id3 = out.CreateLC(*store);
	out << alphabet;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	TRAPD(r,store->CommitL();)
	test(r==KErrNone);
	CleanupStack::PopAndDestroy();// store
	}*/
#endif

GLDEF_C TInt E32Main()
    {// Test permanent file store.
	TheTest.Title();
	setupTestDirectory();
	setupCleanup();
#ifdef _DEBUG
	__UHEAP_MARK;
//
	TheTest.Start(_L("Begin tests"));
	TRAPD(r,SetFileSizeLargerL());
	TheTest.Printf(_L("SetFileSizeLargerL()\n"));
	TEST2(r, KErrNone);
	TRAP(r,SetFileSizeSmallerL());
	TheTest.Printf(_L("SetFileSizeSmallerL()\n"));
	TEST2(r, KErrNone);
	TRAP(r,CorruptFileL());
	TheTest.Printf(_L("CorruptFileL()\n"));
	TEST2(r, KErrNone);
	TRAP(r,CorruptFileToVariousSizesL());
	TheTest.Printf(_L("CorruptFileToVariousSizesL()\n"));
	TEST2(r, KErrNone);
//	TRAP(r,CorruptFileWhileWritingStreamsL());
//	test.Console()->Printf(_L("CorruptFileWhileWritingStreamsL()\n"));
//	test(r==KErrNone);
	TheTest.End();
//
	__UHEAP_MARKEND;
#endif

#ifndef _DEBUG
	TheTest.Start(_L("The tests are not valid in release mode"));
	TheTest.End();
#endif
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.Close();
	return 0;
    }

