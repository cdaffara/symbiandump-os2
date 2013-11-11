// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h>
#include <bautils.h>

RTest TheTest(_L("Central Repository Rom Flashing Tests"));

const TUid KUidRomRepository          = { 0x10033333 };
const TUid KUidRomRepositoryList[]          = { {0x10043333}, {0x10043334}, {0x10043335}};

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

///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4006
@SYMTestCaseDesc		This is the second part of SWI install and rom flash testing. It checks that after
				a rom flash that the SWI,persists and rom settings are merged correctly.
@SYMTestPriority		High
@SYMTestActions			Open Cenrep repository. Get Key values and compare them against expected values.
@SYMTestExpectedResults 	Retrieved merged values equal expected values.
				User added or modified values keep user values.
				Entries that have not been modified by the user but have been modified by the SWI and rom keep the SWI values.
				New entries in the ROM now exist in the persists repository.

@SYMCR				CR1198
*/	
LOCAL_C void SimulateSWIandReflashOnEmulatorPart2L()
	{
	TheTest.Next(_L("Manually start central respository"));

	const TUint32 KNewSetting = 0x2300;
	const TUint32 KNewSetting1 = 0x02010000;
	const TUint32 KModifiedSetting = 0x04010200;
	const TUint32 KModifiedSetting2 = 0x04010400;
	const TUint32 KDeleteSetting1 = 0x02010300;
	const TInt KInt1_UpdatedValue = 12345;
    const TInt KIntValue1 = 1111;
    const TInt KIntValue2 = 1112;

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRomRepository);

	//Check Values have been merged correctly
	//Adds new setting in persists
	const TUint32 newSWIKey = 1000;
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	TInt r = repository->Get(newSWIKey, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == 1234);

	TheTest.Next(_L("Get 'added' Persisted value, keeps new value in persists"));
	settingValue = 0;
	r = repository->Get(KNewSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue1);

	TheTest.Next(_L("Get 'added' rom setting value, new setting exists in persists"));
	settingValue = 0;
	r = repository->Get(KNewSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue2);

	//Modified values
	TheTest.Next(_L("Get 'Modified' ROM value, keeps value in persists"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);


	TheTest.Next(_L("Get 'Modified' Persisted value, keeps value in persists"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting2, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);
	
	//Deleted values
 	TheTest.Next(_L("Get 'Deleted' Persisted value, deleted setting gets reinstated"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting1, settingValue);
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4023
@SYMTestCaseDesc		Test values for after a rom flash,where one of the updated repositories is corrupt that all the rest of the updated repositories are still merged.
@SYMTestPriority		High
@SYMTestActions			Modify repository values.
				Changed the cached rom version file. 
				Restart the server.
				Check the merged values against expected values.
@SYMTestExpectedResults 	User added values remain.
				User modified values remain.
				User rom entries deleted values are reinstated.
				User entries deleted remain deleted.
@SYMCR				CR1198
*/ 
LOCAL_C void ProcessCorruptedFlashL()
	{
	TheTest.Next(_L("Process update rom files,skipping corrupt ones"));

	const TUint32 KNewSetting = 1000;
	const TUint32 KRomNewSetting1 = 0x02010000;
	const TUint32 KNewSetting2 = 1001;
	const TUint32 KDeleteSetting = 0x04010100;
	const TUint32 KModifiedSetting = 0x04010200;
    	const TInt KIntValue = 1234;
    	const TInt KIntValue2 = 1112;
	const TInt KInt1_UpdatedValue = 12345;
	TInt ii = 0;
	while(ii < 3)
	{
	CRepository* repository = CRepository::NewLC(KUidRomRepositoryList[ii]);

	//Check Values have been merged correctly
	//User Deleted keys that were in the ROM are recovered, except for corrupted repository
	//User deleted keys that were never in the ROM and have been deleted remain deleted.
	//User Modified keys stay modified.
	//User Added keys stay added.
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	TInt r = repository->Get(KNewSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue);
	
	//Get Rom added value
	settingValue = 0;
	TheTest.Next(_L("Get 'Rom Added' value"));
	r = repository->Get(KRomNewSetting1, settingValue);
	if(ii == 1)//Corrupted rom repository will mean it wasnt added
	{
	TEST2(r, KErrNotFound);
	}
	else
	{
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue2);
	}

	TheTest.Next(_L("Get Rom 'Deleted' value - will have been readded"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting, settingValue);
	if(ii == 1)//Corrupted rom repository will mean it wasnt readded
	{
	TEST2(r, KErrNotFound);
	}
	else
	{
	TEST2(r, KErrNone);
	}

	TheTest.Next(_L("Get User added and 'Deleted' value - shouldnt be there"));
	settingValue = 0;
	r = repository->Get(KNewSetting2, settingValue);
	TEST2(r, KErrNotFound);
	
	TheTest.Next(_L("Get 'Modified' value"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	ii++;
	}

	}

LOCAL_C void FuncTestsL()
	{ 
	
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4006 Centralrepository -> Rom Flash "));	
	SimulateSWIandReflashOnEmulatorPart2L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4023 Corrupted repository test "));	
	ProcessCorruptedFlashL();
	
	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));

	FuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL(ETrue);
	
	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
