// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <barsc2.h>
#include <barsread2.h>
#include <t_compressed_unicode_1.rsg>
#include <t_compressed_unicode_2.rsg>

RTest test(_L("T_RESOURCE_COMPRESSION2"));
RFs fileServerSession;
_LIT8(KRscFileHeaderData, "0123456789ABCDEF");
_LIT(KRomResourceFileHeader, "z:\\system\\data\\RscHeader.bin");

LOCAL_D const TPtrC rsc_files[] =
	{
	  _L("T_COMPRESSED_UNICODE_1.RSC")
	, _L("T_COMPRESSED_UNICODE_2.RSC")
	, _L("T_DICTIONARY_COMPRESSED_VERSIO_OF_2.RSC")
	, _L("T_CALYPSO_TEST_RESOURCE_FILE_1.RSC")
	, _L("T_NotRscFile.RSC")
	, _L("TRscROMCalypsoComprNewFmt.rsc")
	};
_LIT(KZDir, "z:\\system\\data\\");
_LIT(KCDir, "c:\\");

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	// make sure the file is read/write
	TInt err = fileServerSession.SetAtt(aFullName,0, KEntryAttReadOnly);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error changing attributes file = %d"),err);
		}
	// delete the file
	err = BaflUtils::DeleteFile(fileServerSession, aFullName);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error deleting file = %d"),err);
		}
	}

LOCAL_C TInt FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(fileServerSession, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}

LOCAL_C void CreateFileFromL(const TDesC& aDestFileName, const TDesC& aSrcFileName)
	{
	RFile destFile;
	RFile srcFile;

	CleanupClosePushL(destFile);
	CleanupClosePushL(srcFile);

	BaflUtils::DeleteFile(fileServerSession, aDestFileName);
	User::LeaveIfError(destFile.Create(fileServerSession, aDestFileName, EFileRead | EFileWrite));

	User::LeaveIfError(srcFile.Open(fileServerSession, aSrcFileName, EFileRead));
	TInt size = 0;
	User::LeaveIfError(srcFile.Size(size));
	HBufC8* buf = HBufC8::NewMaxLC(size);
	TPtr8 ptr = buf->Des();
	srcFile.Read(ptr);

	destFile.Write(KRscFileHeaderData);
	destFile.Write(ptr);

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&srcFile);
	CleanupStack::PopAndDestroy(&destFile);
	}

LOCAL_C void CreateFileFromL(TDes& aDestFilePath, const TDesC& aDestFileName, const TDesC& aSrcFilePath)
	{
	aDestFilePath.Copy(KCDir);
	aDestFilePath += _L("N_");
	aDestFilePath += aDestFileName;
	CreateFileFromL(aDestFilePath, aSrcFilePath);
	}

LOCAL_C void Test1L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	test(resourceFile->UidType()==TUidType(TUid::Uid(0x101f4a6b), TUid::Uid(0x000eb205), TUid::Uid(TUint(T_COMPRESSED_UNICODE_1_RESOURCE_1)>>12)));
	HBufC8* resource = NULL;
	RResourceReader resourceReader;

	resource=resourceFile->AllocReadLC(1);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("Gruezi miteinander"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 2);
	test(resourceReader.ReadUint16L()==4);
	test(resourceReader.ReadTPtrCL()==_L("Bonjour"));
	test(resourceReader.ReadTPtrCL()==_L("Ni3 hao3"));
	test(resourceReader.ReadTPtrCL()==_L("Konnichiwa"));
	test(resourceReader.ReadTPtrCL()==_L("Zdravstvuit'e"));
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 3);
	test(resourceReader.ReadUint16L()==3);
	test(resourceReader.ReadTPtrCL()==_L("Gamarjoba"));
	test(resourceReader.ReadTPtrCL()==_L("Gasou"));
	test(resourceReader.ReadTPtrCL()==_L("Ola"));
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 4);
	test(resourceReader.ReadUint16L()==3);
	test(resourceReader.ReadUint8L()==0xbd);
	test(resourceReader.ReadUint8L()==0x5e);
	test(resourceReader.ReadUint8L()==0xf1);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 5);
	test(resourceReader.ReadUint32L()==4);
	test(resourceReader.ReadUint32L()==T_COMPRESSED_UNICODE_1_RESOURCE_5);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resource=resourceFile->AllocReadLC(6);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("_"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 7);
	test(resourceReader.ReadUint8L()==9);
	test(resourceReader.ReadTPtrCL()==_L("To"));
	test(resourceReader.ReadUint8L()==97);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 8);
	test(resourceReader.ReadUint16L()==999);
	test(resourceReader.ReadTPtrCL()==_L("To"));
	test(resourceReader.ReadUint8L()==79);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 9);
	test(resourceReader.ReadUint16L()==53132);
	test(resourceReader.ReadTPtrCL()==_L("T"));
	test(resourceReader.ReadUint8L()==62);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 10);
	test(resourceReader.ReadInt32L()==1253869);
	test(resourceReader.ReadInt32L()==-986324);
	test(resourceReader.ReadUint32L()==0x600ddea1u);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test2L(const TDesC& aResourceFileName, TUid aFirstUid, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	test(resourceFile->UidType()==TUidType(aFirstUid, TUid::Null(), TUid::Uid(TUint(T_COMPRESSED_UNICODE_2_RESOURCE_1)>>12)));
	HBufC8* resource = NULL;
	RResourceReader resourceReader;

	resourceReader.OpenLC(resourceFile, 1);
	test(resourceReader.ReadUint32L()==4);
	test(resourceReader.ReadUint32L()==T_COMPRESSED_UNICODE_2_RESOURCE_1);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resource=resourceFile->AllocReadLC(2);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("My program is cool"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resource=resourceFile->AllocReadLC(3);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("z:\\system\\data\\eikon.mbm"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 4);
	test(resourceReader.ReadInt32L()==9174804);
	test(resourceReader.ReadUint8L()==6);
	test(resourceReader.ReadInt32L()==-6208493);
	test(resourceReader.ReadUint16L()==0x3176);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 5);
	test(resourceReader.ReadInt32L()==-120727);
	test(resourceReader.ReadInt32L()==-82385253);
	test(resourceReader.ReadUint16L()==0x3176);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 6);
	test(resourceReader.ReadUint8L()==2);
	test(resourceReader.ReadTPtrCL()==_L("z:\\system\\data\\uikon.mbm"));
	test(resourceReader.ReadTPtrCL()==_L("z:\\system\\apps\\my_program\\my_program.mbm"));
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test3L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the calypso resoruce file format loading/reading
	 * and was derived from the Test1 function above. Therefore the resource
	 * file it reads is also derived from the file used in test 1.
	 */

	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	HBufC8* resource = NULL;
	RResourceReader resourceReader;

	resource=resourceFile->AllocReadLC(1);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("Gruezi miteinander"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 2);
	test(resourceReader.ReadUint16L()==4);
	test(resourceReader.ReadTPtrCL()==_L("Bonjour"));
	test(resourceReader.ReadTPtrCL()==_L("Ni3 hao3"));
	test(resourceReader.ReadTPtrCL()==_L("Konnichiwa"));
	test(resourceReader.ReadTPtrCL()==_L("Zdravstvuit'e"));
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 3);
	test(resourceReader.ReadUint16L()==3);
	test(resourceReader.ReadTPtrCL()==_L("Gamarjoba"));
	test(resourceReader.ReadTPtrCL()==_L("Gasou"));
	test(resourceReader.ReadTPtrCL()==_L("Ola"));
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 4);
	test(resourceReader.ReadUint16L()==3);
	test(resourceReader.ReadUint8L()==0xbd);
	test(resourceReader.ReadUint8L()==0x5e);
	test(resourceReader.ReadUint8L()==0xf1);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 5);
	test(resourceReader.ReadUint32L()==4);
	test(resourceReader.ReadUint32L()==T_COMPRESSED_UNICODE_1_RESOURCE_5);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resource=resourceFile->AllocReadLC(6);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("_"));
	CleanupStack::PopAndDestroy(resource);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 7);
	test(resourceReader.ReadUint8L()==9);
	test(resourceReader.ReadTPtrCL()==_L("To"));
	test(resourceReader.ReadUint8L()==97);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 8);
	test(resourceReader.ReadUint16L()==999);
	test(resourceReader.ReadTPtrCL()==_L("To"));
	test(resourceReader.ReadUint8L()==79);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 9);
	test(resourceReader.ReadUint16L()==53132);
	test(resourceReader.ReadTPtrCL()==_L("T"));
	test(resourceReader.ReadUint8L()==62);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	resourceReader.OpenLC(resourceFile, 10);
	test(resourceReader.ReadInt32L()==1253869);
	test(resourceReader.ReadInt32L()==-986324);
	test(resourceReader.ReadUint32L()==0x600ddea1u);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	// reading resource 11 takes some time, as it is long
	// and gets compressed.
	// This is useful for the timing measurements in TestDef044572L()
	resourceReader.OpenLC(resourceFile, 11);
	CleanupStack::PopAndDestroy(1, &resourceReader);
	resource = NULL;

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test4L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a calypso resource file
	 * for a signature. Calypso resource files do not have signatures.
	 */

	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile->ConfirmSignatureL(1));
	test(err == KErrCorrupt);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile->ConfirmSignatureL());
	test(err == KErrCorrupt);

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test5L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a resource file
	 * for a signature.
	 * In this case the signature is present and is NOT the first resource.
	 */

	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile->ConfirmSignatureL(1));
	test(err == KErrCorrupt);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile->ConfirmSignatureL());
	test(err == KErrCorrupt);

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test6L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a resource file
	 * for a signature.
	 * In this case the signature is present and is the first resource.
	 */

	CResourceFile* resourceFile = CResourceFile::NewLC(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile->ConfirmSignatureL(1));
	test(err == KErrNone);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile->ConfirmSignatureL());
	test(err == KErrNone);

	CleanupStack::PopAndDestroy(resourceFile);
	}

LOCAL_C void Test7L(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a non resource file.
	 */
	TRAPD(err, (void)CResourceFile::NewL(fileServerSession, aResourceFileName, aFileOffset, aFileSize));
	test(err != KErrNotFound && err != KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0480
@SYMTestCaseDesc        Testing resource files containing Unicode-compressed text
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test1L() and Test2L() functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest1L()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0480 Testing resource files containing Unicode-compressed text "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path += rsc_files[0];
	Test1L(z_path);
	CreateFileFromL(c_path, rsc_files[0], z_path);
	Test1L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[1]);
	Test2L(z_path, TUid::Uid(0x101f4a6b));
	CreateFileFromL(c_path, rsc_files[1], z_path);
	Test2L(c_path, TUid::Uid(0x101f4a6b), KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0481
@SYMTestCaseDesc        Testing dictionary-compressed resource files
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test2L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0481 Testing dictionary-compressed resource files "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[2]);
	Test2L(z_path, TUid::Uid(0x101f5010));
	CreateFileFromL(c_path, rsc_files[2], z_path);
	Test2L(c_path, TUid::Uid(0x101f5010), KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0482-0001
@SYMTestCaseDesc        Testing Calypso ER5u format dictionary-compressed resource files
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test3L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0482-0001 Testing Calypso ER5u format dictionary-compressed resource files "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[3]);
	Test3L(z_path);
	CreateFileFromL(c_path, rsc_files[3], z_path);
	Test3L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[5]);
	Test3L(z_path, FileSizeL(KRomResourceFileHeader), FileSizeL(z_path) - FileSizeL(KRomResourceFileHeader));
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0483-0001
@SYMTestCaseDesc        Testing resource file missing signature behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test4L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0483-0001 Testing resource file missing signature behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[3]);
	Test4L(z_path);
	CreateFileFromL(c_path, rsc_files[3], z_path);
	Test4L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0484-0001
@SYMTestCaseDesc        Testing resource file signature present and not in order behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test5() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0484-0001 Testing resource file signature present and not in order behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[0]);
	Test5L(z_path);
	CreateFileFromL(c_path, rsc_files[0], z_path);
	Test5L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0485-0001
@SYMTestCaseDesc        Testing resource file signature present and in order behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test6L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest6L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0485-0001 Testing resource file signature present and in order behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[1]);
	Test6L(z_path);
	CreateFileFromL(c_path, rsc_files[1], z_path);
	Test6L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0486-0001
@SYMTestCaseDesc        Testing invalid resource file
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test7L() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0486-0001 Testing invalid resource file "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[4]);
	Test7L(z_path);
	CreateFileFromL(c_path, rsc_files[4], z_path);
	Test7L(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
This test is for measuring the performance improvements from
compressed resource file dictionary cache (added DEF044572)

@SYMTestCaseID          SYSLIB-BAFL-CT-0487-0001
@SYMTestCaseDesc        Tests for defect number Def044572L
@SYMTestPriority        High
@SYMTestActions         Attempt for measuring the performance improvements from compressed resource file dictionary cache
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDef044572L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0487-0001 "));
	// measure the time taken to run this test 10 times.
	TTime startTime, stopTime;
	startTime.UniversalTime();

	// run the test 20 times to make the time easier to measure
	for (TInt repeat=0;repeat<20;repeat++)
		{
		// SubTest3 which uses "Calypso ER5u format dictionary-compressed resource files"
		SubTest3L();
		}

	// display the time taken
	// for WINSCW this increase from 3.4 to 1.3 seconds when the
	// compressed resource file dictionary cache was added DEF044572
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Subtest3:%d microseconds\n"), timeTaken.Int64() );
	}

/**
This test checks reading resource files for memory leaks,
when running Out Of Memory.(added as part of DEF044572)

@SYMTestCaseID          SYSLIB-BAFL-CT-0488-0001
@SYMTestCaseDesc        Out of memory tests
@SYMTestPriority        High
@SYMTestActions         Tests for memory leaks while reading resource files.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void MemoryLeakTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0488-0001 "));
	TInt ret = KErrNoMemory;
	TInt pass = 1;

	__UHEAP_MARK;
	// increasing the value of pass until the test completes with KErrNone
	while  (ret != KErrNone )
		{
		// check that the test is failing with KErrNoMemory
		test(ret==KErrNoMemory);

		__UHEAP_FAILNEXT(pass);
		TRAP(ret, SubTest3L());
		__UHEAP_CHECK(0);

		pass++;
		}

	// disable the heap fail
	__UHEAP_RESET;
	__UHEAP_MARKEND;

	}


LOCAL_C void DoE32MainL()
	{
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());

	SubTest1L();
	SubTest2L();
	SubTest3L();
	SubTest4L();
	SubTest5L();
	SubTest6L();
	SubTest7L();
	TestDef044572L();
	MemoryLeakTestL();

	CleanupStack::PopAndDestroy(1, &fileServerSession);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	TRAPD(error, DoE32MainL());
	test(error == KErrNone);
	test.Next(_L("/n"));
	test.End();
	test.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return error;
	}

