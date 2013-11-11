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
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4020
@SYMTestCaseDesc		This is the second part of SWI install and rom flash testing. It checks that after
				a rom flash when the sw.txt file is missing no merging takes place
@SYMTestPriority		High
@SYMTestActions			Open Cenrep repository. Get Key values and compare them against expected values.
@SYMTestExpectedResults 	Retrieved values equal expected values.
				User added or modified values keep user values.
				Entries that have not been modified by the user but have been modified by the SWI and rom keep the SWI values.
				New entries in the ROM do not exist in the persists repository.

@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4021
@SYMTestCaseDesc		This is the second part of SWI install and rom flash testing. It checks that after
				a rom flash when the resource\versions directory is missing no merging takes place
@SYMTestPriority		High
@SYMTestActions			Open Cenrep repository. Get Key values and compare them against expected values.
@SYMTestExpectedResults 	Retrieved values equal expected values.
				User added or modified values keep user values.
				Entries that have not been modified by the user but have been modified by the SWI and rom keep the SWI values.
				New entries in the ROM do not exist in the persists repository.

@SYMCR				CR1198
*/	
LOCAL_C void SimulateReflashOnEmulatorPart2aL()
	{
	
	_LIT(KROMDIR, "Z:\\resource\\versions");
	_LIT(KROMVersionFile, "Z:\\resource\\versions\\sw.txt");

	RFs fs;
	fs.Connect();

	TEntry entry;
	if((fs.Entry(KROMDIR,entry)) == KErrNotFound)
	   {
	   TheTest.Next(_L("SYSLIB-CENTRALREPOSITORY-CT-4021"));
	   }
	else if((fs.Entry(KROMVersionFile,entry)) == KErrNotFound)
	   {
	   TheTest.Next(_L("SYSLIB-CENTRALREPOSITORY-CT-4020"));
	   }
	fs.Close();		

	const TUint32 KNewSetting = 0x2300;
	const TUint32 KNewSetting1 = 0x02010000;
	const TUint32 KModifiedSetting = 0x04010200;
	const TUint32 KModifiedSetting2 = 0x04010400;
	const TUint32 KDeleteSetting1 = 0x02010300;
	const TInt KInt1_UpdatedValue = 12345;
    	const TInt KIntValue1 = 1111;

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
	TEST2(r, KErrNotFound);

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
	TEST2(r, KErrNotFound);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void FuncTestsL()
	{ 
	
    TheTest.Start(_L("Centralrepository -> SimulateReflashOnEmulatorPart2aL Rom Flash"));	
	
	SimulateReflashOnEmulatorPart2aL();
	
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
