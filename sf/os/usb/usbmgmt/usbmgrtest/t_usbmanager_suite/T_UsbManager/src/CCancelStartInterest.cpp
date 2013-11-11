/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CUsbTestStepBase.h"
#include "CCancelStartInterest.h"

CCancelStartInterest::CCancelStartInterest()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCancelStartInterest);
	}

TVerdict CCancelStartInterest::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt err;
	err = SetIdle(iUsb);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Can't stop USB service: %s"), err);
		return TestStepResult();
		}
		
	//Read personality from ini file
	TInt personality;
	_LIT(KPersonality, "personality"); 
	if (!GetIntFromConfig(ConfigSection(), KPersonality, personality))
		{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		return TestStepResult();
		}
	//Read timeout from ini file
	TInt timeout;
	_LIT(KTimeout, "timeout"); 
	if (!GetIntFromConfig(ConfigSection(), KTimeout, timeout))
		{
		INFO_PRINTF1(_L("Can't get timeout from config file"));
		return TestStepResult();
		}
	
	//TryStart and then cancel interest.
	TRequestStatus status;
	iUsb.TryStart(personality, status);
	iUsb.CancelInterest(RUsb::ETryStart);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		INFO_PRINTF2(_L("Bad error code after CancelInterest(EUsbTryStart): %d"), status.Int());
		return TestStepResult();
		}
	//Make sure state is still starting
	TUsbServiceState state;
	err = iUsb.GetServiceState(state);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return TestStepResult();
		}
	else if (state != EUsbServiceStarting)
		{
		INFO_PRINTF2(_L("Bad service state after CancelInterest(EUsbTryStart): %d"), state);
		return TestStepResult();
		}
	
	
	//The Usb service should start even after CancelInterest.
	//The timeout value from the config file is used so that we don't wait forever.
	for (;;)
		{
		err = iUsb.GetServiceState(state);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
			break;
			}
		else if (state == EUsbServiceStarted)
			{
			SetTestStepResult(EPass);
			break;
			}
		else if (state == EUsbServiceFatalError)
			{
			INFO_PRINTF1(_L("Service state is EUsbServiceFatalError"));
			break;
			}
		else if (timeout == 0)
			{
			INFO_PRINTF1(_L("Timeout waiting for service to start"));
			break;
			}
		User::After(1000000);
		timeout--;
		}

	return TestStepResult();
	}
	
