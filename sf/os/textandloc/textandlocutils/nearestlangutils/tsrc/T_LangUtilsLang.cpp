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
#include <f32file.h>
#include <hal.h>
#include <e32std.h>
#include <langutil.h>
#include "LangUtilImpl.h"
#include "T_LangUtilsTestShareFunc.h"
#include "t_langutilslang.h"

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


LOCAL_D RFs                 TheFs;

// Our test diectory and files...

_LIT (KNullFilename, "");
_LIT (KRscFilename,  "C:\\abc.rsc");
_LIT (KRscFilenameNoSuffix,  "C:\\abc.");

_LIT (KAmericanLang, "C:\\abc.r10");
_LIT (KFrenchLang,   "C:\\abc.r02");
_LIT (KJapanEnglishLang, "C:\\abc.r160");
_LIT (KEnglishLang, "C:\\abc.r01");

LOCAL_C void DeleteTestFiles()
{
    User::LeaveIfError (TheFs.Connect ());
    ::CleanupClosePushL (TheFs);

    LangUtilsTest::DeleteFile (TheFs, KRscFilename);
    LangUtilsTest::DeleteFile (TheFs, KAmericanLang);
    LangUtilsTest::DeleteFile (TheFs, KFrenchLang);
    LangUtilsTest::DeleteFile (TheFs, KJapanEnglishLang);
    LangUtilsTest::DeleteFile (TheFs, KEnglishLang);

    CleanupStack::PopAndDestroy (&::TheFs);
}



void CT_LANGUTILSLANG::CreateTestFiles()
{
    User::LeaveIfError (TheFs.Connect ());
    ::CleanupClosePushL (TheFs);
    test (LangUtilsTest::FileExists (TheFs, KRscFilename) == EFalse);
    test (LangUtilsTest::FileExists (TheFs, KAmericanLang) == EFalse);
    test (LangUtilsTest::FileExists (TheFs, KFrenchLang) == EFalse);
    test (LangUtilsTest::FileExists (TheFs, KJapanEnglishLang) == EFalse);
    test (LangUtilsTest::FileExists (TheFs, KEnglishLang) == EFalse);

    // Create the files...
    RFile rFile;
    // KFilename1 can't be created
    rFile.Create (TheFs, KRscFilename, EFileRead);
    rFile.Close ();
    rFile.Create (TheFs, KAmericanLang, EFileRead);
    rFile.Close ();
    rFile.Create (TheFs, KFrenchLang, EFileRead);
    rFile.Close ();

    CleanupStack::PopAndDestroy (&::TheFs);

}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0070
@SYMTestCaseDesc Tests the configuration of the ideal language.
@SYMTestPriority High
@SYMTestActions  Set the ideal language and look for nearest language files.
@SYMTestExpectedResults The test must not fail.
@SYMREQ 3770: Configurable "ideal language" in LangUtil::NearestLanguageFile.
*/
void CT_LANGUTILSLANG::TestIdeaLang()
    {
    INFO_PRINTF1 (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-0070 TestIdeaLang "));

    __UHEAP_MARK;

    User::LeaveIfError (TheFs.Connect ());
    ::CleanupClosePushL (TheFs);

    TBuf <256> filename;
    TLanguage language;

    // Test ideal language code

    // Check initial value of ideal language

    language=LangUtil::IdealLanguage();
    test(language == ELangNone);

    // Set and read back the ideal language

    LangUtil::SetIdealLanguage( ELangGerman);
    language=LangUtil::IdealLanguage();
    test(language == ELangGerman);

    LangUtil::SetIdealLanguage( ELangAmerican);
    language=LangUtil::IdealLanguage();
    test(language == ELangAmerican);

    // Test NearestLanguageFile with empty name
    TFileName resPath;
    LangUtil::NearestLanguageFile (TheFs, resPath);
    test(resPath.Length()==0);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for the null file - %S - is - %S -\n"), &KNullFilename, &filename);

    // Test NearestLanguageFile with no suffix
    filename.Copy (KRscFilenameNoSuffix);
    LangUtil::NearestLanguageFile (TheFs, filename);
    test(filename==KRscFilenameNoSuffix);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoSuffix, &filename);

    // Test NearestLanguageFile
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KAmericanLang);

    // Set ideal language to one we don't have
    LangUtil::SetIdealLanguage( ELangGerman);
    language=LangUtil::IdealLanguage();
    test(language == ELangGerman);

    // Test NearestLanguageFile
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KRscFilename);

    // Restore ideal language to default
    LangUtil::SetIdealLanguage( ELangNone);
    language=LangUtil::IdealLanguage();
    test(language == ELangNone);

    // Test NearestLanguageFile
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KRscFilename);

    // Set ideal language to French

    LangUtil::SetIdealLanguage( ELangFrench);
    language=LangUtil::IdealLanguage();
    test(language == ELangFrench);

    // Test NearestLanguageFile
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KFrenchLang);

    // Restore ideal language to default
    LangUtil::SetIdealLanguage( ELangNone);
    language=LangUtil::IdealLanguage();
    test(language == ELangNone);

    // Release ideal language - free's TLS

    LangUtil::ReleaseIdealLanguage();

    CleanupStack::PopAndDestroy (&::TheFs);

    __UHEAP_MARKEND;
    }

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0071
@SYMTestCaseDesc Tests the overload of LangUtil::NearestLanguageFile.
@SYMTestPriority High
@SYMTestActions  Calls the overload of LangUtil::NearestLanguageFile and checks the returned language.
@SYMTestExpectedResults The test must not fail.
@SYMREQ 3785: Overload of LangUtil::NearestLanguageFile.
*/
void CT_LANGUTILSLANG::TestNearestLang()
    {
    INFO_PRINTF1 (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-0071 TestNearestLang "));

    __UHEAP_MARK;

    User::LeaveIfError (TheFs.Connect ());
    ::CleanupClosePushL (TheFs);

    TBuf <256> filename;
    TLanguage language;

    // Ensure that ideal language is default
    LangUtil::SetIdealLanguage( ELangNone);
    language=LangUtil::IdealLanguage();
    test(language == ELangNone);

    // Test NearestLanguageFile overload with language returned
    language=ELangAmerican;
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename, language);
    INFO_PRINTF4(_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
    test(language == ELangNone);
    test(filename == KRscFilename);

    // Set ideal language to American

    LangUtil::SetIdealLanguage( ELangAmerican);
    language=LangUtil::IdealLanguage();
    test(language == ELangAmerican);

    // Test NearestLanguageFile overload with language returned
    language=ELangNone;
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename, language);
    INFO_PRINTF4 (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
    test(language == ELangAmerican);
    test(filename == KAmericanLang);

    // Set ideal language to French

    LangUtil::SetIdealLanguage( ELangFrench);
    language=LangUtil::IdealLanguage();
    test(language == ELangFrench);

    // Test NearestLanguageFile overload with language returned
    language=ELangNone;
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename, language);
    INFO_PRINTF4 (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
    test(language == ELangFrench);
    test(filename == KFrenchLang);

    // Set ideal language to one we don't have
    LangUtil::SetIdealLanguage( ELangGerman);
    language=LangUtil::IdealLanguage();
    test(language == ELangGerman);

    // Test NearestLanguageFile overload with language returned
    language=ELangAmerican;
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFile (TheFs, filename, language);
    INFO_PRINTF4 (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
    test(language == ELangNone);
    test(filename == KRscFilename);

    // Restore ideal language to default
    LangUtil::SetIdealLanguage( ELangNone);
    language=LangUtil::IdealLanguage();
    test(language == ELangNone);

    // Release ideal language - free's TLS

    LangUtil::ReleaseIdealLanguage();

    CleanupStack::PopAndDestroy (&::TheFs);

    __UHEAP_MARKEND;
    }


void CT_LANGUTILSLANG::TestLANGUTILSLANG ()
    {
    CreateTestFiles();
    TestIdeaLang ();
    TestNearestLang ();
    DeleteTestFiles();
    }

CT_LANGUTILSLANG::CT_LANGUTILSLANG()
    {
    SetTestStepName(KTestStep_T_LANGUTILSLANG);
    }

TVerdict CT_LANGUTILSLANG::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestLANGUTILSLANG());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

