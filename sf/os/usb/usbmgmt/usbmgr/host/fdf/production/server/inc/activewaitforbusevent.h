/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ACTIVEWAITFORBUSEVENT_H
#define ACTIVEWAITFORBUSEVENT_H

#include <e32base.h>

#ifdef __OVER_DUMMYUSBDI__
#include <usbhost/dummyusbdi/d32usbdi_hubdriver.h>
#else
#include <d32usbdi_hubdriver.h>
#endif

NONSHARABLE_CLASS(MBusEventObserver)
	{
public:
	virtual void MbeoBusEvent() = 0;
	};

NONSHARABLE_CLASS(CActiveWaitForBusEvent) : public CActive
	{
public:
	static CActiveWaitForBusEvent* NewL(RUsbHubDriver& aHubDriver, 
		RUsbHubDriver::TBusEvent& aBusEvent,
		MBusEventObserver& aObserver);
	~CActiveWaitForBusEvent();

public:
	void Wait();

private:
	CActiveWaitForBusEvent(RUsbHubDriver& aHubDriver, 
		RUsbHubDriver::TBusEvent& aBusEvent,
		MBusEventObserver& aObserver);

private: // from CActive
	void RunL();
	void DoCancel();

private: // unowned
	RUsbHubDriver& iHubDriver;
	RUsbHubDriver::TBusEvent& iBusEvent;
	MBusEventObserver& iObserver;
	};

#endif // ACTIVEWAITFORBUSEVENT_H
