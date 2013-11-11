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

#include <e32test.h>
#include <baclipb.h>
#include <bautils.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>
#include <e32lang.h>

#include <fbs.h>


LOCAL_D RTest theTest(_L("T_BaflUtils"));

const TInt KMaxDisplayedFullNameLen	= 29;

// File names for copying

const TPtrC KSystem1Folder				=_L("C:\\System1\\");
const TPtrC KSystem1Data1Folder			=_L("C:\\System1\\data1\\");

const TPtrC KSystem1Data1File1			=_L("C:\\System1\\data1\\file1.txt");
const TPtrC KFile1						=_L("C:\\file1.txt");
const TPtrC KSystem1Data1SrcFile1		=_L("C:\\System1\\data1\\src\\file1.txt");
const TPtrC KSystem1Data1DstFile2		=_L("C:\\System1\\data1\\dst\\file2.txt");
const TPtrC KSystem1Data1File123456789	=_L("C:\\System1\\data1\\file123456789.txt");
const TPtrC KSystem1Data1File2			=_L("C:\\System1\\data1\\file2.txt");
const TPtrC KSystem1Data1File5			=_L("C:\\System1\\data1\\file5.txt");
const TPtrC KSystem1Data1SrcFolder		=_L("C:\\System1\\data1\\src\\");
const TPtrC KSystem1Data1DstFolder		=_L("C:\\System1\\data1\\dst\\");

const TPtrC KZFile1						=_L("Z:\\file1.txt");

// Valid Paths / Folder Name
const TPtrC KValidPathName=_L("C:\\System1\\Data1\\");
const TPtrC KValidPathNameWithoutDrive=_L("\\System1\\Data1\\");
const TPtrC KValidPathNameWithoutDrive2=_L("\\System1\\Data1\\file1.txt");
const TPtrC KValidFileName=_L("C:\\file1.txt");
const TPtrC KValidFileNameNoExtension=_L("C:\\file1");
const TPtrC KValidPathAndFileName=_L("C:\\System1\\Data1\\file1.txt");
const TPtrC KValidFolderName=_L("C:\\System1\\Data1\\Dst\\");
const TPtrC KValidDrive=_L("C:\\");
const TPtrC KValidDrive2=_L("C:");
const TPtrC KValidDrive3=_L("C");
const TPtrC KValidDrive4=_L("Z:\\");
const TPtrC KCopyFileFrom=_L("C:\\System1\\Data1\\Src\\file1.txt");
const TPtrC KCopyFileTo=_L("C:\\System1\\Data1\\Dst\\file1.txt");
const TPtrC KDeleteFile=_L("C:\\System1\\Data1\\Dst\\file1.txt");
const TPtrC KRenameFromFile=_L("C:\\System1\\Data1\\file2.txt");
const TPtrC KRenameToFile=_L("C:\\System1\\Data1\\file2New.txt");
const TPtrC KValFolderName=_L("ValidFolderName");
const TPtrC KCurrentPath=_L("C:\\System1\\Data1\\");
const TPtrC KNoFolderName = _L("");
const TPtrC KSessionRoot = _L("\\");

// KFolderNameIsTooLong is 256 chars long
const TPtrC KFolderNameIsTooLong = _L("C:\\ThisFolderNameIsgoingToBeTooLongToBeCreatedOnTheSystemThisFolderNameIsgoingToBeTooLongToBeCreatedOnTheSystemThisFolderNameIsgoingToBeTooLongToBeCreatedOnTheSystemThisFolderNameIsgoingToBeTooLongToBeCreatedOnTheSystemThisFolderNameIsgoingToBeTooLongToBeC\\");
const TPtrC KAbbreviatesFileFrom=_L("C:\\System1\\Data1\\file5.txt");
const TPtrC KAbbreviatesFileFrom2=_L("C:\\System1\\Data1\\file123456789.txt");


// InValid Paths / Folder Names
const TPtrC KNoParameters=_L("");
const TPtrC KInValidParameters=_L("3: \\52324424");
const TPtrC KInValidPathName=_L("C:\\System1\\Data1\\InvalidPathName\\");
const TPtrC KInvalidFileName=_L("C:\\System1\\Data1\\thefiledoesnotexistatthislocation.txt");
const TPtrC KIllegalFileName=_L("C:\\System1\\Data1\\Illega<>.txt");
const TPtrC KIllegalFolderName=_L("C:\\System1\\Data*<\\");
const TPtrC KFolderNameWithWildcard=_L("C:\\System1\\data*\\");
const TPtrC KInValidFolderName=_L("C:\\System1\\Data1\\InValid3+FollderName");
const TPtrC KInValidDrive=_L("P:\\");
const TPtrC KAppAbbName=_L("\\System1");


//-----------------------------------------------------------------------------
//
// Timing macros.
//
//-----------------------------------------------------------------------------
const TInt KMaxIterations     =   100;
#define INCLUDE_VERBOSE

#define TIMERINSTALL \
	TTime start(0); \
	TTime end(0);   \
	TUint delta = 0; \
	TUint cummulative = 0; \
	TUint iterations = 0;


#define TIMERRESET \
	delta = 0, cummulative = 0, iterations = 0;


#define TIMERSTART \
	start.UniversalTime();


#define TIMERSTOP \
	end.UniversalTime(); \
	delta = I64LOW(end.MicroSecondsFrom(start).Int64()); \
	iterations++; \
	cummulative += delta;


#ifdef INCLUDE_VERBOSE
_LIT(KTimerResult,   "Average time = %dus over %d iterations\n");
_LIT(KTimingError,   "Error - no timing results to display\n");


# define TIMERRESULT \
	if (iterations) \
		{ \
		theTest.Printf(KTimerResult, cummulative/iterations, iterations); \
		} \
	else \
		{ \
		theTest.Printf(KTimingError); \
		}

#else
# define TIMERRESULT
#endif


#define TIMERAVERAGE \
		(iterations ? cummulative/iterations : 0xFFFFFFFF)
//-----------------------------------------------------------------------------


//
//BaflUtils::GetEquivalentLanguageList
//BaflUtils::NearestLanguageFileV2 
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

//
//BaflUtils::PathExist test data

// Defines the maximum number of preconditions for each test case.
#define PATH_EXIST_MAX_PRECONDITIONS 1

// Literals for PathExist testing
const TPtrC KPathExistValid = _L("C:\\ValidPath\\Data1\\");
const TPtrC KPathExistInValid = _L("C:\\ValidPath\\Data1\\InvalidPathName\\");
const TPtrC KPathExistFullFile = _L("C:\\ValidPath\\Data1\\file1.txt");

const TPtrC KPathExist257 = _L("c:\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\256\\");
const TPtrC KPathExist256 = _L("c:\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\56\\");
const TPtrC KPathExist255 = _L("c:\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\10Letters\\6\\");

const TPtrC KPathExistShort = _L("c:\\10Letters\\10Letters\\");
const TPtrC KPathExistShortT1 = _L("c:\\10Letters\\10Letters");
const TPtrC KPathExistShortT2 = _L("c:\\10Letters\\10*\\");
const TPtrC KPathExistShortT3 = _L("c:\\10Letters\\10Letter?\\");

const TPtrC KPathExistSessionPath = _L("c:\\");
const TPtrC KPathExistRelativeT1 = _L("\\10Letters");
const TPtrC KPathExistRelativeT2 = _L("\\10Letters\\");
const TPtrC KPathExistRelativeT3 = _L("10Letters\\");

struct TPathExistTestRecord
// Defines the test case structure for PathExist unit testing
{
	// precondititions, directories to create before test run
	const TPtrC *iInPaths[PATH_EXIST_MAX_PRECONDITIONS];

	// the directory to test for
	const TPtrC  *iTestPath;

	// expected results
	TBool iResult;

	// test case line number for quick test identification
	TInt iLineNum;
};

LOCAL_D const TPathExistTestRecord pathExistTestCases[] =
// Test cases for PathExist unit testing
{
	{{&KPathExistValid}, &KPathExistValid, ETrue, 		__LINE__},
	{{&KPathExistValid}, &KPathExistInValid, EFalse, 	__LINE__},
	{{&KPathExistFullFile}, &KPathExistFullFile, EFalse,__LINE__},
	{{}, &KInValidParameters, EFalse, 					__LINE__},
	{{}, &KInValidFolderName, EFalse, 					__LINE__},
	{{}, &KValidDrive2, EFalse, 						__LINE__},
	{{}, &KNoParameters, EFalse, 						__LINE__},

	{{&KPathExistShort}, &KPathExistShort, ETrue, 		__LINE__},
	{{&KPathExistShort}, &KPathExistShortT1, EFalse, 	__LINE__},
	{{&KPathExistShort}, &KPathExistShortT2, EFalse, 	__LINE__},
	{{&KPathExistShort}, &KPathExistShortT3, EFalse, 	__LINE__},
	{{&KPathExistShort}, &KPathExistRelativeT1, EFalse, __LINE__},
	{{&KPathExistShort}, &KPathExistRelativeT2, EFalse, __LINE__},
	{{&KPathExistShort}, &KPathExistRelativeT3, EFalse, __LINE__},

#if !defined __WINS__ && !defined __WINSCW__

// max boundary tests - can only be tested on hardware

	{{&KPathExist255}, &KPathExist255, ETrue, 			__LINE__},
	{{&KPathExist256}, &KPathExist256, ETrue, 			__LINE__},
	{{&KPathExist257}, &KPathExist257, EFalse, 			__LINE__},

#endif //!defined __WINS__ && !defined __WINSCW__

	{{}, NULL, ETrue} // mandatory, last element
};

TBool DoTestPathExistsL(RFs& aFs, const struct TPathExistTestRecord& aTestRecord);

//
//Create/Destroy test environment global functions

//Delete "aFullName" file.
LOCAL_C void DeleteTestFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

//Delete "aFullName" dir.
LOCAL_C void DeleteTestDir(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" dir.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" dir attributes.\n"), err, &aFullName);
				}
			err = fsSession.RmDir(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" dir.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. Dir: %S.\n"), err, &aFullName);
		}
	}

//Delete data files used by the test
LOCAL_C void DeleteTestFiles()
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

//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteTestFiles();
		theTest(EFalse, aLine);
		}
	}
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::DeleteTestFiles();
		theTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)


LOCAL_C void DeleteTestFiles2(RFs &aFs)
{
	BaflUtils::DeleteFile (aFs, KRscFilename);
	BaflUtils::DeleteFile (aFs, KAmericanLang);
	BaflUtils::DeleteFile (aFs, KFrenchLang);
	BaflUtils::DeleteFile (aFs, KJapanEnglishLang);
	BaflUtils::DeleteFile (aFs, KEnglishLang);
}

LOCAL_C void CreateTestFiles2(RFs &aFs)
{
	TEST (BaflUtils::FileExists (aFs, KRscFilename) == EFalse);
	TEST (BaflUtils::FileExists (aFs, KAmericanLang) == EFalse);
	TEST (BaflUtils::FileExists (aFs, KFrenchLang) == EFalse);
	TEST (BaflUtils::FileExists (aFs, KJapanEnglishLang) == EFalse);
	TEST (BaflUtils::FileExists (aFs, KEnglishLang) == EFalse);

	// Create the files...
	RFile rFile;
	TEST (KErrNone == rFile.Create (aFs, KRscFilename, EFileRead));
	rFile.Close ();
	TEST (KErrNone == rFile.Create (aFs, KAmericanLang, EFileRead));
	rFile.Close ();
	TEST (KErrNone == rFile.Create (aFs, KFrenchLang, EFileRead));
	rFile.Close ();
	TEST (KErrNone == rFile.Create (aFs, KEnglishLang, EFileRead));
	rFile.Close ();
}
//
//Copy test file from Z: to C: drive.

LOCAL_C void CopyTestFiles()
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		User::LeaveIfError(fsSession.MkDir(KSystem1Folder));
		User::LeaveIfError(fsSession.MkDir(KSystem1Data1Folder));
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1File1), KErrNone);
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KFile1), KErrNone);
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1File123456789), KErrNone);
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1File2), KErrNone);
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1File5), KErrNone);
		User::LeaveIfError(fsSession.MkDir(KSystem1Data1SrcFolder));
		User::LeaveIfError(fsSession.MkDir(KSystem1Data1DstFolder));
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1SrcFile1), KErrNone);
		TEST2( BaflUtils::CopyFile(fsSession, KZFile1, KSystem1Data1DstFile2), KErrNone);

		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session.\n"), err);
		}
	}



//

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1571
@SYMTestCaseDesc Tests BaflUtils::PersistHAL()
@SYMTestPriority Medium
@SYMTestActions It starts halsetting.exe and persist hal settings
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF083235 	Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestPersistHAL()
	{
	TEST (BaflUtils::PersistHAL() == KErrNone);
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-1571 "));
	theTest.Printf(_L("BaflUtils::PersistHAL() called"));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0001
@SYMTestCaseDesc Tests Baflutils::EnsurePathExists.
@SYMTestPriority Medium
@SYMTestActions Passes in existing and non-existing pathnames and removes any directories created.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestEnsurePathExistsL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0001 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	// Path already exists so -11 will be returned within EnsurePathExistsL, Method Leaves with error code 0
	TRAPD(err, BaflUtils::EnsurePathExistsL(aFs, KValidPathName));
	TEST(!err);
	// Path does not exist so -12 will be returned within EnsurePathExistsL, The Directory will be created, Method Leaves with error code 0
	TRAPD(err2, BaflUtils::EnsurePathExistsL(aFs, KInValidPathName));
	TEST(!err2);
	TEST(!aFs.RmDir(KInValidPathName));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0002
@SYMTestCaseDesc Tests Baflutils::TestFileExists.
@SYMTestPriority Medium
@SYMTestActions Passes in valid and invalid files.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestFileExists(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0002 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TEST (BaflUtils::FileExists(aFs,KValidPathAndFileName));
	TEST (!BaflUtils::FileExists(aFs,KInvalidFileName));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0003
@SYMTestCaseDesc Tests Baflutils::TestFolderExists.
@SYMTestPriority Medium
@SYMTestActions Passes in valid and invalid folders.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestFolderExists(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0003 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TInt x = KMaxIterations;

	TIMERINSTALL

	while (x--)
	{
		TIMERSTART
		TEST(BaflUtils::FolderExists(aFs, KValidPathName));
		TEST(BaflUtils::FolderExists(aFs, KValidFileName));
		TEST(BaflUtils::FolderExists(aFs, KValidFileNameNoExtension));
	 	TEST(BaflUtils::FolderExists(aFs, KValidPathAndFileName));
	 	TEST(BaflUtils::FolderExists(aFs, KValidPathNameWithoutDrive));
	 	TEST(BaflUtils::FolderExists(aFs, KValidPathNameWithoutDrive2));
	 	TEST(BaflUtils::FolderExists(aFs, KValidDrive));
	 	TEST(BaflUtils::FolderExists(aFs, KValidDrive4));
	 	TEST(BaflUtils::FolderExists(aFs, KSessionRoot));
	 	TIMERSTOP
	}

	theTest.Printf(_L("\nBenchmark results for FolderExists tests returning ETrue:\n"));
	TIMERRESULT

	TIMERRESET
	x = KMaxIterations;

	while (x--)
	{
		TIMERSTART
		TEST(!BaflUtils::FolderExists(aFs, KInValidPathName));
		TEST(!BaflUtils::FolderExists(aFs, KNoFolderName));
		TEST(!BaflUtils::FolderExists(aFs, KFolderNameIsTooLong));
	 	TEST(!BaflUtils::FolderExists(aFs, KIllegalFileName));
	 	TEST(!BaflUtils::FolderExists(aFs, KIllegalFolderName));
	 	TEST(!BaflUtils::FolderExists(aFs, KFolderNameWithWildcard));
		TIMERSTOP
	}

	theTest.Printf(_L("\nBenchmark results for FolderExists tests returning EFalse:\n"));
	TIMERRESULT
	theTest.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0004
@SYMTestCaseDesc Tests Baflutils::TestPathExists.
@SYMTestPriority Medium
@SYMTestActions Passes in valid and invalid paths.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
@SYMDEF INC084050 - BaflUtils::PathExists does not work for 256 long folder paths
*/
void TestPathExistsL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0004 "));
	CleanupClosePushL(aFs);

	User::LeaveIfError(aFs.Connect());
	aFs.SetSessionPath(KPathExistSessionPath);

	for (TInt i = 0; pathExistTestCases[i].iTestPath != NULL; i++)
		TEST(DoTestPathExistsL(aFs,pathExistTestCases[i]));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0005
@SYMTestCaseDesc Tests Baflutils::TestCopyFile.
@SYMTestPriority Medium
@SYMTestActions Copy's a file from one location to another.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestCopyFile(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0005 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TEST(!BaflUtils::CopyFile(aFs, KCopyFileFrom, KCopyFileTo));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0006
@SYMTestCaseDesc Tests Baflutils::TestDeleteFile.
@SYMTestPriority Medium
@SYMTestActions Attempts to delete a file after setting its atributes 'Read Only'.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestDeleteFile(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0006 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TEST(!aFs.SetAtt(KDeleteFile, 0, KEntryAttReadOnly));
	TEST(!BaflUtils::DeleteFile(aFs, KDeleteFile));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0007
@SYMTestCaseDesc Tests Baflutils::TestRenameFile.
@SYMTestPriority Medium
@SYMTestActions Attempts to rename or move files or directories.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestRenameFile(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0007 "));
	// Pushes aFs to the stack.
	CleanupClosePushL(aFs);
	// Open Session with the File Server
	User::LeaveIfError(aFs.Connect());

	TUint theSwitch = CFileMan::EOverWrite;

	// Renames or moves one or more files or directories.
	TEST2(BaflUtils::RenameFile(aFs, KRenameFromFile, KRenameToFile, theSwitch), KErrNone);

	//Rename file back to original name
	TEST2(BaflUtils::RenameFile(aFs, KRenameToFile, KRenameFromFile, theSwitch), KErrNone);
	// Pops aFs from the stack
	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0008
@SYMTestCaseDesc Tests Baflutils::TestAbbreviateFileName.
@SYMTestPriority Medium
@SYMTestActions Checks if filename is being abbreviated correctly.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestAbbreviateFileName()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0008 "));
	TFileName originalFileName(KAbbreviatesFileFrom);
	TBuf<KMaxDisplayedFullNameLen> abbrevName;

	BaflUtils::AbbreviateFileName (originalFileName.Left(29), abbrevName);
	TEST(!originalFileName.Compare(abbrevName));
	originalFileName.Copy(KAbbreviatesFileFrom2);
	BaflUtils::AbbreviateFileName (originalFileName.Left(39), abbrevName);
	TEST(originalFileName.Compare(abbrevName)<0);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0009
@SYMTestCaseDesc Tests Baflutils::TestCheckWhetherFullNameRefersToFolder.
@SYMTestPriority Medium
@SYMTestActions Passes in paths with valid and invalid folder names.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestCheckWhetherFullNameRefersToFolder()
	{
	TBool folder=EFalse;
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0009 "));

	TEST(!BaflUtils::CheckWhetherFullNameRefersToFolder(KValidFolderName, folder));
    TEST(folder);
	folder=EFalse;
	TEST(!BaflUtils::CheckWhetherFullNameRefersToFolder(KInValidFolderName, folder));
    TEST(!folder);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0010
@SYMTestCaseDesc Tests Baflutils::TestCopyWithTruncation.
@SYMTestPriority Medium
@SYMTestActions Passes in truncated and non-truncated descriptors and checks return codes.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestCopyWithTruncation()
	{
	_LIT(KDestDataNoneTruncation,"TestCopyWithNoneTruncation");
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0010 "));
	TBuf<26> dest;
	TBuf<30> src(KDestDataNoneTruncation);
	TChar truncationSymbol = KBaflCharTruncation;
	BaflUtils::CopyWithTruncation(dest, src, truncationSymbol);
	TEST(dest.Locate(KBaflCharTruncation)==KErrNotFound);

	_LIT(KDestDataTruncation,"TestCopyWithTruncationLeftMost");
	src.Copy(KDestDataTruncation);
	BaflUtils::CopyWithTruncation(dest, src, truncationSymbol);
	TEST(dest.Locate(KBaflCharTruncation)==25);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0011
@SYMTestCaseDesc Tests Baflutils::TestDriveAndPathFromFullName.
@SYMTestPriority Medium
@SYMTestActions Passes in a valid filename and compares the result.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestDriveAndPathFromFullName()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0011 "));
	TFileName fileName = BaflUtils::DriveAndPathFromFullName(KValidPathName);
	TEST(!KValidPathName.Compare(fileName));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0012
@SYMTestCaseDesc Tests Baflutils::TestFolderNameFromFullName.
@SYMTestPriority Medium
@SYMTestActions Passes in various valid path, file and drive names and compares results with descriptors.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestFolderNameFromFullName()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0012 "));
	TFileName fileName =  BaflUtils::FolderNameFromFullName(KValidPathAndFileName);
	TEST(!fileName.Compare(_L("Data1")));

	fileName =  BaflUtils::FolderNameFromFullName( KValidFileName);
	TEST(!fileName.Compare(_L("C:\\")));

	fileName =  BaflUtils::FolderNameFromFullName(KValidDrive);
	TEST(!fileName.Compare(_L("C:\\")));

	fileName =  BaflUtils::FolderNameFromFullName(KValidDrive2);
	TEST(!fileName.Compare(_L("C:")));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0013
@SYMTestCaseDesc Tests Baflutils::TestMostSignificantPartOfFullName.
@SYMTestPriority Medium
@SYMTestActions Passes in valid path and file names and compares results with descriptors.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestMostSignificantPartOfFullName()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0013 "));
	TFileName mostSignificantPart;

	TEST(!BaflUtils::MostSignificantPartOfFullName(KValidPathName, mostSignificantPart));
	TEST(!mostSignificantPart.Compare(_L("Data1")));
	TEST(!BaflUtils::MostSignificantPartOfFullName(KValidPathAndFileName, mostSignificantPart));
	TEST(!mostSignificantPart.Compare(_L("file1.txt")));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0014
@SYMTestCaseDesc Tests Baflutils::TestParse.
@SYMTestPriority Medium
@SYMTestActions Passes in valid path and file name.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestParse()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0014 "));
	TEST(!BaflUtils::Parse(KValidPathAndFileName));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0015
@SYMTestCaseDesc Tests Baflutils::TestRootFolderPath.
@SYMTestPriority Medium
@SYMTestActions Passes in valid drive and compares against expected output.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestRootFolderPath()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0015 "));
	TFileName fileName = BaflUtils::RootFolderPath(KValidDrive3);
	TEST(!fileName.Compare(_L("C:\\")));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0016
@SYMTestCaseDesc Tests Baflutils::TestValidateFolderNameTypedByUserL.
@SYMTestPriority Medium
@SYMTestActions Passes in various folder names and checks return values are as expected.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestValidateFolderNameTypedByUserL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0016 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TInt retCode=KErrNone;
	retCode=retCode;
	TFileName newFolderFullName = KCurrentPath;

	TEST(User::LeaveIfError(retCode=BaflUtils::ValidateFolderNameTypedByUserL(aFs, KValFolderName, KCurrentPath, newFolderFullName)==KErrNone));
	TEST(User::LeaveIfError(retCode=BaflUtils::ValidateFolderNameTypedByUserL(aFs, KNoFolderName, KCurrentPath, newFolderFullName)==KErrArgument));
	TEST(User::LeaveIfError(retCode=BaflUtils::ValidateFolderNameTypedByUserL(aFs, KInValidFolderName, KCurrentPath, newFolderFullName)==KErrBadName));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0017
@SYMTestCaseDesc Tests Baflutils::TestRemoveSystemDirectory.
@SYMTestPriority Medium
@SYMTestActions Delete system directory.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestRemoveSystemDirectory(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0017 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	CDir* theDir = NULL;
	CDir* fileList = NULL;

	TInt attribs=KEntryAttDir|KEntryAttSystem|KEntryAttMatchExclusive;
	aFs.GetDir(_L("z:\\system"),attribs,ESortNone, fileList, theDir);
	TInt theNumber = theDir -> Count();
	TEST(theNumber == 1);
	BaflUtils::RemoveSystemDirectory(*theDir);
	theNumber = theDir -> Count();
	TEST(!theNumber);

	delete fileList;
	delete theDir;
	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0018
@SYMTestCaseDesc Tests Baflutils::TestCheckFolder.
@SYMTestPriority Medium
@SYMTestActions Pass in various path, file and drive names.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestCheckFolder(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0018 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TEST(!BaflUtils::CheckFolder(aFs, KValidPathName));
	TEST(!BaflUtils::CheckFolder(aFs, KValidPathAndFileName));

	TEST(!BaflUtils::CheckFolder(aFs, KValidFileName));

	TEST(!BaflUtils::CheckFolder(aFs, KValidDrive));
	TEST(BaflUtils::CheckFolder(aFs, KInValidPathName));

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0019
@SYMTestCaseDesc Tests Baflutils::TestDiskIsReadOnly.
@SYMTestPriority Medium
@SYMTestActions Pass in various drives and parameters and checks 'Read Only' status.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestDiskIsReadOnly(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0019 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TBool isReadOnly = EFalse;

	TEST(!BaflUtils::DiskIsReadOnly(aFs, KValidPathName, isReadOnly));
	TEST(!isReadOnly);
	TEST(!BaflUtils::DiskIsReadOnly(aFs, KValidDrive4, isReadOnly));
	TEST(isReadOnly);
	TEST(BaflUtils::DiskIsReadOnly(aFs, KInValidDrive, isReadOnly));
	TEST(isReadOnly);
	TEST(BaflUtils::DiskIsReadOnly(aFs, KNoParameters, isReadOnly));
	TEST(isReadOnly);
	TEST(BaflUtils::DiskIsReadOnly(aFs, KInValidParameters, isReadOnly));
	TEST(isReadOnly);

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0020
@SYMTestCaseDesc Tests Baflutils::TestExtractAppNameFromFullName.
@SYMTestPriority Medium
@SYMTestActions Compares thread name to both valid and invalid application names.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestExtractAppNameFromFullName()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0020 "));
	TPtrC validAppName = BaflUtils::ExtractAppNameFromFullName(RThread().FullName());
	TEST(!validAppName.Compare(RThread().Name()));

	TPtrC inValidAppName = BaflUtils::ExtractAppNameFromFullName(_L("123:56789"));
	TEST(inValidAppName.Compare(_L("123:56789")));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0021
@SYMTestCaseDesc Tests Baflutils::TestIsFirstDriveForSocket.
@SYMTestPriority Medium
@SYMTestActions Passes two differant drives as arguments.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestIsFirstDriveForSocket()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0021 "));
	TDriveUnit driveUnit(_L("C"));

	TEST(BaflUtils::IsFirstDriveForSocket(driveUnit));
	driveUnit =_L("Z");
	TEST(!BaflUtils::IsFirstDriveForSocket(driveUnit));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0022
@SYMTestCaseDesc Tests Baflutils::TestIsFolder.
@SYMTestPriority Medium
@SYMTestActions Passes in paths, folders and drives and checks return values.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestIsFolder(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0022 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TBool isFolder = EFalse;

	TEST(!BaflUtils::IsFolder(aFs, KValidPathName, isFolder));
	TEST(isFolder);

	TEST(BaflUtils::IsFolder(aFs, KInValidFolderName, isFolder));
	TEST(isFolder);

	TEST(!BaflUtils::IsFolder(aFs, KValidDrive4, isFolder));
	TEST(isFolder);

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0023
@SYMTestCaseDesc Tests Baflutils::TestUidTypeMatches.
@SYMTestPriority Medium
@SYMTestActions Compares Uid's to check equality.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestUidTypeMatches()
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0023 "));
	TUid valueOne = {0x1}, valueTwo = {0x2};
	valueOne.Uid(1);
	TUidType fileUid(valueOne), matchUid(valueOne);

	TEST(BaflUtils::UidTypeMatches(fileUid, matchUid));
	valueTwo.Uid(2);
	fileUid = valueTwo;
	TEST(!BaflUtils::UidTypeMatches(fileUid, matchUid));
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0024
@SYMTestCaseDesc Tests Baflutils::TestUpdateDiskListL.
@SYMTestPriority Medium
@SYMTestActions Passes values to check the presence of disks.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void TestUpdateDiskListL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0024 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	CDesCArray* drives = new(ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(drives);

	// According to the Base Peripherals team drive mapping differs between
	// targets' environment and there might be different drives other than C and Z.
	// For the same reason we shouldn't check the number of available disk drives.

	// Test cases for excluding ROM
	TBool includeRom = EFalse;
	TDriveNumber driveToInclude = EDriveA;
	BaflUtils::UpdateDiskListL(aFs, *drives, includeRom, driveToInclude);
	TEST( 'A' == ((*drives)[0])[0] );
	TEST( 'C' == ((*drives)[1])[0] );
	TEST( 'Z' != ((*drives)[drives->MdcaCount() - 1])[0] );

	driveToInclude = EDriveD;
	BaflUtils::UpdateDiskListL(aFs, *drives, includeRom, driveToInclude);
	TEST( 'C' == ((*drives)[0])[0] );
	TEST( 'D' == ((*drives)[1])[0] );
	TEST( 'Z' != ((*drives)[drives->MdcaCount() - 1])[0] );

	// Test cases for including ROM
	includeRom = ETrue;
	driveToInclude = EDriveA;
	BaflUtils::UpdateDiskListL(aFs, *drives, includeRom, driveToInclude);
	TEST( 'A' == ((*drives)[0])[0] );
	TEST( 'C' == ((*drives)[1])[0] );
	TEST( 'Z' == ((*drives)[drives->MdcaCount() - 1])[0] );

	driveToInclude = EDriveD;
	BaflUtils::UpdateDiskListL(aFs, *drives, includeRom, driveToInclude);
	TEST( 'C' == ((*drives)[0])[0] );
	TEST( 'D' == ((*drives)[1])[0] );
	TEST( 'Z' == ((*drives)[drives->MdcaCount() - 1])[0] );

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038-0025
@SYMTestCaseDesc Tests Baflutils::TestGetDiskListL.
@SYMTestPriority Medium
@SYMTestActions Passes values to check the presence of disks.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF051652 - BaflUtils::IsFirstDriveForSocket seems broken
*/
void TestGetDiskListL(RFs& aFs)
	{
	/*
	(Usually) Works in the same way as UpdateDiskListL. To test the difference we have
	to perform a manual test (emulator: F5,F4, h/w: removal of media cards).
	For automatic tests we check the normal functionality.
	*/
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0038-0025 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	CDesCArray* drives = new(ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(drives);

	// According to the Base Peripherals team drive mapping differs between
	// targets' environment and there might be different drives other than C and Z.
	// For the same reason we shouldn't check the number of available disk drives.

	// Test cases for excluding ROM
	BaflUtils::GetDiskListL(aFs, *drives);
	TEST( 'C' == ((*drives)[0])[0] );

#if !defined(__EPOC32__)
	TEST( 'X' == ((*drives)[drives->MdcaCount() - 3])[0] );
#endif

	TEST( 'Z' == ((*drives)[drives->MdcaCount() - 1])[0] );

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-0095
@SYMTestCaseDesc Tests Baflutils::TestDriveIsReadOnlyInternal.
@SYMTestPriority Medium
@SYMTestActions Pass in various drives and parameters and checks 'Read Only Internal' status.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 806 Eliminate scanning for plugins on startup
*/
void TestDriveIsReadOnlyInternal(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID SYSLIB-BAFL-CT-0095 "));
	CleanupClosePushL(aFs);
	User::LeaveIfError(aFs.Connect());

	TBool isReadOnlyInternal = EFalse;

	TEST(!BaflUtils::DriveIsReadOnlyInternal(aFs, KValidPathName, isReadOnlyInternal));
	TEST(!isReadOnlyInternal);
	TEST(!BaflUtils::DriveIsReadOnlyInternal(aFs, KValidDrive4, isReadOnlyInternal));
	TEST(isReadOnlyInternal);
	TEST(BaflUtils::DriveIsReadOnlyInternal(aFs, KInValidDrive, isReadOnlyInternal));
	TEST(isReadOnlyInternal);
	TEST(BaflUtils::DriveIsReadOnlyInternal(aFs, KNoParameters, isReadOnlyInternal));
	TEST(isReadOnlyInternal);
	TEST(BaflUtils::DriveIsReadOnlyInternal(aFs, KInValidParameters, isReadOnlyInternal));
	TEST(isReadOnlyInternal);

	CleanupStack::PopAndDestroy(&aFs);
	}

/**
@SYMTestCaseID TI18N-BAFL-CT-4003
@SYMTestCaseDesc Tests Baflutils::TestNearestLanguageFileV2.
@SYMTestPriority Medium
@SYMTestActions Pass in various resource file names and change system locale setting, check the return values.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 2525 Install device equivalent languages from SIS files
*/
void TestNearestLanguageFileV2(RFs& aFs)
	{
	/* test case ID to be added */
    theTest.Next (_L (" @SYMTestCaseID TI18N-BAFL-CT-4003 "));
    
    __UHEAP_MARK;

	CleanupClosePushL (aFs);
	User::LeaveIfError (aFs.Connect ());
    
    DeleteTestFiles2(aFs);
    CreateTestFiles2(aFs);

	TBuf <256> filename;
	TLanguage lang = ELangNone;
	theTest.Printf (_L("lang initial value is %d\n"), lang);

	// Test NearestLanguageFile with empty name
	TFileName resPath;
	BaflUtils::NearestLanguageFileV2(aFs, resPath, lang);
	TEST(resPath.Length()==0);
	theTest.Printf (_L("lang returned by NLFV2 is %d\n"), lang);
	TEST(ELangNone == lang);
	theTest.Printf (_L ("The NearestLanguageFile for the null file - %S - is - %S -\n"), &KNullFilename, &filename);
	
	// Test NearestLanguageFile with invalid drive letter
	filename.Copy (KInvalidDriveLetterV2);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	//TEST(filename==KRscFilenameNoSuffix);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S, lang is %d\n"), &KInvalidDriveLetterV2, &filename, lang);
	//TEST(ELangNone == lang);
	
	// Test NearestLanguageFile with incorrect ext
	DeleteTestFiles2(aFs);
	RFile rFile;
	TEST (KErrNone == rFile.Create (aFs, KFrenchLang, EFileRead));
	rFile.Close ();
	filename.Copy (KNoSuchLangFile);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	//TEST(filename==KRscFilenameNoSuffix);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S, lang is %d\n"), &KNoSuchLangFile, &filename, lang);
	//TEST(ELangNone == lang);
	BaflUtils::DeleteFile (aFs, KFrenchLang);
	CreateTestFiles2(aFs);
		
	// Test NearestLanguageFile with no suffix
	filename.Copy (KRscFilenameNoSuffix);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	TEST(filename==KRscFilenameNoSuffix);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilenameNoSuffix, &filename);
	TEST(ELangNone == lang);
	
	// change locale to American
	TExtendedLocale loc;
	loc.LoadSystemSettings();
	TInt ret = loc.LoadLocale(_L ("elocl.10"));
	theTest.Printf(_L("LoadLocale returns %d\n"), ret);
	TEST(KErrNone == ret);
	ret = loc.SaveSystemSettings();
	TEST(KErrNone == ret);
	
	// Test NearestLanguageFile: AmE is supported
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	//TEST(filename == KEnglishLang);
	TEST(filename == KAmericanLang);
	TEST(lang == ELangAmerican);

	// Delete geltest.r10, AmE downgrades to En_GB.
	BaflUtils::DeleteFile (aFs, KAmericanLang);
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KEnglishLang);
	TEST(lang == ELangEnglish);
	
	
	// Delete geltest.r01, AmE downgrades to en_GB, to ELangNone.
	BaflUtils::DeleteFile (aFs, KEnglishLang);
	filename.Copy (KRscFilename);
	BaflUtils::NearestLanguageFileV2(aFs, filename, lang);
	theTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KRscFilename, &filename);
	TEST(filename == KRscFilename); // filename remains unchanged.
	TEST(lang == ELangNone);

	// restore locale settings
	theTest.Printf(_L("Restoring locale settings.\n"));
	loc.LoadSystemSettings();
	ret = loc.LoadLocale(_L("elocl.01"));
	TEST(KErrNone == ret);
	ret = loc.SaveSystemSettings();
	TEST(KErrNone == ret);
	
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
void TestGetEquivalentLanguageList()
	{
	/* test case ID to be added */
	theTest.Next (_L (" @SYMTestCaseID TI18N-BAFL-CT-4004 "));
    
    __UHEAP_MARK;
    
	TLanguagePath lp;
	// Test GetNearestLanguageFile();
	// 1- There is an entry in the table for the given language
	BaflUtils::GetEquivalentLanguageList(ELangCyprusGreek, lp);
	TEST(ELangCyprusGreek == lp[0]);
	TEST(ELangGreek == lp[1]);
	TEST(ELangNone == lp[2]);
	
	// 2- There is no entry for the given language
	BaflUtils::GetEquivalentLanguageList(ELangFarsi, lp);
	TEST(ELangFarsi == lp[0]);
	TEST(ELangNone == lp[1]);

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-BAFL-CT-0038
@SYMTestCaseDesc Tests Baflutils.
@SYMTestPriority Medium
@SYMTestActions Executes all T_baflutils tests.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF050397 - Crimson test code improvement to be propagated into MCL
*/
void DoTestsL()
	{
	RFs fs;

	::CopyTestFiles();
	TestPersistHAL();
	TestEnsurePathExistsL(fs);
	TestFileExists(fs);
	TestFolderExists(fs);
	TestPathExistsL(fs);
	TestCopyFile(fs);
	TestDeleteFile(fs);
	TestRenameFile(fs);
	TestAbbreviateFileName();
	TestCheckWhetherFullNameRefersToFolder();
	TestCopyWithTruncation();
	TestDriveAndPathFromFullName();
	TestFolderNameFromFullName();
	TestMostSignificantPartOfFullName();
	TestParse();
	TestRootFolderPath();
	TestValidateFolderNameTypedByUserL(fs);
	TestRemoveSystemDirectory(fs);
	TestCheckFolder(fs);
	TestDiskIsReadOnly(fs);
	TestNearestLanguageFileV2(fs);
	TestGetEquivalentLanguageList();

	TestDriveIsReadOnlyInternal(fs);
	#if defined __WINS__ || defined __WINSCW__

		TestExtractAppNameFromFullName();
	#endif //defined __WINS__ || defined __WINSCW__

	TestIsFirstDriveForSocket();
	TestIsFolder(fs);
	TestUidTypeMatches();
	TestUpdateDiskListL(fs);
	TestGetDiskListL(fs);
	::DeleteTestFiles();
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	theTest.Title();
	theTest.Start(_L("Testing T_BaflUtils "));
    TRAPD(err,DoTestsL());
    TEST(!err);
	theTest.End();
    theTest.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }


//
// Helper method to create a hierarchy of directories

TInt CreateDirectoryTree(RFs& aFs, const TPtrC* aDir)
	{
	TParse parsedPath;
	TBuf<KMaxFileName> newPath;
	TInt err, pos;
	TPtrC path;
	TPtrC fileName;

	// check for the corectness of input parameters
	if (aDir == NULL )
		return EFalse;

	// verify the corectness of the input path
	err = parsedPath.Set(*aDir, NULL, NULL);
	if (err != KErrNone)
	   return err;

	// interatily build the path and create hierarchy of directories in file system
	newPath = parsedPath.Drive();
	newPath.Append('\\');
	path.Set(parsedPath.Path());
	path.Set(path.Mid(1));              // get rid of the first backlash
	while ((pos = path.Find(_L("\\"))) != KErrNotFound)
		{
		newPath.Append(path.Left(pos)); //append next directory
		newPath.Append('\\');
		err = aFs.MkDir(newPath);
		if (err != KErrNone) return err;
		path.Set(path.Mid(pos + 1));
		}

	// directory created.
	// verify if there is a need to create a file
	if (parsedPath.NamePresent())
		{
		RFile file;
		err = file.Create(aFs, *aDir, EFileWrite);
		if (err != KErrNone) return err;
		file.Close();
		}

	return KErrNone;
	}


//
// Helper method to remove a hierarchy of directories
TInt RemoveDirectoryTreeL(RFs& aFs, const TPtrC* aDir)
	{
	TParse parsedPath;
	TBuf<KMaxFileName> newPath;
	TInt err = KErrNone;
	TInt pos;
	TPtrC path;
	CFileMan *fileMan = NULL;

	// Create FileMan.
	// As oposite to RFs it is able to remove a hirarchy of not empty directories
	TRAPD(leaveCode, fileMan = CFileMan::NewL(aFs));
	if (leaveCode != KErrNone)
		return leaveCode;
	CleanupStack::PushL(fileMan);

	// verify the corectness of the input path
	err = parsedPath.Set(*aDir, NULL, NULL);
	if (err == KErrNone)
		{
		// build the path consisting only of the first level of directories hierarchy
		newPath = parsedPath.Drive();
		newPath.Append('\\');
		path.Set(parsedPath.Path());
		path.Set(path.Mid(1));  			// get rid of the first backlash
		if((pos = path.Find(_L("\\"))) != KErrNotFound)
			{
			newPath.Append(path.Left(pos)); //append next directory
			newPath.Append('\\');
			err = fileMan->RmDir(newPath);
			}

		}

	CleanupStack::PopAndDestroy(fileMan);

	return err;;
	}

//
// Helper method for TestPathExist test.
TBool DoTestPathExistsL(RFs& aFs, const struct TPathExistTestRecord& aTestRecord)
{
	TBool err;
	TInt i;

	// 1. Create required directories (preconditions)
	for ( i = 0; aTestRecord.iInPaths[i] != NULL && i < PATH_EXIST_MAX_PRECONDITIONS; i++)
		{
		err = CreateDirectoryTree(aFs, aTestRecord.iInPaths[i]);
		if (err != KErrNone)
			{
			RDebug::Print(_L("TestPathExist(%d): Directory not created. Cause: %d"),
							aTestRecord.iLineNum, err);

			// Do not break the loop here as the path might be incorrectly shaped on purpose.
			}
		}

	// 2. Make a directory test
	TBool testOutcome = BaflUtils::PathExists(aFs, *aTestRecord.iTestPath);

	// 3. Remove directories
	for ( i = 0; aTestRecord.iInPaths[i] != NULL && i < PATH_EXIST_MAX_PRECONDITIONS ; i++)
		{
		err = RemoveDirectoryTreeL(aFs, aTestRecord.iInPaths[i]);

		if (err != KErrNone)
			{
			RDebug::Print(_L("TestPathExist(%d): Warning: Directory not removed. Cause: %d"),
							aTestRecord.iLineNum, err);
			}
		}

	// 4. Check the results
	if (aTestRecord.iResult != testOutcome)
		{
		RDebug::Print(_L("TestPathExist(%d): Failed."), aTestRecord.iLineNum);
		return EFalse;
		}

	RDebug::Print(_L("TestPathExist(%d): Passed."), aTestRecord.iLineNum);
	return ETrue;
}
