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
* Implementation of interface presented by Usbman down to the concrete Usbman Extension Plugins.
*
*/

/**
 @file
 @publishedPartner
 @released
*/
#ifndef __DUMMY_LDD__
#include "musbmanextensionpluginobserver.h"
#else
#include "musbmanextensionpluginobserverdummy.h"
#endif

EXPORT_C RDevUsbcClient& MUsbmanExtensionPluginObserver::DevUsbcClient()
	{
	return MuepoDoDevUsbcClient();
	}

EXPORT_C void MUsbmanExtensionPluginObserver::RegisterStateObserverL(MUsbDeviceNotify& aObserver)
	{
	MuepoDoRegisterStateObserverL(aObserver);
	}

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
EXPORT_C void MUsbmanExtensionPluginObserver::RegisterChargingObserverL(MUsbChargingNotify& aObserver)
	{
	MuepoDoRegisterChargingObserverL(aObserver);
	}
#endif



