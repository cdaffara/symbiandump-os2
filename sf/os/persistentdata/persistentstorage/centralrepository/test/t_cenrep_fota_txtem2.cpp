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

#include "e32math.h"


RTest TheTest(_L("Central Repository Rom Flashing Tests"));

const TUid KUidRomRepository[]          = { {0x10033333} };

_LIT(KCachedVersionFile,"C:\\private\\10202be9\\romversion\\romversion_info.txt");
_LIT(KSwVersion,"z:\\resource\\versions\\sw.txt");
_LIT(KModifiedSwVersion, "z:\\private\\10202be9\\sw_modified1.txt");
_LIT(KCentralRepositoryServerName, "Centralrepositorysrv");


const TUint32 KNewSetting = 1000;
const TUint32 KNewSetting2 = 1001;
const TInt KIntValue = 1234;
const TUint32 KDeleteSetting = 0x04010100;
const TUint32 KModifiedSetting = 0x04010200;
const TInt KInt1_UpdatedValue = 12345;

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
LOCAL_C TInt CheckIsFileExistsL(const TDesC& aFilename)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TEntry entry;
    	TInt err = fs.Entry(aFilename, entry);
	fs.Close();
	return err;
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4002
@SYMTestCaseDesc		This test checks if the rom version file is present then a cached versions
						is created on startup, if one does not already exist.
@SYMTestPriority		High
@SYMTestActions			Remove the cached version.Start the server up. Check that a cached version is created.
@SYMTestExpectedResults Cached rom version file exists after server startup.
@SYMCR					CR1198
*/
LOCAL_C void Test_Caching_Swtxt_FileL()
	{
	TInt Err = CheckIsFileExistsL(KSwVersion);//rom version file should exist
	TEST2(Err,KErrNone);

	// Kill server so that next test will reconnect and recreate resources
	TInt r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	// Remove cachedversion file if it exists
	fm->Delete(KCachedVersionFile);

	// Open repository to ensure server is running
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidRomRepository[0]));
	// Close repository
	CleanupStack::PopAndDestroy(rep);

	Err = CheckIsFileExistsL(KCachedVersionFile);//cached file should exist
	TEST2(Err,KErrNone);

	CleanupStack::PopAndDestroy(2);//fm,fs

	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4004
@SYMTestCaseDesc		Test that after a simulated rom flash, rom and user modified values are merged correctly.
@SYMTestPriority		High
@SYMTestActions			Modify repository values.
						Changed the cached rom version file.
						Restart the server.
						Check the merged values against expected values.
@SYMTestExpectedResults User added values remain.
						User modified values remain.
						User rom entries deleted values are reinstated.
						User entries deleted remain deleted.
@SYMCR					CR1198
*/
LOCAL_C void SimulateFlashL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	CRepository* repository = CRepository::NewLC(KUidRomRepository[0]);
	TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

	const TUint32 KNewSetting3 = 1002;
	const TUint32 KNewSetting4 = 1003;
	const TUint32 KDeleteSetting2 = 0x03010300;
	err=repository->Create(KNewSetting3,KIntValue);
 	TEST2(err,KErrNone);
	err=repository->Create(KNewSetting4,KIntValue);
 	TEST2(err,KErrNone);
	err=repository->Delete(KNewSetting4);
 	TEST2(err,KErrNone);
 	err=repository->Set(KModifiedSetting,KInt1_UpdatedValue);
 	TEST2(err,KErrNone);
	err=repository->Delete(KDeleteSetting2);
 	TEST2(err,KErrNone);

 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

 	CleanupStack::PopAndDestroy();//repository

 	// Wait for cache flushing
	User::After(KGeneralDelay);
	// Kill server so that next test will reconnect and recreate resources
	TInt r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	// It isnt easy to overwrite the rom drive files
	// so instead updating the cached sw.txt file instead will achieve the same objective
	// of causing the merging of persists with rom.

	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));

	CleanupStack::PopAndDestroy(2);	// fs, fm

	TheTest.Next(_L("Manually start central respository"));
	repository = CRepository::NewLC(KUidRomRepository[0]);

	//Check Values have been merged correctly
	//User Deleted keys that were in the ROM are recovered
	//User deleted keys that were never in the ROM and have been deleted remain deleted.
	//User Modified keys stay modified.
	//User Added keys stay added.
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	r = repository->Get(KNewSetting3, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue);


	TheTest.Next(_L("Get Rom 'Deleted' value - will have been readded"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting2, settingValue);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Get User added and 'Deleted' value - shouldnt be there"));
	settingValue = 0;
	r = repository->Get(KNewSetting4, settingValue);
	TEST2(r, KErrNotFound);

	TheTest.Next(_L("Get 'Modified' value"));
	settingValue = 0;
	r = repository->Get(KModifiedSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4003
@SYMTestCaseDesc		This test ensures that values remain unmodified if no rom flash
						has taken place since server startup.
@SYMTestPriority		High
@SYMTestActions			Start server.
						Retrieve and compare settings values against expected values.
@SYMTestExpectedResults repository entries equal expected values.
@SYMCR					CR1198
*/
LOCAL_C void TestNoROMFlash_BehaviourL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CRepository* repository = CRepository::NewLC(KUidRomRepository[0]);
	TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

	err=repository->Create(KNewSetting,KIntValue);
 	TEST2(err,KErrNone);
	err=repository->Create(KNewSetting2,KIntValue);
 	TEST2(err,KErrNone);
	err=repository->Delete(KNewSetting2);
 	TEST2(err,KErrNone);
 	err=repository->Set(KModifiedSetting,KInt1_UpdatedValue);
 	TEST2(err,KErrNone);
	err=repository->Delete(KDeleteSetting);
 	TEST2(err,KErrNone);

 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

 	CleanupStack::PopAndDestroy();//repository

 	// Wait for cache flushing
	User::After(KGeneralDelay);
	// Kill server so that next test will reconnect and recreate resources
	TInt r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	CleanupStack::PopAndDestroy();	// fs

	TheTest.Next(_L("Manually start central respository"));
	repository = CRepository::NewLC(KUidRomRepository[0]);

	//Check Values have been merged correctly
	//User Deleted keys that were in the ROM are recovered
	//User deleted keys that were never in the ROM and have been deleted remain deleted.
	//User Modified keys stay modified.
	//User Added keys stay added.
	TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	r = repository->Get(KNewSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue);


	TheTest.Next(_L("Get Rom 'Deleted' value - will still be gone"));
	settingValue = 0;
	r = repository->Get(KDeleteSetting, settingValue);
	TEST2(r, KErrNotFound);

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
	}



LOCAL_C void FuncTestsL()
	{

    TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4002 Central repository ->Test sw.txt file is cached on startup. "));
	Test_Caching_Swtxt_FileL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4003 Centralrepository -> No ROM version change from cached version "));
	TestNoROMFlash_BehaviourL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4004 Centralrepository -> Simulate a ROM reflash on emulator. "));
	SimulateFlashL();

	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));
	CleanupCDriveL();

	FuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
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
