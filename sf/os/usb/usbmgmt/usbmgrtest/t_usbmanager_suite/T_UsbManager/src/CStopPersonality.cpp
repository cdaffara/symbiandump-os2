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
#include "CUsbTestStepBase.h"
#include "CStopPersonality.h"

CStopPersonality::CStopPersonality()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KStopPersonality);
	//We need the service to continue running after the test so that usbcheck
	//can find the device connected to the PC.
	iStopService = EFalse;
	}

TVerdict CStopPersonality::doTestStepL()
	{
	SetTestStepResult(EFail);
	TRequestStatus status;
	iUsb.Stop(status);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to stop USB service: %d"), status.Int());
		return TestStepResult();
		}

	TUsbServiceState aState;
	TInt err;
	err = iUsb.GetServiceState(aState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting USB service state: %d"), status.Int());
		return TestStepResult();
		}

	if (EUsbServiceIdle != aState)
		{
		INFO_PRINTF2(_L("USB service is in wrong state: %d"), aState);
		return TestStepResult();
		}

	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
