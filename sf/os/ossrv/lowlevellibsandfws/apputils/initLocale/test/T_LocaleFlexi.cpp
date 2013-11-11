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
// That are, 1. Selectable from Multiple Dlls 2. Loaded all from a single Dll
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
_LIT(KFrenchLocale, "elocl.02");

_LIT(KSpanishLocale, "elocl.04");

// Central Repository Data cage file-path
_LIT(KPersistLocale, "C:\\private\\10202be9\\persists\\1020E4D3.*");

LOCAL_D RTest TheTest(_L("T_LocaleFlexi"));

RFs TheFs;

TExtendedLocale initialExtendedLocale;


void GetInitialLocaleSettings()
	{
	initialExtendedLocale.LoadSystemSettings() ;
	}

void RestoreInitialLocaleSettings()
	{
	initialExtendedLocale.SaveSystemSettings() ;
	User::After(1000000);
	}

//Test macros and functions

LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		RestoreInitialLocaleSettings();
		TheTest(EFalse, aLine);
		}
	}

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		RestoreInitialLocaleSettings();
		TheTest(EFalse, aLine);
		}
	}


#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)


/*
This function prepares for next test i.e, tests InitialiseLocale behaves
when persisted file is damaged/corrupt.
*/
void SetUpTestCorruptLocale()
	{
	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;

	TBuf<100> DllName;
	DllName.Copy(KFrenchLocale);

	TheTest.Printf(_L("\n Changing Locale to French Locale(02) so that default values are evident"));

	// Change the locale to Default Locale, all settings from single Dll
	myExtendedLocale.LoadLocale(DllName);

	// Save the changes to system settings
	myExtendedLocale.SaveSystemSettings();

	// Wait so that it persists to repository
	User::After(1000000);

	User::LeaveIfError(TheFs.Connect());
	// Delete/damage the persisted file so that InitialiseLocale.exe
	// detects the damage and loads default locale
	BaflUtils::DeleteFile(TheFs, KPersistLocale);
	TheFs.Close();
	TheTest.Printf(_L("\n Deleted persist file from C: \n"));
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0140
@SYMTestCaseDesc 	Check that locale settings can be selected from several DLLs
@SYMTestPriority 	High
@SYMTestActions  	Ensure that locales are selected, system set and persisted
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1001
*/
LOCAL_D	void ChangeLocaleMultiDll()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0140 \nTest to demonstrate Locale settings by multiple DLLs "));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings();

	TBuf<100> DllName1 ;
	DllName1.Copy(KFrenchLocale);

	TBuf<100> DllName2 ;
	DllName2.Copy(KSpanishLocale);

	TheTest.Printf(_L("Changing Language to French "));

	// Load the language settings of French
	myExtendedLocale.LoadLocaleAspect(ELocaleLanguageSettings, DllName1);


	// Load system charset of French
	myExtendedLocale.LoadLocaleAspect(ELocaleCollateSetting, DllName1);

	// Load TLocale settings, currency symbols of Spanish
	myExtendedLocale.LoadLocaleAspect(ELocaleLocaleSettings, DllName2);

	// Load time and date display formats of Spanish
	myExtendedLocale.LoadLocaleAspect(ELocaleTimeDateSettings, DllName2);

	// Save to system settings
	myExtendedLocale.SaveSystemSettings();

	// Wait
	User::After(1000000);

	TUid LocaleRepositoryUid ;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid ;

	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid);

	// To get names of DLLs to be loaded
	TBuf<100> aDllName ;

	// Test the language settings
	TInt err = repository->Get(KLocaleLanguageDll, aDllName);
	if (err != KErrNone)
		{
		// Oops! I hit it again
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}


	TheTest.Printf(_L("\nTested for language French(02): %S "), &aDllName);

	// in this case we are only interested in the name "eloc.XX"

	TEST(DllName1.CompareF(aDllName.Right(8)) == 0);


	// Test system charset changes
	err = repository->Get(KLocaleCollationDll, aDllName);
	if (err != KErrNone)
		{
		// Oops! I hit it again
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}
	TheTest.Printf(_L("\nTested for Char set of French(02): %S"), &aDllName);
	TEST(DllName1.CompareF(aDllName.Right(8))==0);

	// Test TLocale settings, currency symbols
	err = repository->Get(KLocaleLocaleDll, aDllName) ;
		if (err != KErrNone)
		{
		// Oops! I hit it again
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}
	TheTest.Printf(_L("\nTested for Locale of Spanish(04): %S"), &aDllName);
	TEST(DllName2.CompareF(aDllName.Right(8))==0);

	// Test time and date display formats
	err = repository->Get(KLocaleTimeDateDll, aDllName) ;
		if (err != KErrNone)
		{
		// Oops! I hit it again
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}
	TheTest.Printf(_L("\nTested for Time and date format of Spanish(04): %S\n"), &aDllName);
	TEST(DllName2.CompareF(aDllName.Right(8))==0);

	// Free the central repository object
	CleanupStack::PopAndDestroy (repository);
	TheTest.End() ;
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0141
@SYMTestCaseDesc 	Check that locale settings can be selected from single DLL
@SYMTestPriority 	High
@SYMTestActions  	Ensure that all settings are loaded from a DLL, system set and persisted
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1001
*/
LOCAL_D void ChangeLocaleSingleDll()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0141 Test to demonstrate change of Locale from single DLL "));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;

	TBuf<100> DllName;
	DllName.Copy(KSpanishLocale);

	TheTest.Printf(_L("Changing Locale to Spanish"));

	// Change the locale to French Locale, all settings from single Dll
	myExtendedLocale.LoadLocale(DllName);

	// Save the changes to system settings
	myExtendedLocale.SaveSystemSettings();

	// Wait
	User::After(1000000);

	TUid LocaleRepositoryUid ;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid;
	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid) ;

	// To get names of DLLs to be loaded
	TBuf<100> aDllName ;

	// Test for the language settings
	TInt err = repository->Get(KLocaleLanguageDll, aDllName);
	if (err != KErrNone)
		{
		// Failed to read DLL name, bail out!
		CleanupStack::PopAndDestroy(repository) ;
		return ;
		}
	TheTest.Printf(_L("\nTested for Locale settings loaded by single Dll: %S"), &aDllName);
	TEST(DllName.CompareF(aDllName.Right(8))==0);

	CleanupStack::PopAndDestroy (repository);
	TheTest.End();
	}

// Call the tests that test flexibility of Locale
void TestLocaleFlexibility()
	{
	RProcess process;

	GetInitialLocaleSettings() ;

	// Kick-in InitialiseLocale.exe, This will be done as part of system start in actual
	// release code!
	TInt r = process.Create(KInitPersistLocaleExeName, KNullDesC);
	if(r == KErrNone)
		{
		process.Resume(); // Start the process going
		User::After(1000000);

		ChangeLocaleMultiDll();
		ChangeLocaleSingleDll();

		User::After(1000000);
		// Set up for next test
		SetUpTestCorruptLocale();
		User::After(1000000);
		process.Close();
		User::After(1000000);
		}

	RestoreInitialLocaleSettings() ;
	}


TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TEST(cleanup != NULL);

	__UHEAP_MARK;
	TheTest.Start(_L("Test to demonstrate Locale flexibility"));
	TheTest.Title();

	TRAPD(err, ::TestLocaleFlexibility());

	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	delete cleanup;

	return(KErrNone);
	}
