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

// Name of the exe to be called explicitly
_LIT(KInitPersistLocaleExeName, "InitialiseLocale.exe");

// This value is from last attempted persist of system locale data
const TInt KDefaultCountryCode = 44;

LOCAL_D RTest test(_L("T_CorruptLocale"));


/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0142
@SYMTestCaseDesc 	Check that default Locale settings are loaded if persisted Locale is corrupt
@SYMTestPriority 	High
@SYMTestActions  	Ensure that InitialiseLocale detects damage in persisted locale
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1003
*/
LOCAL_D void TestCorruptLocale()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0142 \nTest for default behavior using corrupt locale "));

	TExtendedLocale myExtendedLocale;
	// Get current system settings
	myExtendedLocale.LoadSystemSettings();

	TLocale* myLocale = myExtendedLocale.GetLocale();

	// Test the default language
	test.Printf(_L("\nDefault Language is : %i "), User::Language());
	test(ELangEnglish == User::Language());

	// Test default country code
	test.Printf(_L("\nCountry code is : %i "), myLocale->CountryCode());
	test(KDefaultCountryCode == myLocale->CountryCode());

	// Test default date format
	test.Printf(_L("\nDate format is : %i "), myLocale->DateFormat());
	test(EDateEuropean == myLocale->DateFormat());
	User::After(9000000);
	}


/** Tests behavior of Initialisation mechanism on damaged/corrupt Locale
This test requires that,
1. The repository persistence file is already damaged/deleted
2. Epoc be shut down
3. Epoc restarted (so that InitialisationLocale.exe is called)
4. Then this test be run
*/
void TestCorruptLocaleBehavior()
	{
	RProcess process;
	// Kick-in InitialiseLocale.exe once again explicitly, This will be
	// done as part of system start in actual release code.
	TInt r = process.Create(KInitPersistLocaleExeName, KNullDesC);
	if(r == KErrNone)
		{
		process.Resume(); // Start the process going
		User::After(1000000);

		TestCorruptLocale();

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
	test.Title();

	TRAPD(err, ::TestCorruptLocaleBehavior());

	test(err == KErrNone);

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	delete cleanup;

	return(KErrNone);
	}
