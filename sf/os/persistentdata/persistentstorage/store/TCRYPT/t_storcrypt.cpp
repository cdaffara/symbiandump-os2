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
#include <s32crypt.h>
#include <e32test.h>
#include <s32mem.h>
#include <pbedata.h>
#include <pbe.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_CRYPT.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);
const TPBPassword KTestPassword(_L("The password"));

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storcrypt"));
LOCAL_D RFs TheFs;
LOCAL_D TStreamId TheTempId;
LOCAL_D TBuf8<KTestLength+1> TheBuf;

LOCAL_D CPBEncryptSet* thePBEKey;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1126
@SYMTestCaseDesc	    Tests for writing and reading an encrypted data to store
@SYMTestPriority 	    High
@SYMTestActions  	    Encrypt data and write to a store.Read the encrypted data for decrypting and compare the result.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testEncryptionDataL()
    {
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1126 "));
	CPBEncryptElement* elementKey = CPBEncryptElement::NewLC(KTestPassword);
	TParsePtrC parse(KFileLocationSpec);
	
	RFileWriteStream writeStream;
    writeStream.PushL();
    User::LeaveIfError(writeStream.Replace(TheFs, parse.NameAndExt(), EFileWrite));

    REncryptStream encryptStream;
    encryptStream.OpenLC(writeStream, *elementKey);

    encryptStream << KTestDes;
    encryptStream.CommitL();
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(2); // encryptStream, writeStream

    const CPBEncryptionData& encryptData = elementKey->EncryptionData();

    CPBEncryptElement* newElementKey = CPBEncryptElement::NewLC(encryptData, KTestPassword);

    RFileReadStream readStream;
    readStream.PushL();
    User::LeaveIfError(readStream.Open(TheFs, parse.NameAndExt(), EFileWrite));

    RDecryptStream decryptStream;
    decryptStream.OpenLC(readStream, *newElementKey);

    decryptStream >> TheBuf;

    test(KTestDes == TheBuf);

	CleanupStack::PopAndDestroy(4, elementKey);
    }

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1127
@SYMTestCaseDesc	    Stream encryption test.
                        Tests the functionality of REncryptStream and RDecryptStream classes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing an encrypted string to a stream,reading back the data,decrypt and
                        check with the original data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSimpleStreamEncryptionL()
    {
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1127 "));
   	CPBEncryptElement* elementKey = CPBEncryptElement::NewL(KTestPassword);
	CleanupStack::PushL(elementKey);
	TParsePtrC parse(KFileLocationSpec);
	
	RFileWriteStream writeStream;
    writeStream.PushL();
    User::LeaveIfError(writeStream.Replace(TheFs, parse.NameAndExt(), EFileWrite));

	REncryptStream encryptStream;
	encryptStream.OpenLC(writeStream, *elementKey);

    encryptStream << KTestDes;
    encryptStream.CommitL();
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(2); // encryptStream, writeStream

    RFileReadStream readStream;
    readStream.PushL();
    User::LeaveIfError(readStream.Open(TheFs, parse.NameAndExt(), EFileWrite));

    RDecryptStream decryptStream;
	decryptStream.OpenLC(readStream, *elementKey);

    decryptStream >> TheBuf;

    test(KTestDes == TheBuf);

    CleanupStack::PopAndDestroy(3, elementKey); // decryptStream, readStream
    }

/**
@SYMTestCaseID          PDS-STORE-CT-4042
@SYMTestCaseDesc	    Tests for attaching write stream and read stream to an encrypted stream.
@SYMTestPriority 	    High
@SYMTestActions  	    Encrypt data and write to a store.Read the encrypted data for decrypting and compare the result.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void testEncryptionDataAttachL()
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4042 "));
	CPBEncryptElement* elementKey = CPBEncryptElement::NewLC(KTestPassword);
	TParsePtrC parse(KFileLocationSpec);
	
	RFileWriteStream writeStream;
	writeStream.PushL();
	User::LeaveIfError(writeStream.Replace(TheFs, parse.NameAndExt(), EFileWrite));
	
	REncryptStream encryptStream;
	TRAPD(ret, encryptStream.AttachL(writeStream, *elementKey));
	test(ret == KErrNone);
	encryptStream.PushL();
	
	encryptStream << KTestDes;
	encryptStream.CommitL();
	writeStream.CommitL();
	
	CleanupStack::PopAndDestroy(2); // encryptStream, writeStream
	
	const CPBEncryptionData& encryptData = elementKey->EncryptionData();
	
	CPBEncryptElement* newElementKey = CPBEncryptElement::NewLC(encryptData, KTestPassword);
	
	RFileReadStream readStream;
	readStream.PushL();
	User::LeaveIfError(readStream.Open(TheFs, parse.NameAndExt(), EFileWrite));
	
	
	RDecryptStream decryptStream;
	decryptStream.AttachL(readStream, *newElementKey);
	decryptStream.PushL();
	
	decryptStream >> TheBuf;
	
	test(KTestDes == TheBuf);
	
	CleanupStack::PopAndDestroy(4, elementKey);
	}


//
// Test writing to a store
//
// Writes two streams in a store - the first containing sections of
// the test data repeated, and the the second containing the test data
// and the id of the first stream.  Returns the id of the second
// stream.
//
LOCAL_C TStreamId testWriteL(CStreamStore& aStore)
	{
	test.Next(_L("Writing..."));
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(aStore);
	for (TInt i=0;i<=KTestLength;++i)
		{
		out.WriteL(KTestDes,i);
		out.WriteL(&KTestData[i],KTestLength-i);
		}
	out.CommitL();
	out.Close();
	TStreamId head=out.CreateL(aStore);
	out<<KTestDes;
	out<<id;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	return head;
	}

//
// Test reading from a store
//
// Test that the data in a stream is the same as the data the
// testWriteL() function writes to the first stream.
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
// Reads back and checks the data written to a store by testWriteL(),
// given the store and the id returned.
//
LOCAL_C void testReadL(const CStreamStore& aStore,TStreamId anId)
	{
	test.Next(_L("Reading..."));
	RStoreReadStream in;
	in.OpenLC(aStore,anId);
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
//
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
@SYMTestCaseID          SYSLIB-STORE-CT-1128
@SYMTestCaseDesc	    Tests for writing to a store
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for writing to a root store and empty store.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1128 Replacing host file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	CFileStore* file=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite);
	file->SetTypeL(file->Layout());
//
	test.Next(_L("Writing root store"));
	RStoreWriteStream stream;
	TStreamId embed=stream.CreateL(*file);
	CEmbeddedStore* store=CEmbeddedStore::NewLC(stream);

	CSecureStore* secure=CSecureStore::NewLC(*store, *thePBEKey);

	store->SetRootL(testWriteL(*secure));
	CleanupStack::PopAndDestroy(secure);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	__UHEAP_MARK;
	TStreamId root=stream.CreateLC(*file);
	REncryptStream encrypt;
	encrypt.AttachLC(stream,*thePBEKey);

	encrypt<<_L8(" root")<<embed;
	encrypt.CommitL();
	CleanupStack::PopAndDestroy(2);
	__UHEAP_MARKEND;

	file->SetRootL(root);
//
	test.Next(_L("Writing temp store"));
	embed=stream.CreateL(*file);
	store=CEmbeddedStore::NewLC(stream);
	secure = CSecureStore::NewLC(*store, *thePBEKey);

	store->SetRootL(testWriteL(*secure));
	CleanupStack::PopAndDestroy(secure);
	store->CommitL();
	CleanupStack::PopAndDestroy();

	__UHEAP_MARK;
	TheTempId=stream.CreateLC(*file);
	encrypt.OpenLC(stream,*thePBEKey);

	encrypt<<_L8(" temp")<<embed;
	encrypt.CommitL();
	CleanupStack::PopAndDestroy();
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	__UHEAP_MARKEND;
//
	file->CommitL();
	CleanupStack::PopAndDestroy(file);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1129
@SYMTestCaseDesc	    Tests for reading from a store
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reading from a root store and temporary store
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1129 Opening host file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	CFileStore* file=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead);
//
	test.Next(_L("Reading from root store"));
	TStreamId root=file->Root();
	RStoreReadStream stream;
	stream.OpenLC(*file,root);
	RDecryptStream decrypt;
	decrypt.AttachLC(stream,*thePBEKey);

	TStreamId embed;
	TBuf8<5> b;
	decrypt>>b>>embed;
	test(b==_L8(" root"));
	CleanupStack::PopAndDestroy(2);
	stream.OpenL(*file,embed);
	CEmbeddedStore* store=CEmbeddedStore::FromLC(stream);
	CSecureStore* secure=NULL;
	secure=CSecureStore::NewLC(*store,*thePBEKey);

	testReadL(*secure,store->Root());
	CleanupStack::PopAndDestroy(2);
//
	test.Next(_L("Reading from temp store"));
	stream.OpenLC(*file,TheTempId);
	decrypt.OpenLC(stream,*thePBEKey);

	decrypt>>b>>embed;
	test(b==_L8(" temp"));
	CleanupStack::PopAndDestroy(2);
	stream.OpenL(*file,embed);
	store=CEmbeddedStore::FromLC(stream);
	secure=CSecureStore::NewLC(*store,*thePBEKey);

	testReadL(*secure,store->Root());
//
	CleanupStack::PopAndDestroy(3);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1130
@SYMTestCaseDesc	    Tests copying from one stream to another stream
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for copying using different transfer sizes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1130 Opening host file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	CFileStore* file=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
//
	test.Next(_L("Opening source (root) store"));
	TStreamId root=file->Root();
	RStoreReadStream src;
	src.OpenLC(*file,root);
	RDecryptStream decrypt;
	decrypt.OpenLC(src,*thePBEKey);

	TBuf8<5> b;
	TStreamId embed;
	decrypt>>b>>embed;
	test(b==_L8(" root"));
	CleanupStack::PopAndDestroy(2); // src, decrypt
	src.OpenL(*file,embed);
	CEmbeddedStore* source=CEmbeddedStore::FromLC(src);
	CSecureStore* ssource=NULL;
	ssource=CSecureStore::NewLC(*source,*thePBEKey);

	test.Next(_L("Creating target store"));
	RStoreWriteStream trg;
	trg.CreateL(*file);
	CEmbeddedStore* target=CEmbeddedStore::NewLC(trg);
	CSecureStore* starget=NULL;
	starget=CSecureStore::NewLC(*target,*thePBEKey);

	test.Next(_L("Copying using small transfers"));
	RStoreReadStream in;
	in.OpenL(*ssource,source->Root());
	in>>TheBuf;
	TStreamId copyId;
	in>>copyId;
	in.Close();
	in.OpenL(*ssource,copyId);
	RStoreWriteStream out;
	TStreamId id=out.CreateL(*starget);
	testCopyL(out,in);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*starget,id);
	testReadL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	in.OpenL(*ssource,copyId);
	id=out.CreateL(*starget);
	in.ReadL(out,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*starget,id);
	testReadL(in);
	in.Close();
	in.OpenL(*ssource,copyId);
	id=out.CreateL(*starget);
	out.WriteL(in,KTestTotal);
	out.CommitL();
	out.Close();
	in.Close();
	in.OpenL(*starget,id);
	testReadL(in);
	in.Close();
//
	CleanupStack::PopAndDestroy(5);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1131
@SYMTestCaseDesc	    Writing and reading back a long stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt to write long data to a stream and read back and test for the integrity of the data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testBugL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1131 Opening host file "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	CFileStore* file=CDirectFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	file->SetTypeL(file->Layout());
	CSecureStore* secure=NULL;

		secure=CSecureStore::NewLC(*file,*thePBEKey);

	test.Next(_L("Writing long stream"));
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(*secure);
	TInt ii;
	for (ii=0;ii<400;++ii)
		out<<_L("a goodly length of data");
	out.CommitL();
	CleanupStack::PopAndDestroy();
	file->SetRootL(out.CreateLC(*secure));
	out<<_L("spam")<<id;
	out.CommitL();
	CleanupStack::PopAndDestroy(2, secure);
	file->CommitL();
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Opening host file"));
	file=CFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileRead);
	secure=CSecureStore::NewLC(*file,*thePBEKey);
//
	TBuf<30> buf;
	test.Next(_L("Reading"));
	RStoreReadStream in;
	in.OpenLC(*secure,file->Root());
	in>>buf>>id;
	CleanupStack::PopAndDestroy();
	test(buf==_L("spam"));
	in.OpenLC(*secure,id);
	for (ii=0;ii<400;++ii)
		{
		in>>buf;
		test (buf==_L("a goodly length of data"));
		}
	CleanupStack::PopAndDestroy(3);
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4016
@SYMTestCaseDesc	    Tests for CPBEncryptElement
@SYMTestPriority 	    High
@SYMTestActions  	    Externalizing and internalizing CPBEncryptionData. Tests for constructors.
@SYMTestExpectedResults Externalizing must not fail. After internalization CPBEncryptionData object should
						be valid. Object created with all constructors should be valid.
@SYMDEF                 DEF135804
*/
LOCAL_C void testForgottenAPI_L()
	{
	test.Next(_L("@SYMTestCaseID PDS-STORE-CT-4016: Tests for CPBEncryptElement"));
	CBufFlat* buffer = CBufFlat::NewL(10*1024);
	CleanupStack::PushL(buffer);
	RBufWriteStream wstr(*buffer,0);
	
	CPBEncryptElement* elementKey = CPBEncryptElement::NewLC(KTestPassword);
    const CPBEncryptionData& encryptData = elementKey->EncryptionData();    
    wstr << encryptData;
    CleanupStack::PopAndDestroy();
	
	wstr.CommitL();
	wstr.Close();
	
	RBufReadStream rstr(*buffer,0);
	CPBEncryptionData* enData = CPBEncryptionData::NewL(rstr);
	test(enData != NULL);
	delete enData;
	enData = NULL;
	rstr.Close();
	rstr.Open(*buffer,0);
	enData = CPBEncryptionData::NewLC(rstr);
	test(enData != NULL);
	CleanupStack::PopAndDestroy();
	enData = NULL;
	
	rstr.Close();
	
	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4019
@SYMTestCaseDesc	    Tests adding and deleting a stream from securestore
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new stream and Delete it again
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void testExtendDeleteL()
	{
	test.Next(_L("@SYMTestCaseID PDS-STORE-CT-4019"));
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
		
	test.Next(_L("Testing Extend"));
	CFileStore* file=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	file->SetTypeL(file->Layout());
	CSecureStore* secure=NULL;

	secure=CSecureStore::NewLC(*file,*thePBEKey);
	TStreamId id = secure->ExtendL();
	test(id != NULL);
	test.Next(_L("Testing Delete"));		
	TRAPD(r, secure->DeleteL(id));
	test(r == KErrNone);
	TRAP(r, secure->DeleteL(id));
	test(r == KErrNotFound);
	
	CleanupStack::PopAndDestroy(2);
	
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4020
@SYMTestCaseDesc	    Tests writing and replacing a stream from securestore
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new stream in the store and then open it for replacement
@SYMTestExpectedResults stream is created and writtenn to successfully
@SYMDEF                 DEF135804
*/
LOCAL_C void testWriteReplaceL()
	{
	test.Next(_L("@SYMTestCaseID PDS-STORE-CT-4020"));
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
		
	test.Next(_L("Testing Extend"));
	CFileStore* file=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	file->SetTypeL(file->Layout());
	CSecureStore* secure=NULL;

	secure=CSecureStore::NewLC(*file,*thePBEKey);
	RStoreWriteStream out;
	TStreamId id = out.CreateL(*secure);
	test(id != NULL);
	out.Close();
	TRAPD(r, out.OpenL(*secure, id) );
	test(r == KErrNone);
	out.WriteL(KTestDes,8);
	out.CommitL();
	out.Close();
	r = secure->Commit();
	test(r == KErrNone);
	
	TRAP(r,out.ReplaceL(*secure, id));
	test(r == KErrNone);
	out.WriteL(KTestDes,8);
	out.CommitL();
	out.Close();
	secure->Revert();
	out.Close();
	
	CleanupStack::PopAndDestroy(2);
		
	}


LOCAL_C void setupTestDirectory()
//
// Prepare the test directory.
//
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

LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
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

LOCAL_C void runTestsL()
    {
	test.Start(_L("Test direct file store"));
	TInt r = KErrNone;

//	New PBE tests
	TRAP(r,testEncryptionDataL());
	test(r==KErrNone);
	TRAP(r,testSimpleStreamEncryptionL());
	test(r==KErrNone);
	
	
	TRAP(r,testEncryptionDataAttachL());
	test(r==KErrNone);
	

//	Old API tests with new PBE parameters
	thePBEKey = CPBEncryptSet::NewL(KTestPassword);

	TRAP(r,testWriteL());
	test(r==KErrNone);
	TRAP(r,testReadL());
	test(r==KErrNone);
	TRAP(r,testCopyL());
	test(r==KErrNone);
	TRAP(r,testBugL());
	test(r==KErrNone);
	TRAP(r,testForgottenAPI_L());
	test(r==KErrNone);
	TRAP(r,testExtendDeleteL());
	test(r==KErrNone);
	TRAP(r,testWriteReplaceL());
		test(r==KErrNone);

	delete thePBEKey;

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
// Test streaming conversions.
//
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
    TRAPD(r, runTestsL());
    test(r == KErrNone);

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
	return r!=KErrNone;
    }
