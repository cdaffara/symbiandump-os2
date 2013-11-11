// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests some code internal to BaflUtils
// 
//

#include "BaUtilsImp.h"
#include <bautils.h>
#include <e32test.h>
#include <baflpan.h>

#define UNUSED_VAR(a) a = a

const TPtrC KValFolderName=_L("ValidFolderName");
const TPtrC KCurrentPath=_L("C:\\System\\Data\\");
const TPtrC KNoFolderName = _L("");
const TPtrC KInValidFolderName=_L("C:\\System\\Data\\InValid3+FollderName");

namespace
{
CTrapCleanup* TrapCleanup;
RTest test(_L("T_BaflUtilsImp - Internal Bafl function tests"));
}

class RTestDirectoryScanner : public RDirectoryScanner
	{
public:
	RTestDirectoryScanner(CDesCArray*& aMember)
		: iOpenError(0), iFilesThatExist(aMember)
		{
		for (TInt i = 0; i != 26; ++i)
			iDriveError[i] = 0;
		}
	virtual TInt Open(RFs&, const TDesC& aMatchPattern)
		{
		iMatch = aMatchPattern;
		iIndex = 0;
		return iOpenError;
		}
	virtual TInt Next(TEntry& aOut)
		{
		if (!iFilesThatExist)
			return KErrEof;
		while (iIndex < iFilesThatExist->Count())
			{
			TInt i = iIndex++;
			if (0 <= (*iFilesThatExist)[i].MatchF(iMatch))
				{
				TPtrC fullName=(*iFilesThatExist)[i];
				TParsePtrC name(fullName);
				aOut.iName=name.NameAndExt();
				if (1 < fullName.Length() && fullName[1] == ':')
					{
					TInt drive = fullName[0];
					if ('a' <= drive && drive <= 'z')
						drive -= 'a';
					else
						{
						ASSERT('A' <= drive && drive <= 'Z');
						drive -= 'A';
						}
					return iDriveError[drive];
					}
				return KErrNone;
				}
			}
		return KErrEof;
		}
	virtual void Close() {}
	TInt iOpenError;
	TInt iDriveError[26];
private:
	CDesCArray *&iFilesThatExist;
	TInt iIndex;
	TFileName iMatch;
	};

class TTestNearestLanguageFileFinder : public TNearestLanguageFileFinder
	{
public:
	TTestNearestLanguageFileFinder(const RFs& aFs)
		: TNearestLanguageFileFinder(aFs), iTestScanner(iFilesThatExist)
		{
		iFilesThatExist = 0;
		iFilesSearchedFor = 0;
		}
	~TTestNearestLanguageFileFinder()
		{
		delete iFilesThatExist;
		delete iFilesSearchedFor;
		}
	virtual TInt GetCustomResourceDriveNumber() const { return iCustomRscDrive; }
	virtual TBool FileExists(const TDesC& aFileName) const
		{
		test(aFileName[1] == ':');
		test(0 == aFileName.Left(1).CompareF(iDrivesChecked.Left(1)));
		iDrivesChecked.Delete(0, 1);
		test(0 <= aFileName.MatchF(iFileCheckedFor));
		if (iFilesSearchedFor)
			{
			test(0 == aFileName.CompareF((*iFilesSearchedFor)[0]));
			iFilesSearchedFor->Delete(0);
			}
		if (!iFilesThatExist)
			return EFalse;
		TInt i;
		iFilesThatExist->Find(aFileName, i);
		return (0 <= i && i < iFilesThatExist->Count())? ETrue : EFalse;
		}
	virtual RDirectoryScanner& DirectoryScanner() { return iTestScanner; }

	TInt iCustomRscDrive;
	CDesCArray* iFilesThatExist;
	// File that is expected to go into FileExists.
	// Can contain wildcards.
	TFileName iFileCheckedFor;
	// Drives expected in calls to FileExists. The first one is checked then
	// discarded each time FileExists is called.
	mutable TBuf<26> iDrivesChecked;
	CDesCArray* iFilesSearchedFor;
	RTestDirectoryScanner iTestScanner;
	};

void TestPath(const TLanguagePath& aPath, const TLanguage* aExpected, TInt aExpectedCount)
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
void TestAddLanguage()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0443 "));
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

struct SDowngradeTest
	{
	TLanguage iIn[4];
	TLanguage iOut[9];
	};

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0444
@SYMTestCaseDesc        Tests for the functionality of TLocale class
@SYMTestPriority        High
@SYMTestActions         Test for language downgrades
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestDowngrade(SDowngradeTest& aTest)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0444 "));
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
void TestMakeLanguageDowngradePath()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0445 "));
	for (int i = 0; i != sizeof(DowngradeData)/sizeof(DowngradeData[0]); ++i)
		TestDowngrade(DowngradeData[i]);
	}

void SetUpFinderForLanguageAndDriveSearchL(
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
void TestNearestLanguageFinder()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0446 "));
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
	finder.iCustomRscDrive = static_cast<TInt>(EDriveC);	// drive c:
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
	finder.iFilesThatExist->AppendL(_L("a:\\abcdefg.0523"));	// not well-formed. Will not count.
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

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0447
@SYMTestCaseDesc        Tests for defect number DEF039977
@SYMTestPriority        High
@SYMTestActions         Test for Panic in BaflUtils::DiskIsReadOnly()
						when unexpected parameter supplied
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestforDEF039977()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0447 "));
	TFileName filename;
	TBool readOnly;
	TInt theRes;
	RFs fileserver;
	TInt err=fileserver.Connect();
	test(err==KErrNone);

	filename=_L("c");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrBadName);

	filename=_L(" z:");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrBadName);

	filename=_L("z\\");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrBadName);

	filename=_L("b:");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrNotReady);

	filename=_L("c:");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrNone);
	test(!readOnly);

	filename=_L("z:");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrNone);
	test(readOnly);

	filename=_L("z:\\");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrNone);

	filename=_L("z:\\system\\data\\");
	theRes=BaflUtils::DiskIsReadOnly(fileserver,filename,readOnly);
	test(theRes==KErrNone);

	fileserver.Close();
	}

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-1284
@SYMTestCaseDesc        Tests for invalid character
@SYMTestPriority        High
@SYMTestActions         Test for panic when an invalid character when
                        supplied to BaflUtils::RootFolderPath's parameter as drive letter
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
TInt TestforInvalidCharacter(TAny* /*aData*/)
/**
 This is a root thread function which runs as part of the TestforDEF040198().
 It tests a panic is produced when an invalid non alphabet character is supplied
 in the aDriveLetter argument in BaflUtils::RootFolderPath(const TBuf<1> aDriveLetter)
*/
	{
	TBuf<1> buf1=_L("?");
	TFileName retValue;
	retValue=BaflUtils::RootFolderPath(buf1);

	// Line above panics if working OK, so should never get here.
	test(0);
	return 0;
	}

/**
This method is a test case to demonstrate the fix for DEF040198 by supplying an invalid
input aDriveLetter argument (e.g "?") to BaflUtils::RootFolderPath(aDriveLetter).

@SYMTestCaseID          SYSLIB-BAFL-CT-0448
@SYMTestCaseDesc        Tests for defect number DEF040198
@SYMTestPriority        High
@SYMTestActions         Check by giving an invalid aDriveLetter argument
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestforDEF040198()
	{
        test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0448 "));
//Check invalid aDriveLetter argument panics with panic code EBaflPanicBadOpenArg
#if defined(_DEBUG)
		TRequestStatus threadStatus;
		RThread thread;
		_LIT(KThreadName,"ThreadTest");
		TInt rc;
		TBool jit;
		jit=User::JustInTime();
		User::SetJustInTime(EFalse);

		test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1284 "));
		rc=thread.Create(KThreadName,TestforInvalidCharacter,KDefaultStackSize,KMinHeapSize,KMinHeapSize*4,NULL);
		test(rc==KErrNone);
		thread.Logon(threadStatus);
		thread.Resume();
		User::WaitForRequest(threadStatus);
		test (thread.ExitType()==EExitPanic);
		test (thread.ExitReason()==EBafPanicBadOpenArg);
		thread.Close();
		User::SetJustInTime(jit);
#endif
	}

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-1285
@SYMTestCaseDesc        Tests for invalid path name
@SYMTestPriority        High
@SYMTestActions         Test for panic when an invalid character supplied to BaflUtils::RootFolderPath's parameter as drive letter
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
TInt TestforInvalidPathName(TAny* /*aData*/)
/**
 This is a root thread function which runs as part of the TestforDEF040299().
 It tests a panic is produced when an invalid name is supplied
 in the aFullName argument in BaflUtils::FolderNameFromFullName(const TDesC& aFullName)
*/
	{	
	TBuf<1> buf1=_L("?");
	TFileName retValue;
	retValue=BaflUtils::FolderNameFromFullName(buf1);

	// Line above panics if working OK, so should never get here.
	test(0);
	return 0;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0449
@SYMTestCaseDesc        Tests for defect number DEF047305
@SYMTestPriority        High
@SYMTestActions         Test for correct syntax
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestForDEF047305()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0449 "));
	RFs aFs;
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TFileName newFolderFullName = KCurrentPath;

	test(User::LeaveIfError(BaflUtils::ValidateFolderNameTypedByUserL(aFs, KValFolderName, KCurrentPath, newFolderFullName)==KErrNone));
 	test(User::LeaveIfError(BaflUtils::ValidateFolderNameTypedByUserL(aFs, KNoFolderName, KCurrentPath, newFolderFullName)==KErrArgument));
 	test(User::LeaveIfError(BaflUtils::ValidateFolderNameTypedByUserL(aFs, KInValidFolderName, KCurrentPath, newFolderFullName)==KErrBadName));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0450
@SYMTestCaseDesc        Tests for defect number DEF040299
@SYMTestPriority        High
@SYMTestActions         Test by supplying an invalid input aFullName argument to BaflUtils::FolderNameFromFullName(aFullName)
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestforDEF040299()
	{
    test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0450 "));
	TBuf<30> buf;
	TFileName fileName;

	//Path name="c:"
	buf=_L("c:");
	fileName=BaflUtils::FolderNameFromFullName(buf);
	test(buf==fileName);

	//Path name="c:\\"
	buf=_L("c:\\");
	fileName=BaflUtils::FolderNameFromFullName(buf);
	test(fileName==buf);

	//Path name="c:\\file.txt"
	buf=_L("c:\\file.txt");
	fileName=BaflUtils::FolderNameFromFullName(buf);
	test(fileName==_L("c:\\"));

	//Path name="c:\\system\\file.txt"
	buf=_L("c:\\system\\file.txt");
	fileName=BaflUtils::FolderNameFromFullName(buf);
	test(fileName==_L("system"));

	/**
	This method is a test case to demonstrate the fix for DEF040299 by supplying an invalid
	input aFullName argument (e.g "?") to BaflUtils::FolderNameFromFullName(aFullName).
	*/
	//Check invalid aFullName argument panics with panic code EBaflPanicBadOpenArg
#if defined(_DEBUG)
		TRequestStatus threadStatus;
		RThread thread;
		//Tread name is changed due to test failing on slow machine
		//Using "TreadTest" here are causing KErrAlreadyExist at tread creating stage.
		//This is because the previous tread maybe still being closed when this thread is created.
		_LIT(KThreadName,"ThreadTest1");
		TInt rc;
		TBool jit;
		jit=User::JustInTime();
		User::SetJustInTime(EFalse);
		test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1285 "));
rc=thread.Create(KThreadName,TestforInvalidPathName,KDefaultStackSize,KMinHeapSize,KMinHeapSize*4,NULL);
		test(rc==KErrNone);
		thread.Logon(threadStatus);
		thread.Resume();
		User::WaitForRequest(threadStatus);
		test (thread.ExitType()==EExitPanic);
		test (thread.ExitReason()==EBafPanicBadOpenArg);
		thread.Close();
		User::SetJustInTime(jit);
#endif

	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-3416
@SYMTestCaseDesc        Tests for defect number DEF101927
@SYMTestPriority        High
@SYMTestActions         Test the simulated Tls functionalities Dll::Tls, Dll::SetTls and Dll::FreeTls
						which is needed for BAUtils.cpp to be complied as part of an .exe.
						Dll::Tls - Recalls the value currently referred to by the Tls pointer
						Dll::SetTls - Sets the the values referred to by the Tls pointer
						Dll::FreeTls - Sets the Tls pointer to NULL
@SYMTestExpectedResults Pass if the TLanguage values is returned as expected after calls to IdealLanguage,
						SetIdealLanguage and ReleaseIdealLanguage.
@SYMDEF                 DEF101927
*/
void TestForDEF101927()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3416 "));
	TLanguage test_language(ELangAmerican);

	test(BaflUtils::IdealLanguage()==ELangNone);

	BaflUtils::SetIdealLanguage(ELangAmerican);
	test(test_language == ELangAmerican);

	test_language = ELangNone;
	test_language = BaflUtils::IdealLanguage();
	test(test_language == ELangAmerican);

	BaflUtils::ReleaseIdealLanguage();
	test_language = BaflUtils::IdealLanguage();
	test(test_language == ELangNone);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-3667
@SYMTestCaseDesc		Tests for defect number DEF109928
@SYMTestPriority		Normal
@SYMTestActions         Test for the extended NextLanguage(). Check if the MakeLanguageDowngradePath()
						can now return the correct default fallback for the languages which are newly
						added into the NextLanguage().
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109928
*/
void TestForDEF109928()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-3667 "));
	TLanguage testDeviceLangs[8] = {ELangEnglish_Japan, ELangSwissFrench, ELangSwissGerman,
									ELangLatinAmericanSpanish, ELangSwissItalian, ELangFinlandSwedish,
									ELangMalay_Apac, ELangBrazilianPortuguese};
	TLanguagePath testOutputPath[8] = {{ELangEnglish_Japan, ELangEnglish}, {ELangSwissFrench, ELangFrench},
				{ELangSwissGerman, ELangGerman}, {ELangLatinAmericanSpanish, ELangSpanish},
				{ELangSwissItalian, ELangItalian}, {ELangFinlandSwedish, ELangSwedish},
				{ELangMalay_Apac, ELangMalay}, {ELangBrazilianPortuguese, ELangPortuguese}};
	TLocale loc;
	loc.SetLanguageDowngrade(0, ELangNone);
	loc.SetLanguageDowngrade(1, ELangNone);
	loc.SetLanguageDowngrade(2, ELangNone);
	TLanguagePath path;
	TLanguage testlang;

	for(TInt i = 0; i < 8 ; i++)
		{
		testlang = testDeviceLangs[i];
		MakeLanguageDowngradePath(path, testlang, ELangNone, loc);
		test(path[0] == testOutputPath[i][0]);
		test(path[1] == testOutputPath[i][1]);
		test(path[2] == ELangNone);
		}
	}

/* The following 3 function is defined so that the functions IdealLanguage,
SetIdealLanguage, ReleaseIdealLanguage will compile as part as an .exe
These functions use a global TAny* to store the data that would have been
stored in the TLS under normal compilation (ie If the BaUtils was compilled
as part of a DLL).
*/

TAny* ptr(NULL);

TAny* Dll::Tls()
	{
	return ptr;
	}

TInt Dll::SetTls(TAny* aPtr)
	{
	ptr = aPtr;
	return KErrNone;
	}

void Dll::FreeTls()
	{
	ptr = NULL;
	}

void RunTestsL()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("BaflUtils Internal Tests: "));

	TestAddLanguage();
	TestMakeLanguageDowngradePath();
	TestNearestLanguageFinder();
	TestforDEF039977();
	TestforDEF040198();
	TestforDEF040299();
	TestForDEF047305();
	TestForDEF101927();
	TestForDEF109928();
	test.End();
	test.Close();

	__UHEAP_MARKENDC(0);
	}

TInt E32Main()
	{
	TrapCleanup = CTrapCleanup::New();
	TRAPD(err, RunTestsL());
    test(err == KErrNone);
	delete TrapCleanup;
	return 0;
	}
