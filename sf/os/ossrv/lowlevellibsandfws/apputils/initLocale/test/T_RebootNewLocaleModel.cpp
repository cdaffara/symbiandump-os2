// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains RTest based test which checks behaviour of InitialiseLocale.exe
// when persisted locale is corrrupt/damaged
// 
//

#include <bautils.h>
#include <f32file.h>
#include <e32cmn.h>
#include <e32test.h>
#include <e32std.h>
#include <e32const.h>
#include <babackup.h>
#include <hal.h>
#include <centralrepository.h>
#include "LocaleRepository.h"
#include "InitialiseLocale.h"
#include "T_CheckLocale.h"


// Name of the exe to be called explicitly
_LIT(KInitPersistLocaleExeName, "InitialiseLocale.exe");

LOCAL_D RTest test(_L("T_RebootLocaleNewLocaleModel"));


// Central Repository Data cage file-path
_LIT(KPersistLocale, "C:\\private\\10202be9\\persists\\1020E4D3.*");

RFs TheFs;

/*
This function prepares for next test i.e, tests InitialiseLocale behaves
when persisted file is damaged/corrupt.
*/
void SetUpTestCorruptLocale()
    {
    TExtendedLocale myExtendedLocale;
    myExtendedLocale.LoadSystemSettings() ;

    test.Printf(_L("\n Changing Locale to Chinese Locale(31) so that default values are evident"));

    // Change the locale to Default Locale, all settings from single Dll
    myExtendedLocale.LoadLocale(KCNLanguage(), KCNRegion(), KCNCollation());

    // Save the changes to system settings
    myExtendedLocale.SaveSystemSettings();

    // Wait so that it persists to repository
    User::After(1000000);

    User::LeaveIfError(TheFs.Connect());
    // Delete/damage the persisted file so that InitialiseLocale.exe
    // detects the damage and loads default locale
    BaflUtils::DeleteFile(TheFs, KPersistLocale);
    TheFs.Close();
    test.Printf(_L("\n Deleted persist file from C: \n"));
    }
/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0142
@SYMTestCaseDesc 	Check that InitialiseLocale can get the correct locale information after reboot in the new locale model.
@SYMTestPriority 	High
@SYMTestActions  	Ensure that InitialiseLocale can get the correct locale information after reboot in the new locale model.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1924
*/
LOCAL_D void TestRebootLocale()
	{
    test.Start(_L(" @SYMTestCaseID:TI18N-INITLOCALE-CT-4006 after rebooting check the locale content"));
    TestLocaleDLLName(test, KCNLanguage(), KCNRegion(), KCNCollation());
    TestLocaleContent(test, KCNLanguageID, KCNCurrencySymbol, KCNTCollationMethodID);
    test.End();
    
	}


/** Tests behavior of Initialisation mechanism after system reboots
This test requires that,
1. The T_LocaleFlexiNewLocaleModel is executed
2. Epoc be shut down
3. Epoc restarted (so that InitialisationLocale.exe is called)
4. Then this test be run
*/
void TestRebootLocaleBehavior()
	{
	RProcess process;
	// Kick-in InitialiseLocale.exe once again explicitly, This will be
	// done as part of system start in actual release code.
	TInt r = process.Create(KInitPersistLocaleExeName, KNullDesC);
	if(r == KErrNone)
		{
		process.Resume(); // Start the process going
		User::After(1000000);

		TestRebootLocale();

		User::After(1000000);
		process.Close();
		User::After(1000000);
		}
	}


TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	test(cleanup != NULL);

	__UHEAP_MARK;
	test.Start(_L("Test to test reboot system"));
	test.Title();

	TRAPD(err, ::TestRebootLocaleBehavior());

	test(err == KErrNone);
	
	TRAPD(err1, ::SetUpTestCorruptLocale());
	test(err1 == KErrNone);

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	delete cleanup;

	return(KErrNone);
	}
