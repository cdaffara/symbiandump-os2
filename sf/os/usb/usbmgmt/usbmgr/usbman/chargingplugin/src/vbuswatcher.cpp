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

#include "vbuswatcher.h"
#include <usb/usbshared.h>

CVBusWatcher* CVBusWatcher::NewL(MOtgPropertiesObserver* aObserver)
	{
	CVBusWatcher* self = new(ELeave) CVBusWatcher(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CVBusWatcher::~CVBusWatcher()
	{
	Cancel();
	iVBusProp.Close();
	}

CVBusWatcher::CVBusWatcher(MOtgPropertiesObserver* aObserver)
	: CActive(EPriorityStandard)
	, iObserver(aObserver), iVBusState(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CVBusWatcher::ConstructL()
	{
#ifdef __CHARGING_PLUGIN_TEST_CODE__
    const TInt8 KPropertyOffsetForChargingTest = 100;
    User::LeaveIfError(iVBusProp.Attach(KUidUsbManCategory, KUsbOtgVBusPoweredProperty + KPropertyOffsetForChargingTest));
#else
    User::LeaveIfError(iVBusProp.Attach(KUidUsbManCategory, KUsbOtgVBusPoweredProperty));
#endif

	iVBusProp.Subscribe(iStatus);
	SetActive();

	TInt err = iVBusProp.Get(iVBusState);
	if (KErrNone != err)
		{
#ifndef __WINS__
		iVBusState = EFalse;
#else
		iVBusState = ETrue;
#endif
		}
	}

void CVBusWatcher::DoCancel()
	{
	iVBusProp.Cancel();
	}

void CVBusWatcher::RunL()
	{
	iVBusProp.Subscribe(iStatus);
	SetActive();

	TInt err = iVBusProp.Get(iVBusState);
	if (KErrNone == err)
		{
		iObserver->MpsoVBusStateChanged(iVBusState);
		}
	else
		{
#ifndef __WINS__
		iVBusState = EFalse;
#else
		iVBusState = ETrue;
#endif
		iObserver->MpsoVBusStateChanged(iVBusState);
		}
	}

TInt CVBusWatcher::VBusState() const
	{
	return iVBusState;
	}
