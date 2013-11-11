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


#include <baclipb.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>
#include <e32lang.h>
#include <langutil.h>
#include "LangUtilImpl.h"
#include "T_LangUtilsTestShareFunc.h"
#include "t_langutils.h"



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

// File names for copying

const TPtrC KSystem1Folder              =_L("C:\\System1\\");
const TPtrC KSystem1Data1Folder         =_L("C:\\System1\\data1\\");

const TPtrC KSystem1Data1File1          =_L("C:\\System1\\data1\\file1.txt");
const TPtrC KFile1                      =_L("C:\\file1.txt");
const TPtrC KSystem1Data1SrcFile1       =_L("C:\\System1\\data1\\src\\file1.txt");
const TPtrC KSystem1Data1DstFile2       =_L("C:\\System1\\data1\\dst\\file2.txt");
const TPtrC KSystem1Data1File123456789  =_L("C:\\System1\\data1\\file123456789.txt");
const TPtrC KSystem1Data1File2          =_L("C:\\System1\\data1\\file2.txt");
const TPtrC KSystem1Data1File5          =_L("C:\\System1\\data1\\file5.txt");
const TPtrC KSystem1Data1SrcFolder      =_L("C:\\System1\\data1\\src\\");
const TPtrC KSystem1Data1DstFolder      =_L("C:\\System1\\data1\\dst\\");

const TPtrC KZFile1                     =_L("Z:\\file1.txt");



///////////////////////////////////////////////////////////////////////////////////////
//LangUtil::GetEquivalentLanguageList
//LangUtil::NearestLanguageFileV2 
//test data
_LIT (KNullFilename, "");
_LIT (KRscFilename,  "C:\\geltest.rsc");
_LIT (KRscFilenameNoSuffix,  "C:\\geltest.");

_LIT (KAmericanLang, "C:\\geltest.r10");
_LIT (KFrenchLang,   "C:\\geltest.r02");
_LIT (KJapanEnglishLang, "C:\\geltest.r160");
_LIT (KEnglishLang, "C:\\geltest.r01");
_LIT (KInvalidDriveLetterV2, "q:\\geltest.rsc");
_LIT (KNoSuchLangFile, "c:\\geltest.r54");


//Delete "aFullName" file.
void CT_LANGUTILS::DeleteTestFile(const TDesC& aFullName)
    {
    RFs fsSession;
    TInt err = fsSession.Connect();
    if(err == KErrNone)
        {
        TEntry entry;
        if(fsSession.Entry(aFullName, entry) == KErrNone)
            {
            INFO_PRINTF2(_L("Deleting \"%S\" file.\n"), &aFullName);
            err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
            if(err != KErrNone)
                {
                INFO_PRINTF3(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
                }
            err = fsSession.Delete(aFullName);
            if(err != KErrNone)
                {
                INFO_PRINTF3(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
                }
            }
        fsSession.Close();
        }
    else
        {
        INFO_PRINTF3(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
        }
    }

//Delete "aFullName" dir.
void CT_LANGUTILS::DeleteTestDir(const TDesC& aFullName)
    {
    RFs fsSession;
    TInt err = fsSession.Connect();
    if(err == KErrNone)
        {
        TEntry entry;
        if(fsSession.Entry(aFullName, entry) == KErrNone)
            {
            INFO_PRINTF2(_L("Deleting \"%S\" dir.\n"), &aFullName);
            err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
            if(err != KErrNone)
                {
                INFO_PRINTF3(_L("Error %d changing \"%S\" dir attributes.\n"), err, &aFullName);
                }
            err = fsSession.RmDir(aFullName);
            if(err != KErrNone)
                {
                INFO_PRINTF3(_L("Error %d deleting \"%S\" dir.\n"), err, &aFullName);
                }
            }
        fsSession.Close();
        }
    else
        {
        INFO_PRINTF3(_L("Error %d connecting file session. Dir: %S.\n"), err, &aFullName);
        }
    }

//Delete data files used by the test
void CT_LANGUTILS::DeleteTestFiles()
    {
        DeleteTestFile(KSystem1Data1File1);
        DeleteTestFile(KFile1);
        DeleteTestFile(KSystem1Data1SrcFile1);
        DeleteTestFile(KSystem1Data1DstFile2);
        DeleteTestFile(KSystem1Data1File123456789);
        DeleteTestFile(KSystem1Data1File2);
        DeleteTestFile(KSystem1Data1File5);
        DeleteTestDir(KSystem1Data1SrcFolder);
        DeleteTestDir(KSystem1Data1DstFolder);
        DeleteTestDir(KSystem1Data1Folder);
        DeleteTestDir(KSystem1Folder);
    }




void CT_LANGUTILS::DeleteTestFiles2(RFs &aFs)
{
    LangUtilsTest::DeleteFile (aFs, KRscFilename);
    LangUtilsTest::DeleteFile (aFs, KAmericanLang);
    LangUtilsTest::DeleteFile (aFs, KFrenchLang);
    LangUtilsTest::DeleteFile (aFs, KJapanEnglishLang);
    LangUtilsTest::DeleteFile (aFs, KEnglishLang);
}

void CT_LANGUTILS::CreateTestFiles2(RFs &aFs)
{
    test(LangUtilsTest::FileExists (aFs, KRscFilename) == EFalse);
    test(LangUtilsTest::FileExists (aFs, KAmericanLang) == EFalse);
    test(LangUtilsTest::FileExists (aFs, KFrenchLang) == EFalse);
    test(LangUtilsTest::FileExists (aFs, KJapanEnglishLang) == EFalse);
    test(LangUtilsTest::FileExists (aFs, KEnglishLang) == EFalse);

    // Create the files...
    RFile rFile;
    test(KErrNone == rFile.Create (aFs, KRscFilename, EFileRead));
    rFile.Close ();
    test(KErrNone == rFile.Create (aFs, KAmericanLang, EFileRead));
    rFile.Close ();
    test (KErrNone == rFile.Create (aFs, KFrenchLang, EFileRead));
    rFile.Close ();
    test(KErrNone == rFile.Create (aFs, KEnglishLang, EFileRead));
    rFile.Close ();
}
///////////////////////////////////////////////////////////////////////////////////////
//Copy test file from Z: to C: drive.

void CT_LANGUTILS::CopyTestFiles()
    {
    RFs fsSession;
    TInt err = fsSession.Connect();
    if(err == KErrNone)
        {
        User::LeaveIfError(fsSession.MkDir(KSystem1Folder));
        User::LeaveIfError(fsSession.MkDir(KSystem1Data1Folder));
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1File1) == KErrNone);
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KFile1) == KErrNone);
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1File123456789) == KErrNone);
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1File2) == KErrNone);
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1File5) == KErrNone);
        User::LeaveIfError(fsSession.MkDir(KSystem1Data1SrcFolder));
        User::LeaveIfError(fsSession.MkDir(KSystem1Data1DstFolder));
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1SrcFile1) == KErrNone);
        test( LangUtilsTest::CopyFile(fsSession, KZFile1, KSystem1Data1DstFile2) == KErrNone);

        fsSession.Close();
        }
    else
        {
        INFO_PRINTF2(_L("Error %d connecting file session.\n"), err);
        }
    }





/**
@SYMTestCaseID TI18N-BAFL-CT-4003
@SYMTestCaseDesc Tests Baflutils::TestNearestLanguageFileV2.
@SYMTestPriority Medium
@SYMTestActions Pass in various resource file names and change system locale setting, check the return values.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 2525 Install device equivalent languages from SIS files
*/
void CT_LANGUTILS::TestNearestLanguageFileV2(RFs& aFs)
    {
    /* test case ID to be added */
    INFO_PRINTF1(_L (" @SYMTestCaseID TI18N-BAFL-CT-4003 "));
    
    __UHEAP_MARK;

    CleanupClosePushL (aFs);
    User::LeaveIfError (aFs.Connect ());
    
    DeleteTestFiles2(aFs);
    CreateTestFiles2(aFs);

    TBuf <256> filename;
    TLanguage lang = ELangNone;
    INFO_PRINTF2 (_L("lang initial value is %d\n"), lang);

    // Test NearestLanguageFile with empty name
    TFileName resPath;
    LangUtil::NearestLanguageFileV2(aFs, resPath, lang);
    test(resPath.Length()==0);
    INFO_PRINTF2 (_L("lang returned by NLFV2 is %d\n"), lang);
    test(ELangNone == lang);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for the null file - %S - is - %S -\n"), &KNullFilename, &filename);
    
    // Test NearestLanguageFile with invalid drive letter
    filename.Copy (KInvalidDriveLetterV2);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    //test(filename==KRscFilenameNoSuffix);
    INFO_PRINTF4 (_L ("The NearestLanguageFile for %S is %S, lang is %d\n"), &KInvalidDriveLetterV2, &filename, lang);
    //test(ELangNone == lang);
    
    // Test NearestLanguageFile with incorrect ext
    DeleteTestFiles2(aFs);
    RFile rFile;
    test (KErrNone == rFile.Create (aFs, KFrenchLang, EFileRead));
    rFile.Close ();
    filename.Copy (KNoSuchLangFile);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    //test(filename==KRscFilenameNoSuffix);
    INFO_PRINTF4 (_L ("The NearestLanguageFile for %S is %S, lang is %d\n"), &KNoSuchLangFile, &filename, lang);
    //test(ELangNone == lang);
    LangUtilsTest::DeleteFile (aFs, KFrenchLang);
    CreateTestFiles2(aFs);
        
    // Test NearestLanguageFile with no suffix
    filename.Copy (KRscFilenameNoSuffix);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    test(filename==KRscFilenameNoSuffix);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoSuffix, &filename);
    test(ELangNone == lang);
    
    // change locale to American
	/*
    TExtendedLocale loc;
    loc.LoadSystemSettings();
    TInt ret = loc.LoadLocaleAspect(_L ("elocl_lan.010"));
    INFO_PRINTF2(_L("LoadLocale returns %d\n"), ret);
    test(KErrNone == ret);
    ret = loc.SaveSystemSettings();
    test(KErrNone == ret);
    
    // Test NearestLanguageFile: AmE is supported
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    //test(filename == KEnglishLang);
    test(filename == KAmericanLang);
    test(lang == ELangAmerican);

    // Delete geltest.r10, AmE downgrades to En_GB.
    LangUtilsTest::DeleteFile (aFs, KAmericanLang);
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KEnglishLang);
    test(lang == ELangEnglish);
    
    
    // Delete geltest.r01, AmE downgrades to en_GB, to ELangNone.
    LangUtilsTest::DeleteFile (aFs, KEnglishLang);
    filename.Copy (KRscFilename);
    LangUtil::NearestLanguageFileV2(aFs, filename, lang);
    INFO_PRINTF3 (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
    test(filename == KRscFilename); // filename remains unchanged.
    test(lang == ELangNone);

    // restore locale settings
    INFO_PRINTF1(_L("Restoring locale settings.\n"));
    loc.LoadSystemSettings();
    ret = loc.LoadLocaleAspect(_L("elocl_lan.001"));
    test(KErrNone == ret);
    ret = loc.SaveSystemSettings();
    test(KErrNone == ret);
	*/
    
    CleanupStack::PopAndDestroy (&aFs);

    __UHEAP_MARKEND;
    }

/**
@SYMTestCaseID TI18N-BAFL-CT-4004
@SYMTestCaseDesc Tests Baflutils::TestGetEquivalentLanguageList.
@SYMTestPriority Medium
@SYMTestActions Pass in various language ID's and check the returned list.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 2525 Install device equivalent languages from SIS files
*/
void CT_LANGUTILS::TestGetEquivalentLanguageList()
    {
    /* test case ID to be added */
    INFO_PRINTF1 (_L (" @SYMTestCaseID TI18N-BAFL-CT-4004 "));
    
    __UHEAP_MARK;
    
    TLanguagePath lp;
    // Test GetNearestLanguageFile();
    // 1- There is an entry in the table for the given language
    LangUtil::GetEquivalentLanguageList(ELangCyprusGreek, lp);
    test(ELangCyprusGreek == lp[0]);
    test(ELangGreek == lp[1]);
    test(ELangNone == lp[2]);
    
    // 2- There is no entry for the given language
    LangUtil::GetEquivalentLanguageList(ELangFarsi, lp);
    test(ELangFarsi == lp[0]);
    test(ELangNone == lp[1]);

    __UHEAP_MARKEND;
    }

/**
@SYMTestCaseID TI18N-BAFL-CT-4004
@SYMTestCaseDesc Tests Baflutils::TestGetDowngradePathL.
@SYMTestPriority Medium
@SYMTestActions Pass in various language ID's and check the returned list.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 2525 Install device equivalent languages from SIS files
*/
void CT_LANGUTILS::TestGetDowngradePathL()
    {
    RFs fsSession;
    TInt err = fsSession.Connect();
    if(err == KErrNone)
        {
        TLanguage currentLang = ELangPrcChinese;
        RArray<TLanguage> languageArray;
        LangUtil::GetDowngradePathL(fsSession, currentLang,languageArray);
        fsSession.Close();
        test(ELangPrcChinese == languageArray[0]);
        test(1 == languageArray.Count());
        languageArray.Close();
        }
    else
        {
        INFO_PRINTF2(_L("Error %d connecting file session. "), err);
        }
    }

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038
@SYMTestCaseDesc Tests Baflutils.
@SYMTestPriority Medium
@SYMTestActions Executes all T_baflutils tests.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void CT_LANGUTILS::TestLANGUTILS()
    {
    RFs fs;

    CopyTestFiles();
    TestNearestLanguageFileV2(fs);
    TestGetEquivalentLanguageList();
    TestGetDowngradePathL();

    DeleteTestFiles();
    }

CT_LANGUTILS::CT_LANGUTILS()
    {
    SetTestStepName(KTestStep_T_LANGUTILS);
    }

TVerdict CT_LANGUTILS::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestLANGUTILS());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

