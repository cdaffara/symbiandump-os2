// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"
#include "../cenrepsrv/srvparams.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Tests"));

TBool OomTesting;

const TUid KUidTestRepository1 = { 0x00000001 };
const TUid KUidCreTestRepository1 = { 0x22222221 };
const TUid KUidTestRepository2 = { 0x00000002 };

const TUid KUidCorruptRepository = { 0x00000003 };
const TUid KUidResetTestRepository = { 0x00000004 };

const TUid KUidDriveCRepository = { 0x00000010 };

const TUid KUidDriveCOnlyRepository = { 0x00000013 };

const TUid KUidTestRepository3 = { 0x00000103 };

//
// Test repository 1
//

const TUint32 KNonExisitentSetting = 0;

const TInt KNumSettings = 9;

const TUint32 KInt1 = 1;
const TInt KInt1_InitialValue = 1;
const TInt KInt1_UpdatedValue = 73;
const TUint32 KInt3 = 257;
const TUint32 KNewInt = 1000;

const TUint32 KReal1 = 2;
const TReal KReal1_InitialValue = 2.732;
const TReal KReal1_UpdatedValue = 72.8;
const TUint32 KReal2 = 8;
const TReal KReal2_InitialValue = 1.5;
const TUint32 KReal3 = 17;
const TUint32 KNewReal = 2000;

const TUint32 KString1 = 5;
_LIT(KString1_InitialValue, "test\\\"string\"");
_LIT(KString1_UpdatedValue, "another one");
const TUint32 KString2 = 12;
_LIT8(KString2_8BitValue, "string");

const TUint32 KString12 = 12;
const TUint32 KNewString8 = 3000;
const TUint32 KNewString16 = 4000;

const TUint32 KNewSetting1 = 0x2001;
const TUint32 KNewSetting2 = 0x2002;
const TUint32 KNewSetting3 = 0x2003;
const TUint32 KNewSetting4 = 0x2004;
const TUint32 KNewSetting5 = 0x2005;
const TUint32 KNewSetting6 = 0x2006;
const TUint32 KNewSetting7 = 0x2007;
const TUint32 KNewSetting8 = 0x2008;

const TInt32 KNewSetting1Value = 4567;
const TReal KNewSetting2Value = 45.67;
_LIT(KNewSetting3Value, "a unicode str");
_LIT8(KNewSetting4Value, "an ascii str");
const TInt32 KNewSetting5Value = 2389;
const TReal KNewSetting6Value = 23.89;
_LIT(KNewSetting7Value, "another unicode str");
_LIT8(KNewSetting8Value, "another ascii str");


//
// Test repository 2
//

const TInt KIntB1 = 807;
const TInt KIntB1_InitialValue = 100;



///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		CleanupCDriveL();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void KillCentRepServerL()
	{
	_LIT(KCentralRepositoryServerName, "Centralrepositorysrv");
	_LIT(KProcessKillProcess, "t_processkillprocess.exe");
	
	TRequestStatus stat;    
	RProcess p;
	User::LeaveIfError(p.Create(KProcessKillProcess, KCentralRepositoryServerName));

	// Asynchronous logon: completes when process terminates with process 
	// exit code
	p.Logon(stat);
	p.Resume();
	User::WaitForRequest(stat);

	TExitType exitType = p.ExitType();
	TInt exitReason = p.ExitReason();
	
	_LIT(KKillServerFailed, "Killing Central Repository Server Failed");
	__ASSERT_ALWAYS(exitType == EExitKill, User::Panic(KKillServerFailed, exitReason));

	p.Close();
	User::LeaveIfError(exitReason);
	}

//This function resets the KUidTestRepository1 repository
//to its intial state to remove any changes made to the repository during
//previous tests
LOCAL_C void ResetTestRepositoryL()
	{
	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository1));
	TInt r = repository->Reset();
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(repository);
	}

//This function restores the state of the files required for this test
//Existing files are deleted and then the required files are copied
//back from the Z drive to the c drive
LOCAL_C void RestoreTestFilesL()
	{
	//Delete all files from C:\\private\\10202BE9\\persists\\ dir
	//and C:\\private\\10202BE9\\ dir
	CleanupCDriveL();

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

//	_LIT(KPersistTargetPath, "c:\\private\\10202BE9\\persists\\*.txt");
	_LIT(KPersistTargetPath, "z:\\private\\10202BE9\\*.txt");
	_LIT(KPersistSourcePath, "Z:\\private\\10202BE9\\*.txc");
	_LIT(KInstallTargetPath, "C:\\private\\10202BE9\\*.txt");
	_LIT(KInstallSourcePath, "Z:\\private\\10202BE9\\*.txi");

	//copy test files from Z: to C:
	CopyTestFilesL(*fm,KPersistSourcePath, KPersistTargetPath);
	CopyTestFilesL(*fm,KInstallSourcePath, KInstallTargetPath);

	CleanupStack::PopAndDestroy(2);

	}


/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1297
@SYMTestCaseDesc		Tests for creation and opening a central repository
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::NewLC(),CRepository::Get() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void OpenRepositoryL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1297 "));
	CRepository* repositoryA;
	CRepository* repositoryB;

	TInt r;

	// When an attempt is made to open a central repository in this test the result will
	// be that a new session (+ subsession) will be created with the server. For subsequent
	// central repository openings subsessions are then created under the same server.
	// Note that by placing the CRepository creation calls in the following order we
	// are exercising among other things the following scenarios:
	// 1. Session is opened and first subsession is created. Error occurs, whole session
	// is pulled down.
	// 2. Session is opened and first subsession is successfully created. 2nd subsession
	// is opened. Error occurs with 2nd subsession. 2nd subsession is pulled down. Session
	// and first sub-sesssion remain up. (DEF105089)

	TRAP(r, CRepository::NewL(TUid::Null()));
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrNotFound);

	repositoryB = CRepository::NewLC(KUidTestRepository2);

	TRAP(r, CRepository::NewL(KUidCorruptRepository));
	if(OomTesting && r==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST2(r, KErrCorrupt);

 	repositoryA = CRepository::NewLC(KUidTestRepository1);

	TInt a;
	r = repositoryA->Get(KInt1, a);
	TEST2(r, KErrNone);
	TEST(a==KInt1_InitialValue);

	r = repositoryB->Get(KIntB1, a);
	TEST2(r, KErrNone);
	TEST(a==KIntB1_InitialValue);

	CleanupStack::PopAndDestroy(repositoryA);

	r = repositoryB->Get(KIntB1, a);
	TEST2(r, KErrNone);
	TEST(a==KIntB1_InitialValue);

	CleanupStack::PopAndDestroy(repositoryB);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1298
@SYMTestCaseDesc		Tests for CRepository's Integer,Real,String's Get and Set functions
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::Get(),CRepository::Set() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void GetSetL(const TUid& aUid)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1298 "));
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	_LIT8(KString12_InitialValue, "string");
	TPtrC8 p8(KString12_InitialValue);

	_LIT(KString11_InitialValue, "string");
	TPtrC p16(KString11_InitialValue);

	TBuf8<20> str8;
	TBuf<20> str;
	TInt i = 0;

	//
	// Integer get/set
	//

	TInt r = repository->Get(KNonExisitentSetting, i);
	TEST2(r, KErrNotFound);
	TEST(i==0);

	r = repository->Get(KReal1, i);
	TEST2(r, KErrArgument);
	TEST(i==0);

	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_InitialValue);

	r = repository->Get(KNonExisitentSetting, i);
	TEST2(r, KErrNotFound);

	r = repository->Set(KNonExisitentSetting, 10);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNonExisitentSetting, i);
	TEST2(r, KErrNone);
	TEST(i==10);

	r = repository->Delete(KNonExisitentSetting);
	TEST2(r, KErrNone);

	r = repository->Set(KReal1, 0);
	TEST2(r, KErrArgument);

	r = repository->Set(KInt1, KInt1_UpdatedValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);

	r = repository->Get(KInt1,str8);
	TEST2(r, KErrArgument);
	//checks if KInt was modified
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);

	r = repository->Set(KInt1, KString12_InitialValue);
	TEST2(r, KErrArgument);
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);

	r = repository->Get(KInt1,str);
	TEST2(r, KErrArgument);
	//checks if KInt was modified
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);

	r = repository->Set(KInt1, KString11_InitialValue);
	TEST2(r, KErrArgument);
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_UpdatedValue);
	//
	// Real get/set
	//

	TReal y = 0;

	r = repository->Get(KNonExisitentSetting, y);
	TEST2(r, KErrNotFound);
	TEST(y==0);

	r = repository->Get(KInt1, y);
	TEST2(r, KErrArgument);
	TEST(y==0);

	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_InitialValue);

	r = repository->Get(KNonExisitentSetting, y);
	TEST2(r, KErrNotFound);

	r = repository->Set(KNonExisitentSetting, 0.0);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNonExisitentSetting, y);
	TEST2(r, KErrNone);
	TEST(y == 0.0);

	r = repository->Delete(KNonExisitentSetting);
	TEST2(r, KErrNone);

	r = repository->Set(KInt1, 0.0);
	TEST2(r, KErrArgument);

	r = repository->Set(KReal1, KReal1_UpdatedValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	r = repository->Get(KReal1,str8);
	TEST2(r, KErrArgument);
	//checks if KInt was modified
	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	r = repository->Get(KReal1,str);
	TEST2(r, KErrArgument);
	//checks if KInt was modified
	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	r = repository->Set(KReal1, KString12_InitialValue);
	TEST2(r, KErrArgument);
	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	r = repository->Set(KReal1, KString11_InitialValue);
	TEST2(r, KErrArgument);
	r = repository->Get(KReal1, y);
	TEST2(r, KErrNone);
	TEST(y==KReal1_UpdatedValue);

	//
	// String get/set
	//
	r = repository->Get(KNonExisitentSetting, str);
	TEST2(r, KErrNotFound);
	TEST(str.Length()==0);

	r = repository->Get(KInt1, str);
	TEST2(r, KErrArgument);
	TEST(str.Length()==0);

	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_InitialValue);

	TBuf<10> tooShort;
	r = repository->Get(KString1, tooShort);
	TEST2(r, KErrOverflow);
	TEST(tooShort.Length()==10);

	r = repository->Get(KNonExisitentSetting, str);
	TEST2(r, KErrNotFound);

	r = repository->Set(KNonExisitentSetting, KString1_UpdatedValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNonExisitentSetting, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	r = repository->Delete(KNonExisitentSetting);
	TEST2(r, KErrNone);

	r = repository->Set(KInt1, str);
	TEST2(r, KErrArgument);

	r = repository->Set(KString1, KString1_UpdatedValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	//testing the new feature: creating an 8 bit string from ini file.
	r = repository->Get(KString12, str8);
	TEST2(r, KErrNone);
	TEST(str8==KString12_InitialValue);

	//get 8-bit string with int
	r = repository->Get(KString12, i);
	TEST2(r, KErrArgument);
	r = repository->Get(KString12, str8);
	TEST2(r, KErrNone);
	TEST(str8==KString12_InitialValue);

	//get 8-bit string with real
	r = repository->Get(KString12, y);
	TEST2(r, KErrArgument);
	r = repository->Get(KString12, str8);
	TEST2(r, KErrNone);
	TEST(str8==KString12_InitialValue);

	//set 8-bit string with int
	r = repository->Set(KString12, i);
	TEST2(r, KErrArgument);
	r = repository->Get(KString12, str8);
	TEST2(r, KErrNone);
	TEST(str8==KString12_InitialValue);

	//set 8-bit string with real
	r = repository->Set(KString12, y);
	TEST2(r, KErrArgument);
	r = repository->Get(KString12, str8);
	TEST2(r, KErrNone);
	TEST(str8==KString12_InitialValue);


	//get 16-bit string with int
	r = repository->Get(KString1,i);
	TEST2(r, KErrArgument);
	//checks if a string was modified
	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	//get 16-bit string with real
	r = repository->Get(KString1,y);
	TEST2(r, KErrArgument);
	//checks if a string was modified
	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	//set 16-bit string with int
	r = repository->Set(KString1,i);
	TEST2(r, KErrArgument);
	//checks if a string was modified
	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	//set 16-bit string with real
	r = repository->Set(KString1,y);
	TEST2(r, KErrArgument);
	//checks if a string was modified
	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);

	const TInt KStringLengthTooBig = KMaxUnicodeStringLength  + 1;
	TBuf<KStringLengthTooBig> tooBig;
	tooBig.SetLength(KStringLengthTooBig);
	r = repository->Set(KString1, tooBig);
	TEST2(r, KErrArgument);

	//- Test null descriptors
	r = repository->Set(KString1, KNullDesC);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KNullDesC);

	r = repository->Set(KString1, KString1_UpdatedValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check set is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_UpdatedValue);
	//-

	// Check that the SetGetParameters interface returns KErrNotSupported in the default macro configuration
#if !defined(__CENTREP_SERVER_PERFTEST__) && !defined(__CENTREP_SERVER_MEMTEST__) && !defined(__CENTREP_SERVER_CACHETEST__)
	TIpcArgs args;
	r = SetGetParameters(args);
	TEST2(r, KErrNotSupported);
#endif

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1299
@SYMTestCaseDesc		Tests for CRepository class's find operations
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::FindL(),CRepository::FindEqL(),CRepository::FindNeqL() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void FindL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1299 "));
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository1));

	RArray<TUint32> foundIds;

	TInt r = KErrNone;
	TInt leaveCode = KErrNone;
	TRAP(leaveCode, r = repository->FindL(0, 0, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;

	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}

	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);
	foundIds.Reset();

	/***************************************************************************************/
	//Tests for FindL
	/***************************************************************************************/
	TRAP(leaveCode, r = repository->FindL(23, 0, foundIds));  // 23 - just a random number, value is not important
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
	{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==KNumSettings);
	foundIds.Reset();


	TRAP(leaveCode, r=repository->FindL(0, 2, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
	{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==6);
	foundIds.Reset();


	TRAP(leaveCode, r=repository->FindL(4, 6, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==2);
	foundIds.Reset();



	TRAP(leaveCode, r= repository->FindL(15, 15, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);


	/***************************************************************************************/
	//Tests for FindEqL for Int
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindEqL(0, 0, KInt1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);
	TEST(foundIds[0]==KInt1);
	foundIds.Reset();


	TRAP(leaveCode, r= repository->FindEqL(0, 0, 0, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);


	/***************************************************************************************/
	//Tests for FindEqL for Real
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindEqL(0, 0, KReal2_InitialValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==2);
	TEST(foundIds[0]==KReal2);
	TEST(foundIds[1]==KReal3);
	foundIds.Reset();


	TRAP(leaveCode, r= repository->FindEqL(0, 0, 7.7, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);


	/***************************************************************************************/
	//Tests for FindEqL for String
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindEqL(0, 0, KString1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);
	TEST(foundIds[0]==KString1);
	foundIds.Reset();

	_LIT(KFoo, "foo");

	TRAP(leaveCode, r= repository->FindEqL(0, 0, KFoo, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);

	/***************************************************************************************/
	//Tests for FindEqL for String8
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindEqL(0, 0, KString2_8BitValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);
	TEST(foundIds[0]==KString2);
	foundIds.Reset();

	_LIT8(KFoo8, "foo");

	TRAP(leaveCode, r= repository->FindEqL(0, 0, KFoo8, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);

	/***************************************************************************************/
	//Tests for FindNeqL for Int
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindNeqL(0, 0, KInt1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==8);
	TEST2(foundIds.Find(KInt1), KErrNotFound);
	foundIds.Reset();


	TRAP(leaveCode, r= repository->FindNeqL(25, 25, KInt1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);


	/***************************************************************************************/
	//Tests for FindNeqL for Real
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindNeqL(0, 0, KReal1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==8);
	TEST2(foundIds.Find(KReal1), KErrNotFound);
	foundIds.Reset();


	TRAP(leaveCode,r= repository->FindNeqL(25, 25, KReal1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);


	/***************************************************************************************/
	//Tests for FindNeqL for String
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindNeqL(0, 0, KString1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==8);
	TEST2(foundIds.Find(KString1), KErrNotFound);
	foundIds.Reset();


	TRAP(leaveCode, r= repository->FindNeqL(25, 25, KString1_UpdatedValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);

	/***************************************************************************************/
	//Tests for FindNeqL for String8
	/***************************************************************************************/
	TRAP(leaveCode, r= repository->FindNeqL(0, 0, KString2_8BitValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==8);
	TEST2(foundIds.Find(KString2), KErrNotFound);
	foundIds.Reset();


	TRAP(leaveCode, r= repository->FindNeqL(25, 25, KString2_8BitValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1300
@SYMTestCaseDesc		CRepository class functionality test
@SYMTestPriority		High
@SYMTestActions			Test for CRepository::NotifyRequest() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void NotifyL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1300 "));
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository1));

	TInt r = KErrNone;
	TInt intval;
	TRequestStatus intStatus;
	TRequestStatus realStatus;
	TRequestStatus stringStatus;
	RThread thisThread;

	//
	// Notification on non-existent setting
	//
	r = repository->NotifyRequest(KNonExisitentSetting, intStatus);
	TEST2(r, KErrNotFound);

	//
	// Basic notification
	//
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);

	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);

	r = repository->NotifyRequest(KString1, stringStatus);
	TEST2(r, KErrNone);


	// Setting to the same value should not cause a notification
	r = repository->Get(KInt1, intval);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, intval);
	TEST2(r, KErrNone);
	TEST(intStatus==KRequestPending);
	TEST(realStatus==KRequestPending);
	TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	// First change to setting should cause notification
	r = repository->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KInt1);
	TEST(realStatus==KRequestPending);
	TEST(stringStatus==KRequestPending);

	// Second change to setting should not cause notification
	intStatus = 7777;
	r = repository->Set(KInt1, 0);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==KRequestPending);
	TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);


	// Setting to the same value should not cause a notification
	TReal realval;
	r = repository->Get(KReal1, realval);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, realval);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==KRequestPending);
	TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	r = repository->Set(KReal1, 0.0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==7777);
	TEST(realStatus==KReal1);
	TEST(stringStatus==KRequestPending);


	// Setting to the same value should not cause a notification
	realStatus = 8888;
	TBuf<20> stringval;
	r = repository->Get(KString1, stringval);
	TEST2(r, KErrNone);
	r = repository->Set(KString1, stringval);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==8888);
	TEST(stringStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	_LIT(KStringValue2, "string2");
	r = repository->Set(KString1, KStringValue2);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==7777);
	TEST(realStatus==8888);
	TEST(stringStatus==KString1);


	//
	// Cancelling single notifications
	//
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(KInt1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	intStatus = 7777;
	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, 1);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, 1.1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==7777);
	TEST(realStatus==KReal1);
	TEST(thisThread.RequestCount()==0);


	//
	// Cancelling all notifications
	//
	realStatus = 8888;
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyRequest(KReal1, realStatus);
	TEST2(r, KErrNone);

	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
	TEST(realStatus==KUnspecifiedKey);

	intStatus = 7777;
	realStatus = 8888;
	r = repository->Set(KInt1, 2);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, 2.2);
	TEST2(r, KErrNone);
	TEST(intStatus==7777);
	TEST(realStatus==8888);
	TEST(thisThread.RequestCount()==0);


	//
	// Group notifications
	//
	r = repository->NotifyRequest(0, ~(KInt1+KReal1), intStatus);
	TEST2(r, KErrNone);

	r = repository->Set(KInt3, 3);
	TEST2(r, KErrNone);
	TEST(intStatus==KRequestPending);
	TEST(thisThread.RequestCount()==0);

	r = repository->Set(KInt1, 3);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KInt1);

	r = repository->NotifyRequest(0, ~(KInt1+KReal1), intStatus);
	TEST2(r, KErrNone);

	r = repository->Set(KReal1, 3.3);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KReal1);


	//
	// Cancelling group notifications
	//
	r = repository->NotifyRequest(0, ~(KInt1+KReal1), intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(0, ~(KInt1+KReal1));
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	r = repository->Set(KInt1, 3);
	TEST2(r, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	TEST(thisThread.RequestCount()==0);

	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyRequest(0, ~(KInt1+KReal1), realStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
	TEST(realStatus==KUnspecifiedKey);

	r = repository->Set(KInt1, 4);
	TEST2(r, KErrNone);
	TEST(intStatus==KUnspecifiedKey);
	TEST(realStatus==KUnspecifiedKey);
	TEST(thisThread.RequestCount()==0);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID         	SYSLIB-CENTRALREPOSITORY-CT-3400
@SYMTestCaseDesc        CRepository class functionality test
@SYMTestPriority        High
@SYMTestActions         Negative tests exercising CRepository::NotifyCancel() functions
						Tests check that central repository notifier works properly when
						client cancels multiple, non-existent notifications, and also
						multiple and non-existent group notificatins in a variety of combinations.
@SYMTestExpectedResults There are 2 main points that we need to check in this test:
						1) when we are cancelling multiple notifications server-side function should
						always return KErrNone or KErrNotFound preventing client requests from waitng
						indefinitely
						2) IPC handling function should convert any error returned from the server
						to KErrNone so that the client-side function NotifyCancel always returns KErrNone
						(for more nformation on this requirement see DEF061504)
@SYMDEF               	INC102413
*/
LOCAL_C void CancelNotificationsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3400 "));
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidTestRepository1));

	TInt r = KErrNone;
	TRequestStatus intStatus;
	TRequestStatus realStatus1;
	TRequestStatus realStatus2;
	TRequestStatus realStatus3;
	RThread thisThread;

	//
	// Cancelling non-existent notifications
	//
	r = repository->NotifyCancel(KInt1);
	TEST2(r, KErrNone);

	//
	// Cancelling multiple notifications
	// This test step checks that NotifyCancel function returns correct
	// value KErrNone regardless of the number of registered notifiers
	//
	r = repository->NotifyRequest(KReal1, realStatus1);
	TEST2(r, KErrNone);
	r = repository->NotifyRequest(KReal2, realStatus2);
	TEST2(r, KErrNone);
	r = repository->NotifyRequest(KReal3, realStatus3);
	TEST2(r, KErrNone);

	r = repository->NotifyCancel(KReal3);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(KReal2);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(KReal1);
	TEST2(r, KErrNone);
	TEST2(thisThread.RequestCount(),3);
	User::WaitForAnyRequest();
	TEST(realStatus3==KUnspecifiedKey);
	User::WaitForAnyRequest();
	TEST(realStatus2==KUnspecifiedKey);
	User::WaitForAnyRequest();
	TEST(realStatus1==KUnspecifiedKey);
	TEST2(thisThread.RequestCount(),0);

	// check that all notificatioins were actually cancelled and we will not
	// get a notification when we change the settings
	realStatus1 = 7777;
	realStatus2 = 8888;
	realStatus3 = 9999;
	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->Set(KReal1, 1.0);
	TEST2(r, KErrNone);
	r = repository->Set(KReal2, 2.0);
	TEST2(r, KErrNone);
	r = repository->Set(KReal3, 3.0);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, 2);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(realStatus1==7777);
	TEST(realStatus2==8888);
	TEST(realStatus3==9999);
	TEST(intStatus==KInt1);
	TEST2(thisThread.RequestCount(),0);

	//
	// Cancelling all notifications when no notifications exist
	//
	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);

	//
	// Cancelling group notifications where some of the notifications have not been set up
	//
	//first make sure that all the settings exist
	r = repository->Set(KReal1, 2.0);
	r = repository->Set(KReal2, 3.0);
	r = repository->Set(KReal3, 4.0);
	//This mask notify request sets up notifies for a number of keys including KReal1 and KReal2
	r = repository->NotifyRequest(0, ~(KReal1+KReal2), realStatus1);
	TEST2(r, KErrNone);
	//This mask notify request cancels notifies for a number of keys including KReal1 and KReal3
	r = repository->NotifyCancel(0, ~(KReal1+KReal3));
	TEST2(r, KErrNone);
	//check that notification request for the setting KReal1 has not been cancelled
	r = repository->Set(KReal1, 1.0);
	User::WaitForAnyRequest();
	TEST2(r, KErrNone);
	TEST(realStatus1==KReal1);

	//
	// Cancelling multiple group notifications
	//
	//set up notification requests for 2 different groups of settings and then cancel them
	// one by one in the reverse order checking that we get an expected return value
	//This mask notify request sets up notifies for a number of keys including KInt1 and KReal3
	r = repository->NotifyRequest(0, ~(KInt1+KReal3), realStatus1);
	TEST2(r, KErrNone);
	//This mask notify request sets up notifies for a number of keys including KReal1 and KReal2
	r = repository->NotifyRequest(0, ~(KReal1+KReal2), realStatus2);
	TEST2(r, KErrNone);

	//This mask notify request cancels notifies for a number of keys including KReal1 and KReal2
	r = repository->NotifyCancel(0, ~(KReal1+KReal2));
	TEST2(r, KErrNone);
	//This mask notify request cancels notifies for a number of keys including KInt1 and KReal3
	r = repository->NotifyCancel(0, ~(KInt1+KReal3));
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	User::WaitForAnyRequest();
	TEST(realStatus1==KUnspecifiedKey);
	TEST(realStatus2==KUnspecifiedKey);

	//
	// Cancelling non-existent group notifications
	//
	r = repository->NotifyCancel(0, ~(KInt1+KReal3));
	TEST2(r, KErrNone);

	//restore the repository value for the later tests to use
	r = repository->Set(KInt1, 1);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1301
@SYMTestCaseDesc		Tests for creation and deletion of new settings for CRepository class
@SYMTestPriority		High
@SYMTestActions			Test for CRepository::Create(),CRepository::Delete() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void CreateDeleteL(const TUid& aUid)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1301 "));
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	TInt x;
	TReal y;
	TBuf8<20> z8;
	TBuf16<20> z16;

	/***************************************************************************************/
	//test int create
	/***************************************************************************************/
	TInt r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	const TInt KIntValue = 1234;
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNone);
	TEST(x==KIntValue);

	r = repository->Delete(KNewInt);
	TEST2(r, KErrNone);

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));
	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);


	// test int create - second run
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNone);
	TEST(x==KIntValue);

	r = repository->Delete(KNewInt);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewInt, x);
	TEST2(r, KErrNotFound);


	/***************************************************************************************/
	//test real create
	/***************************************************************************************/

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNotFound);

	const TReal KRealValue = 5678.0;
	r = repository->Create(KNewReal, KRealValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNone);
	TEST(y==KRealValue);

	r = repository->Delete(KNewReal);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNotFound);

	//create real - second run
	r = repository->Create(KNewReal, KRealValue);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewReal, y);
	TEST2(r, KErrNone);
	TEST(y==KRealValue);

	r = repository->Delete(KNewReal);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewReal, x);
	TEST2(r, KErrNotFound);

	/***************************************************************************************/
	//test string8 create
	/***************************************************************************************/

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNotFound);

	_LIT8(KString8Value, "ABCDEF");
	r = repository->Create(KNewString8, KString8Value);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNone);
	TEST(z8==KString8Value);

	r = repository->Delete(KNewString8);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewString8, KString8Value);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString8, z8);
	TEST2(r, KErrNone);
	TEST(z8==KString8Value);

	r = repository->Delete(KNewString8);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	//second run
	r = repository->Get(KNewString8, x);
	TEST2(r, KErrNotFound);

	/***************************************************************************************/
	//test string16 create
	/***************************************************************************************/

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNotFound);

	_LIT(KString16Value, "ghijklmn");
	r = repository->Create(KNewString16, KString16Value);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNone);
	TEST(z16==KString16Value);

	r = repository->Delete(KNewString16);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNotFound);

	r = repository->Create(KNewString16, KString16Value);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString16, z16);
	TEST2(r, KErrNone);
	TEST(z16==KString16Value);

	r = repository->Delete(KNewString16);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	r = repository->Get(KNewString16, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repository);

	/***************************************************************************************/
	//test create all kinds and delete with mask
	/***************************************************************************************/

	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	//first check that none of the settings exist in the repository
	r = repository->Get(KNewSetting1, x);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting2, y);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting3, z8);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting4, z16);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting5, x);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting6, y);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting7, z8);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting8, z16);
	TEST2(r, KErrNotFound);

	//now create all of the new settings
	x = KNewSetting1Value;
	r = repository->Create(KNewSetting1, x);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting2, KNewSetting2Value);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting3, KNewSetting3Value);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting4, KNewSetting4Value);
	TEST2(r, KErrNone);

	x = KNewSetting5Value;
	r = repository->Create(KNewSetting5, x);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting6, KNewSetting6Value);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting7, KNewSetting7Value);
	TEST2(r, KErrNone);

	r = repository->Create(KNewSetting8, KNewSetting8Value);
	TEST2(r, KErrNone);

	//close and reopen rep to check create is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	//check that the new settings exist in the repository
	r = repository->Get(KNewSetting1, x);
	TEST(x == KNewSetting1Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting2, y);
	TEST(y == KNewSetting2Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting3, z16);
	TEST(z16 == KNewSetting3Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting4, z8);
	TEST(z8 == KNewSetting4Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting5, x);
	TEST(x == KNewSetting5Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting6, y);
	TEST(y == KNewSetting6Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting7, z16);
	TEST(z16 == KNewSetting7Value);
	TEST2(r, KErrNone);

	r = repository->Get(KNewSetting8, z8);
	TEST(z8 == KNewSetting8Value);
	TEST2(r, KErrNone);

	// delete the settings
	TUint32 errorKey = 0;
	r = repository->Delete(0x00002000, 0xFFFFFF00, errorKey);
	TEST2(errorKey, KUnspecifiedKey);
	TEST2(r, KErrNone);

	// close and reopen rep to check delete is persisted
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(aUid));

	//check that none of the settings exist in the repository
	r = repository->Get(KNewSetting1, x);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting2, y);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting3, z16);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting4, z8);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting5, x);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting6, y);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting7, z16);
	TEST2(r, KErrNotFound);

	r = repository->Get(KNewSetting8, z8);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1302
@SYMTestCaseDesc		Multi client test
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::Get(),CRepository::Set() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void MultiClientL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1302 "));
	CRepository* repository1;
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));
	CRepository* repository2;
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository1));

	//
	// Get/Set
	//
	TInt i;
	TInt r = repository1->Set(KInt1, 0);
	TEST2(r, KErrNone);
	r = repository2->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==0);

	r = repository2->Set(KInt1, 123);
	TEST2(r, KErrNone);
	r = repository1->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==123);

	//
	// Notification
	//
	TRequestStatus status;
	r = repository1->NotifyRequest(KInt1, status);
	TEST2(r, KErrNone);

	r = repository2->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status==KInt1);

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1303
@SYMTestCaseDesc		Tests for resetting the new changes on CRepository
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::Reset() function
						Tests for reset on a single setting that exists in the original ROM-based settings.
						Tests for reset for a single setting that does not exist in the original ROM-based settings.
						Tests for repository-wide reset for a repository that exists on ROM.
						Tests for repository-wide reset for a repository that does not exist on ROM.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void ResetL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1303 "));
	CRepository* repositoryA;
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidTestRepository1));

	//
	// Test reset on a single setting that exists in the
	// original ROM-based settings (we'll use KInt1)
	//

	// Ensure KInt1 is set to a different value to its initial value
	TInt r = repositoryA->Set(KInt1, KInt1_InitialValue+10);
	TEST2(r, KErrNone);

	// We're testing we get a notification on a reset as well
	TRequestStatus s;
	r = repositoryA->NotifyRequest(KInt1, s);
	TEST2(r, KErrNone);

	r = repositoryA->Reset(KInt1);
	TEST2(r, KErrNone);

	// Check we got a notifiation
	RThread thisThread;

	TEST2(thisThread.RequestCount(), 1);
	User::WaitForAnyRequest();
	TEST(s==KInt1);

	// Check KInt1 now has the right value
	TInt x;
	r = repositoryA->Get(KInt1, x);
	TEST2(r, KErrNone);
	TEST(x==KInt1_InitialValue);

	// A second reset should not generate notification
	// as value has not changed
	r = repositoryA->NotifyRequest(KInt1, s);
	TEST2(r, KErrNone);
	r = repositoryA->Reset(KInt1);
	TEST2(r, KErrNone);

	//
	// Test reset for a single setting that does not exist
	// in the original ROM-based settings.
	//
	const TInt KIntValue = 1234;
	r = repositoryA->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	// We're testing we get a notification on a reset as well
	r = repositoryA->NotifyRequest(KNewInt, s);
	TEST2(r, KErrNone);

	r = repositoryA->Reset(KNewInt);
	TEST2(r, KErrNone);

	// Check we got a notifiation
	TEST2(thisThread.RequestCount(), 1);
	User::WaitForAnyRequest();
	TEST(s==KNewInt);

	// Check KNewInt has been deleted
	r = repositoryA->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repositoryA);
	User::WaitForAnyRequest(); //cancellation of request
	TEST2(thisThread.RequestCount(), 0);

	//
	// Test reset for a single setting in a repository that
	// does not exist on ROM.
	//
	const TInt KSettingKey = KNonExisitentSetting;
	const TInt KInitialValue = 10;

	CRepository* repositoryB;
	User::LeaveIfNull(repositoryB = CRepository::NewLC(KUidDriveCOnlyRepository));

	r = repositoryB->Create(KSettingKey, KInitialValue);
	TEST2(r, KErrNone);

	// We're testing we get a notification on a reset as well
	r = repositoryB->NotifyRequest(KSettingKey, s);
	TEST2(r, KErrNone);

	r = repositoryB->Reset(KSettingKey);
	TEST2(r, KErrNone);

	// Check we got a notifiation
	TEST2(thisThread.RequestCount(), 1);
	User::WaitForAnyRequest();
	TEST(s==KSettingKey);

	// Check KSettingKey has been deleted
	r = repositoryB->Get(KSettingKey, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repositoryB);
	//
	// Test repository-wide reset for a repository
	// that exists on ROM.
	//
	CRepository* repositoryC;
	User::LeaveIfNull(repositoryC = CRepository::NewLC(KUidResetTestRepository));

	const TInt KNewSetting1 = 5;
	const TInt KNewSetting2 = 6;
	r = repositoryC->Create(KNewSetting1, 0);
	TEST2(r, KErrNone);
	r = repositoryC->Create(KNewSetting2, 0);
	TEST2(r, KErrNone);

	r = repositoryC->Create(65, 1);
	TEST2(r, KErrNone);

	r = repositoryC->Set(70, 1);
	TEST2(r, KErrNone);

	r = repositoryC->Create(80, 1);
	TEST2(r, KErrNone);

	r = repositoryC->Create(90, 1);
	TEST2(r, KErrNone);

	r = repositoryC->NotifyRequest(0, 0, s);
	TEST2(r, KErrNone);

	r = repositoryC->Reset();
	TEST2(r, KErrNone);
	TEST(thisThread.RequestCount()==1);
	User::WaitForAnyRequest();
	TEST(s==KUnspecifiedKey);

	r = repositoryC->Get(10, x);
	TEST2(r, KErrNone);
	TEST(x==10);

	r = repositoryC->Get(40, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repositoryC->Get(50, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repositoryC->Get(60, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	r = repositoryC->Get(70, x);
	TEST2(r, KErrNone);
	TEST(x==0);

	TBuf<10> z;
	r = repositoryC->Get(20, z);
	TEST2(r, KErrNone);
	TEST(z==_L("string"));

	TReal y;
	r = repositoryC->Get(30, y);
	TEST2(r, KErrNone);
	TEST(y==1);

	r = repositoryC->Get(5, x);
	TEST2(r, KErrNotFound);
	r = repositoryC->Get(6, x);
	TEST2(r, KErrNotFound);
	r = repositoryC->Get(65, x);
	TEST2(r, KErrNotFound);
	r = repositoryC->Get(80, x);
	TEST2(r, KErrNotFound);
	r = repositoryC->Get(90, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repositoryC);

	//
	// Test repository-wide reset for a repository
	// that does not exist on ROM.
	//
	CRepository* repositoryD;
	User::LeaveIfNull(repositoryD = CRepository::NewLC(KUidDriveCOnlyRepository));

	r = repositoryD->Create(KSettingKey, KInitialValue);
	TEST2(r, KErrNone);

	// We're testing we get a notification on a reset as well
	r = repositoryD->NotifyRequest(KSettingKey, s);
	TEST2(r, KErrNone);

	r = repositoryD->Reset();
	TEST2(r, KErrNone);

	// Check we got a notifiation
	TEST(thisThread.RequestCount()==1);
	User::WaitForAnyRequest();
	TEST(s==KUnspecifiedKey);

	// Check KSettingKey has been deleted
	r = repositoryD->Get(KSettingKey, x);
	TEST2(r, KErrNotFound);

	// Recreate KSettingKey
	r = repositoryD->Create(KSettingKey, 10);
	TEST2(r, KErrNone);

	// Reset KSettingKey only
	r = repositoryD->Reset(KSettingKey);
	TEST2(r, KErrNone);

	// Check KSettingKey has been deleted
	r = repositoryD->Get(KSettingKey, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repositoryD);
	}

LOCAL_C void ResetCreL()
	{
	CRepository* repositoryA;
	TInt r = KErrNone;
	TRequestStatus s;
	TInt x;
	RThread thisThread;

	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// test for .cre file
	// Test reset on a single setting that exists in the
	// original ROM-based settings (we'll use KInt1)
	//

	// Ensure KInt1 is set to a different value to its initial value
	r = repositoryA->Set(KInt1, KInt1_InitialValue+10);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repositoryA);
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// We're testing we get a notification on a reset as well
	r = repositoryA->NotifyRequest(KInt1, s);
	TEST2(r, KErrNone);

	r = repositoryA->Reset(KInt1);
	TEST2(r, KErrNone);

	// Check we got a notifiation
	TEST2(thisThread.RequestCount(), 1);
	User::WaitForAnyRequest();
	TEST(s == KInt1);

	CleanupStack::PopAndDestroy(repositoryA);
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// Check KInt1 now has the right value
	r = repositoryA->Get(KInt1, x);
	TEST2(r, KErrNone);
	TEST2(x, KInt1_InitialValue);

	// A second reset should not generate notification
	// as value has not changed
	r = repositoryA->NotifyRequest(KInt1, s);
	TEST2(r, KErrNone);

	r = repositoryA->Reset(KInt1);
	TEST2(r, KErrNone);
	TEST2(thisThread.RequestCount(), 0);

	CleanupStack::PopAndDestroy(repositoryA);
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));
	//
	// Test reset for a single setting that does not exist
	// in the original ROM-based settings.
	//
	const TInt KIntValue = 1234;
	r = repositoryA->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repositoryA);
	User::WaitForAnyRequest();//this is the cancellation of last notify request
	TEST2(thisThread.RequestCount(), 0);

	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// We're testing we get a notification on a reset as well
	r = repositoryA->NotifyRequest(KNewInt, s);
	TEST2(r, KErrNone);

	r = repositoryA->Reset(KNewInt);
	TEST2(r, KErrNone);

	// Check we got a notifiation
	TEST2(thisThread.RequestCount(), 1);
	User::WaitForAnyRequest();
	TEST(s == KNewInt);

	CleanupStack::PopAndDestroy(repositoryA);
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// Check KNewInt has been deleted
	r = repositoryA->Get(KNewInt, x);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repositoryA);
	User::LeaveIfNull(repositoryA = CRepository::NewLC(KUidCreTestRepository1));

	// Reset entire repository
	r = repositoryA->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repositoryA);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1304
@SYMTestCaseDesc		Tests for initialising file searching
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::Get() function
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void IniFileSearching()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1304 "));
	const TInt KSettingKey = 1;

	_LIT(KDriveZ, "drive z");

	const TInt KBufLen = 7; // = Max of 3 above string lengths

	//
	// File on drive C should take precedence
	//
	CRepository* repository = CRepository::NewL(KUidDriveCRepository);

	TBuf<KBufLen> str;
	TInt r = repository->Get(KSettingKey, str);
	TEST2(r, KErrNone);
	TEST(str==KDriveZ); 

	delete repository;
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1305
@SYMTestCaseDesc		Tests for creating new repository on drive C:
@SYMTestPriority		High
@SYMTestActions			Tests for CRepository::NewL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void RepositoryOnDriveC()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1305 "));
	const TInt KSettingKey = KNonExisitentSetting;
	const TInt KInitialValue = 10;

	CRepository* repository = CRepository::NewL(KUidDriveCOnlyRepository);
	CleanupStack::PushL(repository);

	TInt val;
	TInt r = repository->Get(KSettingKey, val);
	TEST2(r, KErrNotFound);

	r = repository->Create(KSettingKey, KInitialValue);
	TEST2(r, KErrNone);

	r = repository->Get(KSettingKey, val);
	TEST2(r, KErrNone);
	TEST(val==KInitialValue);

	CleanupStack::PopAndDestroy(repository);
	repository = CRepository::NewL(KUidDriveCOnlyRepository);
	CleanupStack::PushL(repository);

	r = repository->Get(KSettingKey, val);
	TEST2(r, KErrNone);
	TEST(val==KInitialValue);

	r = repository->Set(KSettingKey, KInitialValue+1);
	TEST2(r, KErrNone);

	r = repository->Get(KSettingKey, val);
	TEST2(r, KErrNone);
	TEST(val==KInitialValue+1);

	r = repository->Delete(KSettingKey);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	repository = CRepository::NewL(KUidDriveCOnlyRepository);
	CleanupStack::PushL(repository);

	r = repository->Get(KSettingKey, val);
	TEST2(r, KErrNotFound);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-1656
@SYMTestCaseDesc		Notify-only client optimization test
@SYMTestPriority		High
@SYMTestActions			Tests for on-demand loading in API calls after in-session
						unloading of repositories. Also tests for notification functionality.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ1228
*/
LOCAL_C void NotifyOnlyL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1656 "));
	CRepository* repository1;
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	// check that functions work
	TInt i;
	TInt r = repository1->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==1);
	
	CleanupStack::PopAndDestroy();
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	r = repository1->Set(KInt1, i);
	TEST2(r, KErrNone);
	
	CleanupStack::PopAndDestroy();
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	const TInt KIntValue = 1234;
	r = repository1->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);
	
	CleanupStack::PopAndDestroy();
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	RArray<TUint32> foundIds;
	TInt leaveCode = KErrNone;
	TRAP(leaveCode, r= repository1->FindEqL(0, 0, KIntValue, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	if(OomTesting && r==KErrNoMemory)
		{
		foundIds.Close();
		User::Leave(KErrNoMemory);
		}
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==1);
	TEST(foundIds[0]==KNewInt);
	foundIds.Reset();

	CleanupStack::PopAndDestroy();
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	r = repository1->Delete(KNewInt);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy();

	// check that multiple clients work
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	r = repository1->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==1);

	CleanupStack::PopAndDestroy();
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));

	CRepository* repository2;
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository1));

	r = repository2->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==1);

	r = repository1->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==1);

	CleanupStack::PopAndDestroy(2);

	// check that notifications work
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidTestRepository1));
	TRequestStatus status1;
	r = repository1->NotifyRequest(KInt1, status1);
	TEST2(r, KErrNone);

	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidTestRepository1));
	r = repository2->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status1==KInt1);
	
	r = repository1->NotifyRequest(KInt1, status1);
	TEST2(r, KErrNone);

	CleanupRepositoryCache();

	r = repository2->Set(KInt1, 2);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status1==KInt1);
	
	r = repository1->NotifyRequest(KInt1, status1);
	TEST2(r, KErrNone);
	
	CRepository* repository3;
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository1));
	TRequestStatus status3;
	r = repository3->NotifyRequest(KInt1, status3);
	TEST2(r, KErrNone);

	r = repository2->Set(KInt1, 0);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status1==KInt1);
	TEST(status3==KInt1);
	
	r = repository1->NotifyRequest(KInt1, status1);
	TEST2(r, KErrNone);
	r = repository3->NotifyRequest(KInt1, status3);
	TEST2(r, KErrNone);

	CleanupRepositoryCache();

	r = repository2->Set(KInt1, 1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(status1==KInt1);
	TEST(status3==KInt1);

	CleanupStack::PopAndDestroy(3);
	// Kill the server to force clearing the cache
	KillCentRepServerL();
	User::LeaveIfNull(repository3 = CRepository::NewLC(KUidTestRepository1));

	r = repository3->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==1);

	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID          PDS-CENTRALREPOSITORY-CT-4113
@SYMTestCaseDesc        Validates meta data is not lost before a transaction is committed
                        when deleting a range of settings.
@SYMTestPriority        High
@SYMTestActions         1) Start a transaction.
                        2) Delete a setting using the delete range function.
                        3) Create a new setting (using the deleted key)
                        4) Commit the transaction.
                        5) Check the metadata of the setting.                     
@SYMTestExpectedResults When deleting a range of settings in a keyspace, and then 
                        creating a new setting (with a key of a previously deleted setting)
                        the meta data should not be reset to 0, it should take on the default
                        metadata, if it exists.
@SYMDEF                 DEF144530
*/
LOCAL_C void RangeDeleteMetaL()
    {
    TheTest.Next(_L(" @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4113 "));

    TUint32 partialKey   = 0x0000004;
    TUint32 mask         = 0xFFFFFFF;
    TUint32 meta = 0;
    TUint32 expectedMeta = 0x10; // 16 
    TUint32 keyCount = 0;
    TUint32 expectedKeyCount = 1;
    TUint32 error;

    CRepository* repository;
    User::LeaveIfNull(repository = CRepository::NewL(KUidTestRepository3)); // 00000103.txt
    
    repository->StartTransaction(CRepository::EReadWriteTransaction);

    // Only want to delete 1 specific key, using the range delete function.
    TInt ret = repository->Delete(partialKey, mask, error);
    // We don't care about 'error' if 'ret==KErrNone'.
    TEST2(ret, KErrNone);  
    
    // Create a new setting that is the same key and type as the deleted one.
    ret = repository->Create(partialKey, 200); 
    TEST2(ret, KErrNone);
    
    ret = repository->CommitTransaction(keyCount);
    TEST2(ret, KErrNone);
    // Confirm only 1 setting was updated.
    TEST2(keyCount, expectedKeyCount);
    
    // Check the meta data of the newly created setting. It should be the 
    // the same as the defaultmeta value in the keyspace.
    ret = repository->GetMeta(partialKey, meta);
    TEST2(ret, KErrNone);
    TEST2(meta, expectedMeta);
    
    delete repository;
    }


/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-0494
@SYMTestCaseDesc		Tests the various functions on CentralRepository
@SYMTestPriority		High
@SYMTestActions			Tests the get/set,find,notify,create/delete,
						file searching in repository,the repository on drive c only
						Tests for multiple clients,for restoring factory settings.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void FuncTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0494 Open/Close repository "));
	OpenRepositoryL();

	TheTest.Next(_L("Get/Set .ini"));
	GetSetL(KUidTestRepository1);

	TheTest.Next(_L("Get/Set .cre"));
	GetSetL(KUidCreTestRepository1);

	TheTest.Next(_L("Find"));
	FindL();

	TheTest.Next(_L("Notify"));
	NotifyL();

	TheTest.Next(_L("Create/Delete .ini"));
	CreateDeleteL(KUidTestRepository1);

	TheTest.Next(_L("Create/Delete .cre"));
	CreateDeleteL(KUidCreTestRepository1);

	TheTest.Next(_L("Initialisation file searching"));
	IniFileSearching();

	TheTest.Next(_L("Repository on Drive C only"));
	RepositoryOnDriveC();

	TheTest.Next(_L("Multiple clients"));
	MultiClientL();

	TheTest.Next(_L("Restore factory settings"));
	ResetL();

	TheTest.Next(_L("Restore factory settings from binary based rep"));
	ResetCreL();

	TheTest.Next(_L("Negative testing for the notification cancelling"));
	CancelNotificationsL();

	TheTest.Next(_L("Notify-only client optimizations"));
	NotifyOnlyL();
	
	TheTest.Next(_L("Meta data after a Range Delete in transaction"));
    RangeDeleteMetaL();
    
	TheTest.End();
	}

/**
OomTest
Function to convert a test into an OOM test

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-0495
@SYMTestCaseDesc		Out of memory test
@SYMTestPriority		High
@SYMTestActions			Tests for running out of memory
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void OomTest(void (*testFuncL)())
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0495 "));
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++count);
		User::__DbgMarkStart(RHeap::EUser);

  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		TRAP(error, (testFuncL)());

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST2(endProcessHandleCount, startProcessHandleCount);
		TEST2(endThreadHandleCount, startThreadHandleCount);

		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(error == KErrNoMemory);

	_LIT(KTestFailed, "Out of memory test failure on iteration %d\n");
	__ASSERT_ALWAYS(error==KErrNone, TheTest.Panic(error, KTestFailed, count));

	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
	}

LOCAL_C void OomTestsL()
	{
	TheTest.Start(_L("Open/Close repository"));
	OomTest(OpenRepositoryL);

	TheTest.Next(_L("GetSet for KUidTestRepository1"));
	GetSetL(KUidTestRepository1);
	TheTest.Next(_L("GetSet for KUidCreTestRepository1"));
	GetSetL(KUidCreTestRepository1);

	TheTest.Next(_L("Find"));
	OomTest(FindL);

	TheTest.End();
	}

//This function tests the replacement of RFile with RFileBuf in CIniFileOut class (inifile.h)
//It shows that there is approx. 20% (LUBBOCK) performance boost when using simple "Set"
//operations. It may be even better when using "Commit" functionality.
static void PerformanceTestL()
	{
	CRepository* repository = CRepository::NewL(KUidDriveCOnlyRepository);
	CleanupStack::PushL(repository);
	//Test settings IDs
	const TUint KIntId = 101;
	const TUint KRealId = 102;
	const TUint KDes16Id = 103;
	const TUint KDes8Id = 104;
	//Create test settings
	TInt err = repository->Create(KIntId, 1);
	TEST2(err, KErrNone);
	err = repository->Create(KRealId, 1.1);
	TEST2(err, KErrNone);
	err = repository->Create(KDes16Id, _L16("DES16"));
	TEST2(err, KErrNone);
	err = repository->Create(KDes8Id, _L8("DES8"));
	TEST2(err, KErrNone);
	//Performance test
	TBuf16<32> buf16;
	TBuf8<32> buf8;
	TUint timeStart = User::TickCount();
	for(TInt i=0;i<500;++i)
		{
		buf16.Zero();
		buf16.AppendNum(i);
		buf8.Zero();
		buf8.AppendNum(i);

		TInt err = repository->Set(KIntId, i);
		TEST2(err, KErrNone);
		err = repository->Set(KRealId, (TReal)i);
		TEST2(err, KErrNone);
		err = repository->Set(KDes16Id, buf16);
		TEST2(err, KErrNone);
		err = repository->Set(KDes8Id, buf8);
		TEST2(err, KErrNone);
		}
	TUint timeEnd = User::TickCount();
	CleanupStack::PopAndDestroy(repository);
	TheTest.Printf(_L("\nPerformanceTestL - %d ticks\n"), timeEnd-timeStart);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-0497
@SYMTestCaseDesc		CentralRepository functionality test
@SYMTestPriority		High
@SYMTestActions			Wrapper function calling up test functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void MainL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0497 Functional tests "));
	OomTesting = EFalse;
	// Existance of caching functionality invalidates some tests and
	// makes them fail. So cleanup cace.
	RestoreTestFilesL();
	FuncTestsL();

	TheTest.Next(_L("Out-of-memory tests"));
	OomTesting = ETrue;
	//Reset the KUidTestRepository1 which was modified by the previous test
	//and is used in the next test.
	ResetTestRepositoryL();
	OomTestsL();

	TheTest.Next(_L("Performance test"));
	PerformanceTestL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
