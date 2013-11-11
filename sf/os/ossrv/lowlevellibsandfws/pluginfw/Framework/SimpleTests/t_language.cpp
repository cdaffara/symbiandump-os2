// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/ecom.h>
#include "../EcomTestUtils/EcomTestUtils.h"

#define UNUSED_VAR(a) a = a

LOCAL_D RTest TEST(_L("ECom Language Plugin Test"));

// Files to copy
_LIT(KEComLangPluginDllOnC, "C:\\sys\\bin\\LanguagePlugin.dll");
_LIT(KEComLangPluginDllOnZ, "Z:\\RAMOnly\\LanguagePlugin.dll");

_LIT(KEComLangPluginRscOnC, "C:\\resource\\plugins\\LanguagePlugin.rsc");
_LIT(KEComLangPluginRscOnZ, "Z:\\RAMOnly\\LanguagePlugin.rsc");

_LIT(KEComLangPluginRsc1OnC, "C:\\resource\\plugins\\LanguagePlugin.r01");
_LIT(KEComLangPluginRsc1OnZ, "Z:\\RAMOnly\\LanguagePlugin.r01");

_LIT(KEComLangPluginRsc2OnC, "C:\\resource\\plugins\\LanguagePlugin.r02");
_LIT(KEComLangPluginRsc2OnZ, "Z:\\RAMOnly\\LanguagePlugin.r02");

_LIT(KEComLangPluginRsc3OnC, "C:\\resource\\plugins\\LanguagePlugin.r03");
_LIT(KEComLangPluginRsc3OnZ, "Z:\\RAMOnly\\LanguagePlugin.r03");

#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
    // Language files used for locale settings
    _LIT(KEComEnglishLocale, "elocl.01");
    _LIT(KEComFrenchLocale, "elocl.02");
    _LIT(KEComGermanLocale, "elocl.03");
    _LIT(KEComSpanishLocale, "elocl.04");
#else
// refer base/os/kernelhwsrv/kernel/eka/euser/locmapping.cpp
// Language
    _LIT(KEComEnglishLang,"elocl_lan.001");
    _LIT(KEComFrenchLang,"elocl_lan.002");
    _LIT(KEComGermanLang,"elocl_lan.003");
    _LIT(KEComSpanishLang,"elocl_lan.004");
    // Region
    _LIT(KEComEnglishReg,"elocl_reg.826");
    _LIT(KEComFrenchReg,"elocl_reg.250");
    _LIT(KEComGermanReg,"elocl_reg.276");
    _LIT(KEComSpanishReg,"elocl_reg.724");
    // Collation
    _LIT(KEComEnglishColl,"elocl_col.001");
    _LIT(KEComFrenchColl,"elocl_col.002");
    _LIT(KEComGermanColl,"elocl_col.003");
    _LIT(KEComSpanishColl,"elocl_col.001");
#endif

// Result strings saved in opaque_data of each plugin
_LIT8(KEComDefaultResult, "LANGUAGE_RSC");
_LIT8(KEComEnglishResult, "LANGUAGE_01");
_LIT8(KEComFrenchResult, "LANGUAGE_02");
_LIT8(KEComGermanResult, "LANGUAGE_03");

// Test data for data driven tests.
#define MAX_LANGUAGE_TEST_FILES 5
struct TLangTestEntry
	{
	// Source and destination to copy necessary plugins
	struct FilesInfo
		{
		const TDesC* source;
		const TDesC* dest;
		};
	FilesInfo files[MAX_LANGUAGE_TEST_FILES];
#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
	// Locale file to load
	const TDesC* localeFile;
#else
	const TDesC* lang;
	const TDesC* region;
	const TDesC* collation;
#endif
	// Language downgrade path to set up
	TLanguage downgradePathSetup[3];

	// Expected result - opaque_data of retrieved plugin
	const TDesC8* result;
	};

// Test Data
LOCAL_D const TLangTestEntry LangTestData[] =
	{
		// Data Set 0
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc1OnZ, &KEComLangPluginRsc1OnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
			&KEComEnglishLocale,
            #else
			&KEComEnglishLang,
			&KEComEnglishReg,
			&KEComEnglishColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComEnglishResult
		},
		// Data Set 1
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc1OnZ, &KEComLangPluginRsc1OnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComFrenchLocale,
            #else
            &KEComFrenchLang,
            &KEComFrenchReg,
            &KEComFrenchColl,
            #endif
			{ELangFrench, ELangEnglish, ELangGerman},
			&KEComFrenchResult
		},
		// Data Set 2
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc1OnZ, &KEComLangPluginRsc1OnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangGerman, ELangEnglish, ELangFrench},
			&KEComGermanResult
		},
		// Data Set 3
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComFrenchLocale,
            #else
            &KEComFrenchLang,
            &KEComFrenchReg,
            &KEComFrenchColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComFrenchResult
		},
		// Data Set 4
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComGermanResult
		},
		// Data Set 5
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangSpanish, ELangItalian, ELangGerman},
			&KEComGermanResult
		},
		// Data Set 6
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc2OnZ, &KEComLangPluginRsc2OnC},
				{&KEComLangPluginRsc3OnZ, &KEComLangPluginRsc3OnC}
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComEnglishLocale,
            #else
            &KEComEnglishLang,
            &KEComEnglishReg,
            &KEComEnglishColl,
            #endif
			{ELangSpanish, ELangItalian, ELangSwedish},
			&KEComDefaultResult
		},
		// Data Set 7
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
				{&KEComLangPluginRsc1OnZ, &KEComLangPluginRsc1OnC},
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComEnglishLocale,
            #else
            &KEComEnglishLang,
            &KEComEnglishReg,
            &KEComEnglishColl,
            #endif
			{ELangSpanish, ELangSwedish, ELangItalian},
			&KEComEnglishResult
		},
		// Data Set 8
		{
			{
				{&KEComLangPluginDllOnZ, &KEComLangPluginDllOnC},
				{&KEComLangPluginRscOnZ, &KEComLangPluginRscOnC},
			},
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComEnglishLocale,
            #else
            &KEComEnglishLang,
            &KEComEnglishReg,
            &KEComEnglishColl,
            #endif
			{ELangItalian, ELangSwedish, ELangSpanish},
			&KEComDefaultResult
		},
	};

struct TLangTestEntry2
	{
	// Locale file to load
    #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
        // Locale file to load
        const TDesC* localeFile;
    #else
        const TDesC* lang;
        const TDesC* region;
        const TDesC* collation;
    #endif
	
	// Language downgrade path to set up
	TLanguage downgradePathSetup[3];

	// Expected result - opaque_data of retrieved plugin
	const TDesC8* result;
	};

LOCAL_D const TLangTestEntry2 LangTestData2[] =
	{
		//Data Set 0
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComEnglishLocale,
            #else
            &KEComEnglishLang,
            &KEComEnglishReg,
            &KEComEnglishColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComEnglishResult

		},

		//Data Set 1
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComFrenchLocale,
            #else
            &KEComFrenchLang,
            &KEComFrenchReg,
            &KEComFrenchColl,
            #endif
			{ELangFrench, ELangEnglish, ELangGerman},
			&KEComFrenchResult
		},
		//Data Set 2
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangGerman, ELangEnglish, ELangFrench},
			&KEComGermanResult
		},
		//Data Set 3
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComFrenchLocale,
            #else
            &KEComFrenchLang,
            &KEComFrenchReg,
            &KEComFrenchColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComFrenchResult
		},
		//Data Set 4
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangEnglish, ELangFrench, ELangGerman},
			&KEComGermanResult
		},
		//Data Set 5
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComGermanLocale,
            #else
            &KEComGermanLang,
            &KEComGermanReg,
            &KEComGermanColl,
            #endif
			{ELangSpanish, ELangItalian, ELangGerman},
			&KEComGermanResult
		},
		//Data Set 6
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComEnglishLocale,
            #else
            &KEComEnglishLang,
            &KEComEnglishReg,
            &KEComEnglishColl,
            #endif
			{ELangSpanish, ELangItalian, ELangSwedish},
			&KEComEnglishResult

		},
		//Data Set 7
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComSpanishLocale,
            #else
            &KEComSpanishLang,
            &KEComSpanishReg,
            &KEComSpanishColl,
            #endif
			{ELangSpanish, ELangFrench, ELangEnglish},
			&KEComFrenchResult

		},
		//Data Set 8
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComSpanishLocale,
            #else
            &KEComSpanishLang,
            &KEComSpanishReg,
            &KEComSpanishColl,
            #endif
			{ELangSpanish,ELangItalian, ELangSwedish},
			&KEComDefaultResult
		},

		//Data Set 9
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComSpanishLocale,
            #else
            &KEComSpanishLang,
            &KEComSpanishReg,
            &KEComSpanishColl,
            #endif
			{ELangSpanish, ELangGerman, ELangItalian},
			&KEComGermanResult
		},

		//Data Set 10
		{
            #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            &KEComSpanishLocale,
            #else
            &KEComSpanishLang,
            &KEComSpanishReg,
            &KEComSpanishColl,
            #endif
			{ELangSpanish, ELangItalian, ELangNone},
			&KEComDefaultResult
		}
	};

class REComLanguagePluginTest
	{
public:
	void StaticRunTestsL();
	void DynamicRunTestsOnCL();
	void DynamicRunTestsOnZL();
    #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL	
        void SetLanguage(const TDesC& aLangFile,  const TLanguage aLang[3]);
    #else	
        void SetLanguage(const TDesC& aLanguage,const TDesC& aReg,const TDesC& aColl,const TLanguage aLang[3]);
    #endif
	void CopyFiles(TInt aIndex);
	static void DeleteFiles(TInt aIndex);
	void DoCheckL(TInt testDataId, TUid &aInterfaceId, TDriveUnit aDriveUnit);
	};

/**
Copies plugins.
*/
void REComLanguagePluginTest::CopyFiles(TInt aIndex)
	{
	for (TInt i = 0; i < MAX_LANGUAGE_TEST_FILES && LangTestData[aIndex].files[i].source; i++)
		{
		TRAPD(err, EComTestUtils::FileManCopyFileL(	*(LangTestData[aIndex].files[i].source),
													*(LangTestData[aIndex].files[i].dest)) );
 		TEST(err==KErrNone, __LINE__);
		}
	}

/**
Deletes plugins.
*/
void REComLanguagePluginTest::DeleteFiles(TInt aIndex)
	{
		for (TInt i = 0; i < MAX_LANGUAGE_TEST_FILES && LangTestData[aIndex].files[i].source != NULL; i++)
		{
		TRAPD(err, EComTestUtils::FileManDeleteFileL(*(LangTestData[aIndex].files[i].dest)) );
		UNUSED_VAR(err);
		}
	}
#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
/**
Sets up the system-wide locale and language downgrade path.
*/
void REComLanguagePluginTest::SetLanguage(const TDesC& aLangFile, const TLanguage aLang[3])
	{
	TLocale locale;
	TInt loadval = 0;
	TExtendedLocale extendedLocale;

	extendedLocale.LoadSystemSettings();
	loadval = extendedLocale.LoadLocale(aLangFile);
	TEST(loadval==KErrNone, __LINE__);
	extendedLocale.SaveSystemSettings();

	locale.SetLanguageDowngrade(0, aLang[0]);
	locale.SetLanguageDowngrade(1, aLang[1]);
	locale.SetLanguageDowngrade(2, aLang[2]);
	locale.Set();
	}
#else
/**
Sets up the system-wide locale and language downgrade path for the new locale model.
*/
void REComLanguagePluginTest::SetLanguage(const TDesC& aLanguage,const TDesC& aReg,const TDesC& aColl,const TLanguage aLang[3])
    {
    TLocale locale;
    TExtendedLocale extendedLocale;
    TInt loadval = 0;

    extendedLocale.LoadSystemSettings();
    loadval = extendedLocale.LoadLocale(aLanguage,aReg,aColl);
    TEST(loadval==KErrNone, __LINE__);
    extendedLocale.SaveSystemSettings();

    locale.SetLanguageDowngrade(0, aLang[0]);
    locale.SetLanguageDowngrade(1, aLang[1]);
    locale.SetLanguageDowngrade(2, aLang[2]);
    locale.Set();
    }
#endif

void REComLanguagePluginTest::DoCheckL(TInt testDataId, TUid &aInterfaceId, TDriveUnit aDriveUnit)
	{
	RImplInfoPtrArray implArray;

	REComSession::ListImplementationsL(aInterfaceId, implArray);

	// Verify results
	TBuf<100> buf;
	RDebug::Print(_L("\tArray Count = %d\n"), implArray.Count());
	buf.Copy(implArray[0]->OpaqueData());
	RDebug::Print(_L("\tOpaque Data = %S\n"), &buf);
	TInt drive = implArray[0]->Drive();
	RDebug::Print(_L("\tDrive = %d\n"), drive);

	TEST(implArray.Count() == 1);

	if(aDriveUnit == EDriveZ)
		{
		TEST(implArray[0]->OpaqueData() == *(LangTestData2[testDataId].result));
		}
	else
		{
		TEST(implArray[0]->OpaqueData() == *(LangTestData[testDataId].result));
		}

	TEST(implArray[0]->Drive() == aDriveUnit);

	// Post test clean up
	REComSession::FinalClose();
	implArray.ResetAndDestroy();
	}

/**
Test case for DEF087773: ECOM ValidateRegistry checks for existence of RSC file only

@SYMTestCaseID			SYSLIB-ECOM-CT-0655
@SYMTestCaseDesc		Checks for existence of resource files for different downgrade paths
@SYMTestPriority			high
@SYMTestActions			Data driven tests.
						Create different sets of language dependant resource file on "C"
						Drive and verify the ECom discovery process
@SYMTestExpectedResults The test must not fail.
@SYMDEF					DEF087773
*/
void REComLanguagePluginTest::StaticRunTestsL()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0655 "));
	TInt numberOfElements = sizeof(LangTestData)/sizeof(TLangTestEntry);

	TUid ifUid={0x10009DB9};
	for(TInt i = 0; i < numberOfElements; i++)
		{
		// kill ecom server so that language downgrade changes are picked up
		ECOMTU_KILL_SERVER_L(TEST);

		// Prepare test
        #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
		SetLanguage(*(LangTestData[i].localeFile), LangTestData[i].downgradePathSetup);
        #else
		SetLanguage(*(LangTestData[i].lang),*(LangTestData[i].region),*(LangTestData[i].collation),LangTestData[i].downgradePathSetup);
        #endif
		
		CopyFiles(i);

		// 1st boot
		TEST.Printf(_L("\tLanguage Plugin Test %d - 1st boot \n"), i);
		DoCheckL(i,ifUid, EDriveC );

		// kill ecom server for second boot
		ECOMTU_KILL_SERVER_L(TEST);

		// 2nd boot
		TEST.Printf(_L("\tLanguage Plugin Test %d - 2nd boot \n"), i);
		DoCheckL(i, ifUid, EDriveC);

		DeleteFiles(i);
		WAIT_FOR3s;
		}
	}

/**
Test case for CR0209: Enable Dynamic Language Switching in ECOM

@SYMTestCaseID			SYSLIB-ECOM-CT-3169
@SYMTestCaseDesc		Checks for picking up correct resource files for different downgrade paths in running time
@SYMTestPriority		high
@SYMTestActions			Data driven tests.
						Start up Ecom server
						Create different sets of language dependant resource file on "C" on running time.
						Drive and verify the ECom discovery process
@SYMTestExpectedResults The test must not fail.
@SYMDEF					CR0209
*/
void REComLanguagePluginTest::DynamicRunTestsOnCL()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3169 "));
	TInt numberOfElements = sizeof(LangTestData)/sizeof(TLangTestEntry);

	RImplInfoPtrArray implArray;
	TUid ifUid={0x10009DB9};

	//start up ecom server
	REComSession::ListImplementationsL(ifUid, implArray);

	for(TInt i = 0; i < numberOfElements; i++)
		{

		// Prepare test
		CopyFiles(i);

        #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
        SetLanguage(*(LangTestData[i].localeFile), LangTestData[i].downgradePathSetup);
        #else
        SetLanguage(*(LangTestData[i].lang),*(LangTestData[i].region),*(LangTestData[i].collation),LangTestData[i].downgradePathSetup);
        #endif
		WAIT_FOR3s;

		TEST.Printf(_L("\tLanguage Plugin Test %d - Dynamic switch on C \n"), i);
		DoCheckL(i,ifUid, EDriveC);

		DeleteFiles(i);
		WAIT_FOR3s;
		implArray.ResetAndDestroy();

		}

	// Post test clean up
	REComSession::FinalClose();
	implArray.ResetAndDestroy();

	//kill ecomserver after finishing test
	ECOMTU_KILL_SERVER_L(TEST);

	}

/**
Test case for CR0209: Enable Dynamic Language Switching in ECOM on Z: drive

@SYMTestCaseID			SYSLIB-ECOM-CT-3170
@SYMTestCaseDesc		Checks for picking up correct resource files for different downgrade paths in running time.
@SYMTestPriority		high
@SYMTestActions			Data driven tests.
						Start up Ecom server
						Change languae downgrade path on running time.
						Drive and verify the ECom discovery process.
@SYMTestExpectedResults The test must not fail.
@SYMDEF					CR0209
*/
void REComLanguagePluginTest::DynamicRunTestsOnZL()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3170 "));
	TInt numberOfElements = sizeof(LangTestData2)/sizeof(TLangTestEntry2);

	//Interface ID in a DLL existing on Z: Drive
	TUid ifUid={0x10009DE1};

	//start up ecom server
	RImplInfoPtrArray implArray;
	REComSession::ListImplementationsL(ifUid, implArray);
	WAIT_FOR3s;

	for(TInt i = 0; i < numberOfElements; i++)
		{

		// Prepare test
        #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
        SetLanguage(*(LangTestData2[i].localeFile), LangTestData2[i].downgradePathSetup);
        #else
        SetLanguage(*(LangTestData2[i].lang),*(LangTestData2[i].region),*(LangTestData2[i].collation),LangTestData2[i].downgradePathSetup);
        #endif
		WAIT_FOR3s;

		// Dynamic language downgrade path changing
		TEST.Printf(_L("\tLanguage Plugin Test %d - Dynamic switch on Z \n"), i);
		DoCheckL(i,ifUid, EDriveZ);
		implArray.ResetAndDestroy();

		}

	// Post test clean up
	REComSession::FinalClose();
	implArray.ResetAndDestroy();

	}

LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;
	REComLanguagePluginTest testSuite;
	testSuite.StaticRunTestsL();
	testSuite.DynamicRunTestsOnCL();
	testSuite.DynamicRunTestsOnZL();

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TEST.Title();
	TEST.Start(_L("ECom Language Plugins tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,RunTestL());

	// tidy up in case of leave
	if(err != KErrNone)
		{
		TInt numberOfElements = sizeof(LangTestData)/sizeof(TLangTestEntry);
		for(TInt i = 0; i < numberOfElements; i++)
			{
			REComLanguagePluginTest::DeleteFiles(i);
			}
		}

	TEST(err==KErrNone, __LINE__);

	delete scheduler;
	delete cleanup;

	TEST.End();
	TEST.Close();

	__UHEAP_MARKEND;
	return(0);
	}
