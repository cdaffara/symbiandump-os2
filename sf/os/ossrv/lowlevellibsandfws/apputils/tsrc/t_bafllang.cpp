// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32test.h>
#include <bautils.h>
#include <hal.h>
#include <e32std.h>

LOCAL_D CTrapCleanup*		TheTrapCleanup=NULL;

LOCAL_D RTest				TheTest (_L("T_BaflLang"));
LOCAL_D RFs					TheFs;

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

	BaflUtils::DeleteFile (TheFs, KRscFilename);
	BaflUtils::DeleteFile (TheFs, KAmericanLang);
	BaflUtils::DeleteFile (TheFs, KFrenchLang);
	BaflUtils::DeleteFile (TheFs, KJapanEnglishLang);
	BaflUtils::DeleteFile (TheFs, KEnglishLang);

	CleanupStack::PopAndDestroy (&::TheFs);
}

//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)

LOCAL_C void CreateTestFiles()
{
	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);
	TEST (BaflUtils::FileExists (TheFs, KRscFilename) == EFalse);
	TEST (BaflUtils::FileExists (TheFs, KAmericanLang) == EFalse);
	TEST (BaflUtils::FileExists (TheFs, KFrenchLang) == EFalse);
	TEST (BaflUtils::FileExists (TheFs, KJapanEnglishLang) == EFalse);
	TEST (BaflUtils::FileExists (TheFs, KEnglishLang) == EFalse);

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
@SYMREQ 3770: Configurable "ideal language" in BaflUtils::NearestLanguageFile.
*/
void REQ3770L()
	{
    TheTest.Next (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-0070 REQ3770L "));

    __UHEAP_MARK;

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);

	TBuf <256> filename;
	TLanguage language;

	// Test ideal language code

	// Check initial value of ideal language

	language=BaflUtils::IdealLanguage();
	TEST(language == ELangNone);

	// Set and read back the ideal language

	BaflUtils::SetIdealLanguage( ELangGerman);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangGerman);

	BaflUtils::SetIdealLanguage( ELangAmerican);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangAmerican);

	// Test NearestLanguageFile with empty name
	TFileName resPath;
	BaflUtils::NearestLanguageFile (TheFs, resPath);
	TEST(resPath.Length()==0);
	TheTest.Printf (_L ("The NearestLanguageFile for the null file - %S - is - %S -\n"), &KNullFilename, &filename);

	// Test NearestLanguageFile with no suffix
	filename.Copy (KRscFilenameNoSuffix);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TEST(filename==KRscFilenameNoSuffix);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoSuffix, &filename);

	// Test NearestLanguageFile
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KAmericanLang);

	// Set ideal language to one we don't have
	BaflUtils::SetIdealLanguage( ELangGerman);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangGerman);

	// Test NearestLanguageFile
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KRscFilename);

	// Restore ideal language to default
	BaflUtils::SetIdealLanguage( ELangNone);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangNone);

	// Test NearestLanguageFile
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KRscFilename);

	// Set ideal language to French

	BaflUtils::SetIdealLanguage( ELangFrench);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangFrench);

	// Test NearestLanguageFile
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KFrenchLang);

	// Restore ideal language to default
	BaflUtils::SetIdealLanguage( ELangNone);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangNone);

	// Release ideal language - free's TLS

	BaflUtils::ReleaseIdealLanguage();

	CleanupStack::PopAndDestroy (&::TheFs);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0071
@SYMTestCaseDesc Tests the overload of BaflUtils::NearestLanguageFile.
@SYMTestPriority High
@SYMTestActions  Calls the overload of BaflUtils::NearestLanguageFile and checks the returned language.
@SYMTestExpectedResults The test must not fail.
@SYMREQ 3785: Overload of BaflUtils::NearestLanguageFile.
*/
void REQ3785L()
	{
    TheTest.Next (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-0071 REQ3785L "));

    __UHEAP_MARK;

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);

	TBuf <256> filename;
	TLanguage language;

	// Ensure that ideal language is default
	BaflUtils::SetIdealLanguage( ELangNone);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangNone);

	// Test NearestLanguageFile overload with language returned
	language=ELangAmerican;
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename, language);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
	TEST(language == ELangNone);
	TEST(filename == KRscFilename);

	// Set ideal language to American

	BaflUtils::SetIdealLanguage( ELangAmerican);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangAmerican);

	// Test NearestLanguageFile overload with language returned
	language=ELangNone;
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename, language);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
	TEST(language == ELangAmerican);
	TEST(filename == KAmericanLang);

	// Set ideal language to French

	BaflUtils::SetIdealLanguage( ELangFrench);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangFrench);

	// Test NearestLanguageFile overload with language returned
	language=ELangNone;
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename, language);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
	TEST(language == ELangFrench);
	TEST(filename == KFrenchLang);

	// Set ideal language to one we don't have
	BaflUtils::SetIdealLanguage( ELangGerman);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangGerman);

	// Test NearestLanguageFile overload with language returned
	language=ELangAmerican;
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, filename, language);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S, language = %d\n"), &KRscFilename, &filename, language);
	TEST(language == ELangNone);
	TEST(filename == KRscFilename);

	// Restore ideal language to default
	BaflUtils::SetIdealLanguage( ELangNone);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangNone);

	// Release ideal language - free's TLS

	BaflUtils::ReleaseIdealLanguage();

	CleanupStack::PopAndDestroy (&::TheFs);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-3158
@SYMTestCaseDesc Test the downgrade path from ELangMalay_Apac to ELangMalay
@SYMTestPriority High
@SYMTestActions Calls the BaflUtils::GetDowngradePathL function to check that ELangMalay_Apac downgrades to ELangMalay
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF094490
*/
void DEF094490L()
	{
    TheTest.Next (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-3158 DEF094490L "));

	__UHEAP_MARK;

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);
	RArray<TLanguage> aLanguageArray;

	//Test GetDowngradePathL to see if ELangMalay is included in the path
	BaflUtils::GetDowngradePathL(TheFs,ELangMalay_Apac,aLanguageArray);
	TEST(aLanguageArray.Find(ELangMalay)!= KErrNotFound);

	aLanguageArray.Close();

	CleanupStack::PopAndDestroy (&::TheFs);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-3359
@SYMTestCaseDesc Test correct and incorrect usage of NearestLanguageFile as documented in the developer library
@SYMTestPriority High
@SYMTestActions Calls the BaflUtils::NearestLanguageFile function with different contructed
language downgrade path cases along with different input paths
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF096421
*/
void DEF096421L()
	{
    TheTest.Next (_L (" @SYMTestCaseID SYSLIB-BAFL-CT-3359 DEF096421 "));
	__UHEAP_MARK;

	TBuf <256> path;
	TLanguage language;
	RFile rFile;

	//number of characters in the suffix is less than KInvNameAndMinSuffixLength(2)
	_LIT (KInvalidSuffix1, "C:\\abc.r");

	//number of digits in the suffix is more than KMaxSuffixLength(5)
	_LIT (KInvalidSuffix2, "C:\\abc.r123456");

	//no proceeding alphabetical characters in the suffix
	_LIT (KInvalidSuffix3, "C:\\abc.10");

	//no drive letters supplied in the path
	_LIT (KRscFilenameNoDrive, "\\abc.rsc");

	_LIT (KRscFileInZDrive, "\\system\\data\\T_CALYPSO_TEST_RESOURCE_FILE_1.RSC");
	_LIT (KRScFullFile, "Z:\\system\\data\\T_CALYPSO_TEST_RESOURCE_FILE_1.RSC");

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);

	rFile.Create (TheFs, KJapanEnglishLang, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KEnglishLang, EFileRead);
	rFile.Close ();

	BaflUtils::SetIdealLanguage( ELangAmerican);
	language=BaflUtils::IdealLanguage();
	TEST(language == ELangAmerican);

	//Test match for the ideal language
	path.Copy(KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KAmericanLang);
	TEST(language == ELangAmerican);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &path);

	//Test match when supplied with the language file
	path.Copy(KAmericanLang);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KAmericanLang);
	TEST(language == ELangAmerican);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KAmericanLang, &path);

	//Test when the no. of characters in the suffix is less than KInvNameAndMinSuffixLength(2)
	path.Copy(KInvalidSuffix1);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KInvalidSuffix1);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KInvalidSuffix1, &path);

	//Test when filename ends with a period
	path.Copy(KRscFilenameNoSuffix);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KRscFilenameNoSuffix);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoSuffix, &path);

	//Test when the no. of digits in the suffix is more than KMaxSuffixLength(5)
	path.Copy(KInvalidSuffix2);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KInvalidSuffix2);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KInvalidSuffix2, &path);

	//Test when theres no proceeding alphabetical characters in the suffix
	path.Copy(KInvalidSuffix3);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KInvalidSuffix3);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KInvalidSuffix3, &path);

	//Test when no drive letter is supplied in the path and theres no corresponding file in Z:
	path.Copy(KRscFilenameNoDrive);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KRscFilenameNoDrive);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoDrive, &path);

	BaflUtils::SetIdealLanguage(ELangEnglish_Japan);
	TheTest.Printf(_L("The ideal langauge set to ELangEnglish_Japan\n"));

	//Test when the matching language file is 3 digits long
	path.Copy(KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KJapanEnglishLang);
	TEST(language == ELangEnglish_Japan);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &path);

	BaflUtils::DeleteFile (TheFs, KJapanEnglishLang);
	TheTest.Printf(_L("Deleting C:\\abc.r160\n"));

	//Test match for the current locale (2nd best match)
	path.Copy(KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KEnglishLang);
	TEST(language == ELangEnglish);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &path);

	BaflUtils::DeleteFile (TheFs, KEnglishLang);
	TheTest.Printf(_L("Deleting C:\\abc.r01\n"));

	//Test when theres no matching language file
	path.Copy(KRscFilename);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KRscFilename);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &path);

	//Test when no drive letter is supplied in the
	//path but theres a corresponding file in Z:
	path.Copy(KRscFileInZDrive);
	BaflUtils::NearestLanguageFile (TheFs, path, language);
	TEST(path == KRScFullFile);
	TEST(language == ELangNone);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFileInZDrive, &path);

	CleanupStack::PopAndDestroy (&::TheFs);

	BaflUtils::ReleaseIdealLanguage();
	__UHEAP_MARKEND;

	}
//===============================================================================

/**
Initialise the cleanup stack
*/
LOCAL_C void SetupL ()
    {
	TheTrapCleanup = CTrapCleanup::New ();
	User::LeaveIfNull (TheTrapCleanup);
	}

/**
Cleanup
*/
LOCAL_C void CleanupL ()
    {
	delete TheTrapCleanup;
	}

/**
Invoke the tests
*/
LOCAL_C void DoTestsL ()
    {
    CreateTestFiles();
	REQ3770L ();
	REQ3785L ();
	DEF094490L();
	DEF096421L();
	DeleteTestFiles();
	}


GLDEF_C TInt E32Main ()
	{
	__UHEAP_MARK;

	TheTest.Printf (_L ("\n"));
	TheTest.Title ();
	TheTest.Start (_L("Ideal language Tests "));

	TRAPD (err, SetupL ());
	TheTest (err == KErrNone);

	TRAP (err, DoTestsL ());
	TheTest (err == KErrNone);

	CleanupL ();

	TheTest.End ();
	TheTest.Close ();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
