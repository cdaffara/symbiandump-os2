// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32test.h>  // RTest
#include "pcstoretestdef.h"

static RTest TheTest(_L("Store PC tool Test"));
_LIT(KTxtStore,"STORE");

//File names used in this test
_LIT(KDirectFileStore1,"c:\\stor-tst\\pcstore\\store_sym1");
_LIT(KDirectFileStore2,"c:\\stor-tst\\pcstore\\store_sym2");

_LIT(KDirectFileStoreRef1,"c:\\stor-tst\\pcstore\\store_ref1");
_LIT(KDirectFileStoreRef2,"c:\\stor-tst\\pcstore\\store_ref2");

// public
static RFs TheFsSession;

// Compares 2 files.
// It will leave if it fails in opening or reading any one of the files, or their contents
// differ.
static void CompareFilesL(const TDesC& fileName1, const TDesC& fileName2)
	{
	RFile file1, file2;
	RBuf8 buf1, buf2;
	TInt fileSize;

	// open file1
	User::LeaveIfError(file1.Open(TheFsSession, fileName1, EFileRead));
	CleanupClosePushL(file1);

	// open file2
	User::LeaveIfError(file2.Open(TheFsSession, fileName2, EFileRead));
	CleanupClosePushL(file2);

	// read file1 to buffer
	User::LeaveIfError(file1.Size(fileSize));
	CleanupClosePushL(buf1);
	buf1.CreateMaxL(fileSize);
	User::LeaveIfError(file1.Read(buf1));

	// read file2 to buffer
	User::LeaveIfError(file2.Size(fileSize));
	CleanupClosePushL(buf2);
	buf2.CreateMaxL(fileSize);
	User::LeaveIfError(file2.Read(buf2));

	// compare 2 buffers
	if (buf1 != buf2)
		{
		User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(&buf2);
	CleanupStack::PopAndDestroy(&buf1);
	CleanupStack::PopAndDestroy(&file2);
	CleanupStack::PopAndDestroy(&file1);

	return;
	}

//Creates a reference store file to externalise some data.
//See the test specification for the details of the data's value and
//externalization order.
static void CreateStore1L(const TDesC& directFileStore)
	{
	TheTest.Next(_L("Creates a reference store file to externalise some data."));

	TInt ret;
	// make the directory
	ret = TheFsSession.MkDirAll(directFileStore);
	if((ret != KErrNone) && (ret != KErrAlreadyExists))
		{
		User::Leave(ret);
		}

	TParse	fileStoreName;
	User::LeaveIfError(TheFsSession.Parse(directFileStore,fileStoreName));

	// construct file store object - the file to contain the
	// the store replaces any existing file of the same name.
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFsSession,fileStoreName.FullName(),EFileWrite);

	// Must say what kind of file store
	store->SetTypeL(KDirectFileStoreLayoutUid);

	RStoreWriteStream outstream;

	// the first stream, which contains only a TInt8 data.
	TStreamId id1 = outstream.CreateLC(*store);

	outstream.WriteInt8L(KRefTInt8_1);

	// Commit changes to the stream
	outstream.CommitL();

	// Cleanup the stream object
	CleanupStack::PopAndDestroy(&outstream);

	// the second stream, which contains lots data.
	TStreamId id2 = outstream.CreateLC(*store);

	outstream.WriteReal64L(KRefTReal64 );
	outstream.WriteInt8L(KRefTInt8_2);
	outstream.WriteInt16L(KRefTInt16);

	// Externalize 8-bit descriptor without header.
	// It will be externalized in full length and half length for tests of different APIs
	outstream.WriteL(KRefText8);
	outstream.WriteL(KRefText8, KRefText8.Length()/2);

	outstream.WriteReal32L(KRefTReal32);
	outstream.WriteUint8L(KRefTUint8);
	outstream.WriteInt32L(KRefTInt32);

	// Externalize 16-bit descriptor without header.
	// It will be externalized in full length and half length for tests of different APIs
	outstream.WriteL(KRefText16);
	outstream.WriteL(KRefText16, KRefText16.Length()/2);

	outstream.WriteUint16L(KRefTUint16);
	outstream.WriteUint32L(KRefTUint32);
	outstream << KRefTUid;

	// Externalize descriptors with headers
	// 8-bit descriptor in various lengths
	// Append the KRefText8 a few times to make its length more than 8192
	RBuf8 refRBuf8;
	TInt appends = (8192 / KRefText8.Length()) + 1;
	CleanupClosePushL(refRBuf8);
	refRBuf8.CreateMaxL(KRefText8.Length() * appends);
	refRBuf8.SetLength(0);
	while(appends--)
		{
		refRBuf8.Append(KRefText8);
		}

	refRBuf8.SetLength(CARDINAL_1_BYTE_END);
	outstream << refRBuf8;

	refRBuf8.SetLength(CARDINAL_2_BYTES_START);
	outstream << refRBuf8;

	refRBuf8.SetLength(CARDINAL_2_BYTES_END);
	outstream << refRBuf8;

	refRBuf8.SetLength(CARDINAL_4_BYTES_START);
	outstream << refRBuf8;

	CleanupStack::PopAndDestroy(&refRBuf8);

	// 16-bit descriptor in various lengths, Unicode compression will be applied
	// Append the KRefText16 a few times to make its length more than 8192
	RBuf16 refRBuf16;
	appends = (8192 / KRefText16.Length()) + 1;
	CleanupClosePushL(refRBuf16);
	refRBuf16.CreateMaxL(KRefText16.Length() * appends);
	refRBuf16.SetLength(0);
	while(appends--)
		{
		refRBuf16.Append(KRefText16);
		}

	refRBuf16.SetLength(CARDINAL_1_BYTE_END);
	outstream << refRBuf16;

	refRBuf16.SetLength(CARDINAL_2_BYTES_START);
	outstream << refRBuf16;

	refRBuf16.SetLength(CARDINAL_2_BYTES_END);
	outstream << refRBuf16;

	refRBuf16.SetLength(CARDINAL_4_BYTES_START);
	outstream << refRBuf16;

	CleanupStack::PopAndDestroy(&refRBuf16);

	// The id of this stream
	outstream << id1;

	// Commit changes to the stream
	outstream.CommitL();

	// Cleanup the stream object
	CleanupStack::PopAndDestroy(&outstream);

	// Set the stream id as the root
	store->SetRootL(id2);

	// Commit changes to the store
	store->CommitL();

	// destroy the store object (this also closes
	// the file containing the store)
	CleanupStack::PopAndDestroy(store);
	}

//Creates a store file with no stream.
//This kind of store can be created in Symbian system. But they are not
//supported by the PC tool.
static void CreateStore2L(const TDesC& directFileStore)
	{
	TheTest.Next(_L("Creates a store file with no stream."));

	TInt ret;
	// make the directory
	ret = TheFsSession.MkDirAll(directFileStore);
	if((ret != KErrNone) && (ret != KErrAlreadyExists))
		{
		User::LeaveIfError(ret);
		}

	TParse	fileStoreName;
	User::LeaveIfError(TheFsSession.Parse(directFileStore,fileStoreName));

	// construct file store object - the file to contain the
	// the store replaces any existing file of the same name.
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFsSession,fileStoreName.FullName(),EFileWrite);

	// Must say what kind of file store
	store->SetTypeL(KDirectFileStoreLayoutUid);

	// Commit changes to the store
	store->CommitL();

	// destroy the store object (this also closes
	// the file containing the store)
	CleanupStack::PopAndDestroy(store);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3485
@SYMTestCaseDesc	    Verifying the reference direct store files for PCStore are up-to-date.
@SYMTestPriority 	    High
@SYMTestActions  	    Creates 2 direct store files and compares them with the reference files.
@SYMTestExpectedResults Test must not fail
@SYMEC                 EC042
*/
static void VerifyReferenceFilesL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3485 Verify the direct store files for PCStore are up-to-date. "));
	CreateStore1L(KDirectFileStore1);
	CompareFilesL(KDirectFileStoreRef1, KDirectFileStore1);
	TheFsSession.Delete(KDirectFileStore1);

	CreateStore2L(KDirectFileStore2);
	CompareFilesL(KDirectFileStoreRef2, KDirectFileStore2);
	TheFsSession.Delete(KDirectFileStore2);
	TheTest.End();
	}

static void MainL()
	{
	TInt fsret = TheFsSession.Connect(); // start a file session
	if (fsret != KErrNone)
		{
		TheTest.Printf(_L("file session connect failed.\n"));
		User::Leave(fsret);
		}
	CleanupClosePushL(TheFsSession);
	VerifyReferenceFilesL();
	CleanupStack::PopAndDestroy(&TheFsSession); //close the file session
	}

TInt E32Main()
    {
	TheTest.Title();
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TheTest(cleanup!=NULL);
	TRAPD(error,MainL());
	TheTest(error == KErrNone);
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	TheTest.Close();
	return 0; // and return
    }
