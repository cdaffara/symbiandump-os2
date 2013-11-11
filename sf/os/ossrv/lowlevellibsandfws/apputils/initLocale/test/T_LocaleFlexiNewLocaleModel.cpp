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
// This file contains RTest based tests which check flexibility of Locale.
// That are, 1. Selectable from Multiple traditional locale 2. Loaded all from a single traditional locale.
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


_LIT(KInitPersistLocaleExeName, "InitialiseLocale.exe");

LOCAL_D RTest test(_L("T_LocaleFlexiNewLocaleModel"));


/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0140
@SYMTestCaseDesc 	Check that locale settings can be selected from several traditional locale
@SYMTestPriority 	High
@SYMTestActions  	Ensure that locales are selected, system set and persisted
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1001
*/
LOCAL_D	void ChangeLocaleNewCombination()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0140 \nTest to demonstrate Locale settings by multiple traditional locale "));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings();

    TBuf<KMaxFullName> languageDllName(KUSLanguage);
    TBuf<KMaxFullName> regionDllName(KCNRegion);
    TBuf<KMaxFullName> collationDllName(KGECollation);

    test( KErrNone == myExtendedLocale.LoadLocaleAspect(languageDllName));
    test( KErrNone == myExtendedLocale.LoadLocaleAspect(regionDllName));
    test( KErrNone ==myExtendedLocale.LoadLocaleAspect(collationDllName));
	
    test( KErrNone ==myExtendedLocale.SaveSystemSettings());

	// Wait
	User::After(10000000);

	TUid LocaleRepositoryUid ;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid ;

	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid);

    // To get names of DLLs to be loaded
    TBuf<KMaxFullName> aDllName ;

    // Test for the language settings
    TInt err = repository->Get(KLocaleLanguageDll, aDllName);
    if (err != KErrNone)
        {
        // Failed to read DLL name, bail out!
        CleanupStack::PopAndDestroy(repository) ;
        return ;
        }
    
    test.Printf(_L("\nThe lagnuage DLL names in CRepository is: %S"), &aDllName);
    test( KErrNotFound != aDllName.Find(languageDllName));

    err = repository->Get(KLocaleCollationDll, aDllName);
    if (err != KErrNone)
        {
        // Failed to read DLL name, bail out!
        CleanupStack::PopAndDestroy(repository) ;
        return ;
        }
    
    test.Printf(_L("\nThe collation DLL names in CRepository is: %S"), &aDllName);
    test( KErrNotFound != aDllName.Find(collationDllName));

    
   err = repository->Get(KLocaleLocaleDll, aDllName);
    if (err != KErrNone)
        {
        // Failed to read DLL name, bail out!
        CleanupStack::PopAndDestroy(repository) ;
        return ;
        }
    
    test.Printf(_L("\nThe Region DLL names in CRepository is: %S"), &aDllName);
    test( KErrNotFound != aDllName.Find(regionDllName));
    
	// Free the central repository object
	CleanupStack::PopAndDestroy (repository);
	test.End() ;
	
    //becasue we check whether locale content can be persisted is the system in this code block , we mark this test case here.
    test.Start(_L(" @SYMTestCaseID:TI18N-INITLOCALE-CT-4005 persist system locale content"));
    test.End();
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0141
@SYMTestCaseDesc 	Check that locale settings can be selected from single traditional locale. Also prepare the data for T_RebootNewLocaleModel
@SYMTestPriority 	High
@SYMTestActions  	Ensure that all settings are loaded from a DLL, system set and persisted
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1001
*/
LOCAL_D void ChangeLocaleExistedCombination()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0141 Test to demonstrate change of Locale from one traditional locale "));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;

	TBuf<KMaxFullName> languageDllName(KCNLanguage);
	TBuf<KMaxFullName> regionDllName(KCNRegion);
	TBuf<KMaxFullName> collationDllName(KCNCollation);
	
	test.Printf(_L("Changing all 3 Locale aspects to Chinese"));

	// Change the locale to Chinese Locale, all settings from single Dll
	myExtendedLocale.LoadLocale(languageDllName, regionDllName, collationDllName);

	// Save the changes to system settings
	myExtendedLocale.SaveSystemSettings();

	// Wait
	User::After(10000000);

	TUid LocaleRepositoryUid ;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid;
	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid) ;

	// To get names of DLLs to be loaded
	TBuf<KMaxFullName> aDllName ;

	// Test for the language settings
	TInt err = repository->Get(KLocaleLanguageDll, aDllName);
	if (err != KErrNone)
		{
		// Failed to read DLL name, bail out!
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}
	
	test( KErrNotFound != aDllName.Find(languageDllName));

    err = repository->Get(KLocaleCollationDll, aDllName);
    if (err != KErrNone)
        {
        // Failed to read DLL name, bail out!
        CleanupStack::PopAndDestroy(repository) ;
        return ;
        }
    
    test( KErrNotFound != aDllName.Find(collationDllName));

    
    err = repository->Get(KLocaleLocaleDll, aDllName);
    if (err != KErrNone)
        {
        // Failed to read DLL name, bail out!
        CleanupStack::PopAndDestroy(repository) ;
        return ;
        }
    
    test( KErrNotFound != aDllName.Find(regionDllName));
    
    
    
	CleanupStack::PopAndDestroy (repository);
	test.End();
	}
//prepare for the reboot test
void Prepare4forRebootTest()
    {
    TExtendedLocale myExtendedLocale;
    myExtendedLocale.LoadSystemSettings();
    test( KErrNone == myExtendedLocale.LoadLocale(KCNLanguage(), KCNRegion(), KCNCollation() ));
    test( KErrNone == myExtendedLocale.SaveSystemSettings());
    User::After(10000000);
    }

// Call the tests that test flexibility of Locale
void TestLocaleFlexibility()
	{
	
    RProcess process;

    // Kick-in InitialiseLocale.exe, This will be done as part of system start in actual
    // release code!
    TInt r = process.Create(KInitPersistLocaleExeName, KNullDesC);
    if(r == KErrNone)
        {
        process.Resume(); // Start the process going
        User::After(1000000);

        ChangeLocaleNewCombination();
        ChangeLocaleExistedCombination();

        User::After(1000000);
        Prepare4forRebootTest();
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
	test.Start(_L("Test to demonstrate Locale flexibility in new locale model"));
	test.Title();

	TRAPD(err, ::TestLocaleFlexibility());

	test(err == KErrNone);
    
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	delete cleanup;

	return(KErrNone);
	}
