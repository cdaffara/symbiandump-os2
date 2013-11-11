/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FDFTEST_H__
#define __FDFTEST_H__

#include "testbase.h"
#include "usbhoststack.h"
#include <d32otgdi.h>
#include <d32otgdi_errors.h>

class RUsbOtgDriver;

//--------------------------------

NONSHARABLE_CLASS(MNotifyDeviceEventObserver)
	{
public:
	virtual void MndeoDeviceEvent() = 0;
	};

NONSHARABLE_CLASS(CActiveNotifyDeviceEvent) : public CActive
	{
public:
	static CActiveNotifyDeviceEvent* NewL(RUsbHostStack& aUsbHostStack,
		MNotifyDeviceEventObserver& aObserver,
		TDeviceEventInformation& aDeviceEventInformation);
	~CActiveNotifyDeviceEvent();

public:
	void Post();

private:
	CActiveNotifyDeviceEvent(RUsbHostStack& aUsbHostStack,
		MNotifyDeviceEventObserver& aObserver,
		TDeviceEventInformation& aDeviceEventInformation);

private: // from CActive
	void RunL();
	void DoCancel();

private: // unowned
	RUsbHostStack& iUsbHostStack;
	MNotifyDeviceEventObserver& iObserver;
	TDeviceEventInformation& iDeviceEventInformation;
	};

//--------------------------------

NONSHARABLE_CLASS(MNotifyDevmonEventObserver)
	{
public:
	virtual void MndeoDevmonEvent() = 0;
	};

NONSHARABLE_CLASS(CActiveNotifyDevmonEvent) : public CActive
	{
public:
	static CActiveNotifyDevmonEvent* NewL(RUsbHostStack& aUsbHostStack,
		MNotifyDevmonEventObserver& aObserver,
		TInt& aError);
	~CActiveNotifyDevmonEvent();

public:
	void Post();

private:
	CActiveNotifyDevmonEvent(RUsbHostStack& aUsbHostStack,
		MNotifyDevmonEventObserver& aObserver,
		TInt& aError);

private: // from CActive
	void RunL();
	void DoCancel();

private: // unowned
	RUsbHostStack& iUsbHostStack;
	MNotifyDevmonEventObserver& iObserver;
	TInt& iEvent;
	};

//--------------------------------

/**
Test for Function Driver Framework.
Acts very approximately as USBMAN in the USB Host architecture.
*/
class CFdfTest : public CTestBase,
						public MNotifyDeviceEventObserver,
						public MNotifyDevmonEventObserver
	{
public:
	static CTestBase* NewL(MTestManager& aManager);
	~CFdfTest();

private:
	CFdfTest(MTestManager& aManager);
	void ConstructL();

private: // from CTestBase
	void ProcessKeyL(TKeyCode aKeyCode);
	void DisplayTestSpecificMenu();

private: // from MNotifyDeviceEventObserver
	void MndeoDeviceEvent();

private: // from MNotifyDevmonEventObserver
	void MndeoDevmonEvent();

private: // owned
	// The handle we use when we're pretending to be USBMAN.
	RUsbHostStack iUsbHostStack;

	//needed because it starts usbdi
	RUsbOtgDriver iOtgDriver;
	TInt iBusRequested;
	CActiveNotifyDeviceEvent* iNotifyDeviceEvent;
	TDeviceEventInformation iDeviceEventInformation;

	CActiveNotifyDevmonEvent* iNotifyDevmonEvent;
	TInt iDevmonEvent;
	};

#endif // __FDFTEST_H__
