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
//

#include <e32test.h>
#include <ecom/ecom.h>
#include <f32file.h>
#include <e32base.h>
#include <bautils.h>
#include <e32ldr.h>

#include "../EcomTestUtils/EcomTestUtils.h"
#include "HeapTestInterface.h"

LOCAL_D RTest TheTest(_L("t_ecomcancel"));
LOCAL_D RFs TheFs;

const TInt KWaitDuration = 30000000; // retry delay in usecs

_LIT(KHTI_PluginAtRootOfZ,  "z:\\HeapTestImpl.dll");
_LIT(KHTI_PluginAtRootOfC,  "c:\\HeapTestImpl.dll");
_LIT(KHTIr_PluginAtRootOfZ, "z:\\HeapTestImpl.rsc");
_LIT(KHTIr_PluginAtRootOfC, "c:\\HeapTestImpl.rsc");
_LIT(KHTI_PluginInstalledOnC,  "c:\\sys\\bin\\HeapTestImpl.dll");
_LIT(KHTIr_PluginInstalledOnC, "c:\\resource\\plugins\\HeapTestImpl.rsc");

_LIT(KExample5DllOnC, "C:\\sys\\bin\\EComExample5.dll");
_LIT(KExample5DllOnZ, "z:\\RAMOnly\\EComExample5.dll");
_LIT(KExample5RscOnC, "C:\\resource\\plugins\\EComExample5.rsc");
_LIT(KExample5RscOnZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KExample5TmpRscOnC, "C:\\resource\\plugins\\EComExample5tmp.rsc");

void CleanupFiles()
	{
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KHTI_PluginInstalledOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KHTIr_PluginInstalledOnC));

	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KExample5TmpRscOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KExample5RscOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KExample5DllOnC));

	WAIT_FOR3s;
	}

//
//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::CleanupFiles();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::CleanupFiles();
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
//
//


// Setup files for test
void SetUpPlugins()
	{

	TInt err;
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTI_PluginAtRootOfZ, KHTI_PluginAtRootOfC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTIr_PluginAtRootOfZ, KHTIr_PluginAtRootOfC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTI_PluginAtRootOfZ, KHTI_PluginInstalledOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTIr_PluginAtRootOfZ, KHTIr_PluginInstalledOnC));
	TEST2(err, KErrNone);
	WAIT_FOR3s;

	}

// Copying plugin to the RAM for test purposes
void CopyTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KExample5RscOnZ, KExample5RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KExample5DllOnZ, KExample5DllOnC));
	TEST2(err, KErrNone);
	WAIT_FOR3s;
	}

void ResetEComEnvironmentL()
	{
    ECOMTU_KILL_SERVER_L(TheTest);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0657
@SYMTestCaseDesc	    De-Registering for notification of messages test
                        Test for REComSession class
@SYMTestPriority 	    High
@SYMTestActions  	    Test for no side effects on trying to cancel a request that has been already cancelled
						Test for updation of right status value after cancel request.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void TestCancelL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0657 TestCancelL "));

    // Test normal cancelling
    REComSession& session = REComSession::OpenL();
    TRequestStatus status;
    session.NotifyOnChange(status);
    session.CancelNotifyOnChange(status);
    User::WaitForRequest(status);
    TEST(status == KErrCancel);
    session.Close();

	// Test no side effects on trying to cancel a request
	//  that has already been cancelled
    session = REComSession::OpenL();
    session.NotifyOnChange(status);
    session.CancelNotifyOnChange(status);
    User::WaitForRequest(status);
    TEST(status == KErrCancel);
    session.CancelNotifyOnChange(status);
    session.CancelNotifyOnChange(status);
    TEST(status == KErrCancel);

	// Make sure the right status value is updated after a cancel request.
    TRequestStatus status1, status2, status3;
    session.NotifyOnChange(status);
    TEST(status==KRequestPending);
    session.NotifyOnChange(status1);
    TEST(status1==KRequestPending);
    session.NotifyOnChange(status2);
    TEST(status2==KRequestPending);
    session.NotifyOnChange(status3);
    TEST(status3==KRequestPending);
    session.CancelNotifyOnChange(status);
    User::WaitForRequest(status);
    TEST(status==KErrCancel);
    session.CancelNotifyOnChange(status3);
    session.CancelNotifyOnChange(status2);
    User::WaitForRequest(status3);
    TEST(status3==KErrCancel);
    User::WaitForRequest(status2);
    TEST(status2==KErrCancel);
    session.CancelNotifyOnChange(status1);
    User::WaitForRequest(status1);
    TEST(status1==KErrCancel);
   	session.Close();

	// Test Ends...
	REComSession::FinalClose();
	}


void SimulateDiskChangeL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is not read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KHTI_PluginAtRootOfC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KHTIr_PluginAtRootOfC));

	// Simulate that the implementation dll is changed - change resouce file timestamp
	RFile file;
	User::LeaveIfError(file.Open(TheFs, KHTIr_PluginAtRootOfC, EFileRead | EFileWrite));
	TTime time;
	User::LeaveIfError(file.Modified(time));
	TTimeIntervalSeconds interval = 100;
	time += interval;
	User::LeaveIfError(file.SetModified(time));
	file.Close();

	// Replace the resource file with the newer version (newer timestamp)
	// The ECOM server will run the discovery process and will unload the old dll
	// and replace it with the newer dll
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTI_PluginAtRootOfC, KHTI_PluginInstalledOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KHTIr_PluginAtRootOfC, KHTIr_PluginInstalledOnC));
	TEST2(err, KErrNone);

	WAIT_FOR3s; // wait for copy to complete

	__UHEAP_MARKEND;
	}

// This test step no longer uses bafl to move files as it does not have high
// enough capability - uses CFileMan helper process instead.
void SimulatePluginDeleteBaflL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is not read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KExample5RscOnC));

	// Delete the plugins
	// The ECOM server will send the notification to its clients for change.
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KExample5RscOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KExample5DllOnC));
	WAIT_FOR3s;

	__UHEAP_MARKEND;
	}

// This test step no longer uses bafl to move files as it does not have high
// enough capability - uses CFileMan helper process instead.
void SimulatePluginCopyBaflL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;


	// Make sure the file is not read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KExample5RscOnC));

	// Copy the plugins.
	// The ECOM server will send the notification to its clients for change.
	TRAP(err, EComTestUtils::FileManCopyFileL(KExample5RscOnZ, KExample5RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KExample5DllOnZ, KExample5DllOnC));
	TEST2(err, KErrNone);

	WAIT_FOR5s; // wait for copy to complete

	__UHEAP_MARKEND;
	}

void SimulatePluginDeleteFileManL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KExample5RscOnC));


	// Delete the plugins.
	// The ECOM server will send the notification to its clients for change.
	TRAP(err, EComTestUtils::FileManDeleteFileL(KExample5RscOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KExample5DllOnC));

	WAIT_FOR5s; // wait for copy to complete

	__UHEAP_MARKEND;
	}

void SimulatePluginCopyFileManL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is not read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KExample5RscOnC));

	// Copy the plugins.
	// The ECOM server will send the notification to its clients for change.
	TRAP(err, EComTestUtils::FileManCopyFileL(KExample5RscOnZ, KExample5RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KExample5DllOnZ, KExample5DllOnC));
	TEST2(err, KErrNone);

	WAIT_FOR5s; // wait for copy to complete

	__UHEAP_MARKEND;
	}

void SimulatePluginDeleteRFsL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));
	TRAP(err, EComTestUtils::MakeFileWriteableL(KExample5RscOnC));

	// Delete the plugins using RFs.
	// The ECOM server will send the notification to its clients for change.
	TRAP(err, EComTestUtils::RfsDeleteFileL(KExample5RscOnC));
	TRAP(err, EComTestUtils::RfsDeleteFileL(KExample5DllOnC));

	WAIT_FOR5s; // wait for copy to complete

	__UHEAP_MARKEND;
	}

void SimulatePluginReplaceRFsL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Make sure the file is not read-only
	TRAPD(err, EComTestUtils::MakeFileWriteableL(KExample5DllOnC));

	// Replace the plugins using RFs.
	// The ECOM server will send the notification to its clients for change.
	TRAP(err, EComTestUtils::RfsReplaceFileL(KExample5RscOnC,KExample5TmpRscOnC));
	TEST2(err, KErrNone);

	WAIT_FOR5s; // wait for operation to complete

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0658
@SYMTestCaseDesc	    Tests for REComSession::NotifyOnChange() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the implementation dll(timestamp change) and checks for the notify status
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void TestNotifyL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0658 TestNotifyL "));

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);


	//-----
    REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);

	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());

	TRequestStatus timerStatus;
	TRequestStatus notifyStatus;

	// 1st disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 2nd disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 3rd disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 4th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 5th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 6th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 7th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 8th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 9th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 10th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulateDiskChangeL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	// 11th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulatePluginDeleteBaflL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	//Copy back the deleted plugin for the next test
	CopyTestPlugin();

	// 12th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change, but no regigstry data changed
	SimulatePluginCopyBaflL();

	User::WaitForAnyRequest();

	//since the file copied is same as before, no notify will be send
	TEST(notifyStatus==KRequestPending);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();


	// 13th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulatePluginDeleteFileManL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	//Copy back the deleted plugin for the next test
	CopyTestPlugin();

	// 14th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulatePluginCopyFileManL();

	User::WaitForAnyRequest();

	//since the file copied is same as before, no notify will be send
	TEST(notifyStatus==KRequestPending);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();


	// 15th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulatePluginDeleteRFsL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	//Copy back the deleted plugin for the next test
	CopyTestPlugin();

	// 16th disk change
	// Issue the requests
	timer.After(timerStatus, KWaitDuration);
	ecomSession.NotifyOnChange(notifyStatus);

	// simulate a disk change
	SimulatePluginReplaceRFsL();

	User::WaitForAnyRequest();

	TEST(notifyStatus==KErrNone);
	TEST(timerStatus==KRequestPending);

	timer.Cancel();

	//Replace back the deleted plugin.
	TRAPD(err, EComTestUtils::FileManCopyFileL(KExample5TmpRscOnC, KExample5RscOnC));
	TEST2(err, KErrNone);

	CleanupStack::PopAndDestroy(&timer);
	CleanupStack::PopAndDestroy(&ecomSession);
	//-----

	// Test Ends...
	REComSession::FinalClose();

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-1808
@SYMTestCaseDesc	    Tests the uninstallation of an active plugin
@SYMTestPriority 	    High
@SYMTestActions  	    Creates a plugin implementation and then uninstalls the
                        plugin's files from the C: Drive and waits for an ECOM
                        notification completion, upon which it deletes the
                        plugin.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void TestUninstallL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1808 TestUninstallL "));

	// Set up for heap leak checking and leaking thread handles
	__UHEAP_MARK;
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

    // Step 1: Setup the session to ECOM
    TInt err = KErrNone;
    REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);

    // Step 2: Register for ECOM Server notification.
	TRequestStatus notifyStatus;
    ecomSession.NotifyOnChange(notifyStatus);
	TEST2(notifyStatus.Int(), KRequestPending);

    // Step 3. Create the plugin implementation we will uninstall
    TheTest.Printf(_L("CHeapTestInterface::NewL()\n"));
    CHeapTestInterface* implPtr = 0;
    TRAP(err, implPtr = CHeapTestInterface::NewL());
	TEST2(err, KErrNone);

    // Step 4. Simulate the uninstall of the plugin files
    TheTest.Printf(_L("Deleting HTI plugin - attempt 1, wait 5s...\n"));
  	//TRAP(err, EComTestUtils::FileManDeleteFileL(KHTI_PluginInstalledOnC));
  	TRAP(err, EComTestUtils::RLoaderDeleteFileL(KHTI_PluginInstalledOnC));
#if defined(__WINS__) || defined (__WINSCW__)
   	TEST2(err, KErrAccessDenied); // DLL File locked under Windows emulator due to it being demand paged
   	TheTest.Printf(_L("Error -21 (KErrAccessDenied) expected due to Windows keeping the DLL mapped in the emulator process adress space due to Windows demand paging/virtual memory system\n"));
#elif defined(__EPOC32__)
   	TEST2(err, KErrNone); // DLL File not locked on target hardware under SOS
//#else Do no test at all as its an unexpected platform.
#endif

 	TRAP(err, EComTestUtils::FileManDeleteFileL(KHTIr_PluginInstalledOnC));
   	TEST2(err, KErrNone);

    // Step 5. Wait for ECOM Server notification to arrive, with precautionary hang timer.
	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());
	TRequestStatus timerStatus;
	timer.After(timerStatus, KWaitDuration);
	TEST2(timerStatus.Int(), KRequestPending);

    TheTest.Printf(_L("Before calling WaitForAnyRequest(): Timer Status: %d; ECOM Notif Status: %d\n"), timerStatus.Int(), notifyStatus.Int());
    TheTest.Printf(_L("Checking notification recieved for ECOM Server...\n"));
    User::WaitForRequest(timerStatus, notifyStatus);

	TEST2(timerStatus.Int(), KRequestPending);
	TEST2(notifyStatus.Int(), KErrNone);
	timer.Cancel();

    // Step 6. Destroy the plugin
    TheTest.Printf(_L("DELETE implPtr\n"));
    delete implPtr;


	//CleanupStack::PopAndDestroy(&timer);
    TheTest.Printf(_L("Test over, cleanup...\n"));
   	CleanupStack::PopAndDestroy(&timer);
	CleanupStack::PopAndDestroy(&ecomSession);
	REComSession::FinalClose();

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...
	__UHEAP_MARKEND;

	}

void DoTestsL()
	{
	TestNotifyL();
	TestCancelL();
    TestUninstallL();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title();
	TheTest.Start(_L("Starting tests..."));
	TInt err = KErrNone;
	TInt r = KErrNone;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	r = TheFs.Connect();
	if (r != KErrNone)
		{
			return r;
		}

    // Reset the test environment under textshell in emulator/ROM
	TRAP(err, ResetEComEnvironmentL());
	TEST2(err, KErrNone);

	// Initial setup, runtests, cleanup
	SetUpPlugins();
	CopyTestPlugin();
	TRAP(err,DoTestsL());
	TEST2(err, KErrNone);
	CleanupFiles();

    // Reset the environment for next test executable
	TRAP(err, ResetEComEnvironmentL());
	TEST2(err, KErrNone);

	TheFs.Close();
	delete scheduler;
	delete cleanup;
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return(0);
	}
