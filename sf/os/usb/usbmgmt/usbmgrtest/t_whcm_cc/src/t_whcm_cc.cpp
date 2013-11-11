/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Runs tests upon USB Manager's WHCM Class Controller.
* Relies on the fact that the USB Manager is actually configured to start a WHCM class controller.
* Tests are numbered as in v0.2 of the USB Manager Test Specification.
*
*/
// t_WHCM_cc.cpp

#include <e32test.h>
#include <e32twin.h>
#include <c32comm.h>
#include <d32comm.h>
#include <f32file.h>
#include <hal.h>
#include <usbman.h>

// A test object for test io
LOCAL_D RTest test(_L("T_ACM_CC"));

// A pointer to an instance of the USB Manager
RUsb *usbman;

// An instance of the fileserver
RFs theFs;

// A timer for waiting around
RTimer timer;

// Some wait times for use with the timer (in microseconds)
// May need to tinker with these to get them right
#define CANCEL_START_REQ_DELAY 10
#define CANCEL_STOP_REQ_DELAY 10

// A set of states for the startup and shutdown sequence
enum TestState
{
	 EStart = 0,
	 EFsConnected,
	 EC32Started,
	 EUSBManCreated,
	 EUSBManConnected,
	 EPrimaryRegistered,
	 EUSBManStarted
};
TestState current_test_state;

/**
 * Function to run through the common startup of a test.
 */
TInt CommonStart()
{
	 TInt r;

	test.Printf(_L("CommonStart()\n"));

	 // Loop until the primary client is registered.
     // Do not proceed to start the USB Manager, as that is test specific.
	 while(current_test_state != EPrimaryRegistered)
	 {
		  switch(current_test_state)
		  {
		  case EStart:
			   // Connect to the fileserver
			   r = theFs.Connect();
			   if (r != KErrNone)
			   {
					test.Printf(_L("   Failed to connect to the fs. Error = %d\n"), r);
					return r;
			   }
			   test.Printf(_L("   Connected to file server.\n"));
			   current_test_state = EFsConnected;
			   break;
		  case EFsConnected:
			   // Start C32
			   r = StartC32();
			   if (r!=KErrNone && r !=KErrAlreadyExists)
			   {
					test.Printf(_L("   Failed to start C32. Error = %d\n"), r);
					return r;
			   }
			   test.Printf(_L("   Started C32.\n"));
			   current_test_state = EC32Started;
			   break;
		  case EC32Started:
			   // Create an instance of the USB Manager
			  usbman = new RUsb;
			   if (!usbman)
			   {
					test.Printf(_L("   Failed to instantiate USB Manager.\n"));
					return KErrGeneral;
			   }
			   test.Printf(_L("   Instantiated USB Manager.\n"));
			   current_test_state = EUSBManCreated;
			   break;
		  case EUSBManCreated:
			   // Connect to the USB Manager
			   r = usbman->Connect();
			   if (r != KErrNone)
			   {
					test.Printf(_L("   Failed to connect to USB Manager. Error = %d\n"), r);
					return r;
			   }
			   test.Printf(_L("   Connected to USB Manager.\n"));
			   current_test_state = EUSBManConnected;
			   break;
		  case EUSBManConnected:
			   current_test_state = EPrimaryRegistered;
			   break;
		  default:
			   break;
		  }
	 }

	 test.Printf(_L("CommonStart() done\n"));

	 return KErrNone;
}

/**
 * Function to run through the common shutdown of a test.
 * Shuts down only what is needed, based upon the value of current_test_state,
 * so it can be used to clean up after aborted starts.
 */
TInt CommonCleanup()
{
	 TRequestStatus status;

	 while(current_test_state != EStart)
	 {
		  switch(current_test_state)
		  {
		  case EUSBManStarted:
			   // Stop the USB Manager
			   usbman->Stop(status);
			   User::WaitForRequest(status);
			   current_test_state = EPrimaryRegistered;
			   break;
		  case EPrimaryRegistered:
			   current_test_state = EUSBManConnected;
			   break;
		  case EUSBManConnected:
			   // Don't need to disconnect.
			   current_test_state = EUSBManCreated;
			   break;
		  case EUSBManCreated:
			   delete usbman;
			   current_test_state = EC32Started;
			   break;
		  case EC32Started:
			   // Don't need to stop C32
			   current_test_state = EFsConnected;
			   break;
		  case EFsConnected:
			   theFs.Close();
			   current_test_state = EStart;
			   break;
		  default:
			   break;
		  }
	 }
	 return KErrNone;
}

/**
 * Checks that the USB service state is as expected.
 */
TInt CheckServiceState(TUsbServiceState state)
{
	TUsbServiceState aState;
	TInt r = usbman->GetServiceState(aState);
	if (r != KErrNone)
		{
		test.Printf(_L("Failed to get service state. Error = %d\n"), r);
		return r;
		}
	if (aState != state)
		{
		test.Printf(_L("Service state check failed. State expected: %d. State is: %d (type TUsbServiceState).\n"), state, aState);
		return KErrGeneral;
		}
	test.Printf(_L("Service state ok\n"));

	return KErrNone;
}

/**
 * Executes test A1 (as detailed in the USB Manager Test Specification).
 */
static TInt RunTest_A1()
	{
	TInt r;

	test.Next(_L("Test A1.\n"));

	// Perform common startup
	current_test_state = EStart;
	r = CommonStart();
	if (r != KErrNone)
		 return r;

	// Start the USB Manager
	TRequestStatus status;
	test.Printf(_L("Starting.\n"));
	usbman->Start(status);
	test.Printf(_L("Waiting for request.\n"));
	User::WaitForRequest(status);

	test.Printf(_L("... done. Status: %d.\n"), status.Int());

	current_test_state = EUSBManStarted;
	return KErrNone;
	}

/**
 * Executes test A3 (as detailed in the USB Manager Test Specification).
 */
static TInt RunTest_A3()
	{
	TInt r;

	test.Next(_L("Test A3.\n"));

	// Perform common startup
	current_test_state = EStart;
	r = CommonStart();
	if (r != KErrNone)
		 return r;

	// Start the USB Manager
	TRequestStatus status;
	usbman->Start(status);
	User::WaitForRequest(status);
	test.Printf(_L("Start completed with status %d\n"), status.Int());
	current_test_state = EUSBManStarted;

	// Stop the USB Manager
	usbman->Stop(status);
	User::WaitForRequest(status);
	test.Printf(_L("Stop completed with status %d\n"), status.Int());
	current_test_state = EPrimaryRegistered;

	// Check service status
	r = CheckServiceState(EUsbServiceIdle);
	if ( r != KErrNone)
		 return r;

	return KErrNone;
	}

/**
 * Main function.
 *
 * Runs all the tests in order.
 */
void mainL()
    {
	TInt err;

	// Run all the tests

	err=RunTest_A1();
	if (err != KErrNone)
	{
		test.Printf(_L("Test A1 failed, code: %d\n\n"), err);
	}
	else
		test.Printf(_L("Test A1 passed.\n\n"));
	CommonCleanup();

	err=RunTest_A3();
	if (err != KErrNone)
	{
		test.Printf(_L("Test A3 failed, code: %d\n\n"), err);
	}
	else
		test.Printf(_L("Test A3 passed.\n\n"));
	CommonCleanup();


	// Tests finished
    }

/**
 * Entry point.
 *
 * Creates a cleanup stack and a timer then calls consoleMainL.
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	test.Title();
	test.Start(_L("Starting E32Main"));

	// create the timer for use during some of the tests
	timer.CreateLocal();

	TRAP_IGNORE(mainL());

	test.Printf(_L("\n[Finished. Press any key.]\n"));
	test.Getch();
	test.End();
	test.Close();

	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
