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

#ifndef __T_CHECKLOCALE_H__
#define __T_CHECKLOCALE_H__

#include <e32std.h>
#include <collate.h>
#include <e32test.h>


_LIT(KOldUKLocaleName, "elocl.01") ;
_LIT(KOldCNLocaleName, "elocl.31") ;
_LIT(KOldUSLocaleName, "elocl.10") ;
_LIT(KOldGELocaleName, "elocl.03") ;

_LIT(KEnglishLanguage, "elocl_lan.001") ;
_LIT(KUKRegion, "elocl_reg.826") ;
_LIT(KStandardCollation, "elocl_col.001") ;
const TLanguage KEnglishLanguageID = ELangEnglish;
const TPtrC KUKCurrencySymbol(_S("\x00a3"));
const TUint KStandardCollationID = KUidBasicCollationMethod;


_LIT(KUSLanguage, "elocl_lan.010") ;
_LIT(KUSRegion, "elocl_reg.840") ;
_LIT(KUSCollation, "elocl_col.010") ;
const TLanguage KUSLanguageID = ELangAmerican;
const TPtrC KUSCurrencySymbol(_S("\x0024"));
const TUint KUSTCollationMethodID = KUidBasicCollationMethod;


_LIT(KCNLanguage, "elocl_lan.031") ;
_LIT(KCNRegion, "elocl_reg.156");
_LIT(KCNCollation, "elocl_col.031");
const TLanguage KCNLanguageID = ELangPrcChinese;
const TPtrC KCNCurrencySymbol(_S("\x00a5"));
const TUint KCNTCollationMethodID = 0x10009067;

_LIT(KGermanLanguage, "elocl_lan.003") ;
_LIT(KGERegion, "elocl_reg.276") ;
_LIT(KGECollation, "elocl_col.003");
const TLanguage KGELanguageID = ELangGerman;
const TPtrC KGECurrencySymbol(_S("\x20ac"));
const TUint KGETCollationMethodID = KUidBasicCollationMethod;


_LIT(KInvalidNameLanguage, "elocl_lon.031") ;
_LIT(KInvalidNameRegion, "elocl_reggf.276") ;
_LIT(KInvalidNameCollation, "errelocl_col.003");



void TestLocaleDLLName(RTest& test, const TDesC& alanguage, const TDesC& aRegion, const TDesC& aCollation);
void TestLocaleContent(RTest& test, TLanguage alanguageID, TPtrC aCurrencySymbol, TUint aTCollationMethodID);


#endif
