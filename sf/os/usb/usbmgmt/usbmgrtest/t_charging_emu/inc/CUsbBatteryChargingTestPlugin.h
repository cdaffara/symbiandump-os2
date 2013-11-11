/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef USBBATTERYCHARGINGTESTPLUGIN_H
#define USBBATTERYCHARGINGTESTPLUGIN_H

#include <e32base.h>
#include <dummyldd.h>
#define __D32USBC_H__ // ensure that d32usbc is ignored

#define private protected

#include <usb/charging/cusbbatterychargingplugin.h>


// define extension of the plugin, which includes an API to query the plugin state, etc.

class TPluginTestInfo
	{
public:
	TUint iAvailableMilliAmps;
	TInt iPluginState;
	TUsbDeviceState iDeviceState;
	TInt iCurrentIndexRequested;
	TUint iRequestedCurrentValue;	
	};

class MUsbBatteryChargingTestPluginInterface2
	{
public:
	virtual void GetPluginInfo(TPluginTestInfo& aInfo) = 0;
	};

const TUid KUidUsbBatteryChargingTestPluginInterface2 = {0x1020DEA6};

class CUsbBatteryChargingTestPlugin :	public CUsbBatteryChargingPlugin,
										public MUsbBatteryChargingTestPluginInterface2
	{
public:
	static CUsbBatteryChargingTestPlugin* NewL(MUsbmanExtensionPluginObserver& aObserver);
	CUsbBatteryChargingTestPlugin(MUsbmanExtensionPluginObserver& aObserver);
	void GetPluginInfo(TPluginTestInfo& aInfo);
	TAny* GetInterface(TUid aUid);
	};


#endif // USBBATTERYCHARGINGTESTPLUGIN_H
