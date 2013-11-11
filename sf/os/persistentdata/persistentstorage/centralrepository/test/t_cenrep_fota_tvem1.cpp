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
#include "testexecute/SWI/src/tui.h"
#include <e32std.h>
#include <swi/launcher.h>
#include <bautils.h>
#include "swi/sisregistrysession.h"
#include "swi/sisregistrypackage.h"
#include "swi/sisregistryentry.h"

#include "e32math.h"

RTest TheTest(_L("Central Repository Rom Flashing Tests"));

const TUid KUidRomRepository          = { 0x10033333 };
const TUid KUidRomRepositoryList[]          = { {0x10043333}, {0x10043334}, {0x10043335}};

_LIT(KCachedVersionDir,"C:\\private\\10202be9\\romversion\\");
_LIT(KSisFile,"z:\\private\\10202be9\\RF1.sis");
_LIT(KPersistsRepositoryCre,"C:\\private\\10202be9\\persists\\10033333.cre");
_LIT(KModifiedRepositoryCre,"z:\\private\\10202be9\\10033333_modified.cre");


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
///////////////////////////////////////////////////////////////////////////////////////
LOCAL_C TInt CheckIsFileExistsL(const TDesC& aFilename)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
    	TInt err = file.Open(fs, aFilename, EFileRead | EFileStreamText | EFileShareReadersOnly);
   	file.Close();
	CleanupStack::PopAndDestroy();//fs
	return err;	
	}	
/*
Description:
This performs the setup of the SWI install and modification of user settings.
Actions:
	modify and existing repository using SWI.
	Start the server.
	Modify the cached rom version file.
	Create a new setting.
	Delete two settings one from the ROM and one from the SWI.
*/ 
LOCAL_C void TestSWIAndFlash_SetupL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	// It isnt easy to overwrite the rom drive files
	// so instead updating the cached sw.txt file instead will achieve the same objective
	// of causing the merging of persists with rom.
	if(CheckIsFileExistsL(KCachedVersionDir))
		{
		fs.MkDir(KCachedVersionDir);
		}

	User::LeaveIfError(fm->Copy(KModifiedRepositoryCre,KPersistsRepositoryCre,CFileMan::EOverWrite));

	CleanupStack::PopAndDestroy(2);	//fs, fm
		
    // launch the installation
    TFileName SisFileName;
    SisFileName.Copy(KSisFile);
    
//********************************************************
//Requires stub file to be set up before it will work
//requires techview
//********************************************************
    	{
    	using namespace Swi;
    	TUI* Ui = new(ELeave) TUI();
    	CInstallPrefs* prefs = CInstallPrefs::NewLC();
    	TInt err = Launcher::Install(*Ui, SisFileName, *prefs);
    	TEST2(err, KErrNone);
    	delete Ui;
    	CleanupStack::PopAndDestroy();//prefs
    	}
    	
    User::After(KGeneralDelay);
    
    const TUint32 KNewSetting = 1000;
	const TUint32 KNewSetting2 = 1001;
	const TUint32 KDeleteSetting = 0x04010100;
	const TUint32 KModifiedSetting = 0x04010200;
    const TInt KIntValue = 1234;
	const TInt KInt1_UpdatedValue = 12345;
	const TInt KInt6_UpdatedValue = 10;
   
   	CRepository* rep = NULL;
   	User::LeaveIfNull(rep = CRepository::NewLC(KUidRomRepository));
 
    TInt settingValue = 0;
	TheTest.Next(_L("Get 'Added' value"));
	TInt r = rep->Get(KNewSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KIntValue);


	TheTest.Next(_L("'Deleted' value = 10"));
	settingValue = 0;
	r = rep->Get(KDeleteSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt6_UpdatedValue);

	TheTest.Next(_L("Get User added and 'Deleted' value - shouldnt be there"));
	settingValue = 0;
	r = rep->Get(KNewSetting2, settingValue);
	TEST2(r, KErrNotFound);
	
	TheTest.Next(_L("Get 'Modified' value"));
	settingValue = 0;
	r = rep->Get(KModifiedSetting, settingValue);
	TEST2(r, KErrNone);
	TEST(settingValue == KInt1_UpdatedValue);
 
 	CleanupStack::PopAndDestroy();//repository
 	
 	//Now have the application change some of the values in the persists   
    CRepository* repository = CRepository::NewLC(KUidRomRepository);
	TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);
	const TUint32 KNewSetting3 = 0x2300;
	const TUint32 KModifiedSetting2 = 0x04010400;
	const TUint32 KDeleteSetting1 = 0x02010300;
	
	const TInt KIntValue1 = 1111;
	err=repository->Create(KNewSetting3,KIntValue1);
 	TEST2(err,KErrNone);
 	
 	//ensure deleted value is actually there
 	settingValue = 0;
	r = rep->Get(KDeleteSetting1, settingValue);
	TEST2(r, KErrNone);

	err=repository->Delete(KDeleteSetting1);
 	TEST2(err,KErrNone);
 	err=repository->Set(KModifiedSetting2,KInt1_UpdatedValue);
 	TEST2(err,KErrNone);	
 	
 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);
    
	CleanupStack::PopAndDestroy();//repository	
	}

/*
Test setup for after a rom flash,where one of the updated repositories is corrupt that all the rest of the updated repositories are still merged.
Actions Modify repository values.
	Changed the cached rom version file. 
*/ 
LOCAL_C void ProcessCorruptedFlashSetupL()
	{
	const TUint32 KNewSetting = 1000;
	const TUint32 KNewSetting2 = 1001;
	const TUint32 KDeleteSetting = 0x04010100;
	const TUint32 KModifiedSetting = 0x04010200;
    	const TInt KIntValue = 1234;
	const TInt KInt1_UpdatedValue = 12345;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	TInt ii = 0;
	while(ii < 3)
	{
	CRepository* repository = CRepository::NewLC(KUidRomRepositoryList[ii]);
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
	ii++;
	}


	CleanupStack::PopAndDestroy(2);	// fs, fm
	}


LOCAL_C void FuncTestsL()
	{ 
	
    TheTest.Start(_L("Centralrepository -> User modifications and SWI Install modifcations."));	
	TestSWIAndFlash_SetupL();
	TheTest.Next(_L("CentralRepository -> Setup environment containing a corrupted repository."));
 	ProcessCorruptedFlashSetupL();	
	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-LEGACY-CENREP_FOTA_TVEM1-0001 CentralRepository ROM Flash Tests "));
	CleanupCDriveL(EFalse);

	FuncTestsL();
	
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
