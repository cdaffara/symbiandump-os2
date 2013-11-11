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

LOCAL_D RTest test(_L("t_storfserr"));

#ifdef _DEBUG

LOCAL_D CTrapCleanup* TheTrapCleanup = NULL;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDir=_L("\\STOR-TST\\T_FSERR\\");
const TPtrC FileNameA=_L("A.DAT");
const TPtrC FileNameB=_L("B.DAT");

const TPtrC des2(_S("22"),2);
const TPtrC des3(_S("333"),3);
const TPtrC des4(_S("4444"),4);
const TPtrC des5(_S("55555"),5);
LOCAL_D CFileStore* store = NULL;
RStoreWriteStream out;
RStoreReadStream in;

LOCAL_C void setupCleanup()
    {// Initialise the cleanup stack
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

LOCAL_C void setupTestDirectory()
    {// Prepare the test directory.
	TInt r=TheFs.MkDirAll(KTestDir);
	test(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	test(r==KErrNone);
	}

LOCAL_D void AlterStoreL(TInt aFail)
	{
	RStoreWriteStream out2;
	RStoreWriteStream out3;
	RStoreWriteStream out4;
	RStoreReadStream in;
	TheFs.SetErrorCondition(KErrNotReady,aFail);
	TStreamId id0 = out.CreateLC(*store);
	out << _L("012345678901234567890123456789012345678901234567890123456789");
	out.CommitL();
	CleanupStack::PopAndDestroy();

	TStreamId id2 = out.CreateLC(*store);
	out.CommitL();
	CleanupStack::PopAndDestroy();

	TStreamId id3 = out.CreateLC(*store);
	out.CommitL();
	CleanupStack::PopAndDestroy();

	TStreamId id4 = out.CreateLC(*store);
	out << _L("mum");
	out.CommitL();
	CleanupStack::PopAndDestroy();

	out.ReplaceLC(*store,store->Root());
	out << id2;
	out << id3;
	out << id4;
	out << KNullStreamId;
	out << KNullStreamId;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	in.OpenLC(*store,store->Root());// use the root for in and out streams
	out.ReplaceLC(*store,store->Root());
	out.WriteL(in);
	out.CommitL();
	CleanupStack::PopAndDestroy(2);

	out.ReplaceLC(*store,store->Root());// swap the order
	in.OpenLC(*store,store->Root());
	out.WriteL(in);
	out << KNullStreamId;
	out.CommitL();
	CleanupStack::PopAndDestroy(2);

	store->CommitL();

	in.OpenLC(*store,store->Root());
	TStreamId idX,idZ;
	in >> idX;
	in >> idX;
	in >> idZ;// id4 "mum"
	CleanupStack::PopAndDestroy();
	out.OpenLC(*store,idZ);
	in.OpenLC(*store,idZ);
	out2.OpenLC(*store,idZ);
	out3.OpenLC(*store,idZ);
	out4.OpenLC(*store,idZ);
	out4.WriteL(in);
	out.CommitL();
	CleanupStack::PopAndDestroy(5);

	store->CompactL();
	store->CommitL();

	store->DeleteL(id0);
	store->CommitL();

	store->CompactL();
	store->CommitL();
	}

enum TFileQoS
	{
	ESimple			= 0,//File, "write byte" is an atomic operation
	EBlockAtomic	= 1,//File, "block write" is an atomic operation
	ETransactional	= 2,//Transactional file system.
	ELastQoSType
	};

const TInt KFailPoints[ELastQoSType][2] =
	{
	// non-transactional file system
		{
		5, 54
		},
	// atomic "block write"
		{
		3, 38
		},
	// transactional file system
		{
		3, 38
		}
	};

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

TFileQoS MediaType(const TDesC& aFilename)
	{
	TParse p;
	TheFs.Parse(aFilename, p);
	TDriveInfo dinfo;
	TheFs.Drive(dinfo, TDriveUnit(p.Drive()));
	TBool transactional = dinfo.iDriveAtt & KDriveAttTransaction;
	if(transactional)
		{
		return ETransactional;
		}
	if(IsBlockAtomic(TDriveUnit(p.Drive())))
		{
		return EBlockAtomic;
		}
	return ESimple;
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1160
@SYMTestCaseDesc	    File server test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for failure on call to file server
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void FailOnEveryFileServerCallL()
	{
	TheFs.Delete(FileNameA);

	const TFileQoS KMediaType = MediaType(FileNameA);
	_LIT(KSimpleText, "Simple");
	_LIT(KAtomicText, "Atomic block \"write\"");
	_LIT(KTransactionalText, "Transactional");
	const TPtrC mediaType[ELastQoSType] = {KSimpleText(), KAtomicText(), KTransactionalText()};
	test.Printf(_L("Media type: %S\r\n"), &mediaType[KMediaType]);

	const TInt KStoreCommitted = KFailPoints[KMediaType][0];
	const TInt KLastFail = KFailPoints[KMediaType][1];

	const TInt KError = KErrNotReady;
	const TInt KRootIdSizeBeforeStoreCommit = 0;
	const TInt KRootIdSizeAfterStoreCommit = 6*sizeof(TStreamId);
		
	for (TInt fail=1;fail<=KLastFail+5;++fail)
		{
		store=CPermanentFileStore::ReplaceLC(TheFs,FileNameA,EFileWrite|EFileRead);
		store->SetTypeL(KPermanentFileStoreLayoutUid);
		TStreamId rootId = store->ExtendL();
		store->SetRootL(rootId);
		store->CommitL();
		TRAPD(r,AlterStoreL(fail));
		TheFs.SetErrorCondition(KErrNone);
		if (fail<KLastFail)
			test(r==KError);
		else
			test(r==KErrNone);
		TRAP(r,store->RevertL());
		if (r==KErrNotReady)
			{
			CleanupStack::PopAndDestroy();
			store=CPermanentFileStore::OpenLC(TheFs,FileNameA,EFileWrite|EFileRead);
			}
		else
			test(r==KErrNone);
		CleanupStack::PopAndDestroy();
//
		// open read only
		store=CPermanentFileStore::OpenLC(TheFs,FileNameA,EFileRead);
		RStoreReadStream in;
		in.OpenLC(*store,store->Root());
		TInt size=in.Source()->SizeL();
		if (fail<KStoreCommitted)
			test(size==KRootIdSizeBeforeStoreCommit);
		else //fail>=KStoreCommitted
			test(size==KRootIdSizeAfterStoreCommit);
		CleanupStack::PopAndDestroy(2);
//
		store=CPermanentFileStore::OpenLC(TheFs,FileNameA,EFileRead|EFileWrite);
		store->ExtendL();
		store->RevertL();
		in.OpenLC(*store,store->Root());
		size=in.Source()->SizeL();
		if (fail<KStoreCommitted)
			test(size==KRootIdSizeBeforeStoreCommit);
		else //fail>=KStoreCommitted
			test(size==KRootIdSizeAfterStoreCommit);
		CleanupStack::PopAndDestroy(2);
		}
	}

LOCAL_D void InitialseStoreWithDataL()
	{
	TheFs.Delete(FileNameB);
	store=CPermanentFileStore::CreateLC(TheFs,FileNameB,EFileWrite|EFileRead);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId rootId = store->ExtendL();
	store->SetRootL(rootId);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	store=CPermanentFileStore::OpenLC(TheFs,FileNameB,EFileWrite|EFileRead);
	TStreamId id2 = out.CreateLC(*store);
	out << des2;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	TStreamId id3 = out.CreateLC(*store);
	out << des3;
	out.CommitL();
	CleanupStack::PopAndDestroy();

	out.ReplaceLC(*store,store->Root());
	out << id2;
	out << id3;
	out.CommitL();
	CleanupStack::PopAndDestroy();// out

	store->CommitL();
	CleanupStack::PopAndDestroy();// store
	}

LOCAL_D void AlterStoreDuringFileServerErrorL(TInt aError)
	{
	store=CPermanentFileStore::OpenLC(TheFs,FileNameB,EFileWrite|EFileRead);
	in.OpenLC(*store,store->Root());
	TStreamId id2;
	TStreamId id3;
	in >> id2;
	in >> id3;
	CleanupStack::PopAndDestroy();// in

	out.ReplaceLC(*store,id2);
	out << des4;
	out.CommitL();
	CleanupStack::PopAndDestroy();// out
	TheFs.SetErrorCondition(aError);
	out.ReplaceLC(*store,id3);
	out << des5;
	out.CommitL();
	CleanupStack::PopAndDestroy();// out

	store->CommitL();
	CleanupStack::PopAndDestroy();// store
	}

LOCAL_D void TestStreamDataL(TInt aError)
	{
	store=CPermanentFileStore::OpenLC(TheFs,FileNameB,EFileWrite|EFileRead);
	in.OpenLC(*store,store->Root());
	TStreamId id2;
	TStreamId id3;
	in >> id2;
	in >> id3;
	CleanupStack::PopAndDestroy();// in

	TBuf<16> buf;

	in.OpenLC(*store,id2);
	in >> buf;
	if (aError==KErrNone)
		test(buf==_L("4444"));// store committed
	else
		test(buf==_L("22"));// store reverted

	CleanupStack::PopAndDestroy();// in

	in.OpenLC(*store,id3);
	in >> buf;
	if (aError==KErrNone)
		test(buf==_L("55555"));// store committed
	else
		test(buf==_L("333"));// store reverted

	CleanupStack::PopAndDestroy();// in

	CleanupStack::PopAndDestroy();// store
	}

LOCAL_D void ResetStreamDataL()
	{
	store=CPermanentFileStore::OpenLC(TheFs,FileNameB,EFileWrite|EFileRead);
	in.OpenLC(*store,store->Root());
	TStreamId id2;
	TStreamId id3;
	in >> id2;
	in >> id3;
	CleanupStack::PopAndDestroy();// in

	out.ReplaceLC(*store,id2);
	out << des2;
	out.CommitL();
	CleanupStack::PopAndDestroy();// out

	out.ReplaceLC(*store,id3);
	out << des3;
	out.CommitL();
	CleanupStack::PopAndDestroy();// out

	store->CommitL();
	CleanupStack::PopAndDestroy();// store
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1161
@SYMTestCaseDesc	    Tests for all errors on file server
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for stream data to a store
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void TestAllFsErrorL()
	{
	InitialseStoreWithDataL();
	for (TInt error=0; error >= KErrDied; --error)
		{
		TRAPD(r,AlterStoreDuringFileServerErrorL(error));
		TheFs.SetErrorCondition(KErrNone);
		if (r!=KErrNone)
			{
			store = CPermanentFileStore::OpenLC(TheFs,FileNameB,EFileWrite|EFileRead);
			store->RevertL();
			CleanupStack::PopAndDestroy();// store
			}
		TestStreamDataL(error);
		ResetStreamDataL();
		}
	}

void RunTests()
	{
	setupTestDirectory();
	__UHEAP_MARK;
//
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1160 Random failure on every file server call "));
	TRAPD(r,FailOnEveryFileServerCallL());
	test(r==KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1161 Alter STORE during file server error "));
	TRAP(r,TestAllFsErrorL());
	test(r==KErrNone);
	test.End();

	TheFs.Delete(FileNameA);
	TheFs.Delete(FileNameB);
//
	__UHEAP_MARKEND;
	}

#endif	// _DEBUG

GLDEF_C TInt E32Main()
    {// Test permanent file store.
	test.Title();
#ifdef _DEBUG
	TInt r=TheFs.Connect();
	test(r==KErrNone);
	setupCleanup();
	RunTests();
	delete TheTrapCleanup;
	TheFs.Close();
#else
	test.Start(_L("The tests are not valid in release mode"));
	test.End();
#endif
	test.Close();
	return 0;
    }

