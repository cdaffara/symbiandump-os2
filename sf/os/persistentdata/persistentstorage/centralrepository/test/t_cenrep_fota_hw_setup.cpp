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
#include "../cenrepsrv/srvparams.h"
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"
#include "../cenrepsrv/transstate.h"

RTest TheTest(_L("Central Repository Rom Flashing Tests"));

const TUid KUidRomRepositoryList[]          = { 0x10033335, 0x10033336 };
const TUid KUidRomRepository2            = { 0x10033339 };

_LIT(KSisRF1,"z:\\private\\10202be9\\RF1.sis");

_LIT(KSisRF3,"z:\\private\\10202be9\\RF3.sis");
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}

#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/*The setup part for a number of romflashing tests*/
/* It adds,modifies and deletes settings*/
// Covers tests  SYSLIB-CENTREP-CT-400[7|9]
LOCAL_C void Test_Setup1L(TInt aRepository)
	{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4007 "));
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4009 "));
	const TUint32 KNewSetting1 = 0x1000100;
	const TUint32 KModifiedSetting1 = 0x02010100;
	const TUint32 KModifiedSetting2 = 0x02010400;
	const TUint32 KDeleteSetting1 = 0x02010200;

	const TInt KIntValue = 1234;
	const TInt KInt_UpdatedValue1 = 100;

    CRepository* repository = CRepository::NewLC(KUidRomRepositoryList[aRepository]);
	TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

	//Add a setting
	err=repository->Create(KNewSetting1,KIntValue);
 	TEST2(err,KErrNone);


 	//Modify settings
 	err=repository->Set(KModifiedSetting1,KInt_UpdatedValue1);
 	TEST2(err,KErrNone);

  	err=repository->Set(KModifiedSetting2,KInt_UpdatedValue1);
 	TEST2(err,KErrNone);

 	//ensure deleted value is actually there
 	TInt settingValue = 0;
	TInt r = repository->Get(KDeleteSetting1, settingValue);
	TEST2(r, KErrNone);

	err=repository->Delete(KDeleteSetting1);
 	TEST2(err,KErrNone);


 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

	CleanupStack::PopAndDestroy();//repository
	}

/*This test sets up a repository performing SWI install and having user added,modified and deleted settings*/
// Covers tests  SYSLIB-CENTREP-CT-4008
LOCAL_C void Test_Setup2L()
	{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4008 "));
	const TUint32 KNewSetting1 = 0x1000100;
	const TUint32 KModifiedSetting1 = 0x02010100;
	const TUint32 KModifiedSetting2 = 0x02010400;
	const TUint32 KDeleteSetting1 = 0x02010200;

	const TInt KIntValue = 1234;
	const TInt KInt_UpdatedValue1 = 100;


 // launch the installation
    TFileName SisFileName1;
    SisFileName1.Copy(KSisRF3);

//********************************************************
//Requires stub file to be set up before it will work
//requires techview
//********************************************************
    	{
    	using namespace Swi;
    	TUI* Ui = new(ELeave) TUI();
    	CInstallPrefs* prefs = CInstallPrefs::NewLC();
    	TInt err = Launcher::Install(*Ui, SisFileName1, *prefs);
    	TEST2(err, KErrNone);
    	delete Ui;
    	CleanupStack::PopAndDestroy();//prefs
    	}


        CRepository* repository = CRepository::NewLC(KUidRomRepository2);
	TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

	//Add a setting
	err=repository->Create(KNewSetting1,KIntValue);
 	TEST2(err,KErrNone);


 	//Modify settings
 	err=repository->Set(KModifiedSetting1,KInt_UpdatedValue1);
 	TEST2(err,KErrNone);

  	err=repository->Set(KModifiedSetting2,KInt_UpdatedValue1);
 	TEST2(err,KErrNone);

 	//ensure deleted value is actually there
 	TInt settingValue = 0;
	TInt r = repository->Get(KDeleteSetting1, settingValue);
	TEST2(r, KErrNone);

	err=repository->Delete(KDeleteSetting1);
 	TEST2(err,KErrNone);


 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

	CleanupStack::PopAndDestroy();//repository
	}

/*This is a setup test for romflashing tests. It should only be run as part of t_cenrep_runtvflashtests.bat*/
// Covers tests  SYSLIB-CENTREP-CT-4010
LOCAL_C void Test_Setup3L()
	{

    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTREP-CT-4010 "));
	 // launch the installation
    	TFileName SisFileName;
    	SisFileName.Copy(KSisRF1);

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
	}

LOCAL_C void FuncTestsL()
	{

    TheTest.Start(_L("Central repository ->Test Setup"));
    Test_Setup1L(0);
	Test_Setup1L(1);//Uses a different repository.
	Test_Setup2L();
	Test_Setup3L();
	TheTest.End();

	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));
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
