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

#ifndef DEVICESTATETIMER_H
#define DEVICESTATETIMER_H

#include <e32base.h>

class MUsbChargingDeviceStateTimerObserver
	{
public:
	virtual void DeviceStateTimeout() = 0;
	};

class CUsbChargingDeviceStateTimer : public CTimer
	{
public:
	static CUsbChargingDeviceStateTimer* NewL(MUsbChargingDeviceStateTimerObserver& aObserver);
	~CUsbChargingDeviceStateTimer();
	void Start(TTimeIntervalMicroSeconds32 aTime);
private:
	void ConstructL();
	CUsbChargingDeviceStateTimer(MUsbChargingDeviceStateTimerObserver& aObserver);
	void RunL();
	void DoCancel();

private:
	MUsbChargingDeviceStateTimerObserver& iObserver;
	TTimeIntervalMicroSeconds32 iTime;
	};

#endif // DEVICESTATETIMER_H
