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


_LIT(KInitialiseLocaleExeName, "InitialiseLocale.exe");

// Symbian VendorID as specified in 
// "Platform security engineering guide" Section 2.1.3.2 
const TVendorId KSymbianVendorId(0x70000001);

RTest TheTest(_L("T_Initialise_Locale"));

// const TInt KDefaultCountryCode = 29 ;
const TInt KNewCountryCode = 46 ;
TInt defaultCountryCode ;

TCurrencySymbol defaultCurrencySymbol;

_LIT(KCurrencySymbol, "ABC") ;
TBuf<8> KNewCurrencySymbol (KCurrencySymbol) ;

_LIT(KLongDateFormat, "%F%*D%X %N %Y") ;
TBuf<20> KDefaultLongDateFormat (KLongDateFormat) ;

_LIT(KShortDateFormat, "%F%*D/%*M/%Y") ;
TBuf<20> KDefaultShortDateFormat (KShortDateFormat) ;

_LIT(KTimeFormat, "%F%*I:%T:%S %*A") ;
TBuf<20> KDefaultTimeFormat (KTimeFormat) ;

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

LOCAL_C void Check(TPtrC aValue, TPtrC aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %S, got: %S\r\n"), &aExpected, &aValue);
		RestoreInitialLocaleSettings();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
#define TEST3(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0200
@SYMTestCaseDesc 	Check that default locale values are loaded on startup.
@SYMTestPriority 	High
@SYMTestActions  	Ensure that country code in system locale data matches value in persisted data
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1003
*/
void CheckDefaultLocaleCountryCode()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0200 Check default country code "));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;
	TLocale* myLocale = myExtendedLocale.GetLocale() ;
	TInt countryCode = myLocale->CountryCode();
	TheTest.Printf(_L("Default Country code is %d\n"), defaultCountryCode) ;
	TheTest.Printf(_L("Current Country code is %d\n"), countryCode) ;

	TEST2(countryCode, defaultCountryCode) ;
	TheTest.End();
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-3160
@SYMTestCaseDesc 	Check that default locale values i.e. Currency Symbol and long date, short date & time format.
@SYMTestPriority 	High
@SYMTestActions  	Ensure that these values are set to the default values as defined.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1003
*/
void CheckDefaultLocaleValues()
{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3160 Check default Locale Values "));
	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;

	TPtrC currencySymbolPtr = myExtendedLocale.GetCurrencySymbol();
	TPtrC longDateFormatPtr = myExtendedLocale.GetLongDateFormatSpec();
	TPtrC shortDateFormatPtr = myExtendedLocale.GetShortDateFormatSpec();
	TPtrC timeFormatPtr = myExtendedLocale.GetTimeFormatSpec();

	TheTest.Printf(_L("New Currency Symbol is %S \n"), &currencySymbolPtr);
	TheTest.Printf(_L("Default Long Date Format is %S \n"), &longDateFormatPtr);
	TheTest.Printf(_L("Default Short Date Format is %S \n"), &shortDateFormatPtr);
	TheTest.Printf(_L("Default Time Format is %S \n"), &timeFormatPtr);

	TPtrC expectedCurrSymPtr(KNewCurrencySymbol);
	TEST3(currencySymbolPtr, expectedCurrSymPtr);

	TPtrC expectedlongDatePtr(KDefaultLongDateFormat);
	TEST3(longDateFormatPtr, expectedlongDatePtr);

	TPtrC expectedShortDatePtr(KDefaultShortDateFormat);
	TEST3(shortDateFormatPtr, expectedShortDatePtr);

	TPtrC expectedTimePtr(KDefaultTimeFormat);
	TEST3(timeFormatPtr, expectedTimePtr);

	TheTest.End();
}

/**
@SYMTestCaseID		SYSLIB-BAFL-CIT-0201
@SYMTestCaseDesc 	Check that locale settings can be saved to and read from system locale data through TExtendedLocale.
@SYMTestPriority 	High
@SYMTestActions  	Change a number of locale values, write to system data, read back from data, check result
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1003
*/
void ChangeLocaleCountryCode()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-0201 Define a TLocale "));
	TheTest.Next(_L("Setting CountryCode"));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;
	TLocale* myLocale = myExtendedLocale.GetLocale() ;
	TheTest.Printf(_L("Setting Country code to %d\n"), KNewCountryCode) ;
	myLocale->SetCountryCode(KNewCountryCode);
	TheTest.Printf(_L("Saving new settings to system locale data\n")) ;
	myExtendedLocale.SaveSystemSettings() ;
	TheTest.Printf(_L("Loading new settings from system locale data\n")) ;
	myExtendedLocale.LoadSystemSettings() ;
	myLocale = myExtendedLocale.GetLocale() ;
	TInt countryCode = myLocale->CountryCode() ;
	TheTest.Printf(_L("New Country code is %d\n"), countryCode) ;
	TEST2(countryCode, KNewCountryCode) ;
	TheTest.End();
	}


/**
@SYMTestCaseID      SYSLIB-BAFL-CIT-188
@SYMTestCaseDesc    Check that locale settings are saved into central repository.
@SYMTestPriority    High
@SYMTestActions     Ensure that locale data stored in the central repository match the data in the system locale
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ1003
*/
void CheckRepositoryLocaleCountryCode(CRepository *repository)
   {
   TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CIT-188 Checking persisted country code "));
   TInt datacageCountryCode;
   TheTest.Printf(_L("Getting repository data cage\n")) ;
   TInt err = repository->Get(KLocaleCountryCode, datacageCountryCode) ;
   TEST2(err, KErrNone);
   TheTest.Printf(_L("Country code from repository = %d\n"), datacageCountryCode) ;
   TEST2(datacageCountryCode, KNewCountryCode) ;
   TheTest.End();
   }

/**
@SYMTestCaseID  			SYSLIB-BAFL-CT-4051
@SYMTestCaseDesc  			Check that the VendorID is set to 0x70000001 (Symbian's VendorID)
@SYMTestPriority   			Medium
@SYMTestActions 			1. Create a process for InitialiseLocale.exe
							2. Compare the processes VID against the expected VID (0x70000001)
@SYMTestExpectedResults  	The VIDs should be identical and so the test will pass.
@SYMDEF 					INC121403
*/
void TestVendorIdINC121403(RProcess& aProcess)
	{
	TheTest.Next (_L ("TestVendorIDINC121403"));
	
	// Test that the VendorID of InitialiseLocale.exe is set to the Symbian VendorID
	TEST2(KSymbianVendorId.iId, aProcess.VendorId().iId);
	}

/**
Sets up the Currency Symbol test. Outputs default values then sets the Currency Symbol
to a test value ("ABC") and stores this value in the repository, before InitialiseLocale has run.
*/
void ChangeLocaleCurrencySymbol(CRepository *repository, TCurrencySymbol & defaultCurrencySymbol)
	{
	TheTest.Start(_L("Setting CurrencySymbol\n"));

	TExtendedLocale myExtendedLocale;
	myExtendedLocale.LoadSystemSettings() ;

	TheTest.Printf(_L("Default Currency Symbol is:  %S\n"), &defaultCurrencySymbol);

	TCurrencySymbol datacageCurrencySymbol;
	TInt err = repository->Get(KLocaleCurrencySymbol, datacageCurrencySymbol) ;
	TEST2(err, KErrNone);
	TheTest.Printf(_L("Currency Symbol from repository data cage is:  %S\n"), &datacageCurrencySymbol);

	TheTest.Printf(_L("Setting Currency Symbol to:  %S\n"), &KNewCurrencySymbol);

	TheTest.Printf(_L("Saving Currency Symbol to repository.\n\n"));
	err = repository->Set(KLocaleCurrencySymbol, KNewCurrencySymbol);
	TEST2(err, KErrNone);
	User::SetCurrencySymbol(KNewCurrencySymbol);

	TheTest.End();
	}

/**
After InitialiseLocale has run. Gets the Currency Symbol back from the repository and then
outputs it. This output should be the same value as the test value ("ABC") that was set to the
repository in the above function.
*/
void TestLocaleCurrencySymbol(CRepository *repository)
	{
	TheTest.Start(_L("Checking value in the repository after InitialiseLocale() has run.\n"));

	TCurrencySymbol reposCurrencySymbol;
	TInt err = repository->Get(KLocaleCurrencySymbol, reposCurrencySymbol);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("The Currency Symbol after InitialiseLocale() has ran is: %S\n\n"), &reposCurrencySymbol);

	TEST(KNewCurrencySymbol == reposCurrencySymbol);

	TheTest.End();
	}



//
// Test new locale initialisation and persistence mechanism
void TestLocaleChanges()
	{
	RProcess process;

	GetInitialLocaleSettings() ;

	// Restore default contents of locale settings repository
	TUid LocaleRepositoryUid ;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid ;
	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid) ;
	repository->Reset();

	TInt err = repository->Get(KLocaleCountryCode, defaultCountryCode) ;
	TEST2(err, KErrNone);
	ChangeLocaleCurrencySymbol(repository, defaultCurrencySymbol);

	// Initialise locale data and start persistence notifier by spawning
	// process - this will be done as part of system start in actual
	// release code!
	TInt r = process.Create(KInitialiseLocaleExeName, KNullDesC);
	if(r == KErrNone)
		{
		TRequestStatus stat;
		process.Rendezvous(stat);
		process.Resume(); // Start the process going
		//wait for the locale initialisation to complete first before testing
		User::WaitForRequest(stat);
		TEST2((stat.Int()==KErrNone)||(stat.Int()==KErrAlreadyExists),ETrue);
		TInt flag;
		TUid initLocaleUid=TUid::Uid(KUidLocalePersistProperties);
		r=RProperty::Get(initLocaleUid,(TUint)EInitialisationState,flag);
		TEST2(r,KErrNone);
		//check for the P&S flag that indicates localeinitialisation complete
		TEST2(flag,ELocaleInitialisationComplete);
		TestLocaleCurrencySymbol(repository);
		User::After(1000000);
		CheckDefaultLocaleCountryCode();
		User::After(1000000);
		CheckDefaultLocaleValues();
		User::After(1000000);
		ChangeLocaleCountryCode();
		User::After(1000000);
		CheckRepositoryLocaleCountryCode(repository);
		User::After(1000000);
		TestVendorIdINC121403(process);
		process.Close();
		User::After(1000000);
		}
	CleanupStack::PopAndDestroy(repository) ;
	RestoreInitialLocaleSettings() ;
	}



TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	TheTest.Start(_L("Basic Locale initialisation TheTests"));
	TheTest.Title();
	TRAPD(err, ::TestLocaleChanges())
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;

	delete tc;

	return(KErrNone);
	}
