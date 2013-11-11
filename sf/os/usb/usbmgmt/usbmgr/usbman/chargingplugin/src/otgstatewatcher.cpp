/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Talks directly to the USB Logical Device Driver (LDD) and 
* watches any state changes
*
*/

/**
 @file
*/

#include "otgstatewatcher.h"

COtgStateWatcher* COtgStateWatcher::NewL(MOtgPropertiesObserver* aObserver)
	{
	COtgStateWatcher* self = new(ELeave) COtgStateWatcher(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COtgStateWatcher::~COtgStateWatcher()
	{
	Cancel();
	iOtgStateProp.Close();
	}

COtgStateWatcher::COtgStateWatcher(MOtgPropertiesObserver* aObserver)
	: CActive(EPriorityStandard)
	, iObserver(aObserver)
	, iOtgState(EUsbOtgStateReset)
	{
	CActiveScheduler::Add(this);
	}

void COtgStateWatcher::ConstructL()
	{
#ifdef __CHARGING_PLUGIN_TEST_CODE__
    const TInt8 KPropertyOffsetForChargingTest = 100;
    User::LeaveIfError(iOtgStateProp.Attach(KUidUsbManCategory, KUsbOtgStateProperty + KPropertyOffsetForChargingTest));
#else
    User::LeaveIfError(iOtgStateProp.Attach(KUidUsbManCategory, KUsbOtgStateProperty));
#endif

	iOtgStateProp.Subscribe(iStatus);
	SetActive();

	TInt err = iOtgStateProp.Get((TInt&)iOtgState);
	if (KErrNone != err)
		{
		iOtgState = EUsbOtgStateBPeripheral;
		}
	}

void COtgStateWatcher::DoCancel()
	{
	iOtgStateProp.Cancel();
	}

void COtgStateWatcher::RunL()
	{
	iOtgStateProp.Subscribe(iStatus);
	SetActive();

	TInt err = iOtgStateProp.Get((TInt&)iOtgState);
	if (KErrNone != err)
		{
		iOtgState = EUsbOtgStateBPeripheral;
		}
	iObserver->MpsoOtgStateChangedL(iOtgState);
	}

TUsbOtgState COtgStateWatcher::OtgState() const
	{
	return iOtgState;
	}
