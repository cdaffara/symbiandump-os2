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

#include <e32test.h>
#include <s32file.h>

const TInt KTestCleanupStack=0x40;

_LIT(KTitle,"t_stordictfs");
LOCAL_D RTest test(KTitle);
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;

// These are path specifications and should not be used as is
_LIT(KDicFilePath,"Z:\\stor-tst\\dicfile.ini");
const TUid dicFileUid={19445};
_LIT(KSystemIniFile,"Z:\\System\\System.ini");
_LIT(KBackupSystemIniFile,"Z:\\System\\System.backup");
_LIT(KTestIniFile,"Z:\\stor-tst\\Test.ini");
_LIT(KThreadTestPath, "Z:\\stor-tst\\Thread Test %d.ini");

const TInt KNumThreads=20;
const TInt KThreadStack=0x2000;
const TInt KThreadHeap=0x1000;
const TInt KThreadHeapMax=0x100000;

// some uid's to use for testing
const TUid testUid1={1};
const TUid testUid2={57};
const TUid testUid3={99999};
const TUid testUid4={98765};
const TUid KTestUid={0xf0000000};
//
// some data values for testing
const TInt value1=42;
const TInt value2=147;
const TInt value3=6547;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1183
@SYMTestCaseDesc	    Tests for CDictionaryFileStore::IsPresentL(),CDictionaryFileStore::Remove() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing an entry and commit it.
                        Tests for reading the entry back.
						Tests for rewriting and reading back using LC functions.
						Tests for adding one more entry and check for the content.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test1L(CDictionaryFileStore* aDict)
	{
	// write an entry and commit it
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1183 "));
	RDictionaryWriteStream writeStream;
	TRAPD(ret, writeStream.AssignL(*aDict,testUid1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	// read that entry back
	RDictionaryReadStream readStream;
	TRAP(ret, readStream.OpenL(*aDict,testUid1));
		test(ret==KErrNone);
	TInt val=0;
	TRAP(ret, val = readStream.ReadInt32L());
		test(ret==KErrNone);
		test(val==value1);
	readStream.Close();
	//
	test.Next(_L("Test the stream LC methods"));
	// read that entry back using the LC method
	readStream.OpenLC(*aDict,testUid1);
	TRAP(ret, val = readStream.ReadInt32L());
		test(ret==KErrNone);
		test(val==value1);
	//readStream.Close();
	CleanupStack::PopAndDestroy(); // readStream
	//
	// rewrite that entry using the LC method
	writeStream.AssignLC(*aDict,testUid1);
	
	//do some rading
	MStreamBuf* sbuf = writeStream.Sink();
	HBufC8* buffer = HBufC8::NewLC(400);
	TUint8* buffptr = const_cast<TUint8*>(buffer->Ptr());
	sbuf->ReadL(buffptr, 100);
	TPtr8 buffDes(buffer->Des());
	TRequestStatus rstatus;
	sbuf->ReadL(buffDes, 100, rstatus);
	User::WaitForRequest(rstatus);
	CleanupStack::PopAndDestroy(); //buffer
	
	TRAP(ret, writeStream.WriteInt32L(value1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	//writeStream.Close();
	CleanupStack::PopAndDestroy(); // writestream
	//
	test.Next(_L("Test reverting the store"));
	// commit the store
	TRAP(ret, aDict->CommitL());
		test(ret==KErrNone);
	// replace the 1st entry, commit it, then revert the store
	TRAP(ret, writeStream.AssignL(*aDict,testUid1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value2));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	TRAP(ret, aDict->RevertL());
		test(ret==KErrNone);
	// check the value of the entry
	TRAP(ret, readStream.OpenL(*aDict,testUid1));
		test(ret==KErrNone);
	TRAP(ret, val = readStream.ReadInt32L());
		test(ret==KErrNone);
		test(val==value1);
	readStream.Close();
	
	test.Next(_L("Test reverting the store using alternative Revert method"));
	// replace the 1st entry, commit it, then revert the store
	TRAP(ret, writeStream.AssignL(*aDict,testUid1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value2));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	aDict->Revert();
	// check the value of the entry
	TRAP(ret, readStream.OpenL(*aDict,testUid1));
		test(ret==KErrNone);
	TRAP(ret, val = readStream.ReadInt32L());
		test(ret==KErrNone);
		test(val==value1);
	readStream.Close();
	//
	//
	test.Next(_L("Test IsPresentL() and Remove()"));
	// add another entry
	TRAP(ret, writeStream.AssignL(*aDict,testUid2));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value2));
		test(ret==KErrNone);
	test(!aDict->IsPresentL(testUid2));
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	// test IsPresentL()
	test(aDict->IsPresentL(testUid1));
	test(aDict->IsPresentL(testUid2));
	test(!aDict->IsPresentL(testUid3));
	// test Remove()
	TRAP(ret, aDict->RemoveL(testUid1));
		test(!aDict->IsPresentL(testUid1));
	//
	//
	test.Next(_L("Close the store and re-open it"));
	// commit the store and close it
	TRAP(ret, aDict->CommitL());
		test(ret==KErrNone);
	//
	delete aDict;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1184
@SYMTestCaseDesc	    Tests for CDictionaryFileStore::IsPresentL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the specified UID's has an associated stream within the dictionary store.
						Tests for KErrNone error flag while opening a dictionary file for read,test the value read from the file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test2L(CDictionaryFileStore* aDict)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1184 "));
	test(!aDict->IsPresentL(testUid1));
	test(aDict->IsPresentL(testUid2));
	test(!aDict->IsPresentL(testUid3));
	RDictionaryReadStream readStream;
	TRAPD(ret, readStream.OpenL(*aDict,testUid2));
		test(ret==KErrNone);
	TInt val=0;
	TRAP(ret, val = readStream.ReadInt32L());
		test(ret==KErrNone);
		test(val==value2);
	readStream.Close();
	//
	//
	test.Next(_L("Close the store without commiting"));
	// add a stream, close the store (no commit)
	RDictionaryWriteStream writeStream;
	TRAP(ret, writeStream.AssignL(*aDict,testUid3));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value3));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	delete aDict;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1185
@SYMTestCaseDesc	    Reading from a dictionary file test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the specified UID's has an associated stream within the dictionary store.
                        Tests for KErrNone error flag,while opening a dictionary file for read operation
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test3L(CDictionaryFileStore* aDict)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1185 "));
	test(!aDict->IsPresentL(testUid1));
	test(aDict->IsPresentL(testUid2));
	test(!aDict->IsPresentL(testUid3));
	RDictionaryReadStream readStream;
	TRAPD(ret, readStream.OpenL(*aDict,testUid2));
	test(ret==KErrNone);
	
	TInt val=0;
	TRAP(ret, val = readStream.ReadInt32L());
	test(ret==KErrNone);
	test(val==value2);
	
	readStream.Close();
	delete aDict;
	}
/**
@SYMTestCaseID          PDS-STORE-CT-4022
@SYMTestCaseDesc	    Validate IsNUll() API
@SYMTestPriority 	    High
@SYMTestActions  	    Test that the specified DictionaryStore is not present
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void Test4L(CDictionaryFileStore* aDict)
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4022 "));
	test(aDict->IsNullL());
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4023
@SYMTestCaseDesc	    Validate IsNUll() API
@SYMTestPriority 	    High
@SYMTestActions  	    Test that the specified DictionaryStore is  present
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void Test5L(CDictionaryFileStore* aDict)
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4023 "));
	test(!aDict->IsNullL());
	delete aDict;
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4024
@SYMTestCaseDesc	    Validate Remove() API
@SYMTestPriority 	    High
@SYMTestActions  	    Test that the specified UID is  present
                        Remove Stream and test that UID is not present
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void Test6L(CDictionaryFileStore* aDict)
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4024 "));
	test(aDict->IsPresentL(testUid2));
	aDict->Remove(testUid2);
	test(!aDict->IsPresentL(testUid2));
	delete aDict;
	}


/**
Test the System ini file

@SYMTestCaseID          SYSLIB-STORE-CT-1186
@SYMTestCaseDesc	    Tests for CDictionaryFileStore::SystemL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for creation of system file.Tests for basic operations on the file
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void systemTestL()
	{
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse systemIniFile;
	TParse backupSystemIniFile;
	systemIniFile.Set(drive.Name(), &KSystemIniFile, NULL);
	backupSystemIniFile.Set(drive.Name(), &KBackupSystemIniFile, NULL);

	TheFs.Rename(systemIniFile.FullName(),backupSystemIniFile.FullName());
	TParse parse;
	parse.Set(systemIniFile.FullName(),NULL,NULL);
	TheFs.RmDir(parse.DriveAndPath());
//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1186 Create system file "));
	CDictionaryFileStore* dicFile=NULL;
	TRAPD(ret, dicFile = CDictionaryFileStore::SystemL(TheFs));
	test(ret==KErrNone);
	TEntry entry;
	TInt err = TheFs.Entry(systemIniFile.FullName(),entry);
	test (err==KErrNone);
	Test1L(dicFile);
	test.Next(_L("Opening system file"));
	TRAP(ret, dicFile = CDictionaryFileStore::SystemL(TheFs));
		test(ret==KErrNone);
	Test2L(dicFile);
	TRAP(ret, dicFile = CDictionaryFileStore::SystemL(TheFs));
		test(ret==KErrNone);
	Test3L(dicFile);
	
	TRAP(ret, dicFile = CDictionaryFileStore::SystemL(TheFs));
	test(ret==KErrNone);
	Test6L(dicFile);
	
	TheFs.Delete(systemIniFile.FullName());
	TheFs.Rename(backupSystemIniFile.FullName(),systemIniFile.FullName());
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1187
@SYMTestCaseDesc	    Tests for creating a new file and adding an entry into it
@SYMTestPriority 	    High
@SYMTestActions  	    Execute tests for create,open and check the contents and reopening
                        Check for ErrNone flag during the open operation.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void generalTestsL()
	{
	// set things up...
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	TInt ret = TheFs.MkDirAll(dicFilePath.FullName());
	test((ret==KErrNone)||(ret==KErrAlreadyExists));
	TheFs.Delete(dicFilePath.FullName()); // delete the file if it already exists
	//
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1187 Create a new file and put an entry in it "));
	// create a new dictionary file
	CDictionaryFileStore* dicFile=NULL;
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
	test(ret==KErrNone);
	//Check that file is NULL
	Test4L(dicFile);
	Test1L(dicFile);
	// open it and check the contents
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
	test(ret==KErrNone);
	Test2L(dicFile);
	// open it again and check the contents
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
	test(ret==KErrNone);
	Test3L(dicFile);
	//open it again and check that contents is not NUll
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
	test(ret==KErrNone);
	Test5L(dicFile);
	
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1188
@SYMTestCaseDesc	    Creation of empty files test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by opening empty dictionary file and interrogate it.
                        Tests for opening a stream that does not exist.Tests for reading from the empty file
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void emptyFileTestsL()
	{
	// set things up...
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	TInt ret = TheFs.MkDirAll(dicFilePath.DriveAndPath());
		test((ret==KErrNone)||(ret==KErrAlreadyExists));
	TheFs.Delete(dicFilePath.FullName()); // delete the file if it already exists
	//
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1188 Create a new file, close it while still empty, then open it again "));
	// create a new dictionary file and close it immediately
	CDictionaryFileStore* dicFile=NULL;
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
		test(ret==KErrNone);
	TRAP(ret, dicFile->CommitL());
		test(ret==KErrNone);
	delete dicFile;
	dicFile = NULL;
	//
	// open the empty dic file and interogate it
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
		test(ret==KErrNone);
	test(!dicFile->IsPresentL(testUid1));
	test(!dicFile->IsPresentL(testUid2));
	test(!dicFile->IsPresentL(testUid3));
	//
	//
	test.Next(_L("Open a stream that doesn't exist"));
	// open a stream that does not exist - should get an empty one
	RDictionaryReadStream readStream;
	TRAP(ret, readStream.OpenL(*dicFile,testUid1));
		test(ret==KErrNone);
	// try to read from it to check that it's empty
	TUint8 b;
	test(readStream.Source()->ReadL(&b,1)==0);
	readStream.Close();
	//
	//
	// tidy up
	delete dicFile;
	
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1189
@SYMTestCaseDesc	    Repeatedly opening a dictionary file test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for creating a new dictionary file,write an entry an commit it.
                        Attempt for repeatedly opening the dictionary file,interogate it,and delete it without commiting
                        Attempt for repeatedly opening the dictionary file and interogate it,commit the file after each read
						Attempt for repeatedly opening the dictionary file and re-write a stream, commiting the file after each write
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void repeatedUseTestsL()
	{
	// set things up...
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1189 "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	TInt ret = TheFs.MkDirAll(dicFilePath.DriveAndPath());
		test((ret==KErrNone)||(ret==KErrAlreadyExists));
	TheFs.Delete(dicFilePath.FullName()); // delete the file if it already exists
	//
	// create a new dictionary file
	CDictionaryFileStore* dicFile=NULL;
	TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
		test(ret==KErrNone);
	// write an entry and commit it
	RDictionaryWriteStream writeStream;
	TRAP(ret, writeStream.AssignL(*dicFile,testUid1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.WriteInt32L(value1));
		test(ret==KErrNone);
	TRAP(ret, writeStream.CommitL());
		test(ret==KErrNone);
	writeStream.Close();
	// close the file
	TRAP(ret, dicFile->CommitL());
		test(ret==KErrNone);
	delete dicFile;
	dicFile = NULL;
	//
	//
	test.Next(_L("Repeatedly open a file and interogate it without commiting"));
	// repeatedly open the dic file, interogate it, and delete it without commiting
	TEntry fileEntry;
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	TInt sizeBefore=fileEntry.iSize;
	//
	TInt i;
	for (i=0 ; i<10 ; i++)
		{
		TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
			test(ret==KErrNone);
		RDictionaryReadStream readStream;
		TRAP(ret, readStream.OpenL(*dicFile,testUid1));
			test(ret==KErrNone);
		readStream.Close();
		delete dicFile;
		dicFile = NULL;
		}
	//
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	TInt sizeAfter=fileEntry.iSize;
		test(sizeAfter==sizeBefore);
	//
	//
	test.Next(_L("Repeatedly open a file, commiting after each read"));
	// repeatedly open the dic file and interogate it, commiting the file after each read
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	sizeBefore=fileEntry.iSize;
	//
	for (i=0 ; i<10 ; i++)
		{
		TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
			test(ret==KErrNone);
		RDictionaryReadStream readStream;
		TRAP(ret, readStream.OpenL(*dicFile,testUid1));
			test(ret==KErrNone);
		readStream.Close();
		TRAP(ret, dicFile->CommitL());
			test(ret==KErrNone);
		delete dicFile;
		dicFile = NULL;
		}
	//
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	sizeAfter=fileEntry.iSize;
		test(sizeAfter==sizeBefore);
	//
	//
	test.Next(_L("Repeatedly open a file and re-write a stream"));
	// repeatedly open the dic file and re-write a stream, commiting the file after each write
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	sizeBefore=fileEntry.iSize;
	//
	for (i=0 ; i<10 ; i++)
		{
		TRAP(ret, dicFile = CDictionaryFileStore::OpenL(TheFs,dicFilePath.FullName(),dicFileUid));
			test(ret==KErrNone);
		// write an entry and commit it
		RDictionaryWriteStream writeStream;
		TRAP(ret, writeStream.AssignL(*dicFile,testUid1));
			test(ret==KErrNone);
		TRAP(ret, writeStream.WriteInt32L(i));
			test(ret==KErrNone);
		TRAP(ret, writeStream.CommitL());
			test(ret==KErrNone);
		writeStream.Close();
		// close the file
		TRAP(ret, dicFile->CommitL());
			test(ret==KErrNone);
		delete dicFile;
		dicFile = NULL;
		}
	//
	ret = TheFs.Entry(dicFilePath.FullName(),fileEntry);
		test(ret==KErrNone);
	sizeAfter=fileEntry.iSize;
		test.Printf(_L("  Size before: %d\n  Size after:  %d\n"),sizeBefore,sizeAfter);
	//
	//
	// tidy up
	}

LOCAL_C void AddEntryL()
	{
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	CDictionaryStore* dictionary=CDictionaryFileStore::OpenLC(TheFs,dicFilePath.FullName(),dicFileUid);
	RDictionaryWriteStream s;
	s.AssignLC(*dictionary,testUid4);
	s.WriteInt32L(value1);
	s.CommitL();
	CleanupStack::PopAndDestroy();	// s
   	TInt err = dictionary->Commit(); // changed from CommitL to ensure api coverage
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy();	// dictionary
	}

LOCAL_C TBool CheckEntryL()
	{
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	CDictionaryStore* dictionary=CDictionaryFileStore::OpenLC(TheFs,dicFilePath.FullName(),dicFileUid);
	TBool isPresent=dictionary->IsPresentL(testUid4);
	if (isPresent)
		{
		RDictionaryReadStream s;
		s.OpenLC(*dictionary,testUid4);
		test (s.ReadInt32L()==value1);
		CleanupStack::PopAndDestroy();	// s
		}
	CleanupStack::PopAndDestroy();	// dictionary
	return isPresent;
	}

/**
Test CDictionaryFileStore construction, forcing a leave error at each
possible stage of the process.

@SYMTestCaseID          SYSLIB-STORE-CT-1190
@SYMTestCaseDesc	    Tests for CDictionaryFileStore construction under low memory conditions.
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for construction under low memory conditions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOOML()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1190 Construction under low memory conditions "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse dicFilePath;
	dicFilePath.Set(drive.Name(), &KDicFilePath, NULL);
	
	TheFs.Delete(dicFilePath.FullName()); // delete the file
	TInt failRate=0;

	for (failRate=1;;failRate++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAPD(ret, AddEntryL());
		__UHEAP_MARKEND;
		if (ret==KErrNone)
			break;
		test (ret==KErrNoMemory);
		__UHEAP_RESET;
		test (!CheckEntryL());
		}
	__UHEAP_RESET;
	test (CheckEntryL());
	test.Printf(_L("  #allocs for update: %d\n"),failRate-1);
	//
	// tidy up
	}


// Thread contention tests


class TIniData
	{
public:
	void LoadL(CDictionaryStore& aStore);
	void SaveL(CDictionaryStore& aStore) const;
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(CStreamStore& aStore,TStreamId anId);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	};

void TIniData::LoadL(CDictionaryStore& aStore)
	{
	if (aStore.IsPresentL(KTestUid))
		{
		RDictionaryReadStream stream;
		stream.OpenL(aStore,KTestUid);
		CEmbeddedStore* store=CEmbeddedStore::FromLC(stream);
		RestoreL(*store,store->Root());
		CleanupStack::PopAndDestroy();
		}
	}

void TIniData::SaveL(CDictionaryStore& aStore) const
	{
	RDictionaryWriteStream stream;
	stream.AssignL(aStore,KTestUid);
	CEmbeddedStore* store=CEmbeddedStore::NewLC(stream);
	store->SetRootL(StoreL(*store));
	store->CommitL();
	CleanupStack::PopAndDestroy();
	}

void TIniData::RestoreL(CStreamStore& aStore,TStreamId anId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,anId);
	TStreamId id1,id2;
	stream >> id1 >> id2;
	CleanupStack::PopAndDestroy();
	stream.OpenLC(aStore,id1);
	stream >> *this;
	CleanupStack::PopAndDestroy();
	{
	TReal x=0.501;
	for (TInt ii=0;ii<200;++ii)
		x=4.0*x*(1.0-x);
	}
	stream.OpenLC(aStore,id2);
	stream >> *this;
	CleanupStack::PopAndDestroy();
	}

TStreamId TIniData::StoreL(CStreamStore& aStore) const
	{
	RStoreWriteStream stream;
	TStreamId id2=stream.CreateLC(aStore);
	stream << *this;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	TStreamId id1=stream.CreateLC(aStore);
	stream << *this;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	TStreamId id3=stream.CreateLC(aStore);
	stream << id1 << id2;
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	return id3;
	}

void TIniData::InternalizeL(RReadStream& aStream)
	{
	for (TInt ii=0;ii<150;++ii)
		aStream.ReadUint32L();
	}

void TIniData::ExternalizeL(RWriteStream& aStream) const
	{
	for (TInt ii=0;ii<150;++ii)
		aStream.WriteUint32L(0);
	}

//

void ThreadTestL(TInt aThread)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse testIniFile;
	testIniFile.Set(drive.Name(), &KTestIniFile, NULL);
	
	TIniData data;
	CDictionaryStore* dict=CDictionaryFileStore::OpenLC(fs,testIniFile.FullName(),KTestUid);
	data.LoadL(*dict);
	CleanupStack::PopAndDestroy(); // dict

	TParse threadTestPath;
	threadTestPath.Set(drive.Name(), &KThreadTestPath, NULL);
	
	TFileName name;
	name.Format(threadTestPath.FullName(),aThread);
	dict=CDictionaryFileStore::OpenLC(fs,name,KTestUid);
	data.SaveL(*dict);
	dict->CommitL();
	CleanupStack::PopAndDestroy();
	TReal x=0.501;
	for (TInt ii=0;ii<1000;++ii)
		x=4.0*x*(1.0-x);
	dict=CDictionaryFileStore::OpenLC(fs,name,KTestUid);
	data.LoadL(*dict);
	CleanupStack::PopAndDestroy();	// dict
	fs.Delete(name);
	CleanupStack::PopAndDestroy();	// fs
	}

TInt ThreadTest(TAny* aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (!cleanup)
		return KErrNoMemory;
	TRAPD(error,ThreadTestL(TInt(aPtr)));
	delete cleanup;
	return error;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1191
@SYMTestCaseDesc	    Lock out of dictionary files test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for opening of same dictionary file.Tests for file in use error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void ContentionTestL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1191 Contention tests "));
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse testIniFile;
	testIniFile.Set(drive.Name(), &KTestIniFile, NULL);
	
	CDictionaryStore* dict=CDictionaryFileStore::OpenLC(TheFs,testIniFile.FullName(),KTestUid);
	TIniData data;
	data.SaveL(*dict);
	CleanupStack::PopAndDestroy(); // dict

	test.Next(_L("Lock-out test"));
	CDictionaryFileStore::OpenLC(TheFs,testIniFile.FullName(),KTestUid);
	TUint tick=User::TickCount();
	TRAPD(r,CDictionaryFileStore::OpenL(TheFs,testIniFile.FullName(),KTestUid));
	tick=User::TickCount()-tick;
	test (r==KErrInUse);
	test.Printf(_L("Lock-out discovered: %d ticks\r\n"),tick);
	CleanupStack::PopAndDestroy();

	test.Next(_L("Creating threads"));
	RThread threads[KNumThreads];
	TRequestStatus status[KNumThreads];
	TInt ii;
	for (ii=0;ii<KNumThreads;++ii)
		{
		TName name;
		name.Format(_L("Test_%d"),ii);
		test (threads[ii].Create(name,ThreadTest,KThreadStack,KThreadHeap,KThreadHeapMax,(TAny*)ii,EOwnerThread)==KErrNone);
		threads[ii].SetPriority(EPriorityLess);
		threads[ii].Logon(status[ii]);
		test (status[ii]==KRequestPending);
		}

	for (ii=0;ii<KNumThreads;++ii)
		threads[ii].Resume();

	test.Next(_L("Waiting for completion"));
	for (ii=0;ii<KNumThreads;++ii)
		User::WaitForAnyRequest();
	TInt success=0;
	for (ii=0;ii<KNumThreads;++ii)
		{
		test (status[ii]!=KRequestPending);
		if (status[ii].Int()==KErrNone)
			++success;
		}
	test.Printf(_L("Thread success: %d of %d\r\n"),success,KNumThreads);
	test.End();
	}
//

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	}

GLDEF_C TInt E32Main()
	{
	setupCleanup();
	//
	test.Title();
	test.Start(_L("Testing CDictionaryFileStore et al..."));
	test (TheFs.Connect()==KErrNone);
	//
	// run the testcode (inside an alloc heaven harness)
	__UHEAP_MARK;

	TRAPD(r,systemTestL());
	test(r==KErrNone);

	TRAP(r,generalTestsL());
	test(r==KErrNone);

	TRAP(r,emptyFileTestsL());
	test(r==KErrNone);

	TRAP(r,repeatedUseTestsL());
	test(r==KErrNone);

	TRAP(r,TestOOML());
	test(r==KErrNone);

	TRAP(r,ContentionTestL());
	test(r==KErrNone);
	
	__UHEAP_MARKEND;

	TheFs.Close();
 	test.End();
	test.Close();

	delete TheTrapCleanup;
	return KErrNone;
	}



