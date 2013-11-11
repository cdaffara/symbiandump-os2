// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bautils.h>
#include <barsread2.h>
#include <barsc2.h>
#include <baspi.h>

LOCAL_D RTest TheTest(_L("T_SPI"));
LOCAL_D RFs TheFs;

//Test files
_LIT(KTestDirectoryPath,"z:\\system\\data\\");
_LIT(KRscExtension,".rsc");
_LIT(KXipSpiFile,"z:\\system\\data\\Spi_ECom.SPI");
_LIT(KNonXipSpiFile, "c:\\system\\data\\Spi_ECom.SPI");
_LIT(KXipSpi_EcomRsc1,"z:\\system\\data\\Spi_EcomRsc1.rsc");
_LIT(KXipSpi_EcomRsc2,"z:\\system\\data\\Spi_EcomRsc2.rsc");
_LIT(KXipSpi_EcomRsc3,"z:\\system\\data\\Spi_EcomRsc3.rsc");
_LIT(KNonXipSpi_EcomRsc1,"c:\\system\\data\\Spi_EcomRsc1.rsc");
_LIT(KNonXipSpi_EcomRsc2,"c:\\system\\data\\Spi_EcomRsc2.rsc");
_LIT(KNonXipSpi_EcomRsc3,"c:\\system\\data\\Spi_EcomRsc3.rsc");

//----------------------UTILITY FUNCTIONS----------------------------------
LOCAL_C void CopyFileL(const TDesC& aFrom,const TDesC& aTo)
	{
	TheFs.Delete(aTo);
	CFileMan* man=CFileMan::NewL(TheFs);
	TInt r=man->Copy(aFrom,aTo, CFileMan::ERecurse);
	delete man;
	User::LeaveIfError(r);
	User::LeaveIfError(TheFs.SetAtt(aTo,0,KEntryAttReadOnly)); // clear RO
	}

LOCAL_C void DeleteFile(const TDesC& aFileName)
	{
	// make sure the file is read/write
	TInt err = TheFs.SetAtt(aFileName, 0, KEntryAttReadOnly);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error changing attributes file = %d"),err);
		}
	// delete the file
	err = BaflUtils::DeleteFile(TheFs, aFileName);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error deleting file = %d"),err);
		}
	}

LOCAL_C TInt FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(TheFs, aFileName, EFileRead));
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}

LOCAL_C void DeleteTestFile()
	{
	DeleteFile(KNonXipSpiFile);
	DeleteFile(KNonXipSpi_EcomRsc1);
	DeleteFile(KNonXipSpi_EcomRsc2);
	DeleteFile(KNonXipSpi_EcomRsc3);
	}

//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteTestFile();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)

//---------------------TEST CASE for RResoureArchive------------------
// Tests for RResourceArchive
/*
(1) Test Loading SPI file through RResourceArchive and retrieving the first rsc block
	from XIP media(e.g NOR flash) in basic and OOM environment.
(2) Test Loading SPI file through RResourceArchive and retrieving the first rsc block
	from NON-XIP media(e.g NAND flash) in basic and OOM environment.
(3) Test the Reset() functionality to ensure proper reset functionality by comparing the
	content of the rsc file
(4)	Test Loading some test SPI(with 2-3 random rsc file), interpret the content of rsc file
	through RResourceReader and compare it with the results when opening each rsc file
	separately.
(5) Test Loading some non-spi file using RResourceArchive expecting a KErrCorrupt to be
	returned
*/
class RStaticPluginInfoTest
	{
public:
	static void TestCase1L();
	static void TestCase2L();
	static void TestCase3L();
	static void TestCase4L();
	static void TestCase5L();
private:
	static void TestOpenNextL(const TDesC& aFileName);
	static void TestComparisonL(const TDesC& aFileName);
	};

void RStaticPluginInfoTest::TestCase1L()
	{
	TheTest.Next(_L("Test case 1 Loading/Reading SPI file from XIP media\n"));
	TestOpenNextL(KXipSpiFile);
	}

void RStaticPluginInfoTest::TestCase2L()
	{
	TheTest.Next(_L("Test case 2 Loading/Reading SPI file from Non-XIP media\n"));
	TestOpenNextL(KNonXipSpiFile);
	}

void RStaticPluginInfoTest::TestCase3L()
	{
	__UHEAP_MARK;
	TheTest.Next(_L("Test case 3 Testing functionality of the Reset() function\n"));
	RResourceArchive testArchiveIter;
	testArchiveIter.OpenL(TheFs,KXipSpiFile);

	RResourceReader theReader1;
	RResourceReader theReader3;

	HBufC* rscName1=NULL;
	HBufC* rscName2=NULL;
	HBufC* rscName3=NULL;

	CResourceFile* rscArchive1=testArchiveIter.NextL(rscName1);
	TEST(rscArchive1!=NULL);

	CResourceFile* rscArchive2=testArchiveIter.NextL(rscName2);
	TEST(rscArchive2!=NULL);

	testArchiveIter.Reset();

	CResourceFile* rscArchive3=testArchiveIter.NextL(rscName3);
	TEST(rscArchive3!=NULL);

	//Check to ensure that rscArchive3 content is the same as rscArchive1
	theReader1.OpenLC(rscArchive1,1);
	theReader3.OpenLC(rscArchive3,1);
	TEST(rscArchive1->UidType()==rscArchive3->UidType());
	TEST(rscName1->CompareF(rscName3->Des())==0);
	TEST(theReader1.ReadInt32L()==theReader3.ReadInt32L());

	delete rscName1;
	delete rscName2;
	delete rscName3;
	delete rscArchive1;
	delete rscArchive2;
	delete rscArchive3;

	testArchiveIter.Close();

	CleanupStack::PopAndDestroy(&theReader3);
	CleanupStack::PopAndDestroy(&theReader1);
	__UHEAP_MARKEND;
	}

void RStaticPluginInfoTest::TestCase4L()
	{
	TheTest.Next(_L("Test case 4 Testing comparison of spi and rsc read CResourceFiles\n"));
	TestComparisonL(KXipSpiFile);
	TestComparisonL(KNonXipSpiFile);
	}

void RStaticPluginInfoTest::TestCase5L()
	{
	TheTest.Next(_L("Test case 5 Testing opening a non-spi file\n"));
	__UHEAP_MARK;
	RResourceArchive testArchiveIter;
	TRAPD(err,testArchiveIter.OpenL(TheFs,KXipSpi_EcomRsc1));
	TEST(err==KErrCorrupt);

	__UHEAP_MARKEND;
	}

void RStaticPluginInfoTest::TestComparisonL(const TDesC& aFileName)
	{
	__UHEAP_MARK;
	RResourceArchive testArchiveIter;
	testArchiveIter.OpenL(TheFs,aFileName);
	TUid spiType=testArchiveIter.Type();
	TEST(spiType.iUid=0x10205C2C);

	CResourceFile* spiRscArchive=NULL;
	HBufC* rscName=NULL;
	spiRscArchive=testArchiveIter.NextL(rscName);
	while (spiRscArchive)
		{
		TFileName fullRscName;
		fullRscName.Append(KTestDirectoryPath);
		fullRscName.Append(rscName->Des());
		fullRscName.Append(KRscExtension);
		TInt rscFileSize=FileSizeL(fullRscName);

		//Creating a CResourceFile by opening the individual rsc file
		CResourceFile* existingRscArchive=CResourceFile::NewL(TheFs,fullRscName,0,rscFileSize);
		//Now perform general comparison
		TEST(spiRscArchive->UidType()==existingRscArchive->UidType());
		TEST(spiRscArchive->Offset()==existingRscArchive->Offset());
		//Now perform interpreting and comparing the rsc content
		RResourceReader rscReader;
		RResourceReader spiRscReader;
		spiRscReader.OpenLC(spiRscArchive,1);
		rscReader.OpenLC(existingRscArchive,1);
		//Now perform all the read results and comparison between the two CResourceFile
		const TUid KUidEComResourceFormatV2 = {0x101FB0B9};
		TInt resourceFormatVersion=0;
		resourceFormatVersion+=0;
		//Comparison of Rsc uid
		TUid rscUid = {rscReader.ReadInt32L()};
		TEST(rscUid.iUid==spiRscReader.ReadInt32L());
		if(rscUid==KUidEComResourceFormatV2)
			{
			resourceFormatVersion = 2;
			TUid dllUid={rscReader.ReadInt32L()};
			TEST(dllUid.iUid==spiRscReader.ReadInt32L());
			}
		//Comparison of number of interfaces
		TInt numberOfInterface=rscReader.ReadInt16L();
		TEST(numberOfInterface==spiRscReader.ReadInt16L());
		//Comparison of the each interface uid and implementations under this interface
		for(TInt iFace = 0; iFace < numberOfInterface; ++iFace)
			{
			//Comparison of each interface Uid
			const TUid interfaceUid = {rscReader.ReadInt32L()};
			TEST(interfaceUid.iUid==spiRscReader.ReadInt32L());
			//Comparison of number of implementations under this interface
			const TInt numImplementations = rscReader.ReadInt16L();
			TEST(numImplementations==spiRscReader.ReadInt16L());
			for(TInt impl = 0; impl < numImplementations; ++impl)
				{
				//Comparison of some of the implementation content
				const TUid impUid  = {rscReader.ReadInt32L()};
				TEST(impUid.iUid==spiRscReader.ReadInt32L());
				const TInt version = rscReader.ReadInt8L();
				TEST(version==spiRscReader.ReadInt8L());
				}
			}
		delete existingRscArchive;
		existingRscArchive=NULL;
		delete spiRscArchive;
		spiRscArchive=NULL;
		delete rscName;
		rscName=NULL;
		CleanupStack::PopAndDestroy(&rscReader);
		CleanupStack::PopAndDestroy(&spiRscReader);
		spiRscArchive=testArchiveIter.NextL(rscName);
		}
	testArchiveIter.Close();

	__UHEAP_MARKEND;
	}

void RStaticPluginInfoTest::TestOpenNextL(const TDesC& aFileName)
    {
    //---------Basic testing-------------------------------
	__UHEAP_MARK;

	RResourceArchive testArchiveIter;
	testArchiveIter.OpenL(TheFs,aFileName);
	TUid spiType=testArchiveIter.Type();
	TEST(spiType.iUid==0x10205C2C);
	HBufC* rscName=NULL;
	CResourceFile* rscArchive=testArchiveIter.NextL(rscName);

	delete rscArchive;
	rscArchive=NULL;
	delete rscName;
	rscName=NULL;
	testArchiveIter.Close();

	__UHEAP_MARKEND;

	//-----------OOM testing--------------------------------
	TInt err;
	TInt tryCount = 0;
	tryCount=tryCount;
	RResourceArchive testArchiveIter1;
	testArchiveIter1.OpenL(TheFs,aFileName);
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
		TRAP(err,rscArchive=testArchiveIter1.NextL(rscName));
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete rscArchive;
		rscArchive=NULL;

		delete rscName;
		rscName=NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);
	testArchiveIter1.Close();
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0094
@SYMTestCaseDesc Testing for the functionality of RResourceArchive
@SYMTestPriority High
@SYMTestActions  Test loading and reading Spi file through Xip and Non-Xip media
				 Test interpreting and comparing rsc content through read from
				 spi file and from individual rsc file.
				 Test all exported functionality of this class in general and
				 OOM conditions.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 806 Eliminate scanning for plugins on startup
*/
LOCAL_C void DoAllTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0094 "));
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());

	TheTest.Printf(_L("Copying SPI files to RAM\r\n"));
	//Copying test spi file and individual rsc files that are included inside spi
	CopyFileL(KXipSpiFile,KNonXipSpiFile);
	CopyFileL(KXipSpi_EcomRsc1,KNonXipSpi_EcomRsc1);
	CopyFileL(KXipSpi_EcomRsc2,KNonXipSpi_EcomRsc2);
	CopyFileL(KXipSpi_EcomRsc3,KNonXipSpi_EcomRsc3);

	//Start running all the TestCase
	RStaticPluginInfoTest::TestCase1L();
	RStaticPluginInfoTest::TestCase2L();
	RStaticPluginInfoTest::TestCase3L();
	RStaticPluginInfoTest::TestCase4L();
	RStaticPluginInfoTest::TestCase5L();

	//Clearing all the test files we have copied to C
	DeleteTestFile();

	CleanupStack::PopAndDestroy(1, &TheFs);
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	TheTest.Title();
	TheTest.Start(_L("Testing RResourceArchive"));
    TRAPD(err,DoAllTestsL());
    TheTest.Printf(_L("Error code is %d\n"),err);
    TEST(err==KErrNone);
    TheTest.End();
    TheTest.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }

