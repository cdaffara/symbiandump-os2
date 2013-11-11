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
#include <swi/swispubsubdefs.h>

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
//Test utils for copying the resolver to C
#include "../EcomTestUtils/EcomTestUtils.h"
#include "../EcomTestUtils/TPropertyManager.h"

using namespace Swi;

REComSession EComSess;

LOCAL_D RTest TEST(_L("ECom SWI Test"));

_LIT(KEComExDllOnZ,		"Z:\\ramonly\\EComSwiExample.dll");
_LIT(KEComExDllOnC,		"C:\\sys\\bin\\EComSwiExample.dll");
_LIT(KEComRscFileOnC,	"C:\\resource\\plugins\\EComSwiExample.rsc");
_LIT(KEComRscFileOnZ,	"Z:\\ramonly\\EComSwiExample.rsc");
_LIT(KEComRscPath,		"C:\\resource\\plugins\\");

const TInt KWaitDuration = 5000000; //  delay in usecs

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

LOCAL_C TInt DeletePluginFolder()
	{
	TRAPD(err, EComTestUtils::FileManDeleteDirL(KEComRscPath));
	User::After(1000000);
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

LOCAL_C void RegisterForNotificationL(TRequestStatus& aNotifyStatus)
	{
    REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);

	ecomSession.NotifyOnChange(aNotifyStatus);
	TEST.Printf(_L("Request notification on ECom registration data change\n"));
	TInt result = aNotifyStatus.Int();
	TEST(result == KRequestPending);

	ecomSession.Close();
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void WaitForNotificationL(TBool aExpected, TRequestStatus& aNotifyStatus)
	{
    //Wait for ECOM Server notification to arrive with timeout
	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());
	TRequestStatus timerStatus;

	//Wait for ECom notification
	timer.After(timerStatus, KWaitDuration);
	TEST(timerStatus.Int() == KRequestPending);

    TEST.Printf(_L("Waiting for notification from ECOM Server...\n"));
    User::WaitForRequest(timerStatus, aNotifyStatus);

    if(aExpected)
    	{
	    //Verify that we recieved notification from ECom
		TEST(timerStatus.Int() == KRequestPending);
		TEST(aNotifyStatus.Int() == KErrNone);
    	}
    else
    	{
	    //Verify that we have not recieved notification from ECom
		TEST(timerStatus.Int() == KErrNone);
		TEST(aNotifyStatus.Int() == KRequestPending);
    	}
	timer.Cancel();

	CleanupStack::PopAndDestroy();
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

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3602
@SYMTestCaseDesc		Tests notification processing when SWI does not exist
						DEF108840: Undesireable interaction between ECOM and SWI
@SYMTestActions  	    Request notifcation of ECom registry change
						Copy plugin to c:\ and check that notification is recieved

@SYMTestExpectedResults Notification should be recieved as normal if SWI is not present
@SYMDEF                 DEF108840
*/
LOCAL_C void DoSWITest1L()
	{

	TRequestStatus notifyStatus;
	RegisterForNotificationL(notifyStatus);

	FindImplementationsL(0);

	//Copy plugin file to c:
	TEST.Printf(_L("Copy Plugin Files...\n"));
	CopyPluginsL();

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(1);

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3547
@SYMTestCaseDesc	    Tests notification processing during SWI for
						DEF108840: Undesireable interaction between ECOM and SWI
@SYMTestActions  	    Set P&S variable to indicate SWI in progress.
						Request notifcation of ECom registry change
						Copy plugin to c:\ and check that notification is not recieved
						Clear P&S variable to indicate SWI completion and verify that notification is recieved.
@SYMTestExpectedResults Notification should not be recieved while SWI is in progress, but should be
						recieved when SWI completes
@SYMDEF                 DEF108840
*/
LOCAL_C void DoSWITest2L()
	{
	//Set SWI as running
	TInt r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisInstall);
	TEST.Printf(_L("SWI Started\n"));

	TRequestStatus notifyStatus;
	RegisterForNotificationL(notifyStatus);

	FindImplementationsL(0);

	//Copy plugin file to c:
	TEST.Printf(_L("Copy Plugin Files...\n"));
	CopyPluginsL();

    //Verify that the wait timed out - we didn't recieve notification
	WaitForNotificationL(EFalse,notifyStatus);

	FindImplementationsL(0);

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(1);

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3668
@SYMTestCaseDesc	    Tests notification processing during SWI for
						INC110470 ECOM does not notify when plug-ins get uninstalled
@SYMTestActions  	    Uses P&S variables to simulate SWI install and uninstall
						Request notification of ECom registry change
						Verify that notifications are recieved after both install and uninstall
@SYMTestExpectedResults Notification should be recieved after both SWI install and uninstall
@SYMDEF                 INC110470
*/
LOCAL_C void DoSWITest3L()
	{

	TRequestStatus notifyStatus;
	RegisterForNotificationL(notifyStatus);

	FindImplementationsL(0);

	//Set SWI as running
	TInt r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisInstall);
	TEST.Printf(_L("SWI Started\n"));

	//Copy plugin file to c:
	TEST.Printf(_L("Copy Plugin Files...\n"));
	CopyPluginsL();

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(1);

	RegisterForNotificationL(notifyStatus);

	//Set SWI as running
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisUninstall);
	TEST.Printf(_L("SWI Started\n"));

	//delete files from c:
	TEST.Printf(_L("Delete Plugin Files...\n"));
	DeleteTestPlugin();

	//Delete plugin folder so that drive is unmounted by ECom
	DeletePluginFolder();

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(0);

	RegisterForNotificationL(notifyStatus);

	//Set SWI as running
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisInstall);
	TEST.Printf(_L("SWI Started\n"));

	//Copy plugin file to c:
	TEST.Printf(_L("Copy Plugin Files...\n"));
	CopyPluginsL();

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(1);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-3669
@SYMTestCaseDesc	    Tests robustness of notification processing during SWI
@SYMTestActions  	    Uses P&S variables to simulate SWI install and uninstall
						Request notification of ECom registry change
						Verify that notifications are recieved after both install and uninstall even
						if a SWI scan is pending when SWI begins
@SYMTestExpectedResults Notification should be recieved after both SWI install and uninstall
@SYMDEF                 INC110470
*/
LOCAL_C void DoSWITest4L()
	{

	TRequestStatus notifyStatus;
	RegisterForNotificationL(notifyStatus);

	//Set SWI as complete
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	FindImplementationsL(0);

	//Set SWI as running
	TInt r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisInstall);
	TEST.Printf(_L("SWI Started\n"));

	//Copy plugin file to c:
	TEST.Printf(_L("Copy Plugin Files...\n"));
	CopyPluginsL();

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(1);

	RegisterForNotificationL(notifyStatus);

	//Set SWI as running
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisUninstall);
	TEST.Printf(_L("SWI Started\n"));

	//Delete plugin files
	TEST.Printf(_L("Delete Plugin Files...\n"));
	DeleteTestPlugin();

	//Set SWI as complete
	r = PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,ESASwisNone);
	TEST.Printf(_L("SWI Complete\n"));

	//Wait again for ECom notification
	WaitForNotificationL(ETrue,notifyStatus);

	FindImplementationsL(0);
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
	User::After(2000000);

	return res;
	}

LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;

	TInt res = SetupTest();
	TEST(res == KErrNone);

	res = PropertyManager::DeleteProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue);
	TEST(res == KErrNone);

	//Run the test
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3602 ECom SWI Test - SWI Not Present "));
	DoSWITest1L();

	//Cleanup
	EComSess.Close();

	res = SetupTest();
	TEST(res == KErrNone);

	res = PropertyManager::DefineProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,RProperty::EInt);
	TEST(res == KErrNone);

	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3547 ECom SWI Test - SWI Present "));
	DoSWITest2L();

	//Cleanup
	EComSess.Close();

	res = SetupTest();
	TEST(res == KErrNone);

	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3668 ECom SWI Uninstall Test - SWI Present "));
	DoSWITest3L();

	//Cleanup
	EComSess.Close();

	res = SetupTest();
	TEST(res == KErrNone);

	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3669 ECom SWI Robustness Test "));
	DoSWITest4L();

	res = PropertyManager::DeleteProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue);
	TEST(res == KErrNone);

	//Cleanup
	EComSess.Close();
	REComSession::FinalClose();

	//Ensure plugin files are not on C:
	res = DeleteTestPlugin();
	TEST(res == KErrNone);

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TEST.Title();
	TEST.Start(_L("ECom SWI tests."));

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
