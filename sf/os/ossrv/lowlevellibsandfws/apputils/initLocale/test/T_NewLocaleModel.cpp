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


#include "T_CheckLocale.h"
#include <e32reg.h>

RTest test(_L("T_NewLocaleModel"));


//new locale API should work in the new locale model
void TestNewLocaleAPI()
    {
    TExtendedLocale firstExtendLocale;
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4022 new LoadLocale() interface"));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KGermanLanguage(), KGERegion(), KGECollation()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KGermanLanguage(), KGERegion(), KGECollation() );
    TestLocaleContent(test, KGELanguageID, KGECurrencySymbol, KGETCollationMethodID);
	test( ERegDEU == User::RegionCode() );

    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KCNLanguage(), KCNRegion(), KCNCollation()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KCNLanguage(), KCNRegion(), KCNCollation() );
    TestLocaleContent(test, KCNLanguageID, KCNCurrencySymbol, KCNTCollationMethodID);
    
    //becasue TestLocaleContent() naturally check the system locale content, we mark this test case here.
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4025 publish locale content to the system"));
    test.End();

    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KUSLanguage(), KCNRegion(), KGECollation()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KUSLanguage(), KCNRegion(), KGECollation() );
    TestLocaleContent(test, KUSLanguageID, KCNCurrencySymbol, KGETCollationMethodID);
    test.End();
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4023 new LoadLocaleAspect() interface"));  
    firstExtendLocale.LoadSystemSettings();    
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KCNLanguage));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KUSRegion));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KGECollation));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KCNLanguage(), KUSRegion(), KGECollation() );
    TestLocaleContent(test, KCNLanguageID, KUSCurrencySymbol, KGETCollationMethodID);

    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KEnglishLanguage));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KUKRegion));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(KStandardCollation));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KEnglishLanguage(), KUKRegion(), KStandardCollation() );
    TestLocaleContent(test, KEnglishLanguageID, KUKCurrencySymbol, KStandardCollationID);
    test.End();

    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4020 new LoadLocale() should not work with old locale DLL"));  
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KOldUKLocaleName(), KOldUKLocaleName(), KOldUKLocaleName()) );
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KOldCNLocaleName(), KOldCNLocaleName(), KOldCNLocaleName()) );
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KOldUKLocaleName(), KOldCNLocaleName(), KOldGELocaleName()) );
    test.End();

    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4021 new LoadLocaleAspect() should not work with old locale DLL"));  
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KOldUKLocaleName()));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KOldCNLocaleName()));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KOldGELocaleName()));
    test.End();
    
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CT-4024 check DLL naming convention"));  
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KInvalidNameLanguage(), KUKRegion(), KGECollation()) );
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KEnglishLanguage(), KInvalidNameRegion(), KGECollation()) );
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KEnglishLanguage(), KUKRegion(), KInvalidNameCollation()) );
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocale(KInvalidNameLanguage(), KInvalidNameRegion(), KInvalidNameCollation()) );

    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KInvalidNameLanguage()));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KInvalidNameRegion()));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNotFound == firstExtendLocale.LoadLocaleAspect(KInvalidNameCollation()));
    test.End();
    }

//Old locale API should work with the legacy old locale DLL name in the new locale model
void TestOldLocaleAPI()
    {
    TExtendedLocale firstExtendLocale;
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CIT-4018 old LoadLocale() API should work with the legacy old locale DLL name in the new locale model"));  
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KOldUKLocaleName()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KEnglishLanguage(), KUKRegion(), KStandardCollation() );
    TestLocaleContent(test, KEnglishLanguageID, KUKCurrencySymbol, KStandardCollationID);
   
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KOldCNLocaleName()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KCNLanguage(), KCNRegion(), KCNCollation() );
    TestLocaleContent(test, KCNLanguageID, KCNCurrencySymbol, KCNTCollationMethodID);
    test.End();
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CIT-4017 old LoadLocaleAspect() API should work with the legacy old locale DLL name in the new locale model"));  
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLanguageSettings, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleCollateSetting, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLocaleSettings, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KGermanLanguage(), KGERegion(), KGECollation() );
    TestLocaleContent(test, KGELanguageID, KGECurrencySymbol, KGETCollationMethodID);
    
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLanguageSettings, KOldUKLocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleCollateSetting, KOldCNLocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLocaleSettings, KOldUSLocaleName()));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KEnglishLanguage(), KUSRegion(), KCNCollation() );
    TestLocaleContent(test, KEnglishLanguageID, KUSCurrencySymbol, KCNTCollationMethodID);
    test.End();
    }


void TestNewLocaleModel()
    {   
    TestOldLocaleAPI();   
    TestNewLocaleAPI();
    }

TInt E32Main()
    {
    CTrapCleanup* tc = CTrapCleanup::New();
    test(tc != NULL);

    __UHEAP_MARK;

    test.Start(_L("The test for check old API and new API behavior in new locale model"));
    test.Title();
    TRAPD(err, ::TestNewLocaleModel())
    test(err == KErrNone);

    test.End();
    test.Close();

    __UHEAP_MARKEND;

    delete tc;

    return(KErrNone);
    }
