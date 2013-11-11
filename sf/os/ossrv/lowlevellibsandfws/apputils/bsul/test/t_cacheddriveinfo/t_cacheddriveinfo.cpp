// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bsul/bsul.h>
#include <e32test.h>
#include <bautils.h>

LOCAL_D RTest theTest(_L("t_cacheddriveinfo"));

// Valid Paths / Folder Name
const TPtrC KValidPathNameCSystem1Data1=_L("C:\\System1\\Data1\\");
const TPtrC KValidDriveZ=_L("Z:\\");
const TPtrC KValidDriveC=_L("C:\\");
const TPtrC KValidDriveT=_L("T:\\");
const TPtrC KValidDriveE=_L("E:\\");

// InValid Paths / Folder Names
const TPtrC KNoParameters=_L("");
const TPtrC KInValidParameters=_L("3: \\52324424");
const TPtrC KInValidDriveP=_L("P:\\");

//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		theTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1393
@SYMTestCaseDesc Tests BSUL::CCachedDriveInfo
@SYMTestPriority Medium
@SYMTestActions Pass in various drives and parameters and checks 'Read Only Internal' status.
				The same parameters are checked as in the tests for BaflUtils::DriveIsReadOnlyInternal
@SYMTestExpectedResults The test must not fail.
*/
void TestCachedDriveInfoL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1393 "));
	BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC(aFs);

	// the C drive should not be read-only internal
	TEST(!cachedDriveInfo->IsReadOnlyInternalL(KValidPathNameCSystem1Data1));
	// the Z drive should be read-only internal
	TEST(cachedDriveInfo->IsReadOnlyInternalL(KValidDriveZ));
	// a non-mounted drive should not be read-only internal
	TEST(!cachedDriveInfo->IsReadOnlyInternalL(KInValidDriveP));
	// an invalide path should leave with an error
	TRAPD(err, cachedDriveInfo->IsReadOnlyInternalL(KNoParameters));
	TEST(err != KErrNone);
	// an invalide path should leave with an error
	TRAP(err, cachedDriveInfo->IsReadOnlyInternalL(KInValidParameters));
	TEST(err != KErrNone);

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1680
@SYMTestCaseDesc Tests BSUL::CCachedDriveInfo
@SYMTestPriority Medium
@SYMTestActions Pass in various drives and parameters and checks all kinds attributes:
				KDriveAttSubsted,KDriveAttRemote, KDriveAttLocal, KDriveAttInternal, KDriveAttRemovable
				KDriveAttRedirected,KDriveAttRom.
@SYMTestExpectedResults The test must not fail.
*/
void TestCachedDriveInfoFlagSetL(RFs& aFs)
	{

	theTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1680 "));
	TFileName fileName;
	TInt err;
	if(!BaflUtils::PathExists(aFs, _L("C:\\Logs\\")))
		{
		err = aFs.MkDir(_L("C:\\Logs\\"));
		}

	err=aFs.SetSubst(_L("C:\\Logs\\"),EDriveO);
    TEST(err==KErrNone);
    err=aFs.Subst(fileName,EDriveO);
	TEST(err==KErrNone);
    TEST(fileName==_L("C:\\Logs\\"));

	BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC(aFs);

	// The O drive should be subst drive
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(EDriveO, KDriveAttSubsted));

	// The O drive should not be local drive
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(EDriveO, KDriveAttLocal));

	// The O drive should not be remote drive
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(EDriveO, KDriveAttRemote));

	// The C drive should be local drive
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttLocal));

	// The C drive should be internal drive
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttInternal));

	// The C drive should not be remote drive
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttRemote));

	// The C drive should not be removable drive
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttRemovable));

	// The C drive should be local and internal drive
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttInternal|KDriveAttInternal));

	// The C drive should be local and internal drive, but not remote
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveC, KDriveAttInternal|KDriveAttInternal|KDriveAttRemote));

	// The Z drive should  rom drive.
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveZ, KDriveAttRom));

	//The Z drive should  rom, and internal drive.
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveZ, KDriveAttRom|KDriveAttInternal));

	//The Z drive should  rom and internal drive, but not removable
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveZ, KDriveAttRom|KDriveAttInternal|KDriveAttRemovable));

	// The Z drive should not be Redirected drive
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveZ, KDriveAttRedirected));

	// The Z drive should  rom drive, but not Redirected
	TEST(!cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveZ, KDriveAttRom|KDriveAttRedirected));

	// The T drive should  be a removable, local drive on winscw, E drive on hardware
#if defined __WINSCW__	
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveT, KDriveAttRemovable|KDriveAttLocal));
#elif defined __X86GCC__
	// No removable drives on X86 platform
#else
	TEST(cachedDriveInfo->IsFlagSetOnDriveL(KValidDriveE, KDriveAttRemovable|KDriveAttLocal));
#endif

	//clean up subst drive
	err=aFs.SetSubst(_L(""),EDriveO);
	TEST(err==KErrNone);
	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4014
@SYMTestCaseDesc Tests BSUL::CCachedDriveInfo
@SYMTestPriority Medium
@SYMTestActions Pass in various drives and media types to verify media type of drives.
@SYMTestExpectedResults The media type must be as expected for each drive.
@SYMDEF DEF109839
*/
void TestCachedDriveMediaTypeL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4014 "));
	BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC(aFs);

	// the T drive should be a hard disk media type on WINSCW, E drive on hardware
#ifdef __WINSCW__
	TEST(cachedDriveInfo->MediaTypeL(KValidDriveT,EMediaHardDisk));
#elif defined __X86GCC__
	// No removeable drive in X86 platform
#else
	TEST(cachedDriveInfo->MediaTypeL(KValidDriveE,EMediaHardDisk));
#endif

	// the Z drive should be a Rom drive
	TEST(cachedDriveInfo->MediaTypeL(KValidDriveZ,EMediaRom));

#ifdef __WINSCW__
	// the C drive should report as EMediaHardDisk for the PC Drive
	TEST(cachedDriveInfo->MediaTypeL(KValidDriveC,EMediaHardDisk));
#elif defined __EPOC32__
	// the C drive could be a RAM or NAND flash drive depending on how the ROM is built.
	// the C drive should report as EMediaHardDisk for WDP enabled configuraion
	TEST(cachedDriveInfo->MediaTypeL(KValidDriveC,EMediaRam) || cachedDriveInfo->MediaTypeL(KValidDriveC,EMediaNANDFlash) || cachedDriveInfo->MediaTypeL(KValidDriveC,EMediaHardDisk));
#endif

	// a non-mounted drive should not be EMediaHardDisk
	TMediaType actual;
	TEST(!cachedDriveInfo->MediaTypeL(KInValidDriveP,EMediaHardDisk,&actual));
	TEST(actual == EMediaNotPresent);

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-4015
@SYMTestCaseDesc Tests BSUL::CCachedDriveInfo
@SYMTestPriority Medium
@SYMTestActions Call the GetDefaultRemovableMemoryCardDriveL API on WINSCW and hardware.
@SYMTestExpectedResults The MMC drive must be found.
@SYMDEF DEF109839
*/
void TestCachedDriveRemovableMemoryCardDriveL(RFs& aFs)
	{
	theTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4015 "));
	BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC(aFs);

	// the T drive should be a hard disk media type on WINSCW, E drive on hardware
#ifdef __WINSCW__
	TEST(cachedDriveInfo->GetDefaultRemovableMemoryCardDriveL() == TDriveUnit(KValidDriveT));
#elif defined __X86GCC__
	// No removable drives in X86 platform
	TRAPD(err,cachedDriveInfo->GetDefaultRemovableMemoryCardDriveL());
	TEST(err == KErrNotFound);
#else
	TEST(cachedDriveInfo->GetDefaultRemovableMemoryCardDriveL() == TDriveUnit(KValidDriveE));
#endif

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID SYSLIB-BAFL-CT-1394
@SYMTestCaseDesc Tests BSUL::CCachedDriveInfo
@SYMTestPriority Medium
@SYMTestActions Executes tests for BSUL::CCachedDriveInfo.
@SYMTestExpectedResults The test must not fail.
*/
void DoTests()
	{
	theTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1394 "));
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);

	TestCachedDriveInfoL(fs);
	TestCachedDriveInfoFlagSetL(fs);
	TestCachedDriveMediaTypeL(fs);
	TestCachedDriveRemovableMemoryCardDriveL(fs);

	CleanupStack::PopAndDestroy(&fs);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup *cleanup=CTrapCleanup::New();
	theTest.Title();
	theTest.Start(_L("Testing t_cacheddriveinfo"));
	TRAPD(err,DoTests());
	TEST(!err);
	theTest.End();
	theTest.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return(0);
	}
