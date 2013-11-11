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

#ifndef _LOCALE_REPOSITORY
#define _LOCALE_REPOSITORY

#include <e32std.h>

const TUint32 KLocalePersistRepositoryUid = 0x1020E4D3 ; 

const TUint KLocaleDllBase = 10 ;
const TUint KLocaleLanguageDll = KLocaleDllBase ;
const TUint KLocaleCollationDll = KLocaleDllBase + 1 ;
const TUint KLocaleLocaleDll = KLocaleDllBase + 2 ;
const TUint KLocaleTimeDateDll = KLocaleDllBase + 3 ;


// ===============================================================
// Individually customisable settings
const TUint KLocaleTLocaleBase = 100 ;

// ===============================================================
// Time and Date Settings
const TUint KLocaleTimeAndDateSettingsBase = KLocaleTLocaleBase ;

const TUint KLocaleUtcOffset = KLocaleTimeAndDateSettingsBase ;
const TUint KLocaleDateFormat = KLocaleTimeAndDateSettingsBase + 1 ;
const TUint KLocaleTimeFormat = KLocaleTimeAndDateSettingsBase + 2 ;

// 4 Date Separators
const TUint KLocaleDateSeparatorBase = KLocaleTimeAndDateSettingsBase + 3 ;
const TUint KLocaleDateSeparator0 = KLocaleDateSeparatorBase ;
const TUint KLocaleDateSeparator1 = KLocaleDateSeparatorBase + 1 ;
const TUint KLocaleDateSeparator2 = KLocaleDateSeparatorBase + 2 ;
const TUint KLocaleDateSeparator3 = KLocaleDateSeparatorBase + 3 ;

// 4 Time separators
const TUint KLocaleTimeSeparatorBase = KLocaleTimeAndDateSettingsBase + 7 ;
const TUint KLocaleTimeSeparator0 = KLocaleTimeSeparatorBase ;
const TUint KLocaleTimeSeparator1 = KLocaleTimeSeparatorBase + 1 ;
const TUint KLocaleTimeSeparator2 = KLocaleTimeSeparatorBase + 2 ;
const TUint KLocaleTimeSeparator3 = KLocaleTimeSeparatorBase + 3 ;

const TUint KLocaleAmPmSpaceBetween = KLocaleTimeAndDateSettingsBase + 11 ;
const TUint KLocaleAmPmSymbolPosition = KLocaleTimeAndDateSettingsBase + 12 ;
const TUint KLocaleWorkDays = KLocaleTimeAndDateSettingsBase + 15 ;
const TUint KLocaleStartOfWeek = KLocaleTimeAndDateSettingsBase + 16 ;
const TUint KLocaleClockFormat = KLocaleTimeAndDateSettingsBase + 17 ;
const TUint KLocaleDeviceTimeState = KLocaleTimeAndDateSettingsBase +18 ;


// ===================================================================
// Currency Settings
const TUint KLocaleCurrencySettingsBase = KLocaleTLocaleBase +20;
const TUint KLocaleCurrencySymbolPosition = KLocaleCurrencySettingsBase ;
const TUint KLocaleCurrencySpaceBetween = KLocaleCurrencySettingsBase + 1 ;
const TUint KLocaleCurrencyDecimalPlaces = KLocaleCurrencySettingsBase + 2 ;
const TUint KLocaleCurrencyNegativeInBrackets = KLocaleCurrencySettingsBase + 3 ;
const TUint KLocaleCurrencyTriadsAllowed = KLocaleCurrencySettingsBase + 4 ;
const TUint KLocaleCurrencyNegativeFormat = KLocaleCurrencySettingsBase + 5 ;
const TUint KLocaleCurrencyNegativeLoseSpace = KLocaleCurrencySettingsBase + 6 ;
const TUint KLocaleCurrencySymbolOpposite = KLocaleCurrencySettingsBase + 7 ;
const TUint KLocaleCurrencySymbol = KLocaleCurrencySettingsBase + 8 ;


// ==================================================================
// Units Settings
const TUint KLocaleUnitsSettingsBase = KLocaleTLocaleBase +40;
const TUint KLocaleUnitsGeneral = KLocaleUnitsSettingsBase ;
const TUint KLocaleUnitsDistanceShort = KLocaleUnitsSettingsBase + 1 ;
const TUint KLocaleUnitsDistanceLong = KLocaleUnitsSettingsBase + 2 ;



// ====================================================================
// Numeric Settings
const TUint KLocaleNumericSettingsBase = KLocaleTLocaleBase +50;
const TUint KLocaleDigitType = KLocaleNumericSettingsBase ;
const TUint KLocaleThousandsSeparator = KLocaleNumericSettingsBase + 1 ;
const TUint KLocaleDecimalSeparator = KLocaleNumericSettingsBase + 2 ;


// ====================================================================
// Language related settings
const TUint KLocaleLanguageSettingsBase = KLocaleTLocaleBase +60;
// 3 language downgrade path entries
const TUint KLocaleCountryCode = KLocaleLanguageSettingsBase ;
const TUint KLocaleLanguageDowngradeBase = KLocaleLanguageSettingsBase + 1 ;
const TUint KLocaleLanguageDowngrade0 = KLocaleLanguageDowngradeBase ;
const TUint KLocaleLanguageDowngrade1 = KLocaleLanguageDowngradeBase + 1 ;
const TUint KLocaleLanguageDowngrade2 = KLocaleLanguageDowngradeBase + 2 ;



#endif  // _LOCALE_REPOSITORY
