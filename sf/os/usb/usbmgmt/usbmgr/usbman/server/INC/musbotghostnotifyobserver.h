/*
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
* The mixin used by the USB OTG/Host objects to notify all of
* it's observers of any messages and state changes that occur
*
*/

/**
 @file
*/

#ifndef __MUSBOTGHOSTNOTIFYOBSERVER_H__
#define __MUSBOTGHOSTNOTIFYOBSERVER_H__

#include <usb/usbshared.h>

/**
 * The MUsbOtgHostNotifyObserver class
 *
 * The mixin used by the USB Otg/Host objects to notify all of
 * it's observers of any messages and state changes that occur
 *
 * @internalTechnology
 * @released
 */
class MUsbOtgHostNotifyObserver
	{
public:
	/**
	 * Called when the USB OTG/Host components reports
	 * new message arrival
	 *
	 * @param aMessage The last message code detected
	 */
	virtual void UsbOtgHostMessage(TInt aMessage) = 0;

	/**
	 * Called when the USB Host state has changed
	 *
	 * @param aDevInfo The last device info 
	 */
	virtual void UsbHostEvent(TDeviceEventInformation& aDevInfo) = 0;
	};

#endif
