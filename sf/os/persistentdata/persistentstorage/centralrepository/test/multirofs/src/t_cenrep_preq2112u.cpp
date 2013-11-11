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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include "obsrvr_noc.h"
#include "panic.h"
#include "srvres.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Defect Tests"));


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions
LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

struct TExitDetails
	{
	TExitCategoryName iCategory;
	TExitType iExitType;
	TInt iReason;
	};

typedef void (*TestFunction)();

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


LOCAL_C void PREQ2112L(TUid aUid)
	{
	__UHEAP_MARK;

	CRepository* multi=CRepository::NewLC(aUid);

	//Test retrieving the merged contents

	RArray<TUint32> foundSettings;
	CleanupClosePushL(foundSettings);

	//Check number of settings first(see spreadsheet)
	multi->FindL(0xFFFFFFFF,0,foundSettings);
	TEST(foundSettings.Count()==10);

	//now check value of individual settings value first
	TInt intVal;
	TReal realVal;
	TBuf8<255> string8;
	User::LeaveIfError(multi->Get(1,intVal));
	TEST2(intVal,101);
	User::LeaveIfError(multi->Get(2,string8));
	TEST(string8.Compare(_L8("abc"))==0);
	User::LeaveIfError(multi->Get(3,intVal));
	TEST(intVal==100);
	User::LeaveIfError(multi->Get(4,intVal));
	TEST(intVal==100);
	User::LeaveIfError(multi->Get(5,intVal));
	TEST(intVal==101);
	User::LeaveIfError(multi->Get(6,realVal));
	TEST(realVal==102.102);
	User::LeaveIfError(multi->Get(7,intVal));
	TEST(intVal==100);
	User::LeaveIfError(multi->Get(9,intVal));
	TEST(intVal==900);
	//FOR id=10 if overriding is scucessufl we will fail to read
	//this as it require sid_wr=200
	TInt err=multi->Get(10,intVal);
	TEST(err==KErrPermissionDenied);
	User::LeaveIfError(multi->Get(300,realVal));
	TEST(realVal==1.23);

	//now check the meta
	TUint32 meta;
	User::LeaveIfError(multi->GetMeta(1,meta));
	TEST(meta==0x10);
	User::LeaveIfError(multi->GetMeta(2,meta));
	TEST(meta==0x10);
	User::LeaveIfError(multi->GetMeta(3,meta));
	TEST(meta==0xFF);
	User::LeaveIfError(multi->GetMeta(4,meta));
	TEST(meta==0x10);
	User::LeaveIfError(multi->GetMeta(5,meta));
	TEST(meta==99);
	User::LeaveIfError(multi->GetMeta(6,meta));
	TEST(meta==0x10);
	User::LeaveIfError(multi->GetMeta(7,meta));
	TEST(meta==0x10);
	User::LeaveIfError(multi->GetMeta(9,meta));
	TEST(meta==0x10);
	err=multi->GetMeta(10,meta);
	TEST(err==KErrPermissionDenied);
	User::LeaveIfError(multi->GetMeta(300,meta));
	TEST(meta==999);

	//now check the platsec
	err=multi->Set(7,7);
	TEST(err==KErrPermissionDenied);


	//lets make some modification to test for ResetAll and Reset key
	//modification to existing key
	err=multi->Set(5,123);
	TEST(err==KErrNone);
	//creation of a new key
	err=multi->Set(150,150);
	TEST(err==KErrNone);

	//validating what we set just now
	err=multi->Get(5,intVal);
	TEST(err==KErrNone);
	TEST(intVal==123);
	err=multi->Get(150,intVal);
	TEST(err==KErrNone);
	TEST(intVal==150);

	//SINGLE RESET
	err=multi->Reset(5);
	TEST(err==KErrNone);

	//check value following reset
	err=multi->Get(5,intVal);
	TEST(err==KErrNone);
	TEST(intVal==101);
	err=multi->Get(150,intVal);
	TEST(err==KErrNone);
	TEST(intVal==150);

	//modification again before calling ResetAll
	err=multi->Set(5,132);
	TEST(err==KErrNone);
	err=multi->Get(5,intVal);
	TEST(err==KErrNone);
	TEST(intVal==132);

	//RESET ALL
	err=multi->Reset();
	TEST(err==KErrNone);
	//checking value following reset
	err=multi->Get(5,intVal);
	TEST(err==KErrNone);
	TEST(intVal==101);	//merged ROM value
	err=multi->Get(150,intVal);
	TEST(err==KErrNotFound);

	CleanupStack::PopAndDestroy(2);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4078
@SYMTestCaseDesc Testing single Reset and ResetAll in a multi rofs environment
@SYMTestPriority High
@SYMTestActions  This test the centrep API that will result in reopening of the ROM files in this test case, the
				 main use case is single Reset and ResetAll.This test is repeated for the following ROFS keyspace
                 combination (ROFS1/ROFS2) such as TXT/TXT , CRE/TXT or TXT/CRE and CRE/CRE.
@SYMTestExpectedResults The merged values of the settings should match the expected values.
@SYMREQ REQ11282
*/
LOCAL_C void FuncTestsL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4078 Testing single Reset and ResetAll in a multi rofs environment "));
	__UHEAP_MARK;

	//txt version both core and rofs
	TUid test_uid={0xFFFF0000};
	PREQ2112L(test_uid);

	//cre version both core and rofs(equiv to txt above)

	test_uid.iUid=0xEEEE0000;
	PREQ2112L(test_uid);

	//txt/cre mix core=txt rofs=cre
	test_uid.iUid=0xDDDD0000;
	PREQ2112L(test_uid);

	//txt/cre mix core=cre rofs=txt
	test_uid.iUid=0xCCCC0000;
	PREQ2112L(test_uid);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4052
@SYMTestCaseDesc Server side resolving/processing of multi rofs entry list
@SYMTestPriority High
@SYMTestActions  Call to the static function in TServerResources to generate the cache list and verify each entry
                 in the cache list is correct.The environment for this testing should contain at least one multi
                 rofs keyspace of the following combination (ROFS1/ROFS2) such as TXT/TXT , CRE/TXT or TXT/CRE and CRE/CRE.
@SYMTestExpectedResults The correct cache list should be returned with respect to the order and content.
@SYMREQ REQ11282
*/
LOCAL_C void DoOOML(const TDesC& aTestDesc, TBool aOOMMode)
	{
	TheTest.Next (_L (" @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4052 CENREPSRV server side multirofs list resolving "));

	__UHEAP_MARK;

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	TheTest.Next(aTestDesc);

	//Initializing the server resources
	TServerResources::InitialiseL ();

	//now lets open up some files
	CDir* fileList;
	TFileName searchFileFormat;
	searchFileFormat.Append(_L("z:\\private\\10202be9\\????0000.???[*-00]"));
	User::LeaveIfError(TServerResources::iFs.GetDir(searchFileFormat,KEntryAttNormal,ESortByName,fileList));

	TInt err;
	TInt tryCount = 0;
	do
		{
		for (TInt i=0;i<TServerResources::iObserver->GetMultiRofsList().Count();i++)
			{
			TServerResources::iObserver->GetMultiRofsList()[i].iMountFlagList.Close();
			}
		TServerResources::iObserver->GetMultiRofsList().Reset();
		TEST(TServerResources::iObserver->GetMultiRofsList().Count()==0);

		__UHEAP_MARK;

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, TServerResources::iObserver->ProcessMultiRofsListL(*fileList));
		if (err!=KErrNoMemory)
			TEST(err==KErrNone);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		TEST2(startProcessHandleCount, endProcessHandleCount);
		TEST2(startThreadHandleCount, endThreadHandleCount);

		//validate the result if ok
		if (err==KErrNone)
			{
			//now check the processedList
			RArray<TMultiRofsList> list=TServerResources::iObserver->GetMultiRofsList();
			TEST(list.Count()==6);
			TEST(list[0].iRepUid.iUid==0xaaaa0000);
			TEST(list[0].iMountFlagList.Count()==1);
			TEST(list[0].iMountFlagList[0].iFlag==0x80000001);
			TEST(list[1].iRepUid.iUid==0xbbbb0000);
			TEST(list[1].iMountFlagList.Count()==1);
			TEST(list[1].iMountFlagList[0].iFlag==0x80000001);
			TEST(list[2].iRepUid.iUid==0xcccc0000);
			TEST(list[2].iMountFlagList.Count()==1);
			TEST(list[2].iMountFlagList[0].iFlag==0x80000001);
			TEST(list[3].iRepUid.iUid==0xdddd0000);
			TEST(list[3].iMountFlagList.Count()==1);
			TEST(list[3].iMountFlagList[0].iFlag==0x00000001);
			TEST(list[4].iRepUid.iUid==0xeeee0000);
			TEST(list[4].iMountFlagList.Count()==1);
			TEST(list[4].iMountFlagList[0].iFlag==0x00000001);
			TEST(list[5].iRepUid.iUid==0xffff0000);
			TEST(list[5].iMountFlagList.Count()==1);
			TEST(list[5].iMountFlagList[0].iFlag==0x80000001);
			}

		for (TInt i=0;i<TServerResources::iObserver->GetMultiRofsList().Count();i++)
			{
			TServerResources::iObserver->GetMultiRofsList()[i].iMountFlagList.Close();
			}
		TServerResources::iObserver->GetMultiRofsList().Reset();

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	TEST(err==KErrNone);

 	if (aOOMMode)
		TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
 	//deleting the file list
 	delete fileList;
 	TServerResources::Close();
 	CleanupStack::PopAndDestroy();

 	__UHEAP_MARKEND;
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("Defect tests"));
	CleanupCDriveL();

	FuncTestsL();

	//Testing the processing/resolving of multiple repository list
	DoOOML(_L("Basic Unit Test"),EFalse);
	DoOOML(_L("OOM Unit Test"),ETrue);

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;

	return 0;
	}
