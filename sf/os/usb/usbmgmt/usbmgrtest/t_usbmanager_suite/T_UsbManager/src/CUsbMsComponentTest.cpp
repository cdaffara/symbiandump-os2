/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Example CTestStep derived implementation
*
*/

/**
 @file 
*/
#include <test/testexecutelog.h>
#include "CUsbMsComponentTest.h"


/**
 Compare personalities of USB from device with expected result in
 ini file. The compared fields are: personal IDs, Description
 and Class control IDs.
 */
TVerdict CUsbMsComponentTest::CompareUsbPersonalityL()
	{
		
	//Get Personality IDs from usb server
	TInt checkStatus;
	RArray<TInt> idArray;
	checkStatus = iUsb.GetPersonalityIds(idArray);
	TEST(checkStatus==KErrNone);
	
	// from mmp file
	TInt expectedPersonalityID = 0x10204bbc;
		
	//compare ID with expected result.	
	TEST(expectedPersonalityID == idArray[0]);		
	
	idArray.Close();

	return TestStepResult();
	}

CUsbMsccGetPersonality::CUsbMsccGetPersonality()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbMsccGetPersonality);
	}


/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates USB device read personality list from resource file 
 and validate them when RUsb connect to USB server
 */
TVerdict CUsbMsccGetPersonality::doTestStepL()
	{
	return CompareUsbPersonalityL();
	}	


/**
 * Constructor
 */
CUsbMsccStartStop::CUsbMsccStartStop()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbMsccStartStop);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates TryStart/TryStop personality at different USB server's state
 when RUsb connect to USB server
 */
TVerdict CUsbMsccStartStop::doTestStepL()
	{
	// hard coded
	TInt deviceCurrentID = 114;
	TRequestStatus status;
	TUsbServiceState currentState;
	
	RFs fs;
	// Connect to the server
    TEST(KErrNone == fs.Connect());

    // Add MS file system
	_LIT(KMsFs, "MSFS.FSY");
	TEST(KErrNone == fs.AddFileSystem(KMsFs));

	// Load the logical device
	_LIT(KDriverFileName,"TESTUSBC.LDD");
	TInt ret = User::LoadLogicalDevice(KDriverFileName);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);
	
	// call tryStart when server is in the idle state.
	// the server state should become started
		
	iUsb.TryStart(deviceCurrentID,status);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	// call tryStop when server is in the started state,
	// the server state should become idle
	iUsb.TryStop(status );
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle ==currentState);
	
	fs.Close();
	
	User::FreeLogicalDevice(_L("usbc"));
		
	return TestStepResult();
	}	

