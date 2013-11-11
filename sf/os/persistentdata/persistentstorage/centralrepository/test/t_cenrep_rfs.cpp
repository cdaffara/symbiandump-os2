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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32test.h>  // RTest
#include <e32debug.h> // RDebug::Printf
#include <f32file.h>  // RFs

#include "../cenrepsrv/srvparams.h" //KServerUid3

//using namespace NCentralRepositoryConstants;

_LIT(KSoftReset, "--SoftReset");

RTest TheTest(_L("Central Repository RFS Test"));

const TUid KUidRfsRepository = { 0xffffffff };
const TUid KUidRfsRepositoryInstallOnlyDefaultFile = { 0xfffffffe };
const TUid KUidRfsRepositoryDefaultRfsOn = { 0xfffffffa };

typedef enum
	{
	ERomOnly = 0x01,
	ERomAndInstall,
	EInstallOnly,
	ENoRomOrInstall
	} TRepositoryFileState;

const TUint32 KInt1 = 1;
const TInt KInt1_UpdatedValue = 73;
const TReal KReal1_InitialValue = 2.732;
const TUint32 KNewInt = 1000;
const TUint32 KNewInt2 = 0x0FFF; // outside range meta (in default meta)
const TUint32 KNewInt3 = 0x1000; // inside range meta
const TUint32 KReal1 = 2;
const TReal KReal1_InstallValue = 4.53;
const TReal KReal1_UpdatedValue = 7.32;
const TUint32 KString1 = 5;
_LIT(KString1_InitialValue, "test\\\"string\"");
_LIT(KString1_UpdatedValue, "another one");


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

// This function kills the C32exe.exe process. This commsdat process will
// interfere with the test if not killed. In a nutshell, some of the test cases 
// will kill and then wait for 2 seconds and restart the centrep server 
// with --SoftReset option. During that 2 seconds wait sometimes C32exe.exe 
// will use centrep API, thus starting the server normally without --SoftReset.
LOCAL_C void KillC32Exe()
    {
    _LIT( KC32ServerName, "c32exe");
    KillProcess(KC32ServerName); // Don't need to check the return code, it always return KErrNone anyway.
    User::After(KGeneralDelay);
    }

//This function restores the state of the files required for this test
//Existing files are deleted and then the required files are copied
//back from the Z drive to the c drive
LOCAL_C void RestoreRFSTestFilesL(TRepositoryFileState aState)
	{
	//Delete all files from C:\\private\\10202BE9\\persists\\ dir
	//and C:\\private\\10202BE9\\ dir
	CleanupCDriveL();
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	_LIT(KPersistTargetPath, "C:\\private\\10202BE9\\persists\\ffffffff.txt");
	_LIT(KInstallTargetPath, "C:\\private\\10202BE9\\ffffffff.txt");
	_LIT(KPersistTargetPath1,"C:\\private\\10202BE9\\persists\\fffffffe.txt");
	_LIT(KPersistTargetPath2,"C:\\private\\10202BE9\\persists\\fffffffd.txt");
	_LIT(KInstallTargetPath1,"C:\\private\\10202BE9\\fffffffe.txt");
	_LIT(KRFSTestFileSourcePath, "Z:\\private\\10202BE9\\ffffffff.txc");

	switch(aState)
	{
		case ERomOnly:
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KPersistTargetPath);
			break;

		case ERomAndInstall:
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KPersistTargetPath);
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KInstallTargetPath);
			break;

		case EInstallOnly:
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KPersistTargetPath1);
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KInstallTargetPath1);
			break;

		case ENoRomOrInstall:
			CopyTestFilesL(*fm,KRFSTestFileSourcePath, KPersistTargetPath2);
			break;

		default:
			break;
	}

	CleanupStack::PopAndDestroy(2);

	}

//
// Start the server process or thread
//
LOCAL_C TInt ReStartServerInSoftResetMode()
	{
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KServerImg,
						 KSoftReset,
						 serverUid);

	if (r != KErrNone)
		{
		return r;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);

	if (stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}

	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();

	server.Close();
	return r;
	}


LOCAL_C void RestoreFactorySettingsTestL()
	{
	TheTest.Start(_L("ResetAllRepositoriesTestL"));
	TInt r;
	TInt i;
	TBuf<20> str;

	TheTest.Next(_L("Open repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Add a Setting"));
	const TInt KIntValue = 1234;
	r = repository->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Delete a Setting"));
	r = repository->Delete(KReal1);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Modify a Setting"));
	r = repository->Set(KInt1, KInt1_UpdatedValue);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Modify a String Setting"));
	r = repository->Set(KString1, KString1_UpdatedValue);

	TEST2(r, KErrNone);
	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.Next(_L("Kill the server process"));
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Get 'Added' value"));
	r = repository->Get(KNewInt, i);
	TEST2(r, KErrNone);

	TReal real;
	TheTest.Next(_L("Get 'Deleted' value"));
	r = repository->Get(KReal1, real);
	TEST2(r, KErrNone);
	TEST(real == KReal1_InitialValue);

	TheTest.Next(_L("Get 'Modified' value"));
	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i == KInt1_UpdatedValue);

	r = repository->Get(KString1, str);
	TEST2(r, KErrNone);
	TEST(str==KString1_InitialValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.End();
	}



/**
@SYMTestCaseID 	 SYSLIB-CENTRALREPOSITORY-CT-3341
@SYMTestCaseDesc Restore factory Settings from Rom file.
@SYMTestPriority High
@SYMTestActions  Ensure that the repository file only exists on the Rom.
				 Open the repository and modify a setting.  Force RFS
				 and check that the repository is restored against the ROM file
@SYMTestExpectedResults The test repository should be reset against the ROM file
@SYMDEF 		 PDEF099108
*/
LOCAL_C void RFSRomOnlyL()
	{

	//restore all test files - this ensures we have a repository file
	//only on the z:
	RestoreRFSTestFilesL(ERomOnly);

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3341 ResetAllRepositoriesTestL "));
	TInt r, intVal;
	TReal realVal;

	TheTest.Next(_L("Open repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Modify a Setting"));
	r = repository->Set(KInt1, KInt1_UpdatedValue);
	TEST2(r, KErrNone);

	//verify the update
	r = repository->Get(KInt1, intVal);
	TEST2(r, KErrNone);
	TEST(intVal == KInt1_UpdatedValue);

	TheTest.Next(_L("Modify a Setting"));
	r = repository->Set(KReal1, KReal1_UpdatedValue);
	TEST2(r, KErrNone);

	//verify the update
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_UpdatedValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	//Kill the server
	TheTest.Next(_L("Kill the server process"));
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	//Restart the server in soft reset mode to force a
	//repository reset
	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	//Open the repository
	repository = CRepository::NewLC(KUidRfsRepository);

	//Verify that the real value is reset against the value in the Rom version of
	//the repository file
	TheTest.Next(_L("Get 'Modified' value"));
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_InitialValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.End();
	}



/**
@SYMTestCaseID 	 SYSLIB-CENTRALREPOSITORY-CT-3342
@SYMTestCaseDesc Restore factory Settings from merged repository.
@SYMTestPriority High
@SYMTestActions  Ensure that the repository file  exists in both the Rom and install
					directories.
				 Open the repository and modify a setting.  Force RFS
				 and check that the repository is restored against the merged repository
@SYMTestExpectedResults The test repository should be reset against the merged repository
@SYMDEF 		 PDEF099108
*/
LOCAL_C void RFSRomAndInstallL()
	{

	//restore all test files - this ensures we have a c: and z: file
	//for the test repository which causes a repository merge
	RestoreRFSTestFilesL(ERomAndInstall);

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3342 ResetAllRepositoriesTestL "));
	TInt r;
	TReal realVal;
	TBuf<20> strVal;

	TheTest.Next(_L("Open repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Modify a Setting"));
	r = repository->Set(KReal1, KReal1_UpdatedValue);
	TEST2(r, KErrNone);

	//verify the update
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_UpdatedValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	//Kill the server
	TheTest.Next(_L("Kill the server process"));
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	//Restart the server in soft reset mode to force a
	//repository reset
	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	//Open the repository
	repository = CRepository::NewLC(KUidRfsRepository);

	//Verify that the string value, which is only found in the ROM file is
	//present in the merged repository
	r = repository->Get(KString1, strVal);
	TEST2(r, KErrNone);
	TEST(strVal==KString1_InitialValue);

	//verify that the Real value has been reset against the value in the install
	//repository file
	TheTest.Next(_L("Get 'Modified' value"));
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_InstallValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.End();
	}


/**
@SYMTestCaseID 	 SYSLIB-CENTRALREPOSITORY-CT-3343
@SYMTestCaseDesc Restore factory Settings from Install file.
@SYMTestPriority High
@SYMTestActions  Ensure that the repository file only exists in the Install directory.
				 Open the repository and modify a setting.  Force RFS
				 and check that the repository is restored against the Install file
@SYMTestExpectedResults The test repository should be reset against the Install file
@SYMDEF 		 PDEF099108
*/
LOCAL_C void RFSInstallOnlyL()
	{

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3343 ResetAllRepositoriesTestL "));
	TInt r;
	TReal realVal;
	TBuf<20> strVal;

	//restore all test files - this ensures we have a repository file
	//only on the c:
	RestoreRFSTestFilesL(EInstallOnly);

	TheTest.Next(_L("Open repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRfsRepositoryInstallOnlyDefaultFile);

	TheTest.Next(_L("Modify a Setting"));
	r = repository->Set(KReal1, KReal1_UpdatedValue);
	TEST2(r, KErrNone);

	//verify the update
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_UpdatedValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	//Kill the server
	TheTest.Next(_L("Kill the server process"));
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	//Restart the server in soft reset mode to force a
	//repository reset
	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	//Open the repository
	repository = CRepository::NewLC(KUidRfsRepositoryInstallOnlyDefaultFile);

	//verify that the Real value has been reset against the value in the install
	//repository file
	TheTest.Next(_L("Get 'Modified' value"));
	r = repository->Get(KReal1, realVal);
	TEST2(r, KErrNone);
	TEST(realVal == KReal1_InstallValue);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.End();
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3435
@SYMTestCaseDesc		PDEF105203: Start-up Settings: New access point remains after restoring factory setting
@SYMTestPriority		High
@SYMTestActions			Create a new setting inside a range with range meta RFS bit set, create another setting
						outside the range, restart server in RFS mode, check to see the setting created outside
						the range still exists, check to see the setting created inside the range has been deleted.
						Repeat same steps with another repository which has default meta having RFS on and range meta
						having RFS off, check to see the setting created outside the range has been deleted, check to
						see the setting created inside the range still exists.
@SYMTestExpectedResults Test must not fail
@SYMDEF					PDEF105203
*/
LOCAL_C void PDEF105203()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3435 PDEF105203: Start-up Settings: New access point remains after restoring factory setting "));
	TInt r;
	TInt i;

	TheTest.Next(_L("Open repository to ensure server is running"));
	CRepository* repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Create a new setting outside RFSable range meta area"));
	const TInt KIntValue = 999;
	r = repository->Create(KNewInt2, KIntValue);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Create a new setting in RFSable range meta area"));
	r = repository->Create(KNewInt3, KIntValue);
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.Next(_L("Kill the server process"));
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	repository = CRepository::NewLC(KUidRfsRepository);

	TheTest.Next(_L("Get 'created' value outside range meta"));
	r = repository->Get(KNewInt2, i);
	TEST2(r, KErrNone); // should still exist
	TEST(i == KIntValue);

	TheTest.Next(_L("Get 'created' value inside range meta"));
	r = repository->Get(KNewInt3, i);
	TEST2(r, KErrNotFound); // should have been deleted

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	repository = CRepository::NewLC(KUidRfsRepositoryDefaultRfsOn);

	TheTest.Next(_L("Create a new setting in RFSable default meta area"));
	const TInt KIntValue2 = 990;
	r = repository->Create(KNewInt2, KIntValue2);
	TEST2(r, KErrNone);

	TheTest.Next(_L("Create a new setting in non-RFSable range meta area"));
	r = repository->Create(KNewInt3, KIntValue2);
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.Next(_L("Kill the server process"));
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	User::After(KGeneralDelay);

	TheTest.Next(_L("Manually start central respository"));
	ReStartServerInSoftResetMode();

	TheTest.Next(_L("Re-create the repository to ensure server is running"));
	repository = CRepository::NewLC(KUidRfsRepositoryDefaultRfsOn);

	TheTest.Next(_L("Get 'created' value outside range meta"));
	r = repository->Get(KNewInt2, i);
	TEST2(r, KErrNotFound); // should have been deleted

	TheTest.Next(_L("Get 'created' value inside range meta"));
	r = repository->Get(KNewInt3, i);
	TEST2(r, KErrNone); // should still exist
	TEST(i == KIntValue2);

	// Close repository
	CleanupStack::PopAndDestroy(repository);

	TheTest.End();
	}

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4082		
@SYMTestCaseDesc		Test for PDEF133672: Cannot remove restored access points
@SYMTestPriority		High
@SYMTestActions			Create a repository, delete settings with RFS meta and reset repository, 
						then try to delete the settings again. 
@SYMTestExpectedResults When deleting settings after reset, KErrAlreadyExists should not return.
@SYMDEF					PDEF133672
*/	
LOCAL_C void PDEF133672L()
	{
	TheTest.Start(_L("Test for PDEF133671"));
	
	const TUid KReposUid = {0xCCCCCC99};

	const TInt KTestId1 = {0x00000001};
	const TInt KTestId2 = {0x0000000A};
	
	CRepository* repos=NULL;
		
	repos = CRepository::NewLC(KReposUid);
		
	User::LeaveIfNull(repos);
		
	TInt r;
	
	r = repos->Delete(KTestId1);
	TEST2(r, KErrNone);
	r = repos->Delete(KTestId2);
	TEST2(r, KErrNone);
	
	TheTest.Next(_L("try reset single settings"));
	r = repos->Reset(KTestId1);
	TEST2(r, KErrNone);
	r = repos->Reset(KTestId2);
	TEST2(r, KErrNone);
		
	r = repos->Delete(KTestId1);
	TEST2(r, KErrNone);
	r = repos->Delete(KTestId2);
	TEST2(r, KErrNone);
	
	TheTest.Next(_L("try reset whole repository"));
	r = repos->Reset();
			
	r = repos->Delete(KTestId1);
	TEST2(r, KErrNone);
	r = repos->Delete(KTestId2);
	TEST2(r, KErrNone);
		
	CleanupStack::PopAndDestroy();

	TheTest.End();
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-0497-0001
@SYMTestCaseDesc		CentralRepository functionality test
@SYMTestPriority		High
@SYMTestActions			Wrapper function calling up test functions
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ0000
*/
LOCAL_C void MainL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0497-0001 Restore Factory Settings tests "));
	CleanupCDriveL();
	KillC32Exe(); //Need to kill C32Exe as it is interfering with the test.
	RestoreFactorySettingsTestL();
	PDEF105203();
	RFSRomOnlyL();
	RFSRomAndInstallL();
	RFSInstallOnlyL();
	PDEF133672L();
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
