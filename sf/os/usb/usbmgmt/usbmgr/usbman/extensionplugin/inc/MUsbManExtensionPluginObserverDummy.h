/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface presented by Usbman down to the concrete Usbman Extension Plugins.
*
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef USBMANEXTENSIONPLUGINOBSERVERDUMMY_H
#define USBMANEXTENSIONPLUGINOBSERVERDUMMY_H

#include <e32base.h>
#include "dummyusblddapi.h"

class MUsbDeviceNotify;
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
class MUsbChargingNotify;
#endif

class MUsbmanExtensionPluginObserver
	{

public:
	/**
	Called by the plugin to get a RDevUsbcClient handle from its owner
	@return RDevUsbcClient handle
	*/
	IMPORT_C RDevUsbcClient& DevUsbcClient();

	/**
	Called by the plugin to register for device/service state changes from its owner
	@param aObserver The observer to register for state changes
	*/
	IMPORT_C void RegisterStateObserverL(MUsbDeviceNotify& aObserver);
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1		
	IMPORT_C void RegisterChargingObserverL(MUsbChargingNotify& aObserver);
#endif	

private:
	/**
	@see DevUsbcClient.
	*/
	virtual RDevUsbcClient& MuepoDoDevUsbcClient() = 0;

	/**
	@see RegisterStateObserverL.
	*/
	virtual void MuepoDoRegisterStateObserverL(MUsbDeviceNotify& aObserver) = 0;
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	virtual void MuepoDoRegisterChargingObserverL(MUsbChargingNotify& aObserver) = 0;	
#endif	
	};

#endif // USBMANEXTENSIONPLUGINOBSERVERDUMMY_H
