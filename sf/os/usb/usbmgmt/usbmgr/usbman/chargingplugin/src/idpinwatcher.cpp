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

#include "idpinwatcher.h"
#include <usb/usbshared.h>

CIdPinWatcher* CIdPinWatcher::NewL(MOtgPropertiesObserver* aObserver)
	{
	CIdPinWatcher* self = new(ELeave) CIdPinWatcher(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CIdPinWatcher::~CIdPinWatcher()
	{
	Cancel();
	iIdPinProp.Close();
	}

CIdPinWatcher::CIdPinWatcher(MOtgPropertiesObserver* aObserver)
	: CActive(EPriorityStandard)
	, iObserver(aObserver), iIdPinValue(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CIdPinWatcher::ConstructL()
	{
#ifdef __CHARGING_PLUGIN_TEST_CODE__
    const TInt8 KPropertyOffsetForChargingTest = 100;
    User::LeaveIfError(iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgIdPinPresentProperty + KPropertyOffsetForChargingTest));
#else
    User::LeaveIfError(iIdPinProp.Attach(KUidUsbManCategory, KUsbOtgIdPinPresentProperty));
#endif
    
	iIdPinProp.Subscribe(iStatus);
	SetActive();

	TInt err = iIdPinProp.Get(iIdPinValue);
	if (KErrNone != err)
		{
		iIdPinValue = EFalse;
		}
	}

void CIdPinWatcher::DoCancel()
	{
	iIdPinProp.Cancel();
	}

void CIdPinWatcher::RunL()
	{
	iIdPinProp.Subscribe(iStatus);
	SetActive();

	TInt err = iIdPinProp.Get(iIdPinValue);
	if (KErrNone == err)
		{
		iObserver->MpsoIdPinStateChanged(iIdPinValue);
		}
	else
		{
		iIdPinValue = EFalse;
		}
	}

TInt CIdPinWatcher::IdPinValue() const
	{
	return iIdPinValue;
	}
