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
 @internalComponent
*/

#ifndef REENUMERATOR_H
#define REENUMERATOR_H

#include <e32base.h>

#ifdef __CHARGING_PLUGIN_TEST_CODE__
#include <dummyldd.h>
#define __D32USBC_H__ // ensure that d32usbc is ignored
#else
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
#include <usb/d32usbc.h>
#else
#include <d32usbc.h>
#endif
#endif

class CUsbChargingReEnumerator : public CActive
	{
public:
	static CUsbChargingReEnumerator* NewL(RDevUsbcClient& aLdd);
	~CUsbChargingReEnumerator();
	void ReEnumerate();
private:
	CUsbChargingReEnumerator(RDevUsbcClient& aLdd);
	void RunL();
	void DoCancel();

private:
	RDevUsbcClient& iLdd;
	};

#endif // REENUMERATOR_H
