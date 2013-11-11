// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "..\EcomTestUtils\EcomTestUtils.h"
#include <e32test.h>
#include <Ecom\ECom.h>
#include <Ecom\ImplementationInformation.h>
#include <hal.h>
#include <bautils.h>

TUid KUidMultipeImageInterface = { 0x10282307 };
static RFs TheFs;
static RTest TheTest(_L("t_multipleImageTest.exe"));

#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
    _LIT(KEnglishLocale, "elocl.01");
    _LIT(KFrenchLocale, "elocl.02");
    _LIT(KGermanLocale, "elocl.03");
#else
    // refer base/os/kernelhwsrv/kernel/eka/euser/locmapping.cpp
    // Language
    _LIT(KEComEnglishLang,"elocl_lan.001");
    _LIT(KEComFrenchLang,"elocl_lan.002");
    _LIT(KEComGermanLang,"elocl_lan.003");
    // Region
    _LIT(KEComEnglishReg,"elocl_reg.826");
    _LIT(KEComFrenchReg,"elocl_reg.250");
    _LIT(KEComGermanReg,"elocl_reg.276");
    // Collation
    _LIT(KEComEnglishColl,"elocl_col.001");
    _LIT(KEComFrenchColl,"elocl_col.002");
    _LIT(KEComGermanColl,"elocl_col.003");
#endif

// Test Macroes and Functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

// Type definition for test functions.
// Used in calling t_multipleImageTest functions.
typedef void (*FuncPtrL) (TLanguage* , TPtrC* );

void KillEComServerL()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(error, EComTestUtils::KillProcessL(KEComServerProcessName));
   	TEST2(error, KErrNone);
	}

void SetLanguageDowngradeL(TLanguage* aLanguages)
	{
	TLocale locale;

	TInt i=0;
	RArray<TLanguage> languageArray;

	TLanguage lang = User::Language();

	if(lang != aLanguages[0])
		{
	    TExtendedLocale extendedLocale;
	    extendedLocale.LoadSystemSettings();
		
	    if(aLanguages[0] == ELangEnglish)
			{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
                extendedLocale.LoadLocale(KEnglishLocale);
			#else
                extendedLocale.LoadLocale(KEComEnglishLang,KEComEnglishReg,KEComEnglishColl);
			#endif
			}
		else if(aLanguages[0] == ELangFrench)
			{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL			
                extendedLocale.LoadLocale(KFrenchLocale);
            #else
                extendedLocale.LoadLocale(KEComFrenchLang,KEComFrenchReg,KEComFrenchColl);
			#endif			
			}
		else if(aLanguages[0] == ELangGerman)
			{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL			
                extendedLocale.LoadLocale(KGermanLocale);
			#else
                extendedLocale.LoadLocale(KEComGermanLang,KEComGermanReg,KEComGermanColl);
			#endif          
			}
		else 
			{
			__ASSERT_ALWAYS(EFalse, User::Leave(KErrNotSupported)); //Unexpected test language
			}

	    // Save the changes to system settings
		extendedLocale.SaveSystemSettings();

		TEST(User::Language() == aLanguages[0]);
		}

	for(i=1; aLanguages[i] != ELangNone && i < KMaxDowngradeLanguages; i++)
		{
		locale.SetLanguageDowngrade(i, aLanguages[i]);
		}

	locale.Set();

	BaflUtils::GetDowngradePathL(TheFs,  aLanguages[0], languageArray);

	for(i = languageArray.Count() - 1; i >= 0; i--)
		{
		TLanguage language = languageArray[i];
		TheTest.Printf(_L("Updated language downgrade[%d]: %d\n"), i, language);
		TEST2(languageArray[i], aLanguages[i]);
		}

	languageArray.Close();
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1694
@SYMTestCaseDesc Test that ECOM lists the expected plugins under different language downgrade path settings
@SYMTestPriority High
@SYMTestActions  Set the language downgrade path.Kill ECOM server to ensure ECOM uses the new downgrade path
next time it starts. List implementations. Check that the list contains the expected plugins.
@SYMTestExpectedResults The test must not fail.
@SYMREQ CR0759
*/
void ListImplementationsL(TLanguage* aLanguageDowngradePath, TPtrC* aExpectedImplementations)
	{
	//set the language downgrade path
	SetLanguageDowngradeL(aLanguageDowngradePath);
	//kill ecom server to ensure the use of updated language downgrade path
	KillEComServerL();

	RImplInfoPtrArray implementations;
	TRAPD(err, REComSession::ListImplementationsL(KUidMultipeImageInterface, implementations));
	TEST2(err, KErrNone);

	const TInt implementationCount = implementations.Count();
	TheTest.Printf(_L("Found %d implementations.\n"), implementationCount);

	for (TInt i=0; i<implementationCount; i++)
		{
		const CImplementationInformation* info = implementations[i];
		TheTest.Printf(_L("Implementation[%d]: UID=0x%x DisplayName=\"%S\"\n"), i, (info->ImplementationUid()).iUid, &(info->DisplayName())) ;
		TEST(info->DisplayName() == aExpectedImplementations[i]);
		}

	// Empty the array of implementations to avoid leaks
	implementations.ResetAndDestroy();
	//close the ecom session to avoid leaks
	REComSession::FinalClose();
	}


LOCAL_C void DoBasicTestL(FuncPtrL testFuncL, TLanguage* aLanguageDowngradePath, TPtrC* aExpectedImplementations, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	testFuncL(aLanguageDowngradePath, aExpectedImplementations);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}


LOCAL_C void DoTestsL()
	{
	const TLanguage languageDowngradePath[][7] =
		{
			{ELangEnglish, ELangFrench, ELangGerman, ELangNone},
			{ELangFrench, ELangGerman, ELangEnglish, ELangNone},
			{ELangGerman, ELangFrench, ELangEnglish, ELangNone},
			{ELangFrench, ELangGerman, ELangNone, ELangNone},
			{ELangGerman, ELangFrench, ELangNone, ELangNone},
			{ELangFrench, ELangNone, ELangNone, ELangNone},
			{ELangGerman, ELangNone, ELangNone, ELangNone}
		};

	const TPtrC pluginResolution[][16] =
		{
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 02 French"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 02 French"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 02 French"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 02 French"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 02 French"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 02 French"),
			_L("Example S Image 1 Language 02 French")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 02 French"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 02 French"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 02 French"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 02 French"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 02 French"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 02 French"),
			_L("Example S Image 1 Language 02 French")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 03 German"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 03 German"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 03 German"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 03 German"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 03 German"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 03 German"),
			_L("Example S Image 2 Language 03 German")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 02 French"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 02 French"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 02 French"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 02 French"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 02 French"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 02 French"),
			_L("Example S Image 1 Language 02 French")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 03 German"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 03 German"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 03 German"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 03 German"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 03 German"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 03 German"),
			_L("Example S Image 2 Language 03 German")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example E Image 2 Language 02 French"),
			_L("Example F Image 2 Language 02 French"),
			_L("Example G Image 1 Language 02 French"),
			_L("Example H Image 1 Language 01 UK English"),
			_L("Example I Image 1 Language 02 French"),
			_L("Example K Image 2 Language 02 French"),
			_L("Example L Image 2 Language 01 UK English"),
			_L("Example M Image 2 Language 02 French"),
			_L("Example O Image 2 Language 02 French"),
			_L("Example P Image 2 Language 01 UK English"),
			_L("Example Q Image 2 Language 02 French"),
			_L("Example S Image 1 Language 02 French"),
			_L("Dummy"),
			_L("Dummy")
			},
			{
			_L("Example B Image 2 Language 01 UK English"),
			_L("Example C Image 2 Language 01 UK English"),
			_L("Example G Image 1 Language 03 German"),
			_L("Example H Image 1 Language 03 German"),
			_L("Example I Image 1 Language 03 German"),
			_L("Example J Image 1 Language 03 German"),
			_L("Example K Image 2 Language 03 German"),
			_L("Example L Image 2 Language 03 German"),
			_L("Example M Image 2 Language 03 German"),
			_L("Example N Image 2 Language 03 German"),
			_L("Example O Image 2 Language 03 German"),
			_L("Example P Image 2 Language 03 German"),
			_L("Example Q Image 2 Language 03 German"),
			_L("Example S Image 2 Language 03 German"),
			_L("Dummy"),
			_L("Dummy")
			}
		};

	//make a copy of the current locale
	TLocale oldLocale;

	TRAPD(err,
		{
		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[0],
					(TPtrC*)pluginResolution[0],
					_L("Check Installed Plugins for Downgrade Path English-French-German"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[1],
					(TPtrC*)pluginResolution[1],
					_L("Check Installed Plugins for Downgrade Path French-German-English"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[2],
					(TPtrC*)pluginResolution[2],
					_L("Check Installed Plugins for Downgrade Path German-French-English"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[3],
					(TPtrC*)pluginResolution[3],
					_L("Check Installed Plugins for Downgrade Path French-German"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[4],
					(TPtrC*)pluginResolution[4],
					_L("Check Installed Plugins for Downgrade Path German-French"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[5],
					(TPtrC*)pluginResolution[5],
					_L("Check Installed Plugins for Downgrade Path French"));

		oldLocale.Set();

		DoBasicTestL(&ListImplementationsL,
					(TLanguage*)languageDowngradePath[6],
					(TPtrC*)pluginResolution[6],
					_L("Check Installed Plugins for Downgrade Path German"));
		}
		);
	TExtendedLocale extendedLocale;
	extendedLocale.LoadSystemSettings();
#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
	// Change the locale to Default Locale, all settings from single Dll
	extendedLocale.LoadLocale(KEnglishLocale);
#else
    extendedLocale.LoadLocale(KEComEnglishLang,KEComEnglishReg,KEComEnglishColl);
#endif
	
	// Save the changes to system settings
	extendedLocale.SaveSystemSettings();

	//reset locale
	oldLocale.Set();

	if(err != KErrNone)
		{
		User::Leave(err);
		}
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1694 MultipleImageTests "));

	TEST2(TheFs.Connect(), KErrNone);

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// Construct and install the Active Scheduler. The Active Scheduler is needed
	// by components used by this test as they are ActiveObjects.
	CActiveScheduler* activeScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	TRAPD(err,DoTestsL());
	TEST2(err, KErrNone);

	//delete scheduler;
	delete activeScheduler;
	delete cleanup;

	TheFs.Close();

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;

	User::Heap().Check();

	return KErrNone;
	}


