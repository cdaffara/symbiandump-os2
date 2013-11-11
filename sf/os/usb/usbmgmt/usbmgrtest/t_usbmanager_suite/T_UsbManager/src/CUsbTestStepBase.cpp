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

CUsbTestStepBase::CUsbTestStepBase():
	iDoNotConnect(EFalse),
	iConnected(EFalse),
	iStopService(ETrue)
	{
	}

TVerdict CUsbTestStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	SetTestStepResult(EPass);
	
	if (!iDoNotConnect)
		{
		TInt res = iUsb.Connect();
		if (res != KErrNone)
			{
			INFO_PRINTF2(_L("Error connecting to USB service: %d"), res);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		iConnected = ETrue;
		}
	return TestStepResult();
	}

TVerdict CUsbTestStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	if (iStopService && iConnected)
		{
		TRequestStatus status;
		iUsb.Stop(status);
		User::WaitForRequest(status);
		}
	if (iConnected)
		{
		iUsb.Close();
		}
	__UHEAP_MARKEND;
	return TestStepResult();
	}

TInt CUsbTestStepBase::CheckState(RUsb sess, TUsbServiceState aState)
	{
	TUsbServiceState currState;
	TInt err;
	err = sess.GetServiceState(currState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return err;
		}
	if (currState != aState)
		{
		INFO_PRINTF3(_L("GetServiceState returned unexpected state %d, expected was: %d"), currState, aState);
		return KErrGeneral;
		}
	return KErrNone;
	}

TInt CUsbTestStepBase::SetIdle(RUsb sess)
	{
	TRequestStatus status;
	TUsbServiceState currState;
	TInt err;
	err = sess.GetServiceState(currState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return err;
		}
	if (currState == EUsbServiceIdle)
		{
		return KErrNone;
		}
	
	sess.Stop(status);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to stop service: %d"), status.Int());
		return status.Int();
		}
	return KErrNone;
	}

TInt CUsbTestStepBase::SetStarted(RUsb sess)
	{
	TRequestStatus status;
	TUsbServiceState currState;
	TInt err;
	err = sess.GetServiceState(currState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return err;
		}
	if (currState == EUsbServiceStarted)
		{
		return KErrNone;
		}
	
	sess.Start(status);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to start service: %d"), status.Int());
		return status.Int();
		}
	return KErrNone;
	}

TInt CUsbTestStepBase::SetStarting(RUsb sess)
	{
	TRequestStatus status;
	TUsbServiceState currState;
	TInt err;
	err = sess.GetServiceState(currState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return err;
		}
	if (currState == EUsbServiceStarting)
		{
		return KErrNone;
		}
	
	if (currState == EUsbServiceStarted)
		{
		sess.Stop(status);
		User::WaitForRequest(status);
		if (status != KErrNone)
			{
			INFO_PRINTF2(_L("Unable to stop service: %d"), status.Int());
			return status.Int();
			}
		if (CheckState(sess, EUsbServiceIdle) != KErrNone)
			{
				INFO_PRINTF1(_L("Can't set state to EUsbServiceIdle"));
				return KErrGeneral;
			}
		}
	
	//Start and don't wait for request, otherwise the state will be "started"
	sess.Start(status);
	sess.CancelInterest(RUsb::EStart);
	User::WaitForRequest(status);
	
	return KErrNone;
	}
	
TInt CUsbTestStepBase::SetStopping(RUsb sess)
	{
	TRequestStatus status;
	TUsbServiceState currState;
	TInt err;
	err = sess.GetServiceState(currState);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetServiceState returned error code %d"), err);
		return err;
		}
	if (currState == EUsbServiceStopping)
		{
		return KErrNone;
		}
	
	if (currState == EUsbServiceIdle)
		{
		sess.Start(status);
		User::WaitForRequest(status);
		if (status != KErrNone)
			{
			INFO_PRINTF2(_L("Unable to start service: %d"), status.Int());
			return status.Int();
			}
		if (CheckState(sess, EUsbServiceStarted) != KErrNone)
			{
			INFO_PRINTF1(_L("Can't set state to EUsbServiceStarted"));
			return KErrGeneral;
			}
		}
	
	//Stop and don't wait for request, otherwise the state will be "idle"
	sess.Stop(status);
	sess.CancelInterest(RUsb::EStop);
	User::WaitForRequest(status);
	
	return KErrNone;
	}

