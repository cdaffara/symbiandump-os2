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

#include <e32test.h>
#include <bautils.h>
#include <barsread2.h>
#include <barsc2.h>
#include <baspi.h>
#include "BaArchiveImpl.h"


LOCAL_D RTest TheTest(_L("T_RSCARCHIVE"));
LOCAL_D RFs TheFs;

//Test files
_LIT(KXipSpiFile,"z:\\system\\data\\Spi_ECom.SPI");
_LIT(KNonXipSpiFile, "c:\\system\\data\\Spi_ECom.SPI");


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

LOCAL_C void DeleteTestFile()
	{
	DeleteFile(KNonXipSpiFile);
	//Testcase 1 files
	DeleteFile(_L("c:\\system\\data\\spi_ecom-0-0.spi"));
	DeleteFile(_L("c:\\system\\data\\spi_ecom-1-0.spi"));
	//Testcase 2 files
	DeleteFile(_L("c:\\system\\data\\ecom-1-0.spi"));
	DeleteFile(_L("c:\\system\\data\\ecom-1-0.s02"));
	DeleteFile(_L("c:\\system\\data\\ecom-2-0.spi"));
	DeleteFile(_L("c:\\system\\data\\ecom-2-0.s02"));
	//Testcase 3 files
	DeleteFile(_L("c:\\system\\data\\cecom-0-0.spi"));
	DeleteFile(_L("c:\\system\\data\\cecom-1-0.spi"));
	//Testcase 5 files
	DeleteFile(_L("c:\\system\\data\\spi_ecom_case-0-0.spi"));
	DeleteFile(_L("c:\\system\\data\\spi_ecom_case-1-0.spi"));
	}

LOCAL_C void CopyTestFile()
	{
	CopyFileL(KXipSpiFile,KNonXipSpiFile);
	//Testcase1 files
	CopyFileL(_L("z:\\system\\data\\spi_ecom-0-0.spi"),_L("c:\\system\\data\\spi_ecom-0-0.spi"));
	CopyFileL(_L("z:\\system\\data\\spi_ecom-1-0.spi"),_L("c:\\system\\data\\spi_ecom-1-0.spi"));
	//Testcase2 files
	CopyFileL(_L("z:\\system\\data\\ecom-1-0.spi"),_L("c:\\system\\data\\ecom-1-0.spi"));
	CopyFileL(_L("z:\\system\\data\\ecom-1-0.s02"),_L("c:\\system\\data\\ecom-1-0.s02"));
	CopyFileL(_L("z:\\system\\data\\ecom-2-0.spi"),_L("c:\\system\\data\\ecom-2-0.spi"));
	CopyFileL(_L("z:\\system\\data\\ecom-2-0.s02"),_L("c:\\system\\data\\ecom-2-0.s02"));
	//Testcase 3 files
	CopyFileL(_L("z:\\system\\data\\cecom-0-0.spi"),_L("c:\\system\\data\\cecom-0-0.spi"));
	CopyFileL(_L("z:\\system\\data\\cecom-1-0.spi"),_L("c:\\system\\data\\cecom-1-0.spi"));
	//Testcase 5 files
	CopyFileL(_L("z:\\system\\data\\spi_ecom_case-0-0.spi"),_L("c:\\system\\data\\spi_ecom_case-0-0.spi"));
	CopyFileL(_L("z:\\system\\data\\spi_ecom_case-1-0.spi"),_L("c:\\system\\data\\spi_ecom_case-1-0.spi"));
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
class RStaticPluginInfoTest
	{
public:
	static void TestCase1L(const TDesC& aPath);
	static void TestCase2L(const TDesC& aPath);
	static void TestCase3L(const TDesC& aPath);
	static void TestCase4L(const TDesC& aPath);
	static void TestCase5L(const TDesC& aPath);
	};

typedef void (*FunctionPtr)(const TDesC&);

static void DoBasicTestingL(FunctionPtr aFunctionPtr,const TDesC& aPath,const TDesC& aDescription)
{
	TheTest.Next(aDescription);
	__UHEAP_MARK;
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	aFunctionPtr(aPath);

	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
}


static void DoOOMTestingL(FunctionPtr aFunctionPtr,const TDesC& aPath,const TDesC& aDescription)
{
	TheTest.Next(aDescription);
	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err,aFunctionPtr(aPath));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		//Add ReleaseLanguage for Test4
		BaflUtils::ReleaseIdealLanguage();

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);
		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);
	RDebug::Print(_L("Success at count=%d\n"),tryCount);
}

/**
@SYMTestCaseID SYSLIB-BAFL-UT-1698
@SYMTestCaseDesc Test for creating and deleting newly added constructor for CResourceArchiveImpl
@SYMTestPriority High
@SYMTestActions
-create and delete CResourceArchiveImpl
-check for the resource file entry from multiple spi files
-and test for the replacing/hiding feature
files used
-----------------------------------------------------------
spi_ecom-0-0.spi |	spi_ecom-1-0.spi  |	Visible resources
-----------------------------------------------------------
Spi_EComRsc1	 |	hide Spi_EComRsc1 |	Spi_EComRsc2(id=1)
Spi_EComRsc2	 |	Spi_EComRsc2	  |	Spi_EComRsc3(id=0)
Spi_EComRsc3	 |	Spi_EComRsc4	  |	Spi_EComRsc4(id=1)
@SYMTestExpectedResults The test must not fail.
@SYMREQ CR0759
*/
void RStaticPluginInfoTest::TestCase1L(const TDesC& aPath)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1698 "));
	//create and delete
	CResourceArchiveImpl* impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("Spi_ECom"));
	delete impl;

	//check and read resource entry
	impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("Spi_ECom"));
	CleanupStack::PushL(impl);
	HBufC* rscName=NULL;
	CResourceFile* rscFile=NULL;
	TInt count=0;
	while (impl->NextResourceExist())
		{
		TFileName expectedFile;
		expectedFile.Append(aPath);
		//test the rsc entry file association
		if (count==0 || count==2)
			{
			expectedFile.Append(_L("spi_ecom-1-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		else
			{
			expectedFile.Append(_L("spi_ecom-0-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		count++;
		rscFile=impl->NextL(rscName);
		TEST(rscFile!=NULL);
		TEST(rscName!=NULL);
		TBuf<12> buf;
		TInt temp=count+1;
		buf.AppendFormat(_L("Spi_EComRsc%d"),temp);
		TEST(rscName->Compare(buf)==0);
		delete rscName;
		rscName=NULL;
		delete rscFile;
		rscFile=NULL;
		}
	TEST(count==3);

	//Testing Reset
	TEST(!impl->NextResourceExist());
	impl->Reset();
	TEST(impl->NextResourceExist());

	//Testing Type
	const TUid KEcomSpiFileTypeUid = {0x10205C2C};
	TEST(impl->Type()==KEcomSpiFileTypeUid);

	CleanupStack::PopAndDestroy();
	}


/**
@SYMTestCaseID SYSLIB-BAFL-CT-1695
@SYMTestCaseDesc Test that loading of multiple localised spis return a correct list
of resource files
@SYMTestPriority High
@SYMTestActions  Set different language downgrade path before using CResourceArchiveImpl
to open a path containing a mix of spi files. Check that the list returned when
calling NextL contains the expected resource files.
-------------------------------------------------------------------------------
Ecom-1-0.spi | Ecom-1-0.s02 | Ecom-2-0.spi | Ecom-2-0.s02  |DGP(1)   | DGP(2-1)
ROFS(1)	     | (ROFS1)      | (ROFS2)	   | (ROFS2)       |
-------------------------------------------------------------------------------
A.RSC		 |	A.R02		|   B.RSC	   |  Hide D.R02   |A.RSC(1) | A.R02(1)
B.RSC	     |	D.R02		|			   |  C.R02        |B.RSC(2) | B.RSC(2)
C.RSC        |              |              |               |C.RSC(1) | C.R02(2)
-------------------------------------------------------------------------------
@SYMTestExpectedResults The test must not fail.
@SYMREQ CR0759
*/
void RStaticPluginInfoTest::TestCase2L(const TDesC& aPath)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1695 "));
	//Language DownGradePath(1)
	CResourceArchiveImpl* impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("ECom"));
	CleanupStack::PushL(impl);
	HBufC* rscName=NULL;
	CResourceFile* rscFile=NULL;
	TInt count=0;
	while (impl->NextResourceExist())
		{
		TFileName expectedFile;
		expectedFile.Append(aPath);
		rscFile=impl->NextL(rscName);
		if (count==0)
			{
			TEST(rscName->Compare(_L("Spi_EComRscA"))==0);
			//also testing the spi file association of the resource entry
			expectedFile.Append(_L("ecom-1-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		if (count==1)
			{
			TEST(rscName->Compare(_L("Spi_EComRscB"))==0);
			expectedFile.Append(_L("ecom-2-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		if (count==2)
			{
			TEST(rscName->Compare(_L("Spi_EComRscC"))==0);
			expectedFile.Append(_L("ecom-1-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		count++;
		delete rscName;
		rscName=NULL;
		delete rscFile;
		rscFile=NULL;
		}
	TEST(count==3);
	CleanupStack::PopAndDestroy();

	//Language DownGradePath(2-1)
	User::LeaveIfError(BaflUtils::SetIdealLanguage(ELangFrench));
	impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("ECom"));
	CleanupStack::PushL(impl);
	rscName=NULL;
	rscFile=NULL;
	count=0;
	while (impl->NextResourceExist())
		{
		TFileName expectedFile;
		expectedFile.Append(aPath);
		rscFile=impl->NextL(rscName);
		if (count==0)
			{
			TEST(rscName->Compare(_L("Spi_EComRscA"))==0);
			//also testing the spi file association of the resource entry
			expectedFile.Append(_L("ecom-1-0.s02"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		if (count==1)
			{
			TEST(rscName->Compare(_L("Spi_EComRscB"))==0);
			expectedFile.Append(_L("ecom-2-0.spi"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		if (count==2)
			{
			TEST(rscName->Compare(_L("Spi_EComRscC"))==0);
			expectedFile.Append(_L("ecom-2-0.s02"));
			TEST(impl->iRscList[count].iFileNamePtr.Compare(expectedFile)==0);
			}
		count++;
		delete rscName;
		rscName=NULL;
		delete rscFile;
		rscFile=NULL;
		}
	TEST(count==3);
	CleanupStack::PopAndDestroy();

	BaflUtils::ReleaseIdealLanguage();
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1696
@SYMTestCaseDesc Test opening corrupt file and an inconsistent list of spi files
@SYMTestPriority High
@SYMTestActions  Try to open an invalid/corrupt spi file. Try to open a list
of spi files where one of them has a different spi type, expect this to leave
with KErrCorrupt.
@SYMTestExpectedResults The test must not fail.
@SYMREQ CR0759
*/
//negative testing using cecom-0-0.spi(Corrupt) and cecom-1-0.spi(OK)
void RStaticPluginInfoTest::TestCase3L(const TDesC& aPath)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1696 "));
	TInt err=KErrNone;
	CResourceArchiveImpl* impl=NULL;
	//Testing opening single corrupt spi file
	TFileName filename;
	filename.Append(aPath);
	filename.Append(_L("cecom-0-0.spi"));
	TRAP(err,impl=CResourceArchiveImpl::NewL(TheFs,filename));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	TEST(err==KErrCorrupt);
	delete impl;

	//Testing opening spi files where one is not a valid spi file
	TRAP(err,impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("CECom")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	TEST(err==KErrCorrupt);
	delete impl;
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1697
@SYMTestCaseDesc Test opening file which does not exist
@SYMTestPriority High
@SYMTestActions  Try to open spi files in a specified path but there
is no file that matches the specified pattern to look for.
@SYMTestExpectedResults The test must not fail.
@SYMREQ CR0759
*/
void RStaticPluginInfoTest::TestCase4L(const TDesC& aPath)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1697 "));
	TInt err=KErrNone;
	TRAP(err,CResourceArchiveImpl* impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("SpiECom")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	TEST(err==KErrNotFound);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-UT-3473
@SYMTestCaseDesc Test that resource selection is case insensitive
@SYMTestPriority High
@SYMTestActions
-create a CResourceArchiveImpl
-check the correct resource file entry is chosen from multiple spi files
------------------------------------------------------------------
Spi_ECom_Case-0-0.spi |	Spi_ECom_Case-1-0.spi  | Visible resources
------------------------------------------------------------------
Spi_EComRsc1	      |	Spi_ECOMRSC1           | Spi_ECOMRSC1(id=1)
Spi_ECOMRSC2	      |	Spi_EComRsc2	       | Spi_EComRsc2(id=1)
------------------------------------------------------------------
@SYMTestExpectedResults The overridden resource is chosen regardless of case.
@SYMDEF DEF105972
@SYMUnit CResourceArchiveImpl::ProcessEntryL()
*/
void RStaticPluginInfoTest::TestCase5L(const TDesC& aPath)
	{
	//check and read resource entry
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-3473 "));
	CResourceArchiveImpl* impl=CResourceArchiveImpl::NewL(TheFs,aPath,_L("Spi_ECom_Case"));
	CleanupStack::PushL(impl);
	HBufC* rscName=NULL;
	CResourceFile* rscFile=NULL;
	TInt count=0;
	while (impl->NextResourceExist())
		{
		TFileName expectedFile;
		expectedFile.Append(aPath);
		//test that the overridden resource is always chosen
		expectedFile.Append(_L("spi_ecom_case-1-0.spi"));
		TEST(impl->iRscList[count].iFileNamePtr.CompareF(expectedFile)==0);

		count++;
		rscFile=impl->NextL(rscName);
		TEST(rscFile!=NULL);
		TEST(rscName!=NULL);
		delete rscName;
		rscName=NULL;
		delete rscFile;
		rscFile=NULL;
		}
	TEST(count==2);

	impl->Reset();
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void DoAllTestsL()
	{
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());

	TheTest.Printf(_L("Copying SPI files to RAM\r\n"));

	//Copying test spi file and individual rsc files that are included inside spi
	CopyTestFile();

	//Basic Test
	DoBasicTestingL(RStaticPluginInfoTest::TestCase1L,_L("z:\\system\\data\\"),_L("Test Case 1 Basic Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase1L,_L("c:\\system\\data\\"),_L("Test Case 1 Basic Non-Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase2L,_L("z:\\system\\data\\"),_L("Test Case 2 Basic Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase2L,_L("c:\\system\\data\\"),_L("Test Case 2 Basic Non-Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase3L,_L("z:\\system\\data\\"),_L("Test Case 3 Basic Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase3L,_L("c:\\system\\data\\"),_L("Test Case 3 Basic Non-Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase4L,_L("z:\\system\\data\\"),_L("Test Case 4 Basic Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase4L,_L("c:\\system\\data\\"),_L("Test Case 4 Basic Non-Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase5L,_L("z:\\system\\data\\"),_L("Test Case 5 Basic Xip"));
	DoBasicTestingL(RStaticPluginInfoTest::TestCase5L,_L("c:\\system\\data\\"),_L("Test Case 5 Basic Non-Xip"));

	//OOM Test
	DoOOMTestingL(RStaticPluginInfoTest::TestCase1L,_L("z:\\system\\data\\"),_L("Test Case 1 OOM Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase1L,_L("c:\\system\\data\\"),_L("Test Case 1 OOM Non-Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase2L,_L("z:\\system\\data\\"),_L("Test Case 2 OOM Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase2L,_L("c:\\system\\data\\"),_L("Test Case 2 OOM Non-Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase3L,_L("z:\\system\\data\\"),_L("Test Case 3 OOM Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase3L,_L("c:\\system\\data\\"),_L("Test Case 3 OOM Non-Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase4L,_L("z:\\system\\data\\"),_L("Test Case 4 OOM Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase4L,_L("c:\\system\\data\\"),_L("Test Case 4 OOM Non-Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase5L,_L("z:\\system\\data\\"),_L("Test Case 5 OOM Xip"));
	DoOOMTestingL(RStaticPluginInfoTest::TestCase5L,_L("c:\\system\\data\\"),_L("Test Case 5 OOM Non-Xip"));

	//Clearing all the test files we have copied to C
	DeleteTestFile();

	CleanupStack::PopAndDestroy(1, &TheFs);
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	TheTest.Title();
	TheTest.Start(_L("Testing RResourceArchive internal class"));
    TRAPD(err,DoAllTestsL());
    TheTest.Printf(_L("Error code is %d\n"),err);
    TEST(err==KErrNone);
    TheTest.End();
    TheTest.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }

