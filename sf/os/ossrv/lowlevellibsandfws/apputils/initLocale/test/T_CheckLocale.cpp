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

void TestLocaleDLLName(RTest& test, const TDesC& alanguage, const TDesC& aRegion, const TDesC& aCollation)
    {
    test.Printf(_L("\nThe expected locale language, Region, Collation DLL names are: %S, %S, %S"), &alanguage, &aRegion, &aCollation);
    User::After(1000000);
    TExtendedLocale extendLocale;
    extendLocale.LoadSystemSettings();
    TBuf<KMaxFullName> loadedLocaleDLLName;
    User::After(1000000);
    
    extendLocale.GetLocaleDllName(ELocaleLanguageSettings, loadedLocaleDLLName);
    test.Printf(_L("\nThe system locale lagnuage DLL names is: %S"), &loadedLocaleDLLName);
    test(KErrNotFound != loadedLocaleDLLName.Find(alanguage));
 
    extendLocale.GetLocaleDllName(ELocaleLocaleSettings, loadedLocaleDLLName);
    test.Printf(_L("\nThe system locale Region DLL names is: %S"), &loadedLocaleDLLName);
    test(KErrNotFound != loadedLocaleDLLName.Find(aRegion));
    
    extendLocale.GetLocaleDllName(ELocaleCollateSetting, loadedLocaleDLLName);
    test.Printf(_L("\nThe system locale collation DLL names is: %S"), &loadedLocaleDLLName);
    test(KErrNotFound != loadedLocaleDLLName.Find(aCollation));
    }

void TestLocaleContent(RTest& test, TLanguage alanguageID, TPtrC aCurrencySymbol, TUint aTCollationMethodID)
    {
    test.Printf(_L("\nThe expected locale languageID, CurrencySymbol, TCollationMethodID are: %d, %S, %d"), alanguageID, &aCurrencySymbol, aTCollationMethodID);
    User::After(1000000);
    TExtendedLocale extendLocale;
    extendLocale.LoadSystemSettings();
    User::After(1000000);

    test.Printf(_L("\nThe system locale languageID is: %d"), User::Language());
    test(alanguageID == User::Language());
    TPtrC currencySymbol = extendLocale.GetCurrencySymbol();
    test.Printf(_L("\nThe system locale CurrencySymbol is: %S"), &currencySymbol);
    test(aCurrencySymbol == extendLocale.GetCurrencySymbol());
    test.Printf(_L("\nThe system locale TCollationMethodID is: %d"), extendLocale.GetPreferredCollationMethod().iId);
    test(aTCollationMethodID == extendLocale.GetPreferredCollationMethod().iId);
    }
