/**
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a Session of a Symbian OS server for the RUsb API
* 
*
*/



/**
 @file
*/

#ifndef __CUSBSESSION_H__
#define __CUSBSESSION_H__

#include <e32std.h>
#include "MUsbDeviceNotify.h"
#include <usb/usbshared.h>

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
#include "musbotghostnotifyobserver.h"
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

// device event queue size is number of device states + 1
// (+1 so we can distinguish full & empty!)
// (any state can only appear in the queue once)
const TInt KDeviceStatesQueueSize = KUsbDeviceStates + 1;
const TInt KOtgHostMessageQueueSize = KDeviceStatesQueueSize * 2;

//
// Forward declarations
//
class CUsbServer;

/**
 * The CUsbSession class
 *
 * Implements a Session of a Symbian OS server for the RUsb API
 */
NONSHARABLE_CLASS(CUsbSession) : public CSession2
							   , public MUsbDeviceNotify
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
							   , public MUsbOtgHostNotifyObserver
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	{
public:
	static CUsbSession* NewL(CUsbServer* aServer);
	virtual ~CUsbSession();

	// CSession2
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void CreateL();

	// MUsbDeviceNotify
	virtual void UsbDeviceStateChange(TInt aLastError, TUsbDeviceState aOldState, TUsbDeviceState aNewState);
	virtual void UsbServiceStateChange(TInt aLastError, TUsbServiceState aOldState, TUsbServiceState aNewState);

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// MUsbOtgHostNotifyObserver
	virtual void UsbOtgHostMessage(TInt aMessage);
	virtual void UsbHostEvent(TDeviceEventInformation& aDevInfo);

	TInt DoRequestSession();
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

protected:
	CUsbSession(CUsbServer* aServer);

	void DispatchMessageL(const RMessage2& aMessage);
	TInt StartDeviceL(const RMessage2& aMessage, TBool& aComplete);
	TInt StopDeviceL(const RMessage2& aMessage, TBool& aComplete);
	TInt StartCancel(const RMessage2& aMessage, TBool& aComplete);
	TInt StopCancel(const RMessage2& aMessage, TBool& aComplete);
	TInt RegisterDeviceObserver(const RMessage2& aMessage, TBool& aComplete);
	TInt RegisterServiceObserver(const RMessage2& aMessage, TBool& aComplete);
	TInt GetCurrentServiceState(const RMessage2& aMessage);
	TInt GetCurrentDeviceState(const RMessage2& aMessage);
	TInt DeRegisterServiceObserver();
	TInt DeRegisterDeviceObserver();
	TInt TryStartDeviceL(const RMessage2& aMessage, TBool& aComplete);
	TInt TryStopDeviceL(const RMessage2& aMessage, TBool& aComplete);
	TInt CancelInterest(const RMessage2& aMessage);
	TInt GetCurrentPersonalityId(const RMessage2& aMessage);
	TInt GetSupportedClasses(const RMessage2& aMessage);
	TInt GetPersonalityIds(const RMessage2& aMessage);
	TInt GetDescription(const RMessage2& aMessage);
	TInt ClassSupported(const RMessage2& aMessage);
	TInt GetPersonalityProperty(const RMessage2& aMessage);         

	void UsbDeviceDequeueEvent();

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Common functions
	TInt SetCtlSessionMode(const RMessage2& aMessage);
	TInt RegisterMsgObserver(const RMessage2& aMessage, TBool& aComplete);
	TInt DeRegisterMsgObserver();

	// OTG
	TInt BusRequest();
	TInt BusRespondSrp();

	TInt BusClearError();

	TInt BusDrop();

	// HOST
	TInt EnableFunctionDriverLoading();
	TInt DisableFunctionDriverLoading();
	TInt GetSupportedLanguages(const RMessage2& aMessage);
	TInt GetManufacturerStringDescriptor(const RMessage2& aMessage);
	TInt GetProductStringDescriptor(const RMessage2& aMessage);
	TInt GetOtgDescriptor(const RMessage2& aMessage);

	TInt RegisterHostObserver(const RMessage2& aMessage, TBool& aComplete);
	TInt DeRegisterHostObserver();
	TInt RequestSession();
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

private:
	void HandleServiceStateChangeWhileStarting(TInt aLastError,
		TUsbServiceState aNewState);
	void HandleServiceStateChangeWhileStopping(TInt aLastError,
		TUsbServiceState aNewState);

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	void UsbMsgDequeue();

	void UsbHostEventDequeue();
	TInt GetSupportedLanguagesL(const RMessage2& aMessage);

#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

private:
	CUsbServer* iUsbServer;

	RMessage2 iStartMessage;
	RMessage2 iStopMessage;
	RMessage2 iCancelMessage;
	RMessage2 iServiceObserverMessage;
	RMessage2 iDeviceObserverMessage;

	TBool iStartOutstanding;
	TBool iStopOutstanding;
	TBool iCancelOutstanding;
	TBool iDeviceObserverOutstanding;
	TBool iServiceObserverOutstanding;
	TBool iPersonalityCfged;

 	TFixedArray<TUsbDeviceState, KDeviceStatesQueueSize> iDeviceStateQueue;
 	TUsbDeviceState iNotifiedDevState;
 	TBool iObserverQueueEvents;
 	TInt iDevStateQueueHead;
 	TInt iDevStateQueueTail;

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
 	static CUsbSession* iCtlSession;
 	TBool iSessionCtlMode;

	RMessage2 iMsgObserverMessage;
	TBool iMsgObserverOutstanding;
	TFixedArray<TInt, KOtgHostMessageQueueSize> iMsgQueue;
 	TBool iMsgObserverQueueEvents;
 	TInt iMsgQueueHead;
 	TInt iMsgQueueTail;
 	TInt iNotifiedMsg;

	RMessage2 iHostEventObserverMessage;
	TBool iHostEventObserverOutstanding;
	TFixedArray<TDeviceEventInformation, KDeviceStatesQueueSize> iHostStateQueue;
 	TBool iHostEventObserverQueueEvents;
 	TInt iHostEventQueueHead;
 	TInt iHostEventQueueTail;
 	TDeviceEventInformation iNotifiedHostState;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	};

#endif //__CUSBSESSION_H__
