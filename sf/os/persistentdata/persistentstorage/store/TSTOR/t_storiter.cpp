// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RPermanentFileStoreIter functionality tests
// 
//

#include <e32test.h>
#include <s32file.h>
#include <s32fileiter.h>

LOCAL_D RTest test(_L("t_storiter"));
LOCAL_D CTrapCleanup *TheTrapCleanup;
LOCAL_D RFs TheFs;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\T_ITER1.DAT");
const TInt KMaxTestStreamIds = 512;
LOCAL_D TStreamId StreamIds[KMaxTestStreamIds];//Test stream IDs - used in test functions

//aCount - there should be aCount valid stream IDs in StreamIds array.
//The function aborts program's execution if aStreamId is not in StreamIds array.
LOCAL_C void AssertStreamId(TInt aCount, TStreamId aStreamId)
	{
	__ASSERT_DEBUG(aCount >= 0 && aCount < KMaxTestStreamIds, User::Invariant());
	TInt i = 0;
	for(;(i<aCount)&&(aStreamId!=::StreamIds[i]);i++)
		{
		}
	test(i < aCount);
	}

//aStoreStreamIds array has aCount valid elements.
//The function aborts program's execution if aStoreStreamIds array has duplicated elements.
LOCAL_C void AssertStreamIdDuplication(TStreamId aStoreStreamIds[], TInt aCount)
	{
	for(TInt i=0;i<aCount;i++)
		{
		for(TInt j=(i+1);j<aCount;j++)
			{
			test(aStoreStreamIds[i] != aStoreStreamIds[j]);
			}
		}
	}

//The function leaves if it fails to open aStore's stream with aId stream ID.
LOCAL_C void AssertValidStreamIdL(TStreamId aId, CPermanentFileStore& aStore)
	{
	RStoreReadStream in;
	in.OpenLC(aStore, aId);
	CleanupStack::PopAndDestroy(&in);
	}

//aCount - there should be aCount valid stream Ids in StreamIds array.
//The function aborts program's execution if StreamIds items and count don't match
//aStore stream IDs and count or there are duplicated stream IDs.
//The function leaves if it fails to open aStore's streams.
LOCAL_C void AssertStreamIdsL(CPermanentFileStore& aStore, TInt aCount)
	{
	__ASSERT_DEBUG(aCount >= 0 && aCount < KMaxTestStreamIds, User::Invariant());
	TStreamId storeStreamIds[KMaxTestStreamIds];
	Mem::FillZ(storeStreamIds, sizeof(storeStreamIds));
	RPermanentFileStoreIter iter;
	::CleanupClosePushL(iter);
	TInt count = 0;
	iter.ResetL(aStore);
	TStreamId id;
	while((id = iter.NextL()) != KNullStreamIdValue)
		{
		//verifying that it is a valid stream ID
		::AssertValidStreamIdL(id, aStore);
		//Save stream ID in storeStreamIds array
		storeStreamIds[count++] = id;
		//Assert stream ID
		::AssertStreamId(aCount, id);
		}
	test(count == aCount);
	//Assert duplications
	::AssertStreamIdDuplication(storeStreamIds, count);
	CleanupStack::PopAndDestroy(&iter);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1219
@SYMTestCaseDesc	    CPermanentFileStore-StreamId iteration tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by creating file store instance with stream count to zero.
						Assert if StreamIds items and count don't match
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test1L()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1219 CPermanentFileStore-StreamId iteration tests - 1 "));
	//Create CPermanentFileStore instance. Stream count should be 0
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	RFile file;
	test(file.Create(::TheFs, parse.FullName(), EFileWrite| EFileRead) == KErrNone);
	CPermanentFileStore* store = CPermanentFileStore::NewLC(file);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	store->CommitL();
	::AssertStreamIdsL(*store, 0);
	//Create 1 stream. Stream count: 1
	RStoreWriteStream out;
	::StreamIds[0] = out.CreateLC(*store);
	out.CommitL();
	store->CommitL();
	::AssertStreamIdsL(*store, 1);
	//Create 2 new streams. Stream count: 1 + 2
	RStoreWriteStream out2;
	::StreamIds[1] = out2.CreateLC(*store);
	out2.CommitL();
	RStoreWriteStream out3;
	::StreamIds[2] = out3.CreateLC(*store);
	out3.CommitL();
	store->CommitL();
	::AssertStreamIdsL(*store, 3);
	//Close 1 stream. Stream count: 3
	CleanupStack::PopAndDestroy(&out3);
	::AssertStreamIdsL(*store, 3);
	//Delete 1 stream. Stream count: 2
	store->DeleteL(::StreamIds[2]);
	store->CommitL();
	::AssertStreamIdsL(*store, 2);
	//Cleanup
	CleanupStack::PopAndDestroy(&out2);
	CleanupStack::PopAndDestroy(&out);
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1220
@SYMTestCaseDesc	    CPermanentFileStore-StreamId iteration tests
@SYMTestPriority 	    High
@SYMTestActions  	    Test by creating file store instance with stream count to two.
                        Assert if StreamIds items and count don't match
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1220 CPermanentFileStore-StreamId iteration tests - 2 "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	//Create CPermanentFileStore instance with an existing store file . Stream count: 2
	CPermanentFileStore* store = CPermanentFileStore::OpenLC(::TheFs, parse.FullName(), EFileWrite | EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	::AssertStreamIdsL(*store, 2);
	//Create new stream. Stream count: 3
	RStoreWriteStream out;
	::StreamIds[2] = out.CreateLC(*store);
	out.CommitL();
	store->CommitL();
	::AssertStreamIdsL(*store, 3);
	//Create 2 new streams. Stream count: 3 + 2
	RStoreWriteStream out2;
	::StreamIds[3] = out2.CreateLC(*store);
	out2.CommitL();
	RStoreWriteStream out3;
	::StreamIds[4] = out3.CreateLC(*store);
	out3.CommitL();
	store->CommitL();
	::AssertStreamIdsL(*store, 5);
	//Cleanup
	CleanupStack::PopAndDestroy(&out3);
	CleanupStack::PopAndDestroy(&out2);
	CleanupStack::PopAndDestroy(&out);
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1221
@SYMTestCaseDesc	    CPermanentFileStore-StreamId iteration tests
@SYMTestPriority 	    High
@SYMTestActions  	    Test by creating file store instance with stream count to five.
                        Assert if StreamIds items and count don't match.Read and test data from the streams
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1221 CPermanentFileStore-StreamId iteration tests - 3 "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	//Create CPermanentFileStore instance with an existing store file . Stream count: 5
	CPermanentFileStore* store = CPermanentFileStore::OpenLC(::TheFs, parse.FullName(), EFileWrite | EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	::AssertStreamIdsL(*store, 5);
	//Save some data to the streams. Stream count: 5
	RPermanentFileStoreIter iter;
	iter.ResetLC(*store);
	TStreamId id;
	TInt i = 0;
	while((id = iter.NextL()) != KNullStreamIdValue)
		{
		RStoreWriteStream out;
		out.ReplaceLC(*store, id);
		out << TInt32(++i);
		out.CommitL();
		CleanupStack::PopAndDestroy(&out);
		}
	store->CommitL();
	::AssertStreamIdsL(*store, 5);
	CleanupStack::PopAndDestroy(&iter);
	//Read and test data from the streams. Stream count: 5
	i = 0;
	iter.ResetLC(*store);
	while((id = iter.NextL()) != KNullStreamIdValue)
		{
		RStoreReadStream in;
		in.OpenLC(*store, id);
		TInt32 v = 0;
		in >> v;
		test(v == ++i);
		CleanupStack::PopAndDestroy(&in);
		}
	//Cleanup
	CleanupStack::PopAndDestroy(&iter);
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1222
@SYMTestCaseDesc	    CPermanentFileStore-StreamId iteration tests
@SYMTestPriority 	    High
@SYMTestActions  	    Test by creating file store instance with stream count to five and delete all the streams.
						Assert if StreamIds items and count don't match
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1222 CPermanentFileStore-StreamId iteration tests - 4 "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	//Create CPermanentFileStore instance with an existing store file . Stream count: 5
	CPermanentFileStore* store = CPermanentFileStore::OpenLC(::TheFs, parse.FullName(), EFileWrite | EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	::AssertStreamIdsL(*store, 5);
	TInt i;
	//Delete all streams
	for(i=0;i<5;i++)
		{
		store->DeleteL(::StreamIds[i]);
		}
	store->CommitL();
	::AssertStreamIdsL(*store, 0);
	//Create KMaxTestStreamIds/4 streams
	for(i=0;i<(KMaxTestStreamIds/4);i++)
		{
		RStoreWriteStream out;
		::StreamIds[i] = out.CreateLC(*store);
		out.CommitL();
		CleanupStack::PopAndDestroy(&out);
		}
	store->CommitL();
	::AssertStreamIdsL(*store, KMaxTestStreamIds / 4);
	//Delete last KMaxTestStreamIds/8 streams
	for(i=(KMaxTestStreamIds/8);i<(KMaxTestStreamIds/4);i++)
		{
		store->DeleteL(::StreamIds[i]);
		}
	store->CommitL();
	//There are KMaxTestStreamIds/8 streams in store
	::AssertStreamIdsL(*store, KMaxTestStreamIds / 8);
	//Create KMaxTestStreamIds/2 streams
	for(i=(KMaxTestStreamIds/8);i<(KMaxTestStreamIds/8+KMaxTestStreamIds/2);i++)
		{
		RStoreWriteStream out;
		::StreamIds[i] = out.CreateLC(*store);
		out.CommitL();
		CleanupStack::PopAndDestroy(&out);
		}
	store->CommitL();
	//There are KMaxTestStreamIds / 2 + KMaxTestStreamIds / 8 streams in store
	::AssertStreamIdsL(*store, KMaxTestStreamIds / 2 + KMaxTestStreamIds / 8);
	//Delete first KMaxTestStreamIds/4 streams
	for(i=0;i<(KMaxTestStreamIds/4);i++)
		{
		store->DeleteL(::StreamIds[i]);
		}
	store->CommitL();
	//There are KMaxTestStreamIds / 8 + KMaxTestStreamIds / 4 streams in store
	Mem::Move(::StreamIds, &::StreamIds[KMaxTestStreamIds / 4], sizeof(TStreamId) * (KMaxTestStreamIds / 8 + KMaxTestStreamIds / 4));
	::AssertStreamIdsL(*store, KMaxTestStreamIds / 8 + KMaxTestStreamIds / 4);
	CleanupStack::PopAndDestroy(store);
	}

//Run tests
LOCAL_C void MainL()
	{
	::Test1L();
	::Test2L();
	::Test3L();
	::Test4L();
	test.End();
	}

//Delete created/used store files.
LOCAL_C void Cleanup()
	{
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::TheFs.Delete(parse.FullName());
	}

GLDEF_C TInt E32Main()
    {
	test.Title();
	::TheTrapCleanup = CTrapCleanup::New();

	__UHEAP_MARK;
	test(::TheFs.Connect() == KErrNone);

	::Cleanup();
	TRAPD(err, ::MainL());
	test(err == KErrNone);
	test.Close();
	::Cleanup();

	::TheFs.Close();
	__UHEAP_MARKEND;
	delete ::TheTrapCleanup;
	return 0;
    }

