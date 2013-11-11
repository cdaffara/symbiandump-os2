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

RTest test(_L("T_OldAPIinOldModel"));


//Old locale API should matain backward compatibility  in the old locale model
//(with the old locale DLL format)

void TestOldLocaleAPI()
    {
    TExtendedLocale firstExtendLocale;
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CIT-4015 old LoadLocale() API on old locale model"));
    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KOldCNLocaleName()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );   
    TestLocaleDLLName(test, KOldCNLocaleName(), KOldCNLocaleName(), KOldCNLocaleName() );
    TestLocaleContent(test, KCNLanguageID, KCNCurrencySymbol, KCNTCollationMethodID);

    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocale(KOldUKLocaleName()) );
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );   
    TestLocaleDLLName(test, KOldUKLocaleName(), KOldUKLocaleName(), KOldUKLocaleName() );
    TestLocaleContent(test, KEnglishLanguageID, KUKCurrencySymbol, KStandardCollationID);
    test.End();
    
    //becasue TestLocaleContent() naturally check the system locale content, we mark this test case here.
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CIT-4019 publish locale content to the system"));
    test.End();
    
    test.Start(_L(" @SYMTestCaseID:TI18N-EUSER-CIT-4016 old LoadLocaleAspect() API on old locale model"));
    firstExtendLocale.LoadSystemSettings();  
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLanguageSettings, KOldUKLocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleCollateSetting, KOldCNLocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLocaleSettings, KOldUSLocaleName()));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KOldUKLocaleName(), KOldUSLocaleName(), KOldCNLocaleName() );
    TestLocaleContent(test, KEnglishLanguageID, KUSCurrencySymbol, KCNTCollationMethodID);

    firstExtendLocale.LoadSystemSettings();
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLanguageSettings, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleCollateSetting, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.LoadLocaleAspect(ELocaleLocaleSettings, KOldGELocaleName()));
    test( KErrNone == firstExtendLocale.SaveSystemSettings() );
    TestLocaleDLLName(test, KOldGELocaleName(), KOldGELocaleName(), KOldGELocaleName() );
    TestLocaleContent(test, KGELanguageID, KGECurrencySymbol, KGETCollationMethodID);
    test.End();
    }


void TestOldLocaleModel()
    {   
    TestOldLocaleAPI();   
    }

TInt E32Main()
    {
    CTrapCleanup* tc = CTrapCleanup::New();
    test(tc != NULL);

    __UHEAP_MARK;

    test.Start(_L("The test for old locale API in old locale model"));
    test.Title();
    TRAPD(err, ::TestOldLocaleModel())
    test(err == KErrNone);
    
    test.Printf(_L("\nTest for old locale API in old locale model finished!"));

    test.End();
    test.Close();

    __UHEAP_MARKEND;

    delete tc;

    return(KErrNone);
    }
