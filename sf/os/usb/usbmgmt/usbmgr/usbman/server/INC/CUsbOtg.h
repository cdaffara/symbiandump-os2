/**
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
* Implements the main object of Usbman that manages all the OTG-related activity
* 
*
*/



/**
 @file
*/

#ifndef __CUSBOTG_H__
#define __CUSBOTG_H__

#ifndef __OVER_DUMMYUSBDI__
#include <d32otgdi.h>
#else
#include <usbhost/dummyotgdi/d32otgdi.h>
#endif

#include <d32otgdi_errors.h>
#include <e32std.h>
#include <usb/usblogger.h>

#include "musbinternalobservers.h"
class CUsbServer;
class CUsbOtgWatcher;
class CUsbOtgIdPinWatcher;
class CUsbOtgVbusWatcher;
class CUsbOtgStateWatcher;
class CUsbOtgEventWatcher;
class CUsbOtgConnectionIdleWatcher;
class MUsbOtgHostNotifyObserver;
class RUsbOtgDriver;
class CRequestSessionWatcher;


/**
 * The CUsbOtg class
 *
 * Implements the main object of Usbman that manages all OTG related functionality
 * It owns one instance of CUsbOtgStateWatcher.
 * It also owns an instance of RUsbOtgDriver, a handle on the logical device
 * driver for USB OTG for Symbian OS.
 *
 * CUsbOtg is an active object which starts and stops internal CUsbOtgStateWatcher
 * in order to monitor ID-Pin and VBus state. Its RunL function will be called when
 * one of RUsbOtgDriver::TOtgEvent occurs.
 */
NONSHARABLE_CLASS(CUsbOtg) : public CBase, public MUsbOtgObserver
	{
public:
	static CUsbOtg* NewL();
	virtual ~CUsbOtg();

	void RegisterObserverL(MUsbOtgHostNotifyObserver& aObserver);
	void DeRegisterObserver(MUsbOtgHostNotifyObserver& aObserver);

	void StartL();
	void Stop();
	
	TInt BusRequest();
	TInt BusRespondSrp();
	
	TInt BusClearError();

	TInt BusDrop();

	void NotifyOtgEvent();
	
public:
	// From MUsbMessageObserver
	virtual void NotifyMessage(TInt aMessage);
	
protected:
	CUsbOtg();
	void ConstructL();
	TInt TranslateOtgEvent();

private:
	RPointerArray<MUsbOtgHostNotifyObserver> iObservers;
	TInt iLastError;
	RUsbOtgDriver iOtgDriver;
	RUsbOtgDriver::TOtgEvent iOtgEvent;
	CUsbOtgWatcher* iOtgWatcher;
	CUsbOtgIdPinWatcher* iIdPinWatcher;
	CUsbOtgVbusWatcher* iVbusWatcher;
	CUsbOtgStateWatcher* iOtgStateWatcher;
	CUsbOtgEventWatcher* iOtgEventWatcher;
    CUsbOtgConnectionIdleWatcher* iOtgConnectionIdleWatcher;
	TUint iOtgMessage;
	CRequestSessionWatcher *iRequestSessionWatcher;
	RCriticalSection iCriticalSection;
	};

#endif

