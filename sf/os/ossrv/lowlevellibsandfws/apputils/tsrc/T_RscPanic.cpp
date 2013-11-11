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
// Tests RResourceFile/CResourceFile classes - panic/leave tests
// 
//

#include <e32test.h>
#include <bautils.h>
#include <barsc.h>
#include <barsc2.h>
#include <barsread2.h>

LOCAL_D RTest test(_L("T_RSCPANIC"));
LOCAL_D RFs TheFs;
_LIT8(KRscFileHeaderData, "0123456789ABCDEF");
_LIT(KPanicThread,"panicThread");

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	// make sure the file is read/write
	TInt err = TheFs.SetAtt(aFullName, 0, KEntryAttReadOnly);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error changing attributes file = %d"),err);
		}
	// delete the file
	err = BaflUtils::DeleteFile(TheFs, aFullName);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error deleting file = %d"),err);
		}
	}

LOCAL_C TInt FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(TheFs, aFileName, EFileRead));
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

	BaflUtils::DeleteFile(TheFs, aDestFileName);
	User::LeaveIfError(destFile.Create(TheFs, aDestFileName, EFileRead | EFileWrite));

	User::LeaveIfError(srcFile.Open(TheFs, aSrcFileName, EFileRead));
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

class TestRsc
	{
public:
	void TestOpenReadL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize);
//private:
	void TestReadL(CResourceFile* aRscFile);
    };

void TestRsc::TestOpenReadL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize)
	{
	test.Next(aTitle);
	CResourceFile* rsc = CResourceFile::NewL(TheFs, aFileName, aFileOffset, aFileSize);
	CleanupStack::PushL(rsc);
	TestReadL(rsc);
	CleanupStack::PopAndDestroy(rsc);
	}

void TestRsc::TestReadL(CResourceFile* aRscFile)
	{
	RResourceReader resourceReader;
	const TInt rsc_index[] = {2, 3, 4, 5, 6, 7, 8, 9};
	for(TInt i=0;i<TInt(sizeof(rsc_index)/sizeof(rsc_index[0]));i++)
		{
		const TInt max_size = 10;
		for(TInt j=0;j<max_size;j++)
			{
			resourceReader.OpenLC(aRscFile, rsc_index[i]);
			TUint8 temp[max_size + 1];
			resourceReader.ReadL(temp, j);
			CleanupStack::PopAndDestroy(1, &resourceReader);
			}
		}
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0482
@SYMTestCaseDesc        RResourceReader class test
@SYMTestPriority        High
@SYMTestActions         Tests for the opening of a file,should panic on invalid file
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C TInt DoTestL(const TDesC& aFileName, TUint aFileOffset = 0, TUint aFileSize = 0)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0482 "));
	TestRsc t2;
	TBuf<128> buf;
	buf.Format(_L("Testing with invalid file,C class,file=%S"), &aFileName);
	TRAPD(err, t2.TestOpenReadL(buf, aFileName, aFileOffset, aFileSize));
	return err;
	}

TInt ThreadFunc(TAny*)
  	{
  	CTrapCleanup* cleanup=CTrapCleanup::New();
  	TestRsc panicObj;
  	CResourceFile* rsc=NULL;
  	TRAPD(err,panicObj.TestReadL(rsc))//calls RResourceReader::OpenLC, with NULL as argument, hence panics
  	delete cleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1790
@SYMTestCaseDesc        Testing panics on RResourceReader class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false,by passing a NULL value to RResourceReader::OpenLC()
@SYMTestExpectedResults Tests must panic
@SYMREQ                 REQ0000
*/
void PanicTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1790 "));
 	TRequestStatus threadStatus;
  	RThread thread;
  	TInt rc;
  	TBool jit;
  	jit = User::JustInTime();
  	User::SetJustInTime(EFalse);

  	rc = thread.Create(KPanicThread, ThreadFunc,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
  	test(KErrNone == rc);

  	thread.Logon(threadStatus);
  	thread.Resume();
  	User::WaitForRequest(threadStatus);
  	User::SetJustInTime(jit);

  	test(thread.ExitType() == EExitPanic);
  	thread.Close();
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0483
@SYMTestCaseDesc        Tests for the functionality of RResourceFile/CResourceFile
@SYMTestPriority        High
@SYMTestActions         Tests for creation,deleting,opening of a resource file,
                        should panic on invalid file.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DoTestsL()
    {
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0483 "));
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());

	const TPtrC inv_rsc_files[] =
		{
		  _L("TRsc_Inv1.rsc")			//Zero sized file
		, _L("TRsc_Inv2.rsc")
		, _L("TRsc_Inv3.rsc")
		, _L("TRsc_Inv4.rsc")
		, _L("TRsc_Inv5.rsc")
		, _L("TRsc_Inv6.rsc")
		, _L("TRsc_Inv7.rsc")
		, _L("TRsc_Inv8.rsc")
		, _L("TRsc_Inv9.rsc")
		, _L("TRscCalypso_Inv10.RSC")
		, _L("TRscCalypso_Inv11.RSC")
		, _L("TRscCalypso_Inv12.RSC")
		, _L("TRscComprU_Inv13.RSC")
		, _L("TRscComprU_Inv14.RSC")
		, _L("TRscComprU_Inv15.RSC")
		, _L("TRscCalypso_Inv16.RSC")
		, _L("TRscCalypso_Inv17.RSC")
		, _L("TRscNotExist.RSC")		//This file doesn't exist
		};

	TInt i;
	TInt array_size = TInt(sizeof(inv_rsc_files)/sizeof(inv_rsc_files[0]));

	//Z drive
	_LIT(KZDir, "z:\\system\\data\\");
	for(i=0;i<array_size;i++)
		{
		TBuf<64> path;
		path += KZDir;
		path += inv_rsc_files[i];
		TInt err = DoTestL(path);
		test(err != KErrNone);
		}

	//C drive
	_LIT(KCDir, "c:\\");
	for(i=0;i<array_size;i++)
		{
		TBuf<64> src_path;
		src_path += KZDir;
		src_path += inv_rsc_files[i];

		TBuf<64> dest_path;
		dest_path += KCDir;
		dest_path += inv_rsc_files[i];

		//Copy the file to C drive except the last which doesn't exist.
		//Zero sized file doesn't exist on the assabet/lubbock platforms.
		TInt err = KErrNone;
		if(i > 0 && i < (array_size - 1))
			{
			err = BaflUtils::CopyFile(TheFs, src_path, dest_path);
			test(err == KErrNone || err == KErrAlreadyExists);
			}

		err = DoTestL(dest_path);
		DeleteDataFile(dest_path);
		test(err != KErrNone);
		}

	//C drive - new rsc file format
	for(i=1;i<(array_size-1);i++)
		{
		TBuf<64> dest_path;
		TBuf<64> src_path;

		dest_path += KCDir;
		dest_path += _L("N_");
		dest_path += inv_rsc_files[i];

		src_path += KZDir;
		src_path += inv_rsc_files[i];

		CreateFileFromL(dest_path, src_path);

		TInt err = DoTestL(dest_path, KRscFileHeaderData().Length(), FileSizeL(src_path));
		DeleteDataFile(dest_path);
		test(err != KErrNone);
		}

	//Tests that Raise panics
	PanicTest();

	CleanupStack::PopAndDestroy(1, &TheFs);
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing RResourceFile & CResourceFile panics/leaves"));
    TRAPD(err, DoTestsL());
    test.Printf(_L("Error code is %d\n"), err);
    test(err == KErrNone);
    test.Next(_L("/n"));
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return 0;
    }
