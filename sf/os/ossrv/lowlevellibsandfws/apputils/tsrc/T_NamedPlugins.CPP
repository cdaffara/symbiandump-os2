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
// Tests for CBaNamedPlugins class, which contains CParameters class
// 
//

#include <banamedplugins.h>
#include <e32test.h>
#include <basched.h>
#include <f32file.h>
#include <baflpan.h>

RTest TheTest(_L("T_NamedPlugins"));
RFs TheFs;

//Test macros and functions

LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) Check((arg), __LINE__)
#define TEST2(aValue, aExpected) Check(aValue, aExpected, __LINE__)


TUid KUid1 = {0xE1A01753};
TUid KUid2 = {0xE1B01753};

_LIT(KRscFileName1, "\\resource\\plugins\\E1A01753.RSC");
_LIT(KRscFileName2, "\\resource\\plugins\\E1B01753.RSC");
_LIT(KIdentifier, "plugin");
_LIT(KBadIdentifier, "plugni");
_LIT(KMainPath, "\\system\\data\\");
_LIT(KLitWildCard, "*.RSC");
_LIT(KTextForNone, "NoPlugin");

class CImplementation: public CBaNamedPlugins::MFallBackName
{
	public:

		HBufC* FallBackNameL(const TDesC& aFullResourceFileName) const;
};

HBufC* CImplementation::FallBackNameL(const TDesC& aFullResourceFileName) const
{
	return aFullResourceFileName.AllocL();
}

TBool TestAlgorithmToCompareNames(const TDesC& aName1, const TDesC& aName2)
	{
	return( aName1.CompareC(aName2) == 0);	// Function is supposed to return 1, when CompareC returns 0
	}

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-1756
@SYMTestCaseDesc        CBaNamedPlugins class test
                        Tests for CBaNamedPlugins::IndexOfIdentifier(), CBaNamedPlugins::IndexOfUid(), CBaNamedPlugins::MdcaPoint(), CBaNamedPlugins::IdentifierAtIndex(), CBaNamedPlugins::UidAtIndex(),CBaNamedPlugins::MdcaCount() functions
@SYMTestPriority        Medium
@SYMTestActions         CBaNamedPlugins object is populated with resource file names and checking for identifier and Uid
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void DoTests1()
{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1756 Testing DoTests1... "));
	TBuf<64> buf;

  	CBaNamedPlugins* namedPlugins;
    RArray<CBaNamedPlugins::TResourceFile> arrayOfResourceFiles;
    CleanupClosePushL(arrayOfResourceFiles);

    buf = KRscFileName1;

    CBaNamedPlugins::TResourceFile resourceFile1;
    resourceFile1.iFullFileName = buf.AllocLC();
    resourceFile1.iIdentifier = NULL;
    resourceFile1.iUid = KUid1;
    resourceFile1.iFormat = CBaNamedPlugins::TResourceFile::EFormatTbuf;
    User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile1));
    CleanupStack::Pop(resourceFile1.iFullFileName);

    buf = KRscFileName2;

    CBaNamedPlugins::TResourceFile resourceFile2;
    resourceFile2.iFullFileName = buf.AllocLC();
    buf = KIdentifier;
    resourceFile2.iIdentifier = buf.AllocLC();
    resourceFile2.iUid = KUid2;
    resourceFile2.iFormat = CBaNamedPlugins::TResourceFile::EFormatTbuf;
    User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile2));
    CleanupStack::Pop(resourceFile2.iIdentifier);

    CleanupStack::Pop(resourceFile2.iFullFileName);

    CBaNamedPlugins::CParameters* const parameters = CBaNamedPlugins::CParameters::NewLC(TheFs, arrayOfResourceFiles.Array());
	HBufC* textForNone=KTextForNone().AllocL();
  	parameters->SetTextForNone(textForNone,CBaNamedPlugins::EArrayPositionLast);

  	// This call to SetTextForNoneL will delete textForNone
  	parameters->SetTextForNoneL(KTextForNone,CBaNamedPlugins::EArrayPositionLast);

    namedPlugins = CBaNamedPlugins::NewL(*parameters);

    CBaNamedPlugins::TEquivalentIdentifiers eqIdent = TestAlgorithmToCompareNames;
	TInt i = (*namedPlugins).IndexOfIdentifier(KIdentifier,eqIdent);
	// With the sorting algorithm and resource files benig used KIdentifier is at index 0
	TEST2(i,0);
	i = (*namedPlugins).IndexOfIdentifier(KBadIdentifier,eqIdent);
	TEST2(i,KErrNotFound);

	TInt index = namedPlugins->IndexOfUid(KUid1);
    TEST(index>=0);
    TPtrC name;
    if( index >= 0 )
        {
        name.Set((*namedPlugins).MdcaPoint(index));
        }
    index = (namedPlugins)->IndexOfUid(TUid::Uid(0x123));
    TEST2(index,-1);

    TEST((*namedPlugins).IdentifierAtIndex(1)!= &buf);
    TEST((*namedPlugins).UidAtIndex(0)==KUid2);
    TEST((*namedPlugins).UidAtIndex(1)==KUid1);

	TInt count = (*namedPlugins).MdcaCount();
	TEST2(count,3);

    count = arrayOfResourceFiles.Count();
    for(i = 0; i < count; i++ )
        {
        const CBaNamedPlugins::TResourceFile& resourceFileData=arrayOfResourceFiles[i];
        delete resourceFileData.iFullFileName;
        delete resourceFileData.iIdentifier;
        }

    CleanupStack::PopAndDestroy(2, &arrayOfResourceFiles);
    delete namedPlugins;

	// Test EFormatArrayOfUidNamePairs format
	buf = KRscFileName1;

	CleanupClosePushL(arrayOfResourceFiles);

	CBaNamedPlugins::TResourceFile resourceFile;
	resourceFile.iFullFileName = KRscFileName1().AllocLC();
	resourceFile.iIdentifier = NULL;
	resourceFile.iUid = KNullUid;
	resourceFile.iFormat = CBaNamedPlugins::TResourceFile::EFormatArrayOfUidNamePairs;
	User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile));
	CleanupStack::Pop(resourceFile.iFullFileName);

	CBaNamedPlugins::CParameters* const parameters1 = CBaNamedPlugins::CParameters::NewL(TheFs, arrayOfResourceFiles.Array());
	textForNone=KTextForNone().AllocL();
  	parameters1->SetTextForNone(textForNone,CBaNamedPlugins::EArrayPositionFirst);
    namedPlugins = CBaNamedPlugins::NewL(*parameters1);

	delete resourceFile.iFullFileName;

	CleanupStack::PopAndDestroy(&arrayOfResourceFiles);
	delete parameters1;
	delete namedPlugins;
}

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-1757
@SYMTestCaseDesc        CBaNamedPlugins::CParameters class test
                        Tests for CParameters::SetFallBackName,CParameters::SetCompareNames,getting file names by searching file system
@SYMTestPriority        Medium
@SYMTestActions         Setting the algorithm to compare two plugin names
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void DoTests2()
{
     TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1757 Testing DoTests2... "));
     TBuf<64> buf;
     buf = KMainPath;

     CBaNamedPlugins* namedPlugins;
     RArray<CBaNamedPlugins::TResourceFile> arrayOfResourceFiles;
     CleanupClosePushL(arrayOfResourceFiles);

     TFindFile* findFile = new(ELeave) TFindFile(TheFs);
     CleanupStack::PushL(findFile);

     CDir* directory = NULL;

     TInt err = findFile->FindWildByDir(KLitWildCard, buf, directory);
     TInt mdcaCount=0;
     while(err==KErrNone)
         {
         CleanupStack::PushL(directory);
         for (TInt i=directory->Count()-1; i>=0; --i)
             {
             const TEntry& entry = (*directory)[i];
             CBaNamedPlugins::TResourceFile resourceFile;
             resourceFile.iFullFileName = entry.iName.AllocLC();
             resourceFile.iIdentifier = NULL;
             resourceFile.iUid = KNullUid;
             resourceFile.iFormat = CBaNamedPlugins::TResourceFile::EFormatTbuf;
             ++mdcaCount;
             User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile));
             CleanupStack::Pop(resourceFile.iFullFileName);
             }
         CleanupStack::PopAndDestroy(); // directory
         directory = NULL;
       	 err=findFile->FindWild(directory);
         }
     CleanupStack::PopAndDestroy(); // findfile

     CBaNamedPlugins::CParameters* const parameters = CBaNamedPlugins::CParameters::NewL(TheFs, arrayOfResourceFiles.Array());

     CBaNamedPlugins::TCompareNames compName = TestAlgorithmToCompareNames;
     (*parameters).SetCompareNames(compName);
     namedPlugins = CBaNamedPlugins::NewL(*parameters);

     // Test CBaNamedPlugins constructor when fall back name is set
     CImplementation obj;
     (*parameters).SetFallBackName(obj);
     delete namedPlugins;
     namedPlugins = CBaNamedPlugins::NewL(*parameters);

     TInt count = (*namedPlugins).MdcaCount();
     TEST2(count,mdcaCount);

     count=arrayOfResourceFiles.Count();
     for( TInt i = 0; i < count; i++ )
         {
         const CBaNamedPlugins::TResourceFile& resourceFileData=arrayOfResourceFiles[i];
         delete resourceFileData.iFullFileName;
         }
     CleanupStack::PopAndDestroy(&arrayOfResourceFiles);
     delete parameters;
     delete namedPlugins;
}

LOCAL_C void StartTestPanicBadArrayPositionL()
	{
  	CBaNamedPlugins* namedPlugins;
    RArray<CBaNamedPlugins::TResourceFile> arrayOfResourceFiles;
    CleanupClosePushL(arrayOfResourceFiles);
    RFs fs;
 	fs.Connect();
 	TBuf<64> buf;
    buf = KRscFileName2;

	// Test Panic
	CBaNamedPlugins::TResourceFile resourceFile;
	resourceFile.iFullFileName = buf.AllocLC();
	resourceFile.iIdentifier = NULL;
	resourceFile.iUid = KNullUid;
	resourceFile.iFormat = CBaNamedPlugins::TResourceFile::EFormatTbuf;
	User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile));
	CleanupStack::Pop(resourceFile.iFullFileName);

	CBaNamedPlugins::CParameters* const parameters = CBaNamedPlugins::CParameters::NewL(fs, arrayOfResourceFiles.Array());

	HBufC* textForNone=KTextForNone().AllocL();
  	parameters->SetTextForNone(textForNone,static_cast<CBaNamedPlugins::TArrayPosition>(255));		// Pick an illegal position

	// This should panic
    namedPlugins = CBaNamedPlugins::NewL(*parameters);

	delete resourceFile.iFullFileName;

	CleanupStack::PopAndDestroy(&arrayOfResourceFiles);
	delete parameters;
	delete namedPlugins;
	}

LOCAL_C TInt TestPanicBadArrayPosition(TAny* /*aData*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, StartTestPanicBadArrayPositionL());

	// Won't get here but add this line to get rid of ARMV5 warning
	TEST2(err, KErrNone);

	delete cleanup;
	__UHEAP_MARKEND;

	return (KErrNone);
	}

LOCAL_C void StartTestPanicBadResourceFileFormatL()
	{
  	CBaNamedPlugins* namedPlugins;
    RArray<CBaNamedPlugins::TResourceFile> arrayOfResourceFiles;
    CleanupClosePushL(arrayOfResourceFiles);
    RFs fs;
 	fs.Connect();
 	TBuf<64> buf;
    buf = KRscFileName2;

	// Test Panic
	CBaNamedPlugins::TResourceFile resourceFile;
	resourceFile.iFullFileName = buf.AllocLC();
	resourceFile.iIdentifier = NULL;
	resourceFile.iUid = KNullUid;
	resourceFile.iFormat = static_cast<CBaNamedPlugins::TResourceFile::TFormat>(255);		// Pick an illegal format
	User::LeaveIfError(arrayOfResourceFiles.Append(resourceFile));
	CleanupStack::Pop(resourceFile.iFullFileName);

	CBaNamedPlugins::CParameters* const parameters = CBaNamedPlugins::CParameters::NewL(fs, arrayOfResourceFiles.Array());

	// This should panic
    namedPlugins = CBaNamedPlugins::NewL(*parameters);

	delete resourceFile.iFullFileName;

	CleanupStack::PopAndDestroy(&arrayOfResourceFiles);
	delete parameters;
	delete namedPlugins;
	}

LOCAL_C TInt TestPanicBadResourceFileFormat(TAny* /*aData*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, StartTestPanicBadResourceFileFormatL());

	// Won't get here but add this line to get rid of ARMV5 warning
	TEST2(err, KErrNone);

	delete cleanup;
	__UHEAP_MARKEND;

	return (KErrNone);
	}

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-3379
@SYMTestCaseDesc        CBaNamedPlugins::CParameters class test
                        Tests the class panic conditions
@SYMTestPriority        Medium
@SYMTestActions         Cause the class to panic by setting bad enums.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 DEF101753
*/
LOCAL_C void NamedPluginsPanicConditionsThread()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3379 Testing CBaNamedPlugins Panics... "));
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	TRequestStatus status;

	_LIT(KName, "NamedPlugins_Panic_Thread");

	RThread thread;
	const TInt KThreadStackSize =0x2000; // 8k
	const TInt KThreadMinHeapSize   =0x4000; // 16k
	const TInt KThreadMaxHeapSize   =0xf000; // 60k

	// Test EBafPanicBadResourceFileFormat
	TInt rc = thread.Create(KName,TestPanicBadResourceFileFormat,KThreadStackSize,KThreadMinHeapSize,KThreadMaxHeapSize,NULL);

	TEST(rc==KErrNone);

	thread.Logon(status);
	thread.Resume();

	User::WaitForRequest(status);

	// Should result in a EExitPanic exit type and an EBafPanicBadResourceFileFormat exit reason
	TEST(thread.ExitType() == EExitPanic);
	TEST(thread.ExitReason() == EBafPanicBadResourceFileFormat);

	CLOSE_AND_WAIT(thread);

	// Test EBafPanicBadArrayPosition
	rc = thread.Create(KName,TestPanicBadArrayPosition,KThreadStackSize,KThreadMinHeapSize,KThreadMaxHeapSize,NULL);

	TEST(rc==KErrNone);

	thread.Logon(status);
	thread.Resume();

	User::WaitForRequest(status);

	// Should result in a EExitPanic exit type and an EBafPanicBadArrayPosition exit reason
	TEST(thread.ExitType() == EExitPanic);
	TEST(thread.ExitReason() == EBafPanicBadArrayPosition);

	thread.Close();

	User::SetJustInTime(jitEnabled);
	}


void DoTestsL()
{
	TheTest.Next(_L("Calling the tests"));
	TheFs.Connect();
	CleanupClosePushL(TheFs);

	DoTests1();//Getting the resource file names from literals
	DoTests2();//Getting the resource file names by performing a search on the file system
	NamedPluginsPanicConditionsThread();	// Test Panic condition

 	CleanupStack::PopAndDestroy(); // TheFs
}

TInt E32Main()
{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TheTest.Title();
	TheTest.Start(_L("Testing CBaNamedPlugins Class"));
	TRAPD(err,DoTestsL());
	TEST2(err,KErrNone);
	TheTest.End();
	TheTest.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	return(0);
}
