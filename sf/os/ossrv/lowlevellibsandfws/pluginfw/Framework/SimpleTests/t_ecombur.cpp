// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32panic.h>
#include <f32file.h>
#include <bautils.h>
#include <babackup.h>

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
//Test utils for copying the resolver to C
#include "../EcomTestUtils/EcomTestUtils.h"

REComSession EComSess;

LOCAL_D RTest TEST(_L("ECom BUR Test"));

_LIT(KEComExDllOnZ,		"Z:\\ramonly\\EComSwiExample.dll");
_LIT(KEComExDllOnC,		"C:\\sys\\bin\\EComSwiExample.dll");
_LIT(KEComRscFileOnC,	"C:\\resource\\plugins\\EComSwiExample.rsc");
_LIT(KEComRscFileOnZ,	"Z:\\ramonly\\EComSwiExample.rsc");

#define UNUSED_VAR(a) a = a
inline LOCAL_C TInt DeleteTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
	if((err == KErrNone)||(err == KErrNotFound))
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
		}
	if(err == KErrNotFound)
		{
		err = KErrNone;
		}
	return err;
	}

/**
Copies the Resolver Plugins to C:\ drive
*/
LOCAL_C TInt CopyPluginsL()
    {
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
 	TEST(err==KErrNone, __LINE__);
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
 	TEST(err==KErrNone, __LINE__);
 	User::After(1000000);
 	return err;
	}


LOCAL_C void FindImplementationsL(TInt aExpected)
	{
    REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);

	//Get a list of available implementations
	TUid interfaceUid={0x10009DD9};
	RImplInfoPtrArray ifArray;

	ecomSession.ListImplementationsL(interfaceUid,ifArray);

	//Verify that the expected number of implementations were found
	TInt count = ifArray.Count();
	TEST(count == aExpected);

	TEST.Printf(_L("%d Implementations found...\n"),count);

	//cleanup
	ifArray.ResetAndDestroy();
	ecomSession.Close();
	CleanupStack::PopAndDestroy();
	}

LOCAL_C TInt SetupTest()
	{
	//Ensure plugin files are not on C:
	TInt res = DeleteTestPlugin();
	TEST.Printf(_L("Deleting test plugin...\n"));

	//Wait to ensure files are deleted
	User::After(2000000);

	//Create an ECom session to ensure ECom is up and running
	EComSess = REComSession::OpenL();

	//Wait to ensure ECom startup has occurred
	//Wait here for 20s as it takes 15s for the server to register its backup notification
	User::After(20000000);

	return res;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-4003
@SYMTestCaseDesc	    Tests that notifications will not be ignored during a backup/restore.
@SYMTestActions  	    Uses P&S variables to simulate backup/restore
						Simulates a backup operation, which should suspend the scanning timer
						Copy plugin to the relevant directory and check to see that this is discovered
						after the scanning timer is resumed (after backup is complete)
@SYMTestExpectedResults Notification of a directory change should occur during backup/restore, but not processed until afterwards
@SYMDEF                 DEF103909
*/
LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;

	//ensure that ecom server is already running
	TInt res = SetupTest();
	TEST(res == KErrNone);

	//use the backup client to initiate a backup
	CBaBackupSessionWrapper* backupClient= CBaBackupSessionWrapper::NewL();

	//emulate start of backup/restore
	TBackupOperationAttributes attribs;
	attribs.iFileFlag=MBackupObserver::EReleaseLockReadOnly;
	attribs.iOperation=MBackupOperationObserver::EStart;
	backupClient->NotifyBackupOperationL(attribs);

	TEST(backupClient->IsBackupOperationRunning());

	User::After(2000000);

	//now do copying of some plugin files
	CopyPluginsL();

	User::After(2000000);

    // check ecom has not discovered the plugins as idle scanning timer is disabled
    FindImplementationsL(0);

	//emulate end of backup/restore
	attribs.iFileFlag=MBackupObserver::EReleaseLockReadOnly;
	attribs.iOperation=MBackupOperationObserver::EEnd;
	backupClient->NotifyBackupOperationL(attribs);

	User::After(2000000);

	//now check whether our plugins that is installed during the backup is registered
	FindImplementationsL(1);

	//Cleanup
	EComSess.Close();
	REComSession::FinalClose();

	//Ensure plugin files are not on C:
	res = DeleteTestPlugin();
	TEST(res == KErrNone);

	delete backupClient;

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TEST.Title();
	TEST.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4003 ECom BUR tests. "));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,RunTestL());
	TEST(err==KErrNone, __LINE__);

	delete scheduler;
	delete cleanup;

	TEST.End();
	TEST.Close();

	__UHEAP_MARKEND;
	return(0);
	}
