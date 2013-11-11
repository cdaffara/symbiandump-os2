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
#include <swi/launcher.h>
#include <bautils.h>
#include "e32math.h"
#include "../cenrepsrv/srvparams.h"
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"
#include "../cenrepsrv/transstate.h"

RTest TheTest(_L("Central Repository Rom Flashing Tests"));

using namespace NCentralRepositoryConstants;


_LIT(KCRep1File,                                "c:\\private\\10202BE9\\persists\\10033335.cre");
_LIT(KCRep2File,                                "z:\\private\\10202BE9\\10033335.cre");
_LIT(KCRep3File,                                "z:\\private\\10202BE9\\10033335.txt");

const TUid KUidRomRepository          = { 0x10033333 };
const TUid KUidRomRepository1         = { 0x10033336 };
const TUid KUidRomRepository2         = { 0x10033339 };
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
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-CENTREP-CT-4007
@SYMTestCaseDesc		This test checks that if repository is removed from the ROM and has no SWI, it is removed from the C:\ .
@SYMTestPriority		High
@SYMTestActions			This test should only be run as part of t_cenrep_runtvflashtests.bat
@SYMTestExpectedResults         Repository in C:\ has been removed.
@SYMCR				CR1198
*/
LOCAL_C void Test_RepositoryRemovedL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

   	TEST2 (BaflUtils::FileExists (fs, KCRep1File), EFalse);
   	TEST2 (BaflUtils::FileExists (fs, KCRep2File), EFalse);
   	TEST2 (BaflUtils::FileExists (fs, KCRep3File), EFalse);

	CleanupStack::PopAndDestroy();//fs

	}

///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID			SYSLIB-CENTREP-CT-4008
@SYMTestCaseDesc		A repository is removed from the ROM and has SWI, it remains on the C:\ but all clean settings that do not exist in the SWI install file are removed
@SYMTestActions		  	This test should only be run as part of t_cenrep_runtvflashtests.bat
				Open Cenrep repository. Get Key values and compare them against expected values.
				unmodified values that were in the rom, but not in the SWI are removed.
@SYMTestExpectedResults 	Retrieved merged values equal expected values.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/
LOCAL_C void TestCenrepRomflashing001L()
	{

	const TUint32 KNewSetting1 = 0x1000100;
	const TUint32 KModifiedSetting1 = 0x02010100;
	const TUint32 KModifiedSetting2 = 0x02010400;
	const TUint32 KDeleteSetting1 = 0x02010200;
	const TUint32 KDeleteSetting2 = 0x03010100;
	const TInt KInt1_UpdatedValue = 100;
        const TInt KIntValue1 = 1234;

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRomRepository2);

	//Check Values have been merged correctly
	//Adds new setting in persists
	const TUint32 newSWIKey = 0x1000;
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	TInt r = repository->Get(newSWIKey, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue1);

	TheTest.Next(_L("Get 'added' Persisted value, keeps new value in persists"));
	settingValue = 0;
	r = repository->Get(KNewSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue1);

	//Modified values
	TheTest.Next(_L("Get 'Modified' ROM value, keeps value in persists"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);


	TheTest.Next(_L("Get 'Modified' Persisted value, keeps value in persists"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting2, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);

	//Deleted values
 	TheTest.Next(_L("Get 'Deleted' Persisted value - restored becos it is in SWI also"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == 20);

	//Deleted values
 	TheTest.Next(_L("Check that clean rom setting was removed that does not exist in SWI"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting2, settingValue);
	TEST2(r, KErrNotFound);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}

///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID			SYSLIB-CENTREP-CT-4009
@SYMTestCaseDesc		Test that the flash merging rules are implemented correctly
@SYMTestActions			This test should only be run as part of t_cenrep_runtvflashtests.bat.
				Open Cenrep repository. Get Key values and compare them against expected values.
@SYMTestExpectedResults 	Retrieved merged values equal expected values.
				User added setting 0x1000100 exists,
				User modified setting 0x2010100 has value 100
				Rom deleted setting 0x2010200 is reinstated.
				New rom setting 0x5010100 exists.
				Rom modified setting 0x2010200 has value 101.
				Rom removed setting 0x2010300 is removed.
				Rom removed setting 0x2010400 is still exists.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/
LOCAL_C void TestCenrepRomflashing002L()
	{

	const TUint32 KNewSetting1 = 0x1000100;
	const TUint32 KRomNewSetting1 = 0x5010100;
	const TUint32 KModifiedSetting1 = 0x02010100;
	const TUint32 KUserDeleteSetting1 = 0x02010200;
	const TUint32 KRomDeleteSetting1 = 0x02010300;
	const TUint32 KRomDeleteSetting2 = 0x02010400;


	const TInt KInt1_UpdatedValue = 100;
    	const TInt KIntValue1 = 1234;
    	const TInt KIntValue2 = 10;

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRomRepository1);

	//Check Values have been merged correctly
	//User added value remains
	TheTest.Next(_L("Get 'User added' Persisted value, keeps new value in persists"));
	TInt settingValue = 0;
	TInt r = repository->Get(KNewSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue1);

	TheTest.Next(_L("Get 'ROM added' now exists in persists"));
	settingValue = 0;
	r = repository->Get(KRomNewSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue2);

	//User Modified values
	TheTest.Next(_L("Get 'Modified' ROM value, keeps value in persists"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);

	//Deleted values
 	TheTest.Next(_L("Get 'Deleted' Persisted value - restored becos it is in ROM"));
	settingValue = 0;
	r = repository->Get(KUserDeleteSetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == 101);

	//Deleted values
 	TheTest.Next(_L("Check that clean rom setting was removed that does not exist in ROM"));
	settingValue = 0;
	r = repository->Get(KRomDeleteSetting1, settingValue);
	TEST2(r, KErrNotFound);

 	TheTest.Next(_L("Check that rom setting altered by user is not deleted"));
	settingValue = 0;
	r = repository->Get(KRomDeleteSetting2, settingValue);
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}

///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID			SYSLIB-CENTREP-CT-4010
@SYMTestCaseDesc		Test that setting that is altered by SWI value is not overwritten by a rom flash.
@SYMTestActions			This test should only be run as part of t_cenrep_runtvflashtests.bat
				1.Create a standard rom respository using 10033333.txt.
				2.Run t_cenrep_romflashing_007.exe to alter a repository using SWI install. Settings 0x4010200 to value 12345.
				3.Create a rom using an altered repository that contains a new value for setting 0x4010100 value 10
				4.Run t_cenrep_romflashing_008. This test checks that setting 0x4010100 retains value 12345.
@SYMTestExpectedResults 	Retrieved merged values equal expected values.
				0x4010100 should have the SWI value of 100 not the rom value of 10
@SYMTestPriority    Medium
@SYMCR				CR1198
*/
LOCAL_C void TestCenrepRomflashing003L()
	{
	TheTest.Next(_L("Manually start central respository"));

	const TUint32 KSWISetting1 = 0x4010200;
	const TInt KInt1_Value = 12345;


	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRomRepository);

	//Check Values have been merged correctly, SWI should dominate over new ROM values
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'setting 0x4010200 ' value"));
	TInt r = repository->Get(KSWISetting1, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_Value);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void FuncTestsL()
	{

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4007 Cenrep: SYSLIB-CENTREP-CT-4007"));
	Test_RepositoryRemovedL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4008 Cenrep: SYSLIB-CENTREP-CT-4008"));
	TestCenrepRomflashing001L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4009 Cenrep: SYSLIB-CENTREP-CT-4009"));
	TestCenrepRomflashing002L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4010 Cenrep: SYSLIB-CENTREP-CT-4010"));
	TestCenrepRomflashing003L();
	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));

	FuncTestsL();

	CleanupCDriveL();

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
