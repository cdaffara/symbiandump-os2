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
*
*/

/**
 @file
*/
#include <test/testexecutelog.h>
#include <d32usbc.h>
#include "CUsbTestStepBase.h"
#include "CSimCablePulling.h"

CSimCablePulling::CSimCablePulling()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSimCablePulling);
	//We need the service to continue running after the test so that usbcheck
	//can find the device connected to the PC.
	iStopService = EFalse;
	}

/*
 This test simulates pulling and plugging of the usb cable while the service is starting.
 The device is actually connected to a host PC which checks the correct personality is loaded
 when the cable is plugged back in.
*/
TVerdict CSimCablePulling::doTestStepL()
	{
	SetTestStepResult(EFail);
	//Read personality from ini file
	TInt personality;
	_LIT(KPersonality, "personality"); 
	if (!GetIntFromConfig(ConfigSection(), KPersonality, personality))
		{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		return TestStepResult();
		}
	//Read waiting time from ini file
	TInt wait;
	_LIT(KWait, "wait"); 
	if (!GetIntFromConfig(ConfigSection(), KWait, wait))
		{
		INFO_PRINTF1(_L("Can't get waiting time from config file"));
		return TestStepResult();
		}
		
	TUsbServiceState state;
	TInt err;
	err = SetIdle(iUsb);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to stop USB service: %d"), err);
		return TestStepResult();
		}
		
	//TryStart with new personality
	TRequestStatus status;
	iUsb.TryStart(personality, status);
	
	//Simulate cable pull, wait for specified amount of time and then simulate cable plug-in
	RDevUsbcClient client;
	client.Open(1);
	err = client.DeviceDisconnectFromHost();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("DeviceDisconnectFromHost returned error code: %d"), err);
		return TestStepResult();
		}
	User::After(wait*1000000);
	err = client.DeviceConnectToHost();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("DeviceConnectToHost returned error code: %d"), err);
		return TestStepResult();
		}
	client.Close();
	

	//Check TryStart result
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to start USB service: %d"), status.Int());
		return TestStepResult();
		}
	err = iUsb.GetServiceState(state);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return TestStepResult();
		}
	if (state != EUsbServiceStarted)
		{
		INFO_PRINTF2(_L("USB service is in the wrong state: %d"), state);
		return TestStepResult();
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
