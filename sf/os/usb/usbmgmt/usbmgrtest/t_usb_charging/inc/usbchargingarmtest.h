/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __USBCHARGINGARM_H__
#define __USBCHARGINGARM_H__

#include <e32property.h>
#include <usbman.h>
#include <centralrepository.h>
#include <usbchargingpublishedinfo.h>
#include <f32file.h>

#include "testbase.h"

class CUsbChargingCurrentWatcher;
class CUsbStateWatcher;
/**
*/
class CUsbChargingArmTest : public CTestBase 
	{
public:
	static CTestBase* NewL(MTestManager& aManager);
	~CUsbChargingArmTest();
	void ShowUsbChargingValue(TPublishedUsbChargingInfo& aValue);
	void ShowUsbDeviceState(TUsbDeviceState aDeviceState);
	inline RUsb& Usb();

private:
	CUsbChargingArmTest(MTestManager& aManager);
	void ConstructL();
    void ShowChargingType(TUsbChargingPortType aChargingType);
    void ShowConnStatus(TUsbConnectionStatus aStatus);
	
	int StartMassStorage();

private: // from CTestBase
	void ProcessKeyL(TKeyCode aKeyCode);
	void DisplayTestSpecificMenu();

private: // owned
	RUsb iUsbMan;
	CUsbChargingCurrentWatcher* iChargingCurrentWatcher;
	CUsbStateWatcher* iUsbStateWatcher;
	RFs iFs;
	};

inline RUsb& CUsbChargingArmTest::Usb()
    {
    return iUsbMan;
    }

#endif // __USBCHARGINGARM_H__
