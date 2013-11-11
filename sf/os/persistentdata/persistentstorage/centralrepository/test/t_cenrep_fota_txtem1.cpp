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

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <e32std.h>
#include <bautils.h>

#include "e32math.h"

RTest TheTest(_L("Central Repository Rom Flashing Tests"));

const TUid KUidRomRepository          = { 0x10033333 };

_LIT(KCentralRepositoryServerName, "Centralrepositorysrv");
_LIT(KRepositoryFile,"C:\\private\\10202be9\\persists\\10033333.cre");
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
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4005
@SYMTestCaseDesc		Test that after a rom flash that removes a rom repository, with the condition that there is no SWI install file, then the repository is completely removed.
@SYMTestPriority		High
@SYMTestActions			Run the t_centrep_runtvflashtests.bat that copies a modified repository into the persists directory and changes the romversion_info.txt file.
						Start The server.
@SYMTestExpectedResults Repository doesnt exist. It has been removed on server startup becos there is no record of it in install or rom.
@SYMCR 					CR1198
*/
LOCAL_C void SimulateRomRemovalFlashL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4005 "));

	// Kill server so that next test will reconnect and recreate resources
	TInt r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	//Force Startup of centrep
	CRepository* repository = CRepository::NewLC(KUidRomRepository);
 	RFs fs;
 	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TEntry entry;
    TInt err = fs.Entry(KRepositoryFile, entry);
	CleanupStack::PopAndDestroy();//fs
 	TEST(err==KErrNotFound);
 	CleanupStack::PopAndDestroy();//repository

	}


LOCAL_C void FuncTestsL()
	{

    TheTest.Start(_L("Central repository ->Test removed repository from rom is removed after flash SYSLIB-CENTRALREPOSITORY-CT-4005"));

	SimulateRomRemovalFlashL();

	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));

	FuncTestsL();

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
