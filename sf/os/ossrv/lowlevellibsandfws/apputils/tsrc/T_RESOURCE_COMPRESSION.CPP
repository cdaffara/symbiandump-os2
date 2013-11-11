// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <barsc.h>
#include <barsread.h>
#include <t_compressed_unicode_1.rsg>
#include <t_compressed_unicode_2.rsg>

RTest test=_L("T_RESOURCE_COMPRESSION");
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

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0451
@SYMTestCaseDesc        RResourceReader class functionality test
@SYMTestPriority        High
@SYMTestActions         Tests for reading descriptors,integers
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test1(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0451 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	test(resourceFile.UidType()==TUidType(TUid::Uid(0x101f4a6b), TUid::Uid(0x000eb205), TUid::Uid(TUint(T_COMPRESSED_UNICODE_1_RESOURCE_1)>>12)));
	HBufC8* resource;
	TResourceReader resourceReader;

	resource=resourceFile.AllocReadLC(1);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("Gruezi miteinander"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(2);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==4);
	test(resourceReader.ReadTPtrC()==_L("Bonjour"));
	test(resourceReader.ReadTPtrC()==_L("Ni3 hao3"));
	test(resourceReader.ReadTPtrC()==_L("Konnichiwa"));
	test(resourceReader.ReadTPtrC()==_L("Zdravstvuit'e"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(3);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==3);
	test(resourceReader.ReadTPtrC()==_L("Gamarjoba"));
	test(resourceReader.ReadTPtrC()==_L("Gasou"));
	test(resourceReader.ReadTPtrC()==_L("Ola"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(4);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==3);
	test(resourceReader.ReadUint8()==0xbd);
	test(resourceReader.ReadUint8()==0x5e);
	test(resourceReader.ReadUint8()==0xf1);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(5);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint32()==4);
	test(resourceReader.ReadUint32()==T_COMPRESSED_UNICODE_1_RESOURCE_5);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(6);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("_"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(7);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint8()==9);
	test(resourceReader.ReadTPtrC()==_L("To"));
	test(resourceReader.ReadUint8()==97);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(8);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==999);
	test(resourceReader.ReadTPtrC()==_L("To"));
	test(resourceReader.ReadUint8()==79);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(9);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==53132);
	test(resourceReader.ReadTPtrC()==_L("T"));
	test(resourceReader.ReadUint8()==62);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(10);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadInt32()==1253869);
	test(resourceReader.ReadInt32()==-986324);
	test(resourceReader.ReadUint32()==0x600ddea1u);
	CleanupStack::PopAndDestroy(resource);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0452
@SYMTestCaseDesc        RResourceReader class test
                        Tests for RResourceReader::UidType() function
@SYMTestPriority        High
@SYMTestActions         Tests for reading compressed unicode
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test2(const TDesC& aResourceFileName, TUid aFirstUid, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0452 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	test(resourceFile.UidType()==TUidType(aFirstUid, TUid::Null(), TUid::Uid(TUint(T_COMPRESSED_UNICODE_2_RESOURCE_1)>>12)));
	HBufC8* resource;
	TResourceReader resourceReader;

	resource=resourceFile.AllocReadLC(1);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint32()==4);
	test(resourceReader.ReadUint32()==T_COMPRESSED_UNICODE_2_RESOURCE_1);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(2);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("My program is cool"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(3);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("z:\\system\\data\\eikon.mbm"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(4);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadInt32()==9174804);
	test(resourceReader.ReadUint8()==6);
	test(resourceReader.ReadInt32()==-6208493);
	test(resourceReader.ReadUint16()==0x3176);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(5);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadInt32()==-120727);
	test(resourceReader.ReadInt32()==-82385253);
	test(resourceReader.ReadUint16()==0x3176);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(6);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint8()==2);
	test(resourceReader.ReadTPtrC()==_L("z:\\system\\data\\uikon.mbm"));
	test(resourceReader.ReadTPtrC()==_L("z:\\system\\apps\\my_program\\my_program.mbm"));
	CleanupStack::PopAndDestroy(resource);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0453
@SYMTestCaseDesc        RResourceReader class test
						Tests the calypso resource file format for loading and reading
@SYMTestPriority        High
@SYMTestActions         Tests for reading calypso resource file format
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test3(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the calypso resoruce file format loading/reading
	 * and was derived from the Test1 function above. Therefore the resource
	 * file it reads is also derived from the file used in test 1.
	 */

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0453 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);
	HBufC8* resource;
	TResourceReader resourceReader;

	resource=resourceFile.AllocReadLC(1);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("Gruezi miteinander"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(2);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==4);
	test(resourceReader.ReadTPtrC()==_L("Bonjour"));
	test(resourceReader.ReadTPtrC()==_L("Ni3 hao3"));
	test(resourceReader.ReadTPtrC()==_L("Konnichiwa"));
	test(resourceReader.ReadTPtrC()==_L("Zdravstvuit'e"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(3);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==3);
	test(resourceReader.ReadTPtrC()==_L("Gamarjoba"));
	test(resourceReader.ReadTPtrC()==_L("Gasou"));
	test(resourceReader.ReadTPtrC()==_L("Ola"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(4);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==3);
	test(resourceReader.ReadUint8()==0xbd);
	test(resourceReader.ReadUint8()==0x5e);
	test(resourceReader.ReadUint8()==0xf1);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(5);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint32()==4);
	test(resourceReader.ReadUint32()==T_COMPRESSED_UNICODE_1_RESOURCE_5);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(6);
	test(resource->Length()%sizeof(TText)==0);
	test(TPtrC((TText*)resource->Ptr(), resource->Length()/sizeof(TText))==_L("_"));
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(7);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint8()==9);
	test(resourceReader.ReadTPtrC()==_L("To"));
	test(resourceReader.ReadUint8()==97);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(8);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==999);
	test(resourceReader.ReadTPtrC()==_L("To"));
	test(resourceReader.ReadUint8()==79);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(9);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadUint16()==53132);
	test(resourceReader.ReadTPtrC()==_L("T"));
	test(resourceReader.ReadUint8()==62);
	CleanupStack::PopAndDestroy(resource);

	resource=resourceFile.AllocReadLC(10);
	resourceReader.SetBuffer(resource);
	test(resourceReader.ReadInt32()==1253869);
	test(resourceReader.ReadInt32()==-986324);
	test(resourceReader.ReadUint32()==0x600ddea1u);
	CleanupStack::PopAndDestroy(resource);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0454
@SYMTestCaseDesc        Tests for reading calypso resource file for signature.
						Calypso resource files do not have signatures.
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceFile::ConfirmSignatureL() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test4(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a calypso resource file
	 * for a signature. Calypso resource files do not have signatures.
	 */

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0454 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile.ConfirmSignatureL(1));
	test(err == KErrCorrupt);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile.ConfirmSignatureL());
	test(err == KErrCorrupt);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0456
@SYMTestCaseDesc        Tests the behaviour of BAFL when reading a resource file for a signature.
@SYMTestPriority        High
@SYMTestActions         Tests for reading a resource file for signature.
						In this case the signature is present and is NOT the first resource.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test5(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a resource file
	 * for a signature.
	 * In this case the signature is present and is NOT the first resource.
	 */

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0456 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile.ConfirmSignatureL(1));
	test(err == KErrCorrupt);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile.ConfirmSignatureL());
	test(err == KErrCorrupt);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0457
@SYMTestCaseDesc        Tests the behaviour of BAFL when reading a resource file for a signature.
						The signature is present and is the first resource.
@SYMTestPriority        High
@SYMTestActions         Tests for RResourceFile::ConfirmSignatureL() function
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test6(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a resource file
	 * for a signature.
	 * In this case the signature is present and is the first resource.
	 */

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0457 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;
	TRAP(err, resourceFile.ConfirmSignatureL(1));
	test(err == KErrNone);

	// New function that has further test to confirm absence of signature
	// Hopefully we can get clients to move over to this in future.
	TRAP(err, resourceFile.ConfirmSignatureL());
	test(err == KErrNone);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0458
@SYMTestCaseDesc        Tests the behaviour of BAFL when reading a non resource file
@SYMTestPriority        High
@SYMTestActions         Attempt to open a non resource file.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test7(const TDesC& aResourceFileName, TUint aFileOffset = 0, TInt aFileSize = 0)
	{
	/* This function tests the behaviour of bafl when reading a non resource file.
	 */

	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0458 "));
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);

	// Need to pass the TInt as required, but doesn't
	// use it. Legacy for BC sake.
	TInt err = KErrNone;

	TRAP(err, resourceFile.OpenL(fileServerSession, aResourceFileName, aFileOffset, aFileSize));
	test(err != KErrNotFound && err != KErrNone);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0459
@SYMTestCaseDesc        Tests resource files containing Unicode-compressed text
@SYMTestPriority        High
@SYMTestActions         Calls up Test1() and Test2() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest1L()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0459 Testing resource files containing Unicode-compressed text "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path += rsc_files[0];
	Test1(z_path);
	CreateFileFromL(c_path, rsc_files[0], z_path);
	Test1(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[1]);
	Test2(z_path, TUid::Uid(0x101f4a6b));
	CreateFileFromL(c_path, rsc_files[1], z_path);
	Test2(c_path, TUid::Uid(0x101f4a6b), KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0460
@SYMTestCaseDesc        Tests for dictionary-compressed resource files
@SYMTestPriority        High
@SYMTestActions         Check by reading the dictionary-compressed resource files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0460 Testing dictionary-compressed resource files "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[2]);
	Test2(z_path, TUid::Uid(0x101f5010));
	CreateFileFromL(c_path, rsc_files[2], z_path);
	Test2(c_path, TUid::Uid(0x101f5010), KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0461
@SYMTestCaseDesc        Tests for reading the Calypso ER5u format dictionary-compressed resource files
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test3() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0461 Testing Calypso ER5u format dictionary-compressed resource files "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[3]);

	Test3(z_path);
	CreateFileFromL(c_path, rsc_files[3], z_path);
	Test3(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[5]);
	Test3(z_path, FileSizeL(KRomResourceFileHeader), FileSizeL(z_path) - FileSizeL(KRomResourceFileHeader));
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0462
@SYMTestCaseDesc        Tests for resource file missing signature behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test4() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0462 Testing resource file missing signature behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[3]);

	Test4(z_path);
	CreateFileFromL(c_path, rsc_files[3], z_path);
	Test4(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0463
@SYMTestCaseDesc        Testing resource file signature present and not in order behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test5() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0463 Testing resource file signature present and not in order behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[0]);

	Test5(z_path);
	CreateFileFromL(c_path, rsc_files[0], z_path);
	Test5(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0464
@SYMTestCaseDesc        Tests for resource file signature and in order behaviour
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test6() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest6L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0464 Testing resource file signature present and in order behaviour "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[1]);

	Test6(z_path);
	CreateFileFromL(c_path, rsc_files[1], z_path);
	Test6(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0465
@SYMTestCaseDesc        Tests for invalid resource file
@SYMTestPriority        High
@SYMTestActions         Wrapper function calling up Test7() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void SubTest7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0465 Testing invalid resource file "));
	TBuf<128> z_path;
	TBuf<128> c_path;

	z_path.Copy(KZDir);
	z_path.Append(rsc_files[4]);

	Test7(z_path);
	CreateFileFromL(c_path, rsc_files[4], z_path);
	Test7(c_path, KRscFileHeaderData().Length(), FileSizeL(z_path));
	DeleteDataFile(c_path);
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

