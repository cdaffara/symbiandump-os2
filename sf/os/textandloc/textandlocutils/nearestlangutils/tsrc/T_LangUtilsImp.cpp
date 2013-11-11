// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_langutilsimp.h"


#define test(cond)                                          \
    {                                                       \
    TBool __bb = (cond);                                    \
    TEST(__bb);                                             \
    if (!__bb)                                              \
        {                                                   \
        ERR_PRINTF1(_L("ERROR: Test Failed"));              \
        User::Leave(1);                                     \
        }                                                   \
    }



void CT_LANGUTILSIMP::TestPath(const TLanguagePath& aPath, const TLanguage* aExpected, TInt aExpectedCount)
    {
    for (int i = 0; i != aExpectedCount; ++i)
        test(aPath[i] == *aExpected++);
    test(aPath[aExpectedCount] == ELangNone);
    }

const TLanguage ExpectedPath[] = { ELangFrench, ELangJapanese, ELangGerman, ELangGreek,
    ELangArabic, ELangEnglish, ELangItalian, ELangHebrew };

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0443
@SYMTestCaseDesc        Tests TLanguagePath class
@SYMTestPriority        High
@SYMTestActions         Test for adding different language paths
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_LANGUTILSIMP::TestAddLanguage()
    {
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0443 "));
    TLanguagePath path;
    path[0] = ELangNone;
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 0);
    AddLanguage(path, ELangFrench);
    TestPath(path, ExpectedPath, 1);
    AddLanguage(path, ELangFrench);
    TestPath(path, ExpectedPath, 1);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 1);
    AddLanguage(path, ELangFrench);
    TestPath(path, ExpectedPath, 1);
    AddLanguage(path, ELangJapanese);
    TestPath(path, ExpectedPath, 2);
    AddLanguage(path, ELangFrench);
    TestPath(path, ExpectedPath, 2);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 2);
    AddLanguage(path, ELangGerman);
    TestPath(path, ExpectedPath, 3);
    AddLanguage(path, ELangGreek);
    TestPath(path, ExpectedPath, 4);
    AddLanguage(path, ELangArabic);
    TestPath(path, ExpectedPath, 5);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 5);
    AddLanguage(path, ELangEnglish);
    TestPath(path, ExpectedPath, 6);
    AddLanguage(path, ELangItalian);
    TestPath(path, ExpectedPath, 7);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 7);
    AddLanguage(path, ELangHebrew);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangHebrew);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangEnglish);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangItalian);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangFrench);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangNone);
    TestPath(path, ExpectedPath, 8);
    AddLanguage(path, ELangGerman);
    TestPath(path, ExpectedPath, 8);
    }



/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0444
@SYMTestCaseDesc        Tests for the functionality of TLocale class
@SYMTestPriority        High
@SYMTestActions         Test for language downgrades
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_LANGUTILSIMP::TestDowngrade(SDowngradeTest& aTest)
    {
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0444 "));
    TLocale loc;
    loc.SetLanguageDowngrade(0, aTest.iIn[1]);
    loc.SetLanguageDowngrade(1, aTest.iIn[2]);
    loc.SetLanguageDowngrade(2, aTest.iIn[3]);
    TLanguagePath path;
    MakeLanguageDowngradePath(path, aTest.iIn[0], ELangNone, loc);
    for (int i = 0; ; ++i)
        {
        test(i < 9);
        test(aTest.iOut[i] == path[i]);
        if (path[i] == ELangNone)
            return;
        }
    
    MakeLanguageDowngradePath(path, ELangAustralian, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangNewZealand, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangSouthAfricanEnglish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangInternationalEnglish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangAmerican, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_Apac, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_Taiwan, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_HongKong, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_Prc, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_Japan, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangEnglish_Thailand, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangSwissFrench, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangBelgianFrench, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangInternationalFrench, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangCanadianFrench, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangSwissGerman, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangAustrian, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangInternationalSpanish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangLatinAmericanSpanish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangSwissItalian, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangFinlandSwedish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangCyprusTurkish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangBelgianFlemish, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangHongKongChinese, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangCyprusGreek, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangMalay_Apac, ELangNone, loc);
    MakeLanguageDowngradePath(path, ELangBrazilianPortuguese, ELangNone, loc);
    
    }


// Test Dialect enumeration.
// Each value represents a dialect (variant) of a TLanguage language.
// E.g. AmericanSprint is represented by ELangAmerican | EDialectSprint

// The dialect location -- top six bits
const TUint KDialectFactor=0x0400;
enum TDialect
    {
    // The following are example dialects for testing only
    EDialectSprint = (1*KDialectFactor),
    EDialectOrange = (2*KDialectFactor),
    EDialectMaximum = EDialectOrange // This must always be equal to the last TDialect enum.
    // Up to 63 dialects.
    };

const TLanguage ELangEnglishOrange = static_cast<TLanguage>(ELangEnglish|EDialectOrange);
const TLanguage ELangAmericanSprint = static_cast<TLanguage>(ELangAmerican|EDialectSprint);

SDowngradeTest DowngradeData[] =
    {
        // 0 = test language
        // 1-3 = language downgrade for locale
        // 4- = calculated language downgrade path, terminated with ELangNone.
        {{ELangEnglishOrange, ELangNone, ELangNone, ELangNone},
            {ELangEnglishOrange, ELangEnglish, ELangNone}},
        {{ELangAmericanSprint, ELangNone, ELangNone, ELangNone},
            {ELangAmericanSprint, ELangAmerican, ELangEnglish, ELangNone}},
        {{ELangFrench, ELangNone, ELangNone, ELangNone},
            {ELangFrench, ELangNone}},
        {{ELangSwissFrench, ELangNone, ELangNone, ELangNone},
            {ELangSwissFrench, ELangFrench, ELangNone}},
        {{ELangCanadianEnglish, ELangNone, ELangNone, ELangNone},
            {ELangCanadianEnglish, ELangAmerican, ELangEnglish, ELangNone}},
        {{ELangSwissFrench, ELangNone, ELangFrench, ELangNone},
            {ELangSwissFrench, ELangFrench, ELangNone}},
        {{ELangCanadianEnglish, ELangEnglish, ELangNone, ELangAmerican},
            {ELangCanadianEnglish, ELangEnglish, ELangAmerican, ELangNone}},
        {{ELangCanadianEnglish, ELangNone, ELangNone, ELangSwissFrench},
            {ELangCanadianEnglish, ELangSwissFrench, ELangAmerican, ELangEnglish, ELangNone}},
        {{ELangCanadianEnglish, ELangFrench, ELangAmerican, ELangSwissFrench},
            {ELangCanadianEnglish, ELangFrench, ELangAmerican, ELangSwissFrench, ELangEnglish, ELangNone}},
        {{ELangCanadianEnglish, ELangNone, ELangGerman, ELangSwissFrench},
            {ELangCanadianEnglish, ELangGerman, ELangSwissFrench, ELangAmerican, ELangEnglish, ELangNone}},
        {{ELangCanadianEnglish, ELangFinlandSwedish, ELangGerman, ELangSwissFrench},
            {ELangCanadianEnglish, ELangFinlandSwedish, ELangGerman, ELangSwissFrench, ELangAmerican, ELangEnglish, ELangNone}},
        {{ELangFinlandSwedish, ELangFinlandSwedish, ELangFinlandSwedish, ELangFinlandSwedish},
            {ELangFinlandSwedish, ELangSwedish, ELangNone}},
    };

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0445
@SYMTestCaseDesc        Tests for language downgrades
@SYMTestPriority        High
@SYMTestActions         Call up downgrade path test function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_LANGUTILSIMP::TestMakeLanguageDowngradePath()
    {
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0445 "));
    for (int i = 0; i != sizeof(DowngradeData)/sizeof(DowngradeData[0]); ++i)
        TestDowngrade(DowngradeData[i]);
    }

void CT_LANGUTILSIMP::SetUpFinderForLanguageAndDriveSearchL(
    TTestNearestLanguageFileFinder& aFinder)
    {
    aFinder.iDrives = _L("ZAG");
    aFinder.iFilesSearchedFor = new(ELeave) CDesCArraySeg(10);
    aFinder.iFilesSearchedFor->AppendL(_L("z:\\dir\\name.e02"));
    aFinder.iFilesSearchedFor->AppendL(_L("a:\\dir\\name.e02"));
    aFinder.iFilesSearchedFor->AppendL(_L("g:\\dir\\name.e02"));
    aFinder.iFilesSearchedFor->AppendL(_L("z:\\dir\\name.e15702")); //Extended lang
    aFinder.iFilesSearchedFor->AppendL(_L("a:\\dir\\name.e15702"));
    aFinder.iFilesSearchedFor->AppendL(_L("g:\\dir\\name.e15702"));
    aFinder.iFilesSearchedFor->AppendL(_L("z:\\dir\\name.e01"));
    aFinder.iFilesSearchedFor->AppendL(_L("a:\\dir\\name.e01"));
    aFinder.iFilesSearchedFor->AppendL(_L("g:\\dir\\name.e01"));
    aFinder.iFilesSearchedFor->AppendL(_L("z:\\dir\\name.ext"));
    aFinder.iFilesSearchedFor->AppendL(_L("a:\\dir\\name.ext"));
    aFinder.iFilesSearchedFor->AppendL(_L("g:\\dir\\name.ext"));
    aFinder.iDrivesChecked = _L("zagzagzagzagzag");
    }

_LIT(KTestFileName1, "z:\\dir\\name.ext");
//_LIT(KTestFileName2, "z:\\dir\\náme.ext");
_LIT(KTestFileName2, "z:\\dir\\n\u00c1me.ext");
_LIT(KTestFileName3, "a:\\dir\\name.ext");
_LIT(KTestFileName4, "c:\\dir\\name.ext");
_LIT(KTestFileName5, "d:\\dir\\name.ext");
_LIT(KTestFileName6, "y:\\dir\\name.ext");
_LIT(KTestFileName7, "A:\\dir\\name.ext");
_LIT(KTestFileName8, "C:\\dir\\name.ext");
_LIT(KTestFileName9, "Y:\\dir\\name.ext");
_LIT(KTestFileName11, "\\abc\\defg.hijkl");
_LIT(KTestFileName256Chars, "\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz0.ext");

_LIT(KExpectedFileName256Chars, "?:\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz01234\
\\abcdefghijklmnopqrstuvwxyz01234\\abcdefghijklmnopqrstuvwxyz0.e");
_LIT(KExpectedFileName1, "?:\\dir\\name.e");
_LIT(KExpectedSuffix1, "xt");
_LIT(KExpectedFileName2, "?:\\abc\\defg.hij");
_LIT(KExpectedSuffix2, "kl");
_LIT(KTestFileName1French, "z:\\dir\\name.e02");
_LIT(KTestFileName1Japanese, "z:\\dir\\name.e32");
_LIT(KTestFileName1Mongolian, "z:\\dir\\name.e54321");
_LIT(KTestFileName1AmericanSprint, "z:\\dir\\name.e1034"); //American=0x0A, Sprint=0x0400. 0x040A=1034.
_LIT(KCheckedForFrench, "?:\\dir\\name.e02");

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0446
@SYMTestCaseDesc        Nearest language finder test
@SYMTestPriority        High
@SYMTestActions         Test drive sequence construction
                        Test addition of language number
                        Test searching each drive for the specified file
                        Test searching language and drive down the language path
                        Test filename matching
                        Test searching for any language file within a drive
                        Test searching all drives for any language
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_LANGUTILSIMP::TestNearestLanguageFinder()
    {
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0446 "));
    RFs dummy;
    TTestNearestLanguageFileFinder finder(dummy);

    // Test file name setting and disecting
    // SetFileName
    // RepairFileName
    TFileName fileName(KTestFileName1);
    finder.SetFileName(fileName);
    test(0 <= finder.iFileName->MatchF(KExpectedFileName1));
    test(0 == finder.iSuffix.Compare(KExpectedSuffix1));
    test('z' == finder.iInitialDriveLetter);
    finder.RepairFileName();
    test(0 == finder.iFileName->Compare(KTestFileName1));
    fileName = KTestFileName11;
    finder.SetFileName(fileName);
    test(0 <= finder.iFileName->MatchF(KExpectedFileName2));
    test(0 == finder.iSuffix.Compare(KExpectedSuffix2));
    test(-1 == finder.iInitialDriveLetter);
    finder.RepairFileName();
    test(0 == finder.iFileName->Compare(KTestFileName11));
    fileName = KTestFileName256Chars;
    finder.SetFileName(fileName);
    test(0 <= finder.iFileName->MatchF(KExpectedFileName256Chars));
    test(0 == finder.iSuffix.Compare(KExpectedSuffix1));
    test(-1 == finder.iInitialDriveLetter);
    finder.RepairFileName();
    test(0 == finder.iFileName->Compare(KTestFileName256Chars));

    // Test drive sequence construction
    // AddCustomResourceDrive
    finder.iCustomRscDrive = static_cast<TInt>(EDriveC);    // drive c:
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CZ")));
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CA")));
    fileName = KTestFileName4;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrAlreadyExists);
    test(0 == finder.iDrives.CompareF(_L("C")));
    fileName = KTestFileName5;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CD")));
    fileName = KTestFileName6;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CY")));
    fileName = KTestFileName7;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CA")));
    fileName = KTestFileName8;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrAlreadyExists);
    test(0 == finder.iDrives.CompareF(_L("C")));
    fileName = KTestFileName9;
    finder.SetFileName(fileName);
    test(finder.AddCustomResourceDrive() == KErrNone);
    test(0 == finder.iDrives.CompareF(_L("CY")));
    // AddAllDrives
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("ZYXWVUTSRQPONMLKJIHGFEDCBA")));
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("AYXWVUTSRQPONMLKJIHGFEDCBZ")));
    fileName = KTestFileName4;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("CYXWVUTSRQPONMLKJIHGFEDBAZ")));
    fileName = KTestFileName5;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("DYXWVUTSRQPONMLKJIHGFECBAZ")));
    fileName = KTestFileName6;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("YXWVUTSRQPONMLKJIHGFEDCBAZ")));
    fileName = KTestFileName7;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("AYXWVUTSRQPONMLKJIHGFEDCBZ")));
    fileName = KTestFileName8;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("CYXWVUTSRQPONMLKJIHGFEDBAZ")));
    fileName = KTestFileName9;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    test(0 == finder.iDrives.CompareF(_L("YXWVUTSRQPONMLKJIHGFEDCBAZ")));

    // Test addition of language number
    // AppendLanguageCode
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(ELangFrench);
    test(0 == finder.iFileName->CompareF(KTestFileName1French));
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(ELangJapanese);
    test(0 == finder.iFileName->CompareF(KTestFileName1Japanese));
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(ELangAmericanSprint);
    test(0 == finder.iFileName->CompareF(KTestFileName1AmericanSprint));

    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(static_cast<TLanguage>(54321));
    test(0 == finder.iFileName->CompareF(KTestFileName1Mongolian));
    fileName = KTestFileName256Chars;
    finder.SetFileName(fileName);
    test(!finder.AppendLanguageCode(ELangFrench));

    // Test searching each drive for the specified file
    // FindDrive
    finder.iFileCheckedFor = KCheckedForFrench;
    finder.iDrivesChecked = _L("D");
    fileName = KTestFileName5;
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(ELangFrench);
    finder.FindDrive();
    test(finder.iDrivesChecked.Length() == 0);
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    finder.iDrivesChecked = _L("dyxwvutsrqponmlkjihgfecbaz");
    finder.AppendLanguageCode(ELangFrench);
    finder.FindDrive();
    test(finder.iDrivesChecked.Length() == 0);
    fileName = KTestFileName5;
    finder.SetFileName(fileName);
    finder.AddAllDrives();
    finder.iDrivesChecked = _L("dyxwvutsrqponmlkjihgfecbaz");
    finder.AppendLanguageCode(ELangFrench);
    finder.iFilesThatExist = new (ELeave) CDesCArraySeg(10);
    finder.iFilesThatExist->AppendL(_L("o:\\dIR\\NAmE.E02"));
    test(finder.FindDrive());
    test(0 == finder.iFileName->CompareF(_L("O:\\dir\\name.e02")));
    fileName = KTestFileName2;
    finder.SetFileName(fileName);
    finder.AppendLanguageCode(ELangFrench);
    finder.AddAllDrives();
    finder.iDrivesChecked = _L("zyxwvutsrqponmlkjihgfedcba");
    finder.iFilesThatExist->AppendL(_L("v:\\dIR\\NA\x301mE.E02"));
    //finder.iFileCheckedFor = _L("?:\\dir\\nÁme.e02");
    finder.iFileCheckedFor = _L("?:\\dir\\n\u00c1me.e02");
    test(finder.FindDrive());
    //test(0 == finder.iFileName->CompareF(_L("v:\\dir\\nÁme.e02")));
    test(0 == finder.iFileName->CompareF(_L("v:\\dir\\n\u00c1me.e02")));
    delete finder.iFilesThatExist;
    finder.iFilesThatExist = 0;

    // Test searching language and drive down the language path
    // FindLanguageAndDrive
    finder.iPath[0] = static_cast<TLanguage>(2);
    finder.iPath[1] = static_cast<TLanguage>(15702);
    finder.iPath[2] = static_cast<TLanguage>(1);
    finder.iPath[3] = ELangNone;
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    SetUpFinderForLanguageAndDriveSearchL(finder);
    finder.iFileCheckedFor = _L("?:\\dir\\name.e*");
    test(!finder.FindLanguageAndDrive());
    // check that iFileName is undamaged
    finder.RepairFileName();
    test(0 == finder.iFileName->CompareF(KTestFileName1));
    delete finder.iFilesSearchedFor;
    finder.iFilesThatExist = new (ELeave) CDesCArraySeg(10);
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e15702"));
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    SetUpFinderForLanguageAndDriveSearchL(finder);
    test(finder.FindLanguageAndDrive());
    test(0 == finder.iFileName->CompareF(_L("a:\\dir\\name.e15702")));
    delete finder.iFilesSearchedFor;
    finder.iFilesSearchedFor = 0;

    finder.iPath[0] = static_cast<TLanguage>(7);
    finder.iPath[1] = ELangNone;
    finder.iFilesThatExist->AppendL(_L("g:\\dir\\name.ext"));
    fileName = KTestFileName1;
    finder.SetFileName(fileName);
    finder.iDrives = _L("ZAG");
    finder.iDrivesChecked = _L("ZAGZAG");
    test(finder.FindLanguageAndDrive());
    test(0 == finder.iFileName->CompareF(_L("g:\\dir\\name.ext")));
    delete finder.iFilesThatExist;
    finder.iFilesThatExist = 0;

    // Test filename matching
    // LanguageNumberFromFile
    test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb04"), _L("AVB")));
    //test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("ávb04"), _L("A\x301VB")));
    //test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("a\x301vb04"), _L("ÁVB")));
    test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("\u00e1vb04"), _L("A\x301VB")));
    test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("a\x301vb04"), _L("\u00c1VB")));
    test(5254 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb5254"), _L("aVb")));
    test(KErrNotFound == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb04"), _L("avb04")));
    test(KErrNotFound == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb04"), _L("av")));
    test(KErrNotFound == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb04"), _L("")));
    test(0 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb0000000"), _L("AVB00000")));
    test(4 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb000000004"), _L("AVB0000000")));
    test(100004 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb000100004"), _L("AVB000")));
    test(111114 == TNearestLanguageFileFinder::LanguageNumberFromFile(_L("avb0111114"), _L("AVB0")));

    // Test searching for any language file within a drive
    // FindFirstLanguageFile
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    finder.iFilesThatExist = new (ELeave) CDesCArraySeg(10);
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e107"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e07"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e02"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.ee01"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.01"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name01"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name01.e01"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e15108"));
    finder.iFilesThatExist->AppendL(_L("a:\\dir\\name.e03"));
    finder.iTestScanner.iDriveError[0] = KErrNoMemory;
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    test(KErrNotFound == finder.FindFirstLanguageFile(dummy));
    finder.iTestScanner.iDriveError[0] = KErrAbort;
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    test(KErrNotFound == finder.FindFirstLanguageFile(dummy));
    finder.iTestScanner.iDriveError[0] = KErrNone;
    fileName = KTestFileName3;
    finder.SetFileName(fileName);
    test(0 <= finder.FindFirstLanguageFile(dummy));
    test(0 == finder.iFileName->CompareF(_L("a:\\dir\\name.e02")));

    finder.iFilesThatExist->AppendL(_L("a:\\abaffafg.100"));
    finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.9539"));
    finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.19539"));
    finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.1000"));
    finder.iFilesThatExist->AppendL(_L("a:\\abaffafg.102"));
    fileName = _L("a:\\abcdefg.xx");
    finder.SetFileName(fileName);
    test(0 <= finder.FindFirstLanguageFile(dummy));
    test(0 == finder.iFileName->CompareF(_L("a:\\abcdefg.1000")));
    finder.iFilesThatExist->AppendL(_L("a:\\abaffafg.aaa100"));
    finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.0523"));    // not well-formed. Will not count.
    finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.1001"));
    fileName = _L("a:\\abcdefg.xx");
    finder.SetFileName(fileName);
    test(0 <= finder.FindFirstLanguageFile(dummy));
    test(0 == finder.iFileName->CompareF(_L("a:\\abcdefg.1000")));
    fileName = _L("a:\\abaffafg.xx");
    finder.SetFileName(fileName);
    test(0 <= finder.FindFirstLanguageFile(dummy));
    test(0 == finder.iFileName->CompareF(_L("a:\\abaffafg.100")));
    finder.iFilesThatExist->AppendL(_L("a:\\abaffafg.09"));
    fileName = _L("a:\\abaffafg.xx");
    finder.SetFileName(fileName);
    test(0 <= finder.FindFirstLanguageFile(dummy));
    test(0 == finder.iFileName->CompareF(_L("a:\\abaffafg.09")));

    // Test searching all drives for any language
    // FindFirstLanguageFileAndDrive
    finder.iFilesThatExist->AppendL(_L("z:\\abcdefg.05"));
    fileName = _L("c:\\abcdefg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("az");
    test(0 <= finder.FindFirstLanguageFileAndDrive());
    test(0 == finder.iFileName->CompareF(_L("a:\\abcdefg.1000")));
    fileName = _L("c:\\abaffafg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("az");
    test(0 <= finder.FindFirstLanguageFileAndDrive());
    test(0 == finder.iFileName->CompareF(_L("a:\\abaffafg.09")));
    fileName = _L("c:\\abcdefg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("za");
    test(0 <= finder.FindFirstLanguageFileAndDrive());
    test(0 == finder.iFileName->CompareF(_L("z:\\abcdefg.05")));
    fileName = _L("c:\\xxx.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("za");
    test(KErrNotFound == finder.FindFirstLanguageFileAndDrive());
    finder.iTestScanner.iDriveError[25] = KErrAbort;
    fileName = _L("c:\\abcdefg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("za");
    test(0 <= finder.FindFirstLanguageFileAndDrive());
    test(0 == finder.iFileName->CompareF(_L("a:\\abcdefg.1000")));
    fileName = _L("c:\\abcdefg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("za");
    finder.iTestScanner.iDriveError[0] = KErrNoMemory;
    test(KErrNotFound == finder.FindFirstLanguageFileAndDrive());
    fileName = _L("c:\\abcdefg.xx");
    finder.SetFileName(fileName);
    finder.iDrives = _L("za");
    finder.iTestScanner.iDriveError[0] = KErrAbort;
    test(KErrNotFound == finder.FindFirstLanguageFileAndDrive());

    }





void CT_LANGUTILSIMP::TestLANGUTILSIMP()
    {
    __UHEAP_MARK;

    INFO_PRINTF1(_L("LangUtils Internal Tests: "));

    TestAddLanguage();
    TestMakeLanguageDowngradePath();
    TestNearestLanguageFinder();



    __UHEAP_MARKENDC(0);
    }

CT_LANGUTILSIMP::CT_LANGUTILSIMP()
    {
    SetTestStepName(KTestStep_T_LANGUTILSIMP);
    }

TVerdict CT_LANGUTILSIMP::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestLANGUTILSIMP());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

